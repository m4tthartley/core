//
//  video_win32.c
//  Core
//
//  Created by Matt Hartley on 26/10/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "core.h"
#include "sysvideo.h"

// #define CORE_IMPL
#include "glex.h"


// #include "audio.h"

// #define double __DONT_USE_DOUBLE__
// #define float __DONT_USE_FLOAT__

LRESULT CALLBACK _core_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// void _update_button(button_t *button, b32 new_state) {
// 	button->pressed = new_state && !button->down;
// 	button->released = !new_state && button->down;
// 	button->down = new_state;
// }

void _print_win32_error(char* msg) {
	char* err = _win32_error(NULL);
	sys_print_err(msg);
	sys_print_err(": ");
	sys_print_err(err);
	sys_print_err("\n");
	LocalFree(err);
}

_Bool sys_init_window(sys_window_t* window, char* title, int width, int height, int flags) {
	*window = (sys_window_t){0};
	// HINSTANCE hinstance = __ImageBase;
	WNDCLASS windowClass = {0};
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = _core_wndproc;
	// TODO: Test, apparently getting the hInstance this way can cause issues if used in a dll
	HMODULE hinstance = GetModuleHandle(NULL);
	if (!hinstance) {
		_print_win32_error("GetModuleHandle");
		return FALSE;
	}
	// windowClass.hInstance = hInstance;
	windowClass.lpszClassName = "CoreWindowClass";
	windowClass.hCursor = LoadCursor(0, IDC_ARROW);

	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	if(flags & WINDOW_BORDERLESS) {
		// style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN;
		style = WS_VISIBLE;
	}

	RECT windowRect;
	windowRect.left = 0;
	windowRect.right = width;
	windowRect.top = 0;
	windowRect.bottom = height;
	// if(!(flags & WINDOW_BORDERLESS)) {
		AdjustWindowRectEx(&windowRect, style, FALSE, 0);
	// }

	if(!RegisterClassA(&windowClass)) {
		_print_win32_error("RegisterClassA");
		return FALSE;
	}

	POINTS adjusted = {
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
	};
	POINTS monitor_size = {
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
	};
	int px = CW_USEDEFAULT;
	int py = CW_USEDEFAULT;
	if(flags & WINDOW_CENTERED) {
		px = (monitor_size.x - adjusted.x) / 2;
		py = (monitor_size.y - adjusted.y) / 2;
	}
	HWND hwnd = CreateWindowExA(
		0,
		windowClass.lpszClassName,
		title,
		0,
		px, py,
		adjusted.x,
		adjusted.y,
		0,
		0,
		hinstance,
		0);

	if(!hwnd) {
		_print_win32_error("CreateWindowExA");
		return FALSE;
	}

	SetWindowLong(hwnd, GWL_STYLE, style);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(window->hwnd);
	window->hwnd = hwnd;
	window->hdc = GetDC(window->hwnd);
	window->width = width;
	window->height = height;
    window->flags = flags;
	ZeroMemory(&window->keyboard, sizeof(window->keyboard));
	// printf("hwnd %#08x hdc %#08x \n", window->hwnd, window->hdc);

	RAWINPUTDEVICE mouse_raw_input;
	mouse_raw_input.usUsagePage = 1;
	mouse_raw_input.usUsage = 2;
	mouse_raw_input.dwFlags = 0;
	mouse_raw_input.hwndTarget = window->hwnd;
	if(!RegisterRawInputDevices(&mouse_raw_input, 1, sizeof(mouse_raw_input))) {
		_print_win32_error("RegisterRawInputDevices");
	}

	window->quit = FALSE;
	SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window);
	SetFocus(hwnd);

	return TRUE;
}

LRESULT CALLBACK _core_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	sys_window_t* window = (sys_window_t*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

	LRESULT result = 0;
	switch (message) {
		case WM_DESTROY: {
			exit(0);
		} break;
		case WM_INPUT: {
			RAWINPUT raw = {0};
			UINT cbsize = sizeof(raw);
			int x = GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &raw, &cbsize, sizeof(RAWINPUTHEADER));
			int y = 0;
			if (raw.header.dwType == RIM_TYPEMOUSE && raw.data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
				window->mouse.pos_dt.x += raw.data.mouse.lLastX;
				window->mouse.pos_dt.y += raw.data.mouse.lLastY;

				USHORT flags = raw.data.mouse.usButtonFlags;
				if(flags & RI_MOUSE_LEFT_BUTTON_DOWN)
					_update_button(&window->mouse.left, TRUE);
				if(flags & RI_MOUSE_LEFT_BUTTON_UP)
					_update_button(&window->mouse.left, FALSE);
				if(flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
					_update_button(&window->mouse.right, TRUE);
				if(flags & RI_MOUSE_RIGHT_BUTTON_UP)
					_update_button(&window->mouse.right, FALSE);

				if (flags & RI_MOUSE_WHEEL) {
					SHORT w = raw.data.mouse.usButtonData;
					window->mouse.wheel_dt += ((float)w / WHEEL_DELTA);
				}
			}

			result = DefWindowProc(hwnd, message, wparam, lparam);
			break;
		}
		case WM_SIZE: {
			if (window) {
				RECT rect;
				GetClientRect(hwnd, &rect);
				window->width = rect.right - rect.left;
				window->height = rect.bottom - rect.top;
			}
			break;
		}
		case WM_MOUSEMOVE: {
			POINTS p = MAKEPOINTS(lparam);
			window->mouse.pos.x = p.x;
			window->mouse.pos.y = p.y;
			break;
		}
		default: {
			result = DefWindowProc(hwnd, message, wparam, lparam);
		} break;
	}
	return result;
}

// TODO: Check this functionality is equivalent to newer OSX layer
void sys_poll_events(sys_window_t* window) {
	SetWindowLongPtrA(window->hwnd, GWLP_WNDPROC, (LONG_PTR)_core_wndproc);

	BYTE keyboard[256] = {0};
	GetKeyboardState(keyboard);
	for (int i = 0; i < 256; ++i) {
		_update_button(&window->keyboard[i], keyboard[i]>>7);
	}

	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(window->hwnd, &mouse);
	window->mouse.pos.x = mouse.x;
	window->mouse.pos.y = mouse.y;

	window->mouse.pos_dt.x = 0;
	window->mouse.pos_dt.y = 0;
	window->mouse.wheel_dt = 0;

	window->mouse.left.pressed = FALSE;
	window->mouse.left.released = FALSE;
	window->mouse.right.pressed = FALSE;
	window->mouse.right.released = FALSE;

	MSG Message;
	while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE)) {
		switch (Message.message) {
			default:
			TranslateMessage(&Message);
			DispatchMessageA(&Message);
		}
	}
}

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042

#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

typedef HGLRC WINAPI wglCreateContextAttribsARB_proc(HDC hDC, HGLRC hShareContext, const int *attribList);
wglCreateContextAttribsARB_proc *wglCreateContextAttribsARB;
typedef BOOL WINAPI wglChoosePixelFormatARB_proc(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_proc *wglChoosePixelFormatARB;

void sys_init_opengl(sys_window_t* window) {
	HDC hdc = window->hdc;
	PIXELFORMATDESCRIPTOR pixelFormat = {0};
	pixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelFormat.nVersion = 1;
	pixelFormat.iPixelType = PFD_TYPE_RGBA;
	pixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pixelFormat.cColorBits = 32;
	pixelFormat.cAlphaBits = 8;
	pixelFormat.iLayerType = PFD_MAIN_PLANE;

	int suggestedIndex = ChoosePixelFormat(hdc, &pixelFormat);
	if (!suggestedIndex) {
		_print_win32_error("ChoosePixelFormat");
		goto err;
	}
	PIXELFORMATDESCRIPTOR suggested;
	DescribePixelFormat(hdc, suggestedIndex, sizeof(PIXELFORMATDESCRIPTOR), &suggested);
	if (!SetPixelFormat(hdc, suggestedIndex, &suggested)) {
		_print_win32_error("SetPixelFormat");
		goto err;
	}

	HGLRC glContext = wglCreateContext(hdc);
	if (!glContext) {
		_print_win32_error("wglCreateContext");
		goto err;
	}
	if (!wglMakeCurrent(hdc, glContext)) {
		_print_win32_error("wglMakeCurrent");
		goto err;
	}

	// Upgrade to extended context
	wglCreateContextAttribsARB = (wglCreateContextAttribsARB_proc*)wglGetProcAddress("wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (wglChoosePixelFormatARB_proc*)wglGetProcAddress("wglChoosePixelFormatARB");
	wglDeleteContext(glContext);

	int format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,

		/*WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 8,*/
		0,
	};
	int format;
	UINT num_formats;
	BOOL choose_format_result = wglChoosePixelFormatARB(hdc, format_attribs, NULL, 1, &format, &num_formats);
	if (!choose_format_result) {
		_print_win32_error("wglChoosePixelFormatARB");
		goto err;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};
	HGLRC context = wglCreateContextAttribsARB(hdc, 0, attribs);
	if (!context) {
		_print_win32_error("wglCreateContextAttribsARB");
		goto err;
	}

	BOOL make_current_result = wglMakeCurrent(hdc, context);
	if (!make_current_result) {
		_print_win32_error("wglMakeCurrent");
		goto err;
	}

	const GLubyte* gl_extensions = glGetString(GL_EXTENSIONS);

	window->wglSwapIntervalEXT = (wglSwapIntervalEXT_proc)wglGetProcAddress("wglSwapIntervalEXT");
	window->wglGetSwapIntervalEXT = (wglGetSwapIntervalEXT_proc)wglGetProcAddress("wglGetSwapIntervalEXT");

	if (window->wglSwapIntervalEXT) {
		window->wglSwapIntervalEXT(1);
	}

	_load_opengl_extensions();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	window->fbWidth = viewport[2];
	window->fbHeight = viewport[3];
	glViewport(0, 0, window->fbWidth, window->fbHeight);

	return;

err:
	sys_print_err("Failed to initialize OpenGL, exiting... \n");
	exit(1);
}

void sys_present_opengl(sys_window_t* window) {
	SwapBuffers(window->hdc);
}

