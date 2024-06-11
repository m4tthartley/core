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


#ifdef __SDL__
#	define KEY_A 4
#	define KEY_B 5
#	define KEY_C 6
#	define KEY_D 7
#	define KEY_E 8
#	define KEY_F 9
#	define KEY_G 10
#	define KEY_H 11
#	define KEY_I 12
#	define KEY_J 13
#	define KEY_K 14
#	define KEY_L 15
#	define KEY_M 16
#	define KEY_N 17
#	define KEY_O 18
#	define KEY_P 19
#	define KEY_Q 20
#	define KEY_R 21
#	define KEY_S 22
#	define KEY_T 23
#	define KEY_U 24
#	define KEY_V 25
#	define KEY_W 26
#	define KEY_X 27
#	define KEY_Y 28
#	define KEY_Z 29
#	define KEY_1 30
#	define KEY_2 31
#	define KEY_3 32
#	define KEY_4 33
#	define KEY_5 34
#	define KEY_6 35
#	define KEY_7 36
#	define KEY_8 37
#	define KEY_9 38
#	define KEY_0 39

#	define KEY_F1      58
#	define KEY_F2      59
#	define KEY_F3      60
#	define KEY_F4      61
#	define KEY_F5      62
#	define KEY_F6      63
#	define KEY_F7      64
#	define KEY_F8      65
#	define KEY_F9      66
#	define KEY_F10     67
#	define KEY_F11     68
#	define KEY_F12     69
#	define KEY_LEFT    80
#	define KEY_UP      82
#	define KEY_RIGHT   79
#	define KEY_DOWN    81
#	define KEY_BACK    42
#	define KEY_TAB     43
#	define KEY_RETURN  40
#	define KEY_SHIFT   225
#	define KEY_CONTROL 224
#	define KEY_MENU    118
#	define KEY_ESC     41
#	define KEY_SPACE   44
#elif defined(__WIN32__)
#	define KEY_A 'A'
#	define KEY_B 'B'
#	define KEY_C 'C'
#	define KEY_D 'D'
#	define KEY_E 'E'
#	define KEY_F 'F'
#	define KEY_G 'G'
#	define KEY_H 'H'
#	define KEY_I 'I'
#	define KEY_J 'J'
#	define KEY_K 'K'
#	define KEY_L 'L'
#	define KEY_M 'M'
#	define KEY_N 'N'
#	define KEY_O 'O'
#	define KEY_P 'P'
#	define KEY_Q 'Q'
#	define KEY_R 'R'
#	define KEY_S 'S'
#	define KEY_T 'T'
#	define KEY_U 'U'
#	define KEY_V 'V'
#	define KEY_W 'W'
#	define KEY_X 'X'
#	define KEY_Y 'Y'
#	define KEY_Z 'Z'
#	define KEY_1 '1'
#	define KEY_2 '2'
#	define KEY_3 '3'
#	define KEY_4 '4'
#	define KEY_5 '5'
#	define KEY_6 '6'
#	define KEY_7 '7'
#	define KEY_8 '8'
#	define KEY_9 '9'
#	define KEY_0 '0'

#	define KEY_F1      0x70
#	define KEY_F2      0x71
#	define KEY_F3      0x72
#	define KEY_F4      0x73
#	define KEY_F5      0x74
#	define KEY_F6      0x75
#	define KEY_F7      0x76
#	define KEY_F8      0x77
#	define KEY_F9      0x78
#	define KEY_F10     0x79
#	define KEY_F11     0x7A
#	define KEY_F12     0x7B
#	define KEY_LEFT    0x25
#	define KEY_UP      0x26
#	define KEY_RIGHT   0x27
#	define KEY_DOWN    0x28
#	define KEY_BACK    0x08
#	define KEY_TAB     0x09
#	define KEY_RETURN  0x0D
#	define KEY_SHIFT   0x10
#	define KEY_CONTROL 0x11
#	define KEY_MENU    0x12
#	define KEY_ESC     0x1B
#	define KEY_SPACE   0x20
#endif


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
