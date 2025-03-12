//
//  Created by Matt Hartley on 22/02/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef __COREHOTRELOAD__
#define __COREHOTRELOAD__

#include <dlfcn.h>
#include <objc/runtime.h>

#include "sys.h"
#include "core.h"

typedef void (*reload_entry_point_func_t)();
typedef struct {
	char* name;
	// void* exePtr;
	void* tmpPtr;
	u32 size;
} hotreload_lib_state_t;
typedef struct {
	char* name;
	void (*func)(void* data);
} hotreload_lib_func_t;
typedef struct {
	b32 active;
	b32 init;
	u64 libModifiedTime;
	char libFilename[MAX_PATH_LENGTH];
	void* lib;
	reload_entry_point_func_t libEntryPoint;
	b32 (*reload)();
	hotreload_lib_state_t libState[64];
	int libStateCount;
	hotreload_lib_func_t libFuncs[64];
	int libFuncCount;
} hotreload_t;

#endif

#ifdef CORE_IMPL

hotreload_t hotreload = {0};

u64 _get_file_modified_time(char* filename) {
	u64 result;
	file_t file = sys_open(filename);
	if (!file) {
		print_error("Library file not found: %s", filename);
		exit(1);
	}
	stat_t stat = sys_stat(file);
	result = stat.modified;
	sys_close(file);
	return result;
}

void _reload_load_lib() {
	hotreload.lib = dlopen(hotreload.libFilename, RTLD_LOCAL | RTLD_LAZY);
	if (!hotreload.lib) {
		print_error("Library file failed to load: %s", hotreload.libFilename);
		exit(1);
	}
	// hotreload.libEntryPoint = dlsym(hotreload.lib, "main");
	// if (!hotreload.libEntryPoint) {
	// 	print_error("Entry point failed to load: main");
	// 	exit(1);
	// }

	FOR (i, hotreload.libStateCount) {
		if (hotreload.libState[i].tmpPtr) {
			void* libPtr = dlsym(hotreload.lib, hotreload.libState[i].name);
			if (!libPtr) {
				print_error("Failed to load state pointer: %s", hotreload.libState[i].name);
				exit(1);
			}
			sys_copy_memory(libPtr, hotreload.libState[i].tmpPtr, hotreload.libState[i].size);
		}
	}
}

void _reload_unload_lib() {
	FOR (i, hotreload.libStateCount) {
		void* libPtr = dlsym(hotreload.lib, hotreload.libState[i].name);
		if (!libPtr) {
			print_error("Failed to load state pointer: %s", hotreload.libState[i].name);
			exit(1);
		}
		if (!hotreload.libState[i].tmpPtr) {
			hotreload.libState[i].tmpPtr = malloc(hotreload.libState[i].size);
		}
		sys_copy_memory(hotreload.libState[i].tmpPtr, libPtr, hotreload.libState[i].size);
	}

	hotreload.libFuncCount = 0;

	if (hotreload.lib) {
		dlclose(hotreload.lib);
	}
}

void _reload_update_lib_state(void* lib) {
	void* statePtr = dlsym(lib, "hotreload");
	if (statePtr) {
		sys_copy_memory(statePtr, &hotreload, sizeof(hotreload));
	}
}

void reload_init(char* libName) {
	char_copy(hotreload.libFilename, libName, MAX_PATH_LENGTH);
	char* ext = ".so";
	if (!str_find(hotreload.libFilename, ext)) {
		char_append(hotreload.libFilename, ext, MAX_PATH_LENGTH);
	}

	hotreload.libModifiedTime = _get_file_modified_time(hotreload.libFilename);

	_reload_load_lib();
}

void reload_update() {
	u64 modifiedTime = _get_file_modified_time(hotreload.libFilename);
	if (modifiedTime > hotreload.libModifiedTime) {
		hotreload.libModifiedTime = modifiedTime;

		// RELOAD
		_reload_unload_lib();
		_reload_load_lib();
	}
}

void reload_register_state(char* name, void* ptr, u32 size) {
	assert(hotreload.libStateCount < 64);
	hotreload.libState[hotreload.libStateCount].name = name;
	// hotreload.libState[hotreload.libStateCount].exePtr = ptr;
	hotreload.libState[hotreload.libStateCount].size = size;
	++hotreload.libStateCount;
}

void reload_run_func(char* name, void* data) {
	FOR (i, hotreload.libFuncCount) {
		if (str_compare(name, hotreload.libFuncs[i].name)) {
			hotreload.libFuncs[i].func(data);
			return;
		}
	}

	void* libPtr = dlsym(hotreload.lib, name);
	if (!libPtr) {
		print_error("Failed to load state pointer: %s", name);
		exit(1);
	}

	assert(hotreload.libFuncCount < 64);
	hotreload.libFuncs[hotreload.libFuncCount].name = name;
	hotreload.libFuncs[hotreload.libFuncCount].func = libPtr;
	hotreload.libFuncs[hotreload.libFuncCount].func(data);
	++hotreload.libFuncCount;
}

// void reload_load(char* libName) {
// 	char_copy(hotreload.libFilename, libName, MAX_PATH_LENGTH);
// 	char* ext = ".so";
// 	char_append(hotreload.libFilename, ext, MAX_PATH_LENGTH);

// 	hotreload.libModifiedTime = _get_file_modified_time(hotreload.libFilename);

// 	if (hotreload.lib) {
// 		dlclose(hotreload.lib);
// 	}

// 	hotreload.lib = dlopen(hotreload.libFilename, RTLD_LOCAL | RTLD_LAZY);
// 	if (!hotreload.lib) {
// 		print_error("Library file failed to load: %s", hotreload.libFilename);
// 		exit(1);
// 	}
// 	hotreload.libEntryPoint = dlsym(hotreload.lib, "main");
// 	if (!hotreload.libEntryPoint) {
// 		print_error("Entry point failed to load: main");
// 		exit(1);
// 	}
// }

// b32 reload_reload() {
// 	u64 modifiedTime = _get_file_modified_time(hotreload.libFilename);
// 	if (modifiedTime > hotreload.libModifiedTime) {
// 		hotreload.libModifiedTime = modifiedTime;

// 		// RELOAD
// 		return TRUE;
// 	}

// 	return FALSE;
// }

// void _update_lib_state(void* lib) {
// 	void* statePtr = dlsym(lib, "hotreload");
// 	if (statePtr) {
// 		copy_memory(statePtr, &hotreload, sizeof(hotreload));
// 	}
// }

// void hotreload_run(char* libName) {
// 	reload_load(libName);
	
// 	hotreload.active = TRUE;
// 	hotreload.init = TRUE;
// 	hotreload.reload = reload_reload;
	
// 	for (;;) {
// 		_update_lib_state(hotreload.lib);
// 		hotreload.libEntryPoint();
// 		hotreload.init = FALSE;
// 		reload_load(libName);
// 	}
// }

#endif