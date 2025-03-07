//
//  platform_macos.c
//  Core
//
//  Created by Matt Hartley on 29/05/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#include <dlfcn.h>

#include "system.h"


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