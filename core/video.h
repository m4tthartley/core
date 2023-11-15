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

#include <gl/gl.h>


#define KEY_F1 0x70
#define KEY_F2 0x71
#define KEY_F3 0x72
#define KEY_F4 0x73
#define KEY_F5 0x74
#define KEY_F6 0x75
#define KEY_F7 0x76
#define KEY_F8 0x77
#define KEY_F9 0x78
#define KEY_F10 0x79
#define KEY_F11 0x7A
#define KEY_F12 0x7B
#define KEY_LEFT 0x25
#define KEY_UP 0x26
#define KEY_RIGHT 0x27
#define KEY_DOWN 0x28
#define KEY_BACK 0x08
#define KEY_TAB 0x09
#define KEY_RETURN 0x0D
#define KEY_SHIFT 0x10
#define KEY_CONTROL 0x11
#define KEY_MENU 0x12
#define KEY_ESC 0x1B
#define KEY_SPACE 0x20


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
} core_button_t;

typedef struct {
} core_keyboard_t;

typedef struct {
	struct {
        int x;
        int y;
    } pos;
	struct {
        int x;
        int y;
    } pos_dt;
	core_button_t left;
	core_button_t right;
	int wheel_dt;
} core_mouse_t;

typedef struct {
#ifdef __WIN32__
	HWND hwnd;
	HDC hdc;
    wglSwapIntervalEXT_proc wglSwapIntervalEXT;
	wglGetSwapIntervalEXT_proc wglGetSwapIntervalEXT;
#endif

	int width;
	int height;
	b32 quit;
    u32 flags;
	core_button_t keyboard[256];
	core_mouse_t mouse;
} core_window_t;


b32 core_window(core_window_t* window, char* title, int width, int height, int flags);
b32 core_opengl(core_window_t* window);
void core_window_update(core_window_t* window);
void core_opengl_swap(core_window_t* window);


#endif
