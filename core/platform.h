//
//  platform.h
//  Core
//
//  Created by Matt Hartley on 16/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_PLATFORM_HEADER__
#define __CORE_PLATFORM_HEADER__


#define CORE_MAX_PATH_LENGTH 256


// Structures
enum {
	CORE_MB_OK = (1<<0),
	CORE_MB_YES_NO = (1<<1), // TODO
};

typedef struct {
	u64 sec;
	u64 msec;
} core_time_t;

typedef struct {
	u64 created;
	u64 modified;
	size_t size;
	b32 is_directory;
	char filename[CORE_MAX_PATH_LENGTH];
} core_stat_t;


// Platform functions
// void core_zero(byte* address, int size);
// void core_copy(byte* dest, byte* src, int size);
// void core_print(char* fmt, ...);
// void core_error(b32 fatal, char* fmt, ...);
// u32 s_len(char* str);
// void s_free(char* str);
// char* s_format(char* fmt, ...);
// char* core_convert_wide_string(wchar_t* str);
// void s_copy(char* dest, char* src);
// void s_ncopy(char* dest, char* src, int n);
// b32 s_compare(char* a, char* b);


// Threading
typedef b32 core_barrier_t;

inline void core_barrier_start(volatile core_barrier_t* barrier) {
	int num = 0;
	while (!__sync_bool_compare_and_swap((long volatile*)barrier, TRUE, FALSE)) {
		++num;
	}
	core_print("SPIN LOCK %i", num);
}

inline void core_barrier_end(volatile core_barrier_t* barrier) {
	__sync_lock_test_and_set((long volatile*)barrier, FALSE);
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
typedef void* core_handle_t; // HANDLE
typedef SOCKET core_socket_t;

char* core_win32_error(DWORD error_code);

#endif


// Misc definitions
void core_message_box(char* msg, char* caption, int type);

// Memory definitions
void* core_reserve_virtual_memory(size_t size);
void* core_commit_virtual_memory(void* addr, size_t size);
void* core_allocate_virtual_memory(size_t size);
void core_free_virtual_memory(void* addr, size_t size);
void core_zero_memory(void* addr, size_t size);
void core_copy_memory(void* dest, void* source, size_t size);

// Atomic definitions
typedef struct {
#ifdef __WIN32__
	CRITICAL_SECTION handle;
#endif
} core_critical_section_t;

void core_init_critical_section(core_critical_section_t* section);
void core_enter_critical_section(core_critical_section_t* section);
void core_exit_critical_section(core_critical_section_t* section);
int core_swap32(void *ptr, int swap);
b32 core_compare_swap32(void *ptr, int cmp, int swap);
int core_add32(void *ptr, int value);
int core_sub32(void *ptr, int value);
int core_read32(void *ptr);

// Time definitions
core_time_t core_system_time();
char* core_format_time(core_time_t time);

// File definitions
core_handle_t core_open(char* path);
core_handle_t core_create(char* path);
core_handle_t core_open_dir(char* path);
void core_create_dir(char* path);
int core_dir_list(char* path, b32 recursive, core_stat_t* output, int length);
b32 core_read(core_handle_t file, size_t offset, void* output, size_t size);
b32 core_write(core_handle_t file, size_t offset, void* data, size_t size);
core_stat_t core_stat(core_handle_t file);
void core_close(core_handle_t file);
void core_current_dir(char* output, size_t size);
void core_change_dir(char* path);

// Directory watcher definitions
typedef struct {
	core_handle_t handle;
	char path[CORE_MAX_PATH_LENGTH];
	struct core_directory_watcher_t* watcher;
} core_watcher_thread_t;

typedef struct {
	core_handle_t semaphore;
	core_handle_t ready_event;
	core_watcher_thread_t threads[64];
	int directory_count;
	DWORD filter;

	core_stat_t results[64];
	int result_count;
} core_directory_watcher_t;

b32 core_watch_directory_changes(core_directory_watcher_t* watcher, char** dir_paths, int dir_count);
int core_wait_for_directory_changes(core_directory_watcher_t* watcher, core_stat_t* output, int output_size);


#endif
