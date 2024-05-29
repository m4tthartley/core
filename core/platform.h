//
//  platform.h
//  Core
//
//  Created by Matt Hartley on 16/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_PLATFORM_HEADER__
#define __CORE_PLATFORM_HEADER__


#define MAX_PATH_LENGTH 256


// Structures
enum {
	MESSAGE_BOX_OK = (1<<0),
	MESSAGE_BOX_YES_NO = (1<<1), // TODO
};

// typedef struct {
// 	u64 sec;
// 	u64 msec;
// } core_time_t;

// typedef u64 time_t; // Milliseconds
// typedef u64 core_time_micro_t;
// typedef u64 core_time_nano_t;

typedef struct {
	u64 created;
	u64 modified;
	size_t size;
	b32 is_directory;
	char filename[MAX_PATH_LENGTH];
} stat_t;


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
typedef b32 barrier_t;

inline void barrier_start(volatile barrier_t* barrier) {
	int num = 0;
	while (!__sync_bool_compare_and_swap((long volatile*)barrier, TRUE, FALSE)) {
		++num;
	}
	print("SPIN LOCK %i", num);
}

inline void barrier_end(volatile barrier_t* barrier) {
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
typedef void* handle_t; // HANDLE
typedef SOCKET socket_t;

char* _win32_error(DWORD error_code);

#endif


// Misc definitions
void message_box(char* msg, char* caption, int type);

// Memory definitions
void* reserve_virtual_memory(size_t size);
void* commit_virtual_memory(void* addr, size_t size);
void* allocate_virtual_memory(size_t size);
void free_virtual_memory(void* addr, size_t size);
void zero_memory(void* addr, size_t size);
void copy_memory(void* dest, void* source, size_t size);

// Atomic definitions
typedef struct {
#ifdef __WIN32__
	CRITICAL_SECTION handle;
#endif
} critical_section_t;

void init_critical_section(critical_section_t* section);
void enter_critical_section(critical_section_t* section);
void exit_critical_section(critical_section_t* section);
int sync_swap32(void *ptr, int swap);
b32 sync_compare_swap32(void *ptr, int cmp, int swap);
int sync_add32(void *ptr, int value);
int sync_sub32(void *ptr, int value);
int sync_read32(void *ptr);

// Time definitions
time_t system_time();
char* format_time(time_t time);

// File definitions
handle_t 	open_file(char* path);
handle_t 	create_file(char* path);
handle_t 	open_dir(char* path);
void 		create_dir(char* path);
int 		dir_list(char* path, b32 recursive, stat_t* output, int length);
b32 		read_file(handle_t file, size_t offset, void* output, size_t size);
b32 		write_file(handle_t file, size_t offset, void* data, size_t size);
stat_t 		stat_file(handle_t file);
void 		close_file(handle_t file);
void 		current_dir(char* output, size_t size);
void 		change_dir(char* path);

// Directory watcher definitions
typedef struct {
	u64 modified;
	char filename[MAX_PATH_LENGTH];
} file_change_t;

typedef struct watcher_thread_t {
	handle_t handle;
	char path[MAX_PATH_LENGTH];
	struct directory_watcher_t* watcher;
} watcher_thread_t;

typedef struct directory_watcher_t {
	handle_t semaphore;
	handle_t ready_event;
	watcher_thread_t threads[64];
	int directory_count;
	DWORD filter;

	file_change_t results[64];
	int result_count;
} directory_watcher_t;

b32 watch_directory_changes(directory_watcher_t* watcher, char** dir_paths, int dir_count);
int wait_for_directory_changes(directory_watcher_t* watcher, file_change_t* output, int output_size);


#endif
