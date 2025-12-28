//
//  Created by Matt Hartley on 28/12/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "video.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
// NOTE: I have to do this to stop it erroring for some reason
typedef signed long int GLintptr;
typedef signed long int GLsizeiptr;
#include <GL/glx.h>


#define TRUE 1
#define FALSE 0


CORE window_t vid_init_window(char* title, int width, int height, int flags)
{
	window_t vid = {
		.width = width,
		.height = height,
		.flags = flags
	};

	Display* display = XOpenDisplay(NULL);

	Window parent = XDefaultRootWindow(display);
	Window window = XCreateSimpleWindow(display, parent, 100, 100, width, height, 0, 0, 0);

	XStoreName(display, window, title);

	XSelectInput(display, window, KeyPressMask | KeyReleaseMask | ClientMessage | StructureNotifyMask);

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
