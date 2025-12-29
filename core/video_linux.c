//
//  Created by Matt Hartley on 28/12/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "video.h"

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
// NOTE: I have to do this to stop it erroring for some reason
// typedef signed long int GLintptr;
// typedef signed long int GLsizeiptr;
#include <GL/glx.h>


#define TRUE 1
#define FALSE 0


void __vid_print(char* str) {
	int len = 0;
	while (str[len]) ++len;
	write(STDOUT_FILENO, str, len);
}

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
		__vid_print("XMatchVisualInfo failed \n");
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

	XSelectInput(display, window, KeyPressMask | KeyReleaseMask | ClientMessage | StructureNotifyMask | ResizeRedirectMask);

	XMapWindow(display, window);
	XFlush(display);

	vid.sysDisplay = display;
	vid.sysWindow = window;

	return vid;
}

CORE void vid_poll_events(window_t* vid)
{
	Display* display = vid->sysDisplay;

	while (XPending(display) > 0) {
		XEvent event = {0};
		XNextEvent(display, &event);

		// printf("event %i \n", event.type);
		switch (event.type) {
			case DestroyNotify:
				vid->quit = TRUE;
				break;
			case ResizeRequest:
				// vid->width = event.xresizerequest.width;
				// vid->height = event.xresizerequest.height;
				break;

			case KeyPress:
			case KeyRelease: {
				// event.xbutton.state
			} break;
		}
	}
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
}
