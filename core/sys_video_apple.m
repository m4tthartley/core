//
//  Created by Matt Hartley on 08/03/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

// #include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
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
	// int sys_objc_state_size = sizeof(sys_objc_state_t);
	// assert(sizeof(sys.objc_state) >= sys_objc_state_size);
	// sys_objc_state_t* state = (sys_objc_state_t*)sys.objc_state;

	sys_window_t result = {
		.width = width,
		.height = height,
		.flags = flags
	};

	NSApplication* app = [[NSApplication sharedApplication] retain];
	[app setActivationPolicy: NSApplicationActivationPolicyRegular];
	// AppDelegate* delegate = [[AppDelegate alloc] init];
	// [video.app setDelegate: delegate];

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
	// video.window = window;

	[window setDelegate: delegate];
	// NSRect frame = NSMakeRect(0, 0, video.screenSize.x, video.screenSize.y);
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

	// _update_button(&win->mouse.left, win->mouse.left.down);
	// _update_button(&win->mouse.right, win->mouse.right.down);
	NSUInteger mouseButtonMask = [NSEvent pressedMouseButtons];
	_update_button(&win->mouse.left, mouseButtonMask & (1<<0));
	_update_button(&win->mouse.right, mouseButtonMask & (1<<1));

	NSPoint mousePos = [NSEvent mouseLocation];
	// mousePos = [state->window convertScreenToBase: mousePos]; // needed for old mac versions
	mousePos = [window convertPointFromScreen: mousePos];
	win->mouse.pos_dt.x = mousePos.x - win->mouse.pos.x;
	win->mouse.pos_dt.y = mousePos.y - win->mouse.pos.y;
	win->mouse.pos.x = mousePos.x;
	win->mouse.pos.y = mousePos.y;
	win->mouse.wheel_dt = 0;

	NSEvent* event;
	while ((event = [app nextEventMatchingMask: NSEventMaskAny untilDate: nil inMode: NSDefaultRunLoopMode dequeue: YES])) {
		// print("event %i", event.type);

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

			// case NSEventTypeLeftMouseDown: {
			// 	_update_button(&win->mouse.left, _True);
			// } break;
			// case NSEventTypeLeftMouseUp: {
			// 	_update_button(&win->mouse.left, _False);
			// } break;
			// case NSEventTypeRightMouseDown: {
			// 	_update_button(&win->mouse.right, _True);
			// } break;
			// case NSEventTypeRightMouseUp: {
			// 	_update_button(&win->mouse.right, _False);
			// } break;

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

		

		// if (event.type == NSEventTypeApplicationDefined) {
		// 	exit(1);
		// }

		// if (event.type == NSEventTypeKeyDown) {
		// 	assert(event.keyCode < 256);
		// 	_update_button(&win->keyboard[event.keyCode], TRUE);
		// }
		// if (event.type == NSEventTypeKeyUp) {
		// 	assert(event.keyCode < 256);
		// 	_update_button(&win->keyboard[event.keyCode], FALSE);
		// }

		// if (event.type == NSEventTypeLeftMouseDown) {
		// 	_update_button(&win->mouse.left, TRUE);
		// }
		// if (event.type == NSEventTypeLeftMouseUp) {
		// 	_update_button(&win->mouse.left, FALSE);
		// }
		// if (event.type == NSEventTypeRightMouseDown) {
		// 	_update_button(&win->mouse.left, TRUE);
		// }
		// if (event.type == NSEventTypeRightMouseUp) {
		// 	_update_button(&win->mouse.left, FALSE);
		// }
		
		// [app sendEvent: event];
		// [app updateWindows];
	}
}
