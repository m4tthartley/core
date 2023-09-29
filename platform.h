
#include "core.h"
#include "math.c"

#include <gl/gl.h>


// #include "audio.h"

// #define double __DONT_USE_DOUBLE__
// #define float __DONT_USE_FLOAT__

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

typedef BOOL (*wglSwapIntervalEXT_proc)(int interval);
typedef int (*wglGetSwapIntervalEXT_proc)(void);

// // void core_fatal_error
// void core_error(char* err, ...) {
// 	char str[1024];
// 	va_list va;
// 	va_start(va, err);
// 	vsnprintf(str, 1024, err, va);
// 	print(REDF "%s\n" RESET, str);
// 	MessageBox(NULL, str, NULL, MB_OK);
// 	va_end(va);
// }
//
// // void core_error_exit(char* err) {
// // 	core_error(err);
// // 	exit(1);
// // }
// #define core_error_exit(...)\
// 	core_error(__VA_ARGS__);\
// 	exit(1);

// char _win32_error_buffer[1024];
// char* _win32_error() {
// 	DWORD error = GetLastError();
// 	FormatMessage(
// 		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
// 		NULL,
// 		error,
// 		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
// 		(LPWSTR)_win32_error_buffer,
// 		sizeof(_win32_error_buffer),
// 		NULL);
// 	return _win32_error_buffer;
// }
// char* _win32_hresult_string(HRESULT hresult) {
// 	FormatMessage(
// 		/*FORMAT_MESSAGE_ALLOCATE_BUFFER |*/ FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
// 		NULL,
// 		hresult,
// 		0,
// 		(LPWSTR)_win32_error_buffer,
// 		sizeof(_win32_error_buffer),
// 		NULL);
// 	return _win32_error_buffer;
// }

enum {
	WINDOW_DEFAULT = (1<<0),
	WINDOW_CENTERED = (1<<1),
	WINDOW_BORDERED = (1<<2),
	WINDOW_BORDERLESS = (1<<3),
};

typedef struct {
	b32 down;
	b32 pressed;
	b32 released;
} core_button_t;

typedef struct {
} core_keyboard_t;

typedef struct {
	vec2_t pos;
	vec2_t pos_dt;
	core_button_t left;
	core_button_t right;
	int wheel_dt;
} core_mouse_t;

typedef struct {
	HWND hwnd;
	HDC hdc;
	int width;
	int height;
	b32 quit;
	core_button_t keyboard[256];
	core_mouse_t mouse;
	// f32 last_frame_time;

	// GL
	wglSwapIntervalEXT_proc wglSwapIntervalEXT;
	wglGetSwapIntervalEXT_proc wglGetSwapIntervalEXT;
} core_window_t;

LRESULT CALLBACK _core_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void _core_update_button(core_button_t *button, b32 new_state) {
	button->pressed = new_state && !button->down;
	button->released = !new_state && button->down;
	button->down = new_state;
}

void core_window(core_window_t* window, char* title, int width, int height, int flags) {
	// HINSTANCE hinstance = __ImageBase;
	WNDCLASS windowClass = {0};
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = _core_wndproc;
	// TODO: Test, apparently getting the hInstance this way can cause issues if used in a dll
	HMODULE hInstance = GetModuleHandle(NULL);
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
		core_win32_error(0, TRUE, "RegisterClassA failed");
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
		hInstance,
		0);

	if(!hwnd) {
		core_win32_error(0, TRUE, "CreateWindowA failed");
	}

	SetWindowLong(hwnd, GWL_STYLE, style);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(window->hwnd);
	window->hwnd = hwnd;
	window->hdc = GetDC(window->hwnd);
	window->width = width;
	window->height = height;
	ZeroMemory(&window->keyboard, sizeof(window->keyboard));
	printf("hwnd %#08x hdc %#08x \n", window->hwnd, window->hdc);

	RAWINPUTDEVICE mouse_raw_input;
	mouse_raw_input.usUsagePage = 1;
	mouse_raw_input.usUsage = 2;
	mouse_raw_input.dwFlags = 0;
	mouse_raw_input.hwndTarget = window->hwnd;
	if(!RegisterRawInputDevices(&mouse_raw_input, 1, sizeof(mouse_raw_input))) {
		core_win32_error(0, FALSE, "RegisterRawInputDevices failed");
	}

	window->quit = FALSE;
	SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)window);
	SetFocus(hwnd);

	return;
}

LRESULT CALLBACK _core_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	core_window_t* window = (core_window_t*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

	LRESULT result = 0;
	switch (message) {
		case WM_DESTROY: {
			// rain->quit = true;
			print("WM_DESTROY");
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
					_core_update_button(&window->mouse.left, TRUE);
				if(flags & RI_MOUSE_LEFT_BUTTON_UP)
					_core_update_button(&window->mouse.left, FALSE);
				if(flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
					_core_update_button(&window->mouse.right, TRUE);
				if(flags & RI_MOUSE_RIGHT_BUTTON_UP)
					_core_update_button(&window->mouse.right, FALSE);

				if (flags & RI_MOUSE_WHEEL) {
					SHORT w = raw.data.mouse.usButtonData;
					window->mouse.wheel_dt += (w/ WHEEL_DELTA);
				}
			}

			result = DefWindowProc(hwnd, message, wparam, lparam);
			break;
		}
		case WM_SIZE: {
			if (window) {
				RECT rect;
				// GetWindowRect(hwnd, &rect);
				GetClientRect(hwnd, &rect);
				printf("%i %i \n", rect.right - rect.left, rect.bottom - rect.top);
				window->width = rect.right - rect.left;
				window->height = rect.bottom - rect.top;

				// TODO doesn't seem to run during resize
				// _last_frame_time = core_time_raw();

				// RECT windowRect;
				// windowRect.left = 0;
				// windowRect.right = rect.right - rect.left;
				// windowRect.top = 0;
				// windowRect.bottom = rect.bottom - rect.top;
				// AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE, 0);

				// int2 diff = {
				// 	(windowRect.right - windowRect.left) - (rect.right - rect.left),
				// 	(windowRect.bottom - windowRect.top) - (rect.bottom - rect.top),
				// };

				// rain->window_width = (rect.right - rect.left) - diff.x;
				// rain->window_height = (rect.bottom - rect.top) - diff.y;
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

void core_opengl(core_window_t* window) {
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
		core_win32_error(0, FALSE, "OpenGL initialisation failed: ChoosePixelFormat");
	}
	PIXELFORMATDESCRIPTOR suggested;
	DescribePixelFormat(hdc, suggestedIndex, sizeof(PIXELFORMATDESCRIPTOR), &suggested);
	if (!SetPixelFormat(hdc, suggestedIndex, &suggested)) {
		core_win32_error(0, FALSE, "OpenGL initialisation failed: SetPixelFormat");
	}

	HGLRC glContext = wglCreateContext(hdc);
	if (!glContext) {
		core_win32_error(0, FALSE, "OpenGL initialisation failed: wglCreateContext");
	}
	if (!wglMakeCurrent(hdc, glContext)) {
		core_win32_error(0, FALSE, "OpenGL initialisation failed: wglMakeCurrent");
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
	int num_formats;
	wglChoosePixelFormatARB(hdc, format_attribs, NULL, 1, &format, &num_formats);

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};
	HGLRC context = wglCreateContextAttribsARB(hdc, 0, attribs);
	wglMakeCurrent(hdc, context);

	const char* gl_extensions = glGetString(GL_EXTENSIONS);
	// printf(gl_extensions);
	window->wglSwapIntervalEXT = (wglSwapIntervalEXT_proc)wglGetProcAddress("wglSwapIntervalEXT");
	window->wglGetSwapIntervalEXT = (wglGetSwapIntervalEXT_proc)wglGetProcAddress("wglGetSwapIntervalEXT");

	if (window->wglSwapIntervalEXT) {
		window->wglSwapIntervalEXT(1);
	}
}

void core_window_update(core_window_t* window) {
	SetWindowLongPtrA(window->hwnd, GWLP_WNDPROC, (LONG_PTR)_core_wndproc);

	// f32 time = core_time();
	// window->dt = (time - window->last_frame_time) * 1000.0f;
	// window->last_frame_time = time;

	// _window_width = window->width;
	// _window_height = window->height;

	// memset(&os->input.
	//os->input.keysLast[Message.wParam] = os->input.keys[Message.wParam];
	//memcpy(rain->input.keys_last, rain->input.keys, sizeof(rain->input.keys));
	// core_time_update(window);

	BYTE keyboard[256] = {0};
	GetKeyboardState(keyboard);
	for (int i = 0; i < 256; ++i) {
		_core_update_button(&window->keyboard[i], keyboard[i]>>7);
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

void core_opengl_swap(core_window_t* window) {
	SwapBuffers(window->hdc);
}


