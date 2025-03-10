//
//  Created by Matt Hartley on 08/03/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

// #include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#include <unistd.h>

#include "sys_video.h"


#undef _True
#undef _False
#define _True ((_Bool)1)
#define _False ((_Bool)0)


void _sys_video_print(char* str) {
	write(STDOUT_FILENO, str, strlen(str));
}


@interface SysVideoAppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end
@implementation SysVideoAppDelegate

- (void) windowWillClose: (NSNotification*) notification {
	char* str = "Window close requested\n";
	write(STDOUT_FILENO, str, strlen(str));
	exit(1);
}

- (void) applicationDidFinishLaunching:(NSNotification*)notification {
	char* str = "applicationDidFinishLaunching\n";
	write(STDOUT_FILENO, str, strlen(str));
}

@end


CORE_VIDEO_FUNC sys_window_t sys_init_window(char* title, int width, int height, int flags) {
	sys_window_t result = {
		.width = width,
		.height = height,
		.flags = flags
	};

	NSApplication* app = [[NSApplication sharedApplication] retain];
	[app setActivationPolicy: NSApplicationActivationPolicyRegular];

	NSWindowStyleMask style = NSWindowStyleMaskClosable;
	if (flags & WINDOW_RESIZEABLE) {
		style |= NSWindowStyleMaskResizable;
	}
	if (!(flags & WINDOW_BORDERLESS)) {
		style |= NSWindowStyleMaskTitled;
	}

	SysVideoAppDelegate* delegate = [[[SysVideoAppDelegate alloc] init] retain];
	[app setDelegate: delegate];
	[NSApp finishLaunching];
	
	NSRect frame = NSMakeRect(0, 0, width, height);
	NSWindow* window = [[
		[NSWindow alloc] initWithContentRect: frame
		styleMask: style
		backing: NSBackingStoreBuffered
		defer: NO
	] retain];

	[window setDelegate: delegate];
	if (flags & WINDOW_CENTERED) {
		[window center];
	}
	[window makeKeyAndOrderFront: nil];
	[app activateIgnoringOtherApps: YES];

	result.sysApp = app;
	result.sysWindow = window;
	result.active = TRUE;
	return result;

init_window_err:
	return (sys_window_t){0};
}

CORE_VIDEO_FUNC void sys_poll_events(sys_window_t* win) {
	NSApplication* app = win->sysApp;
	NSWindow* window = win->sysWindow;

	_update_button(&win->modifier_keys.command, win->modifier_keys.command.down);
	_update_button(&win->modifier_keys.option, win->modifier_keys.option.down);
	_update_button(&win->modifier_keys.control, win->modifier_keys.control.down);
	_update_button(&win->modifier_keys.shift, win->modifier_keys.shift.down);

	for (int i=0; i<256; ++i) {
		_update_button(win->keyboard+i, win->keyboard[i].down);
	}

	_update_button(&win->mouse.left, win->mouse.left.down);
	_update_button(&win->mouse.right, win->mouse.right.down);
	// NSUInteger mouseButtonMask = [NSEvent pressedMouseButtons];
	// _update_button(&win->mouse.left, mouseButtonMask & (1<<0));
	// _update_button(&win->mouse.right, mouseButtonMask & (1<<1));

	// NSPoint mousePos = [NSEvent mouseLocation];
	// // mousePos = [state->window convertScreenToBase: mousePos]; // needed for old mac versions
	// mousePos = [window convertPointFromScreen: mousePos];
	// win->mouse.pos_dt.x = mousePos.x - win->mouse.pos.x;
	// win->mouse.pos_dt.y = mousePos.y - win->mouse.pos.y;
	// win->mouse.pos.x = mousePos.x;
	// win->mouse.pos.y = mousePos.y;

	win->mouse.pos_dt.x = 0;
	win->mouse.pos_dt.y = 0;
	win->mouse.wheel_dt = 0;

	NSEvent* event;
	while ((event = [app nextEventMatchingMask: NSEventMaskAny untilDate: nil inMode: NSDefaultRunLoopMode dequeue: YES])) {
		switch (event.type) {
			case NSEventTypeKeyDown:
			case NSEventTypeKeyUp: {
				_sys_video_print("key event");
				uint32_t keyCode = [event keyCode];
				assert(keyCode < 256);
				uint32_t modifierFlags = [event modifierFlags];
				uint8_t modifierKeys = 0;
				if (modifierFlags & /*NSCommandKeyMask*/ NSEventModifierFlagCommand) {
					_sys_video_print("Command modifier\n");
					modifierKeys |= SYS_MODIFIER_KEY_COMMAND;
				}
				if (modifierFlags & /*NSAlternateKeyMask*/ NSEventModifierFlagOption) {
					_sys_video_print("Option modifier\n");
					modifierKeys |= SYS_MODIFIER_KEY_OPTION;
				}
				if (modifierFlags & /*NSControlKeyMask*/ NSEventModifierFlagControl) {
					_sys_video_print("Control modifier\n");
					modifierKeys |= SYS_MODIFIER_KEY_CONTROL;
				}
				if (modifierFlags & /*NSShiftKeyMask*/ NSEventModifierFlagShift) {
					_sys_video_print("Shift modifier\n");
					modifierKeys |= SYS_MODIFIER_KEY_SHIFT;
				}

				_Bool keyDown = ([event type] == NSEventTypeKeyDown);
				_update_button(&win->keyboard[keyCode], keyDown);
				win->keyboard[keyCode].modifiers = modifierKeys;
			} break;

			case NSEventTypeLeftMouseDown:
			case NSEventTypeRightMouseDown: {
				uint32_t button = [event buttonNumber];
				if (button < 2) {
					_update_button(&win->mouse.buttons[button], _True);
				}

				[NSApp sendEvent: event];
			} break;
			case NSEventTypeLeftMouseUp:
			case NSEventTypeRightMouseUp: {
				uint32_t button = [event buttonNumber];
				if (button < 2) {
					_update_button(&win->mouse.buttons[button], _False);
				}

				[NSApp sendEvent: event];
			} break;

			case NSEventTypeMouseMoved: {
				if ([event window]) {
					NSPoint pos = [event locationInWindow];
					win->mouse.pos.x = pos.x;
					win->mouse.pos.y = pos.y;
				}
				win->mouse.pos_dt.x = [event deltaX];
				win->mouse.pos_dt.y = [event deltaY];
			} break;

			case NSEventTypeScrollWheel: {
				win->mouse.wheel_dt += [event scrollingDeltaY];
			} break;

			case /*NSFlagsChanged*/ NSEventTypeFlagsChanged: {
				_sys_video_print("NSFlagsChanged\n");
				uint32_t modifierFlags = [event modifierFlags];
				if (modifierFlags & /*NSCommandKeyMask*/ NSEventModifierFlagCommand) {
					_sys_video_print("Command modifier\n");
				}
				if (modifierFlags & /*NSAlternateKeyMask*/ NSEventModifierFlagOption) {
					_sys_video_print("Option modifier\n");
				}
				if (modifierFlags & /*NSControlKeyMask*/ NSEventModifierFlagControl) {
					_sys_video_print("Control modifier\n");
				}
				if (modifierFlags & /*NSShiftKeyMask*/ NSEventModifierFlagShift) {
					_sys_video_print("Shift modifier\n");
				}

				_update_button(&win->modifier_keys.command, modifierFlags & NSEventModifierFlagCommand);
				_update_button(&win->modifier_keys.option, modifierFlags & NSEventModifierFlagOption);
				_update_button(&win->modifier_keys.control, modifierFlags & NSEventModifierFlagControl);
				_update_button(&win->modifier_keys.shift, modifierFlags & NSEventModifierFlagShift);
			} break;

			default:
				[NSApp sendEvent: event];
		}

		// [app updateWindows];
	}
}

CORE_VIDEO_FUNC _Bool sys_message_box(char* title, char* msg, char* yesOption, char* noOption) {
	NSAlert* alert = [[NSAlert alloc] init];
	[alert setMessageText: [NSString stringWithUTF8String: title]];
	[alert setInformativeText: [NSString stringWithUTF8String: msg]];
	[alert addButtonWithTitle: [NSString stringWithUTF8String: yesOption]];
	if (noOption) {
		[alert addButtonWithTitle: [NSString stringWithUTF8String: noOption]];
	}
	[alert setAlertStyle: NSAlertStyleWarning];

	NSModalResponse result = [alert runModal];
	[alert release];

	if (result == NSAlertFirstButtonReturn) {
		return _True;
	}
	if (result == NSAlertSecondButtonReturn) {
		return _False;
	}

	return _False;
}

@interface SysVideoMetalView : NSView
@end
@implementation SysVideoMetalView

- (instancetype) initWithFrame: (NSRect) frame {
	self = [super initWithFrame: frame];
	if (self) {
		self.wantsLayer = YES;
	}
	return self;
}

@end

void _sys_init_metal_cocoa_view(sys_window_t* win) {
	NSWindow* window = win->sysWindow;

	NSRect frame = NSMakeRect(0, 0, win->width, win->height);
	SysVideoMetalView* metalView = [[[SysVideoMetalView alloc] initWithFrame: frame] retain];
	[window setContentView: metalView];
}

CORE_VIDEO_FUNC void sys_init_metal(sys_window_t* win) {
	_sys_init_metal_cocoa_view(win);

	NSWindow* window = win->sysWindow;

	id<MTLDevice> device = [MTLCreateSystemDefaultDevice() retain];
	CAMetalLayer* layer = [CAMetalLayer layer];

	layer.device = device;
	layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
	layer.framebufferOnly = YES;
	layer.frame = window.contentView.bounds;
	layer.drawableSize = window.contentView.bounds.size;

	id<MTLCommandQueue> commandQueue = [[device newCommandQueue] retain];

	win->mtlDevice = device;
	win->mtlLayer = layer;
	win->mtlCommandQueue = commandQueue;
}
