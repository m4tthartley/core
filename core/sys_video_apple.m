//
//  Created by Matt Hartley on 08/03/2025.
//  Copyright 2023 GiantJelly. All rights reserved.
//

// #include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <unistd.h>

#include "sys_video.h"


@interface SysVideoAppDelegate : NSObject <NSWindowDelegate>
@end
@implementation SysVideoAppDelegate

- (void) windowWillClose: (NSNotification*) notification {
	char* str = "Window close requested";
	write(STDOUT_FILENO, str, strlen(str));
	exit(1);
}

- (void) applicationDidFinishLaunching:(NSNotification*)notification {
	char* str = "applicationDidFinishLaunching";
	write(STDOUT_FILENO, str, strlen(str));
}

@end


CORE_VIDEO_FUNC window_t sys_init_window(char* title, int width, int height, int flags) {
	// int sys_objc_state_size = sizeof(sys_objc_state_t);
	// assert(sizeof(sys.objc_state) >= sys_objc_state_size);
	// sys_objc_state_t* state = (sys_objc_state_t*)sys.objc_state;

	window_t result = {
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
	
	NSRect frame = NSMakeRect(0, 0, width, height);
	NSWindow* window = [[
		[NSWindow alloc] initWithContentRect: frame
		styleMask: style
		backing: NSBackingStoreBuffered
		defer: NO
	] retain];
	// video.window = window;

	SysVideoAppDelegate* delegate = [[[SysVideoAppDelegate alloc] init] retain];
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
	return (window_t){0};
}
