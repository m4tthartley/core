//
//  Created by Matt Hartley on 14/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//


#ifndef __CORE_VIDEO_HEADER__
#define __CORE_VIDEO_HEADER__


#include <stdint.h>


#define CORE_VIDEO_FUNC


// #ifdef __WIN32__
// #   include <gl/gl.h>
// #endif
// #ifdef __LINUX__
// #   include <gl/gl.h>
// #endif
// #ifdef __MACOS__
// #   define GL_SILENCE_DEPRECATION
// #   include <OpenGL/gl.h>
// #endif


#ifdef __SDL__

#elif defined(__WIN32__)

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

enum {
#ifdef __APPLE__
	MODIFIER_KEY_CAPSLOCK = (/*NSEventModifierFlagCapsLock*/(1 << 16) >> 16),
	MODIFIER_KEY_SHIFT = (/*NSEventModifierFlagShift*/(1 << 17) >> 16),
	MODIFIER_KEY_CONTROL = (/*NSEventModifierFlagControl*/(1 << 18) >> 16),
	MODIFIER_KEY_OPTION = (/*NSEventModifierFlagOption*/(1 << 19) >> 16),
	MODIFIER_KEY_COMMAND = (/*NSEventModifierFlagCommand*/(1 << 20) >> 16),
	MODIFIER_KEY_NUMPAD = (/*NSEventModifierFlagNumericPad*/(1 << 21) >> 16),
	MODIFIER_KEY_HELP = (/*NSEventModifierFlagHelp*/(1 << 22) >> 16),
	MODIFIER_KEY_FUNCTION = (/*NSEventModifierFlagFunction*/(1 << 23) >> 16),
#else
	MODIFIER_KEY_CAPSLOCK = (1<<0),
	MODIFIER_KEY_SHIFT = (1<<1),
	MODIFIER_KEY_CONTROL = (1<<2),
	MODIFIER_KEY_OPTION = (1<<3),
	MODIFIER_KEY_COMMAND = (1<<4),
	MODIFIER_KEY_NUMPAD = (1<<5),
	MODIFIER_KEY_HELP = (1<<6),
	MODIFIER_KEY_FUNCTION = (1<<7),
#endif

	/*
	NSEventModifierFlagCapsLock
	NSEventModifierFlagShift
	NSEventModifierFlagControl
	NSEventModifierFlagOption
	NSEventModifierFlagCommand
	NSEventModifierFlagNumericPad
	NSEventModifierFlagHelp
	NSEventModifierFlagFunction
	*/
};

typedef struct {
	_Bool down;
	_Bool pressed;
	_Bool released;
	uint8_t modifiers;
} sys_button_t;

typedef struct {
	sys_button_t keys[256];
} sys_keyboard_t;

typedef struct {
	struct {
		int x;
		int y;
	} pos;
	struct {
		int x;
		int y;
	} pos_dt;
	union {
		struct {
			sys_button_t left;
			sys_button_t right;
		};
		sys_button_t buttons[2];
	};
	float wheel_dt;
	struct {
		float x;
		float y;
	} drag;
} sys_mouse_t;

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
	int fbWidth;
	int fbHeight;
	_Bool quit;
	_Bool flags;
	_Bool active;

	sys_mouse_t mouse;
	sys_button_t keyboard[256];
	union {
		struct {
			sys_button_t capslock;
			sys_button_t shift;
			sys_button_t control;
			sys_button_t option;
			sys_button_t command;
			sys_button_t numpad;
			sys_button_t help;
			sys_button_t function;
		};

		sys_button_t keys[8];
	} modifier_keys;

#ifdef __APPLE__
	void* sysApp;
	void* sysWindow;

	void* mtlDevice;
	void* mtlLayer;
	void* mtlCommandQueue;

	void* glContext;

#endif
#ifdef __LINUX__
#endif
#ifdef __WIN32__
	HWND sysWindow;
#endif
} sys_window_t;


// _Bool start_window(window_t* window, char* title, int width, int height, int flags);
// _Bool start_opengl(window_t* window);
// void update_window(window_t* window);
// void opengl_swap_buffers(window_t* window);

CORE_VIDEO_FUNC _Bool sys_init_window(sys_window_t* win, char* title, int width, int height, int flags);
CORE_VIDEO_FUNC void sys_poll_events(sys_window_t* win);
CORE_VIDEO_FUNC _Bool sys_message_box(char* title, char* msg, char* yesOption, char* noOption);

CORE_VIDEO_FUNC void sys_init_metal(sys_window_t* win);
CORE_VIDEO_FUNC void sys_init_opengl(sys_window_t* win);
CORE_VIDEO_FUNC void sys_present_opengl(sys_window_t* win);

static inline void _update_button(sys_button_t *button, _Bool new_state) {
	button->pressed = new_state && !button->down;
	button->released = !new_state && button->down;
	button->down = new_state;
}


#ifdef __SDL__

typedef enum {
	KEY_A=4, KEY_B=5, KEY_C=6, KEY_D=7, KEY_E=8, KEY_F=9, KEY_G=10, KEY_H=11, KEY_I=12, KEY_J=13,
	KEY_K=14, KEY_L=15, KEY_M=16, KEY_N=17, KEY_O=18, KEY_P=19, KEY_Q=20, KEY_R=21, KEY_S=22,
	KEY_T=23, KEY_U=24, KEY_V=25, KEY_W=26, KEY_X=27, KEY_Y=28, KEY_Z=29,
	
	KEY_1=30, KEY_2=31, KEY_3=32, KEY_4=33, KEY_5=34,
	KEY_6=35, KEY_7=36, KEY_8=37, KEY_9=38, KEY_0=39,
	
	KEY_F1=58, KEY_F2=59, KEY_F3=60, KEY_F4=61, KEY_F5=62, KEY_F6=63,
	KEY_F7=64, KEY_F8=65, KEY_F9=66, KEY_F10=67, KEY_F11=68, KEY_F12=69,
	
	KEY_LEFT=80, KEY_UP=82, KEY_RIGHT=79, KEY_DOWN=81,
	
	KEY_RETURN=40, KEY_BACKSPACE=42, KEY_SPACE=44, KEY_ESC=41, KEY_TAB=43,
	KEY_SHIFT=0xE1, KEY_RSHIFT=0xE5, KEY_CONTROL=0xE0, KEY_RCONTROL=0xE4,
	KEY_MENU=0xE2, KEY_RMENU=0xE6, KEY_COMMAND=0xE3, KEY_RCOMMAND=0xE7,
} sys_key_code_t;

#elif defined(__APPLE__)

#	include <Carbon/Carbon.h>
typedef enum {
	KEY_A=kVK_ANSI_A, KEY_B=kVK_ANSI_B, KEY_C=kVK_ANSI_C, KEY_D=kVK_ANSI_D, KEY_E=kVK_ANSI_E, KEY_F=kVK_ANSI_F,
	KEY_G=kVK_ANSI_G, KEY_H=kVK_ANSI_H, KEY_I=kVK_ANSI_I, KEY_J=kVK_ANSI_J, KEY_K=kVK_ANSI_K, KEY_L=kVK_ANSI_L,
	KEY_M=kVK_ANSI_M, KEY_N=kVK_ANSI_N, KEY_O=kVK_ANSI_O, KEY_P=kVK_ANSI_P, KEY_Q=kVK_ANSI_Q, KEY_R=kVK_ANSI_R,
	KEY_S=kVK_ANSI_S, KEY_T=kVK_ANSI_T, KEY_U=kVK_ANSI_U, KEY_V=kVK_ANSI_V, KEY_W=kVK_ANSI_W, KEY_X=kVK_ANSI_X,
	KEY_Y=kVK_ANSI_Y, KEY_Z=kVK_ANSI_Z,

	KEY_1=kVK_ANSI_1, KEY_2=kVK_ANSI_2, KEY_3=kVK_ANSI_3, KEY_4=kVK_ANSI_4, KEY_5=kVK_ANSI_5,
	KEY_6=kVK_ANSI_6, KEY_7=kVK_ANSI_7, KEY_8=kVK_ANSI_8, KEY_9=kVK_ANSI_9, KEY_0=kVK_ANSI_0,
	
	KEY_F1=kVK_F1, KEY_F2=kVK_F2, KEY_F3=kVK_F3, KEY_F4=kVK_F4, KEY_F5=kVK_F5, KEY_F6=kVK_F6,
	KEY_F7=kVK_F7, KEY_F8=kVK_F8, KEY_F9=kVK_F9, KEY_F10=kVK_F10, KEY_F11=kVK_F11, KEY_F12=kVK_F12,
	
	KEY_LEFT=kVK_LeftArrow, KEY_UP=kVK_UpArrow, KEY_RIGHT=kVK_RightArrow, KEY_DOWN=kVK_DownArrow,
	
	KEY_RETURN=kVK_Return, KEY_BACKSPACE=kVK_Delete, KEY_SPACE=kVK_Space, KEY_ESC=kVK_Escape, KEY_TAB=kVK_Tab,
	KEY_SHIFT=kVK_Shift, KEY_RSHIFT=kVK_RightShift, KEY_CONTROL=kVK_Control, KEY_RCONTROL=kVK_RightControl,
	KEY_MENU=kVK_Option, KEY_RMENU=kVK_RightOption, KEY_COMMAND=kVK_Command, KEY_RCOMMAND=kVK_RightCommand,
} sys_key_code_t;

#elif defined(__LINUX__)

#error "LINUX KEY CODES ARE NOT IMPLEMENTED"

#elif defined(__WIN32__)

typedef enum {
	KEY_A='A', KEY_B='B', KEY_C='C', KEY_D='D', KEY_E='E', KEY_F='F',
	KEY_G='G', KEY_H='H', KEY_I='I', KEY_J='J', KEY_K='K', KEY_L='L',
	KEY_M='M', KEY_N='N', KEY_O='O', KEY_P='P', KEY_Q='Q', KEY_R='R',
	KEY_S='S', KEY_T='T', KEY_U='U', KEY_V='V', KEY_W='W', KEY_X='X',
	KEY_Y='Y', KEY_Z='Z',

	KEY_1='1', KEY_2='2', KEY_3='3', KEY_4='4', KEY_5='5',
	KEY_6='6', KEY_7='7', KEY_8='8', KEY_9='9', KEY_0='0',

	KEY_F1=0x70, KEY_F2=0x71, KEY_F3=0x72, KEY_F4=0x73, KEY_F5=0x74, KEY_F6=0x75,
	KEY_F7=0x76, KEY_F8=0x77, KEY_F9=0x78, KEY_F10=0x79, KEY_F11=0x7A, KEY_F12=0x7B,

	KEY_LEFT=0x25, KEY_UP=0x26, KEY_RIGHT=0x27, KEY_DOWN=0x28,

	KEY_RETURN=0x0D, KEY_BACKSPACE=0x08, KEY_SPACE=0x20, KEY_ESC=0x1B, KEY_TAB=0x09,
	KEY_SHIFT=0x10, KEY_LSHIFT=0xA0, KEY_RSHIFT=0xA1, KEY_CONTROL=0x11, KEY_LCONTROL=0xA2, KEY_RCONTROL=0xA3,
	KEY_MENU=0x12, KEY_LMENU=0xA4, KEY_RMENU=0xA5, KEY_COMMAND=0x5B, KEY_RCOMMAND=0x5C,
} sys_key_code_t;

#endif


#	ifdef CORE_IMPL


#ifdef __APPLE__
#	include "sysvideo_osx.m"
#endif
#ifdef __LINUX__
#	include "sys_video_linux.h"
#endif
#ifdef __WIN32__
#	include "sys_video_win32.h"
#endif


#	endif
#endif
