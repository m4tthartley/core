//
//  video.h
//  Core
//
//  Created by Matt Hartley on 14/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_VIDEO_HEADER__
#define __CORE_VIDEO_HEADER__


#include "core.h"

#ifdef __WIN32__
#include <gl/gl.h>
#endif
#ifdef __LINUX__
#include <gl/gl.h>
#endif
#ifdef __MACOS__
#include <OpenGL/gl.h>
#endif


#define KEY_F1      0x70
#define KEY_F2      0x71
#define KEY_F3      0x72
#define KEY_F4      0x73
#define KEY_F5      0x74
#define KEY_F6      0x75
#define KEY_F7      0x76
#define KEY_F8      0x77
#define KEY_F9      0x78
#define KEY_F10     0x79
#define KEY_F11     0x7A
#define KEY_F12     0x7B
#define KEY_LEFT    0x25
#define KEY_UP      0x26
#define KEY_RIGHT   0x27
#define KEY_DOWN    0x28
#define KEY_BACK    0x08
#define KEY_TAB     0x09
#define KEY_RETURN  0x0D
#define KEY_SHIFT   0x10
#define KEY_CONTROL 0x11
#define KEY_MENU    0x12
#define KEY_ESC     0x1B
#define KEY_SPACE   0x20


#ifdef __WIN32__
typedef BOOL (*wglSwapIntervalEXT_proc)(int interval);
typedef int (*wglGetSwapIntervalEXT_proc)(void);
#endif

enum {
	WINDOW_DEFAULT = (1<<0),
	WINDOW_CENTERED = (1<<1),
	WINDOW_BORDERED = (1<<2),
	WINDOW_BORDERLESS = (1<<3),
	WINDOW_FULLSCREEN = (1<<4),
	WINDOW_RESIZEABLE = (1<<5),
};

typedef struct {
	b32 down;
	b32 pressed;
	b32 released;
} button_t;

typedef struct {
	button_t keys[256];
} keyboard_t;

typedef struct {
	struct {
		int x;
		int y;
	} pos;
	struct {
		int x;
		int y;
	} pos_dt;
	button_t left;
	button_t right;
	int wheel_dt;
} mouse_t;

typedef struct {
#if defined(__SDL__)
	void* sdl_window; // SDL_Window*
#elif defined(__WIN32__)
	HWND hwnd;
	HDC hdc;
	wglSwapIntervalEXT_proc wglSwapIntervalEXT;
	wglGetSwapIntervalEXT_proc wglGetSwapIntervalEXT;
#endif

	int width;
	int height;
	b32 quit;
	u32 flags;
	button_t keyboard[256];
	mouse_t mouse;
} window_t;


b32 start_window(window_t* window, char* title, int width, int height, int flags);
b32 start_opengl(window_t* window);
void update_window(window_t* window);
void opengl_swap_buffers(window_t* window);

static inline void _update_button(button_t *button, b32 new_state) {
	button->pressed = new_state && !button->down;
	button->released = !new_state && button->down;
	button->down = new_state;
}


#endif
