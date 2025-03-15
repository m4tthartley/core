//
//  platform_macos.c
//  Core
//
//  Created by Matt Hartley on 29/05/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <Carbon/Carbon.h>
#include <stdio.h>

#include "sys.h"


// Dynamic libraries
// dylib_t load_dynamic_library(char *file) {
// 	dylib_t lib;
// 	char path[256];
// 	snprintf(path, 255, "%s.dylib", file);
// 	lib.handle = dlopen(path, RTLD_LAZY);
// 	return lib;
// }

// void *load_library_proc(dylib_t lib, char *proc) {
// 	return dlsym(lib.handle, proc);
// }

_Bool sys_get_bundle_path(char* buffer, int bufferSize, char* filename) {
	CFBundleRef bundle = CFBundleGetMainBundle();
	if (!bundle) {
		return _False;
	}
	CFURLRef bundleUrl = CFBundleCopyBundleURL(bundle);
	if (!bundleUrl) {
		return _False;
	}
	
	Boolean result = CFURLGetFileSystemRepresentation(bundleUrl, true, (UInt8*)buffer, bufferSize);
	if (!result) {
		perror("Error getting bundle path");
		return _False;
	}
	CFRelease(bundleUrl);

	strncat(buffer, "/", bufferSize);
	strncat(buffer, filename, bufferSize);

	return _True;
}

// _Bool sys_get_bundle_file(char* buffer, int bufferSize, char* filename) {
// 	sys_get_bundle_path(buffer, bufferSize);
// 	strcat(buffer, filename);
// }

_Bool sys_get_resource_path(char* buffer, int bufferSize, char* resourceFile) {
	CFBundleRef bundle = CFBundleGetMainBundle();
	if (!bundle) {
		return _False;
	}

	CFStringRef cfResourceFile = CFStringCreateWithCString(NULL, resourceFile, kCFStringEncodingASCII);
	CFURLRef bundleUrl = CFBundleCopyResourceURL(bundle, cfResourceFile, NULL, NULL);
	CFRelease(cfResourceFile);
	if (!bundleUrl) {
		return _False;
	}
	
	Boolean result = CFURLGetFileSystemRepresentation(bundleUrl, true, (UInt8*)buffer, bufferSize);
	if (!result) {
		perror("Error getting bundle path");
		return _False;
	}
	CFRelease(bundleUrl);

	return _True;
}
