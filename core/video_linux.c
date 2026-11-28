//
//  Created by Matt Hartley on 28/12/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "video.h"

#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xfixes.h>
#include <GL/gl.h>
// NOTE: I have to do this to stop it erroring for some reason
// typedef signed long int GLintptr;
// typedef signed long int GLsizeiptr;
#include <GL/glx.h>


#ifndef TRUE
#	define TRUE 1
#endif
#ifndef FALSE
#	define FALSE 0
#endif


void _init_xinput2(window_t* vid);

// void __vid_print(char* str) {
// 	int len = 0;
// 	while (str[len]) ++len;
// 	write(STDOUT_FILENO, str, len);
// }

CORE window_t vid_init_window(char* title, int width, int height, int flags)
{
	// TODO: Error handling
	// TODO: Maybe switch to EGL

	window_t vid = {
		.width = width,
		.height = height,
		.flags = flags
	};

	Display* display = XOpenDisplay(NULL);

	Window root = XDefaultRootWindow(display);
	int screen = XDefaultScreen(display);

	XVisualInfo visualInfo;
	if (!XMatchVisualInfo(display, screen, 24, TrueColor, &visualInfo)) {
		sys_print_err("XMatchVisualInfo failed \n");
		exit(1);
	}

	XSetWindowAttributes attrs = {
		.background_pixel = 0,
		.colormap = XCreateColormap(display, root, visualInfo.visual, AllocNone),
	};
	unsigned long attrMask = CWBackPixel | CWColormap;

	Window window = XCreateWindow(
		display, root,
		0, 0,
		width, height, 0,
		visualInfo.depth, InputOutput, visualInfo.visual, attrMask, &attrs
	);
	// Window window = XCreateSimpleWindow(display, parent, 100, 100, width, height, 0, 0, 0);

	XStoreName(display, window, title);

	long eventMask = ClientMessage | StructureNotifyMask |
		KeyPressMask|KeyReleaseMask |
		ButtonPressMask|ButtonReleaseMask |
		PointerMotionMask;
	XSelectInput(display, window, eventMask);

	XMapWindow(display, window);
	XFlush(display);

	vid.sysDisplay = display;
	vid.sysWindow = window;

	_init_xinput2(&vid);

	return vid;
}

CORE void vid_init_opengl(window_t* vid)
{
	Display* display = vid->sysDisplay;

	int attrs[] = {
		GLX_RGBA,
		GLX_DEPTH_SIZE, 24,
		GLX_DOUBLEBUFFER,
		None,
	};
	XVisualInfo* visualInfo = glXChooseVisual(display, 0, attrs);

	GLXContext context = glXCreateContext(display, visualInfo, 0, TRUE);
	glXMakeCurrent(display, vid->sysWindow, context);

	vid->sysGLContext = context;
}

void _init_xinput2(window_t* vid)
{
	int event;
	int error;
	if (!XQueryExtension(vid->sysDisplay, "XInputExtension", &vid->sysXiOpcode, &event, &error)) {
		sys_print_err("X11 XInputExtension is not available.");
		return;
	}

	int major = 2;
	int minor = 0;
	if (XIQueryVersion(vid->sysDisplay, &major, &minor) == BadRequest) {
		sys_print_err("X11 XInput minimum version is not available.");
		return;
	}

	uint8_t maskBits[XIMaskLen(XI_LASTEVENT)] = {0};
	XISetMask(maskBits, XI_RawMotion);

	XIEventMask mask = {
		.deviceid = XIAllMasterDevices,
		.mask_len = sizeof(maskBits),
		.mask = maskBits
	};
	XISelectEvents(vid->sysDisplay, DefaultRootWindow(vid->sysDisplay), &mask, 1);
}

Cursor __create_blank_pointer(window_t* vid)
{
	char data[1] = {0};
	Pixmap pix = XCreateBitmapFromData(vid->sysDisplay, vid->sysWindow, data, 1, 1);
	XColor black = {0};
	Cursor cursor = XCreatePixmapCursor(vid->sysDisplay, pix, pix, &black, &black, 0, 0);
	XFreePixmap(vid->sysDisplay, pix);
	return cursor;
}

CORE void vid_grab_pointer(window_t* vid)
{
	if (!vid->pointerGrabbed) {
		Cursor cursor = __create_blank_pointer(vid);
		XGrabPointer(vid->sysDisplay, vid->sysWindow, True, PointerMotionMask, GrabModeAsync, GrabModeAsync, vid->sysWindow, cursor, CurrentTime);
		vid->pointerGrabbed = TRUE;
		// XFixesHideCursor(vid->sysDisplay, vid->sysWindow);
	}
}

CORE void vid_release_pointer(window_t* vid)
{
	if (vid->pointerGrabbed) {
		XUngrabPointer(vid->sysDisplay, CurrentTime);
		vid->pointerGrabbed = FALSE;
		// XFixesShowCursor(vid->sysDisplay, vid->sysWindow);
	}
}

CORE void vid_poll_events(window_t* vid)
{
	Display* display = vid->sysDisplay;

	KeyCode superKeycode = XKeysymToKeycode(display, XK_Super_L);
	KeyCode optionKeycode = XKeysymToKeycode(display, XK_Alt_L);
	KeyCode controlKeycode = XKeysymToKeycode(display, XK_Control_L);
	KeyCode shiftKeycode = XKeysymToKeycode(display, XK_Shift_L);

	_update_button(&vid->modifier_keys.super, vid->modifier_keys.super.down);
	_update_button(&vid->modifier_keys.option, vid->modifier_keys.option.down);
	_update_button(&vid->modifier_keys.control, vid->modifier_keys.control.down);
	_update_button(&vid->modifier_keys.shift, vid->modifier_keys.shift.down);

	for (int i=0; i<256; ++i) {
		_update_button(vid->keyboard+i, vid->keyboard[i].down);
	}

	_update_button(&vid->mouse.left, vid->mouse.left.down);
	_update_button(&vid->mouse.right, vid->mouse.right.down);

	// NOTE: Does using this mean we don't need to bother setting from the motion events
	// 		and just use the motion events for incrementing the delta?
	// Window root;
	// Window child;
	// int mouseX;
	// int mouseY;
	// int mouseRootX;
	// int mouseRootY;
	// uint32_t mouseMask;
	// XQueryPointer(display, vid->sysWindow, &root, &child, &mouseRootX, &mouseRootY, &mouseX, &mouseY, &mouseMask);
	// vid->mouse.pos.x = mouseX;
	// vid->mouse.pos.y = mouseY;

	vid->mouse.delta.x = 0;
	vid->mouse.delta.y = 0;
	vid->mouse.wheel_dt = 0;

	// Center grabbed pointer
	if (vid->pointerGrabbed) {
		XWarpPointer(vid->sysDisplay, None, vid->sysWindow, 0, 0, 0, 0, vid->width/2, vid->height/2);
	}

	while (XPending(display) > 0) {
		XEvent event = {0};
		XNextEvent(display, &event);

		// printf("event %i \n", event.type);
		switch (event.type) {
			case DestroyNotify:
				vid->quit = TRUE;
				break;
			case ResizeRequest:
				vid->width = event.xresizerequest.width;
				vid->height = event.xresizerequest.height;
				// printf("resize %i, %i \n", event.xresizerequest.width, event.xresizerequest.height);
				break;

			// case ConfigureNotify:
			// 	vid->width = event.xconfigure.width;
			// 	vid->height = event.xconfigure.height;
			// 	printf("resize %i, %i \n", event.xconfigure.width, event.xconfigure.height);
			// 	// XResizeWindow(display, vid->sysWindow, vid->width, vid->height);
			// 	break;

			case KeyPress:
			case KeyRelease: {
				// event.xbutton.state
				XKeyPressedEvent* keyEvent = (XKeyPressedEvent*)&event;
				bool state = (event.type == KeyPress);
				// KeySym sym = XLookupKeysym(&event.xkey, 0);
				// KeyCode keycode = XKeysymToKeycode(display, XK_W);
				// KeyCode w = XKeycodeToKeysym(display, XK_W);
				// XKeycodeToKeysym(Display *, KeyCode, int)
				_update_button(&vid->keyboard[keyEvent->keycode], state);
				
				if (keyEvent->keycode == superKeycode) {
					_update_button(&vid->modifier_keys.super, state);
				}
				if (keyEvent->keycode == optionKeycode) {
					_update_button(&vid->modifier_keys.option, state);
				}
				if (keyEvent->keycode == controlKeycode) {
					_update_button(&vid->modifier_keys.control, state);
				}
				if (keyEvent->keycode == shiftKeycode) {
					_update_button(&vid->modifier_keys.shift, state);
				}

			} break;

			case ButtonPress:
			case ButtonRelease: {
				XButtonPressedEvent* buttonEvent = (XButtonPressedEvent*)&event;
				bool state = (event.type == ButtonPress);

				// buttonEvent->button
				if (buttonEvent->button == Button1) {
					_update_button(&vid->mouse.left, state);
				}
				if (buttonEvent->button == Button3) {
					_update_button(&vid->mouse.right, state);
				}

				if (buttonEvent->button == Button4) {
					vid->mouse.wheel_dt -= 0.5f;
				}
				if (buttonEvent->button == Button5) {
					vid->mouse.wheel_dt += 0.5f;
				}
			} break;

			case MotionNotify: {
				XMotionEvent* motionEvent = (XMotionEvent*)&event;
				// motionEvent->
				vid->mouse.pos.x = motionEvent->x;
				vid->mouse.pos.y = motionEvent->y;
			} break;

			case GenericEvent: {
				XGetEventData(vid->sysDisplay, &event.xcookie);
				if (event.xcookie.evtype == XI_RawMotion) {
					XIRawEvent* raw = event.xcookie.data;
					if (raw->valuators.mask_len) {
						// float xDelta = 0.0f;
						// float yDelta = 0.0f;
						if (XIMaskIsSet(raw->valuators.mask, 0)) {
							vid->mouse.delta.x += raw->raw_values[0];
						}
						if (XIMaskIsSet(raw->valuators.mask, 1)) {
							vid->mouse.delta.y += raw->raw_values[1];
						}

						if (vid->pointerGrabbed) {
							// XWarpPointer(vid->sysDisplay, None, vid->sysWindow, 0, 0, 0, 0, vid->width/2, vid->height/2);
						}

						// printf("raw mount motion %f, %f \n", vid->mouse.delta.x, vid->mouse.delta.y);
					}
				}
				XFreeEventData(vid->sysDisplay, &event.xcookie);
			} break;
		}

		// if (event.xcookie.type==GenericEvent && event.xcookie.extension==vid->sysXiOpcode) {
		// 	if (XGetEventData(vid->sysDisplay, &event.xcookie)) {

		// 	}
		// }
	}
}

CORE vid_button_t vid_get_key_state(window_t* vid, sys_key_code_t keycode)
{
	KeyCode physicalKeycode = XKeysymToKeycode(vid->sysDisplay, keycode);
	vid_button_t key = vid->keyboard[physicalKeycode];
	return key;
}

CORE bool vid_key_down(window_t* vid, sys_key_code_t keycode)
{
	vid_button_t key = vid_get_key_state(vid, keycode);
	return key.down;
}

CORE bool vid_key_pressed(window_t* vid, sys_key_code_t keycode)
{
	vid_button_t key = vid_get_key_state(vid, keycode);
	return key.pressed;
}

CORE bool vid_key_released(window_t* vid, sys_key_code_t keycode)
{
	vid_button_t key = vid_get_key_state(vid, keycode);
	return key.released;
}
