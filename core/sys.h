//
//  Created by Matt Hartley on 16/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//


#ifndef __CORE_SYSTEM_HEADER__
#define __CORE_SYSTEM_HEADER__


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "targetconditionals.h"

#define CORE_SYSTEM_FUNC
#define CORE_MEMORY_FUNC
#define CORE_FILE_FUNC
#define CORE_DYLIB_FUNC
#define CORE_TIME_FUNC

#undef _True
#undef _False
#define _True ((_Bool)1)
#define _False ((_Bool)0)

#define MAX_PATH_LENGTH 256


enum {
	MESSAGE_BOX_OK = (1<<0),
	MESSAGE_BOX_YES_NO = (1<<1), // TODO
};

typedef struct {
	uint64_t created;
	uint64_t modified;
	size_t size;
	_Bool is_directory;
	char filename[MAX_PATH_LENGTH];
} stat_t;

typedef struct {
	stat_t stat;
	uint8_t data[];
} file_data_t;


void sys_print(char* str);
void sys_print_err(char* str);


// Threading
typedef _Bool thread_barrier_t;

inline void thread_lock_barrier(volatile thread_barrier_t* barrier) {
	int num = 0;
	// Todo: what size are these working with?
	while (!__sync_bool_compare_and_swap((long volatile*)barrier, _True, _False)) {
		++num;
	}
}

inline void thread_unlock_barrier(volatile thread_barrier_t* barrier) {
	__sync_lock_test_and_set((long volatile*)barrier, _False);
}


#ifdef __WIN32__

#	include <winsock2.h>
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
// #include <mmeapi.h>

#	define HANDLE_NULL INVALID_HANDLE_VALUE
#	define HANDLE_STDOUT stdout
// #define INVALID_SOCKET  (SOCKET)(~0)
// #define SOCKET_ERROR            (-1)
typedef void* file_t; // HANDLE
typedef SOCKET socket_t;

char* _win32_error(DWORD error_code);

#endif
#ifdef __LINUX__
typedef int file_t;
typedef int socket_t;
#endif
#ifdef __MACOS__
typedef int file_t;
typedef int socket_t;
#endif


// Misc definitions
void message_box(char* msg, char* caption, int type);


// MEMORY
CORE_MEMORY_FUNC void* sys_reserve_memory(size_t size);
CORE_MEMORY_FUNC void* sys_commit_memory(void* addr, size_t size);
CORE_MEMORY_FUNC void* sys_alloc_memory(size_t size);
CORE_MEMORY_FUNC void sys_free_memory(void* addr, size_t size);
CORE_MEMORY_FUNC void sys_zero_memory(void* addr, size_t size);
CORE_MEMORY_FUNC void sys_copy_memory(void* dest, void* src, size_t size);


// ATOMICS
typedef struct {
#ifdef __WIN32__
	CRITICAL_SECTION handle;
#endif
} critical_section_t;

void init_critical_section(critical_section_t* section);
void enter_critical_section(critical_section_t* section);
void exit_critical_section(critical_section_t* section);
int sync_swap32(void *ptr, int swap);
_Bool sync_compare_swap32(void *ptr, int cmp, int swap);
int sync_add32(void *ptr, int value);
int sync_sub32(void *ptr, int value);
int sync_read32(void *ptr);


// TIME
CORE_TIME_FUNC time_t sys_time();
CORE_TIME_FUNC char* sys_format_time(time_t timestamp);


// FILES
CORE_FILE_FUNC file_t	sys_open(char* path);
CORE_FILE_FUNC file_t	sys_create(char* path);
CORE_FILE_FUNC size_t	sys_read(file_t file, size_t offset, void* buffer, size_t size);
CORE_FILE_FUNC size_t	sys_write(file_t file, size_t offset, void* buffer, size_t size);
CORE_FILE_FUNC _Bool	sys_truncate(file_t file, size_t size);
CORE_FILE_FUNC stat_t	sys_stat(file_t file);
CORE_FILE_FUNC void		sys_close(file_t file);
CORE_FILE_FUNC file_t	sys_open_dir(char* path);
CORE_FILE_FUNC file_t	sys_create_dir(char* path);
CORE_FILE_FUNC int		sys_list_dir(char* path, _Bool recursive, stat_t* output, int length);
CORE_FILE_FUNC void		sys_current_dir(char* output, size_t size);
CORE_FILE_FUNC void		sys_change_dir(char* path);


// Dynamic libraries
typedef struct {
#if defined(__WIN32__)
	HMODULE handle;
#elif defined(__MACOS__)
	void* handle;
#endif
} dylib_t;

CORE_DYLIB_FUNC dylib_t sys_load_lib(char *file);
CORE_DYLIB_FUNC void *sys_load_lib_sym(dylib_t lib, char *proc);

// Directory watcher definitions
#ifdef __WIN32__
typedef struct {
	uint64_t modified;
	char filename[MAX_PATH_LENGTH];
} file_change_t;

typedef struct watcher_thread_t {
	file_t handle;
	char path[MAX_PATH_LENGTH];
	struct directory_watcher_t* watcher;
} watcher_thread_t;

typedef struct directory_watcher_t {
	file_t semaphore;
	file_t ready_event;
	watcher_thread_t threads[64];
	int directory_count;
	DWORD filter;

	file_change_t results[64];
	int result_count;
} directory_watcher_t;

_Bool watch_directory_changes(directory_watcher_t* watcher, char** dir_paths, int dir_count);
int wait_for_directory_changes(directory_watcher_t* watcher, file_change_t* output, int output_size);
#endif


// Bundles
_Bool sys_get_bundle_path(char* buffer, int bufferSize, char* filename);
_Bool sys_get_resource_dir(char* buffer, int bufferSize);
_Bool sys_get_resource_path(char* buffer, int bufferSize, char* resourceFile);


#	ifdef CORE_IMPL


#ifdef __POSIX__
#	include "sys_posix.h"
#endif
#ifdef __APPLE__
#	include "sys_apple.h"
#endif
// #ifdef __LINUX__
// #	include "system_linux.h"
// #endif
#ifdef __WIN32__
#	include "sys_win32.h"
#endif


#	endif
#endif
