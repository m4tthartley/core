//
//  Created by Matt Hartley on 26/10/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#include "sys.h"

// char* error_with_source_location(char* errstr, char* file, int line_number) {
// 	char* buffer = malloc(64);
// 	snprintf(buffer, 64, "%s (%s:%i)", errstr, file, line_number);
// 	return buffer;
// }
// #define strerror(err)\
// 	error_with_source_location(strerror(errno), __FILE__, __LINE__)


file_t logFileDescriptor = -1;


void sys_init_log(char* filename) {
	logFileDescriptor = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
}


void sys_print(char* str) {
	write(STDOUT_FILENO, str, strlen(str));
	if (logFileDescriptor != -1) {
		write(logFileDescriptor, str, strlen(str));
	}
}

void sys_print_err(char* str) {
	// int fd = open()
	char* esc_red = "\x1B[1;91m";
	char* esc_reset = "\x1B[0m";
	write(STDERR_FILENO, esc_red, strlen(esc_red));
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, esc_reset, strlen(esc_reset));
	if (logFileDescriptor != -1) {
		write(logFileDescriptor, esc_red, strlen(esc_red));
		write(logFileDescriptor, str, strlen(str));
		write(logFileDescriptor, esc_reset, strlen(esc_reset));
	}
}


// Virtual memory
CORE_MEMORY_FUNC void* sys_reserve_memory(size_t size) {
	return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

CORE_MEMORY_FUNC void* sys_commit_memory(void* addr, size_t size) {
	int result = mprotect(addr, size, PROT_READ | PROT_WRITE);
	if (!result) {
		return addr;
	} else {
		return NULL;
	}
}

CORE_MEMORY_FUNC void* sys_alloc_memory(size_t size) {
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

CORE_MEMORY_FUNC void sys_free_memory(void* addr, size_t size) {
	munmap(addr, size);
}

CORE_MEMORY_FUNC void sys_zero_memory(void* addr, size_t size) {
	uint8_t* p = addr;
	uint8_t* end = p+size;
	while(p<end) {
		*p++ = 0;
	}
}

CORE_MEMORY_FUNC void sys_copy_memory(void* dest, void* src, size_t size) {
	uint8_t* out = dest;
	uint8_t* in = src;
	uint8_t* end = out+size;
	// while(out<end) {
	// 	*out++ = *in++;
	// }
	for (size_t i=0; i<size; ++i) {
		out[i] = in[i];
	}
}


// Threading and atomic operations
// typedef b32 core_barrier_t;

// void core_barrier_start(volatile core_barrier_t* barrier) {
// 	int num = 0;
// 	while (!__sync_bool_compare_and_swap((long volatile*)barrier, TRUE, FALSE)) {
// 		++num;
// 	}
// 	printf("SPIN LOCK %i\n", num);
// }

// void core_barrier_end(volatile core_barrier_t* barrier) {
// 	__sync_lock_test_and_set((long volatile*)barrier, FALSE);
// }

int sync_swap32(void *ptr, int swap) {
	return __sync_lock_test_and_set((long volatile*)ptr, swap);
}

_Bool sync_compare_swap32(void *ptr, int cmp, int swap) {
	return __sync_val_compare_and_swap((long volatile*)ptr, swap, cmp) == cmp;
}

int sync_add32(void *ptr, int value) {
	return __sync_fetch_and_add((long volatile*)ptr, value);
}

int sync_sub32(void *ptr, int value) {
	return __sync_fetch_and_sub((long volatile*)ptr, value);
}

int sync_read32(void *ptr) {
	return __sync_fetch_and_add((long volatile*)ptr, 0);
}


// Message Box
void core_message_box(char* msg, char* caption, int type) {
	// TODO
	// Manually create message box window with X11 :(
}


// Time and Dates
CORE_TIME_FUNC time_t sys_time() {
	// number of 100-nanosecond intervals
	// FILETIME time;
	// GetSystemTimeAsFileTime(&time);
	// u64 result2 = ((u64)time.dwHighDateTime << 32) | time.dwLowDateTime;
	// u64 nsec100 = *(u64*)&time;
	// timestamp_t result = {
	// 	nsec100 / 10000
	// };
	// return result;

	struct timeval systime;
	struct timezone zone;
	int gtod = gettimeofday(&systime, &zone);

	time_t result = systime.tv_sec*1000 + systime.tv_usec/1000;
	// timestamp.sec = systime.tv_sec;
	// timestamp.msec = systime.tv_sec*1000 + systime.tv_usec/1000;

	return result;
}

CORE_TIME_FUNC char* sys_format_time(time_t timestamp) {
	char* months[] = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December",
	};

	char* short_months[] = {
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec",
	};

	char* days[] = {
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
		"Sunday",
	};
	char* short_days[] = {
		"Mon",
		"Tue",
		"Wed",
		"Thu",
		"Fri",
		"Sat",
		"Sun",
	};

	time_t t = time(NULL);
	struct tm* date = gmtime(&t);

	char buffer[64];
	snprintf(
		buffer,
		64,
		"%s, %i %s %i %i:%i:%i GMT",
		short_days[date->tm_wday],
		date->tm_mday,
		short_months[date->tm_mon],
		date->tm_year,
		date->tm_hour,
		date->tm_min,
		date->tm_sec);

	// Todo: EW
	char* result = malloc(strlen(buffer));
	strcpy(result, buffer);
	return result;
}


// Files
_Bool _sys_valid_handle(file_t file) {
	return (file != -1);
}

// #define _print_file_error()\
// 	print_error("%s: %s", path, strerror(errno))

CORE_FILE_FUNC file_t sys_open(char* path) {
	int handle = open(path, O_RDWR);
	if (handle == 0) {
		// Note: Somehow we got stdout,
		// dup new handle so 0 can be NULL
		int newFd = dup(handle);
		close(handle);
		handle = newFd;
	}
	if(!_sys_valid_handle(handle)) {
		sys_print_err(strerror(errno));
		sys_print_err(": ");
		sys_print_err(path);
		sys_print_err("\n");
		// if (errno != EACCES) {
		// }
		return 0;
	}
	return handle;
}

CORE_FILE_FUNC file_t sys_create(char* path) {
	int handle = open(path, O_RDWR | O_CREAT, 0644);
	if (handle == 0) {
		// Note: Somehow we got stdout,
		// dup new handle so 0 can be NULL
		int newFd = dup(handle);
		close(handle);
		handle = newFd;
	}
	if(!_sys_valid_handle(handle)) {
		// _print_file_error();
		sys_print_err(strerror(errno));
		return 0;
	}
	return handle;
}

CORE_FILE_FUNC size_t sys_read(file_t file, size_t offset, void* buffer, size_t size) {
	ssize_t result = pread(file, buffer, size, offset);
	if (result == -1) {
		sys_print_err(strerror(errno));
		return 0;
	}
	if (result != size) {
		sys_print_err(strerror(errno));
	}
	return result;
}

CORE_FILE_FUNC size_t sys_write(file_t file, size_t offset, void* buffer, size_t size) {
	ssize_t result = pwrite(file, buffer, size, offset);
	if (result == -1) {
		sys_print_err(strerror(errno));
		return 0;
	}
	if (result != size) {
		sys_print_err(strerror(errno));
	}
	return result;
}

CORE_FILE_FUNC _Bool sys_truncate(file_t file, size_t size) {
    int result = ftruncate(file, size);
    if (result != 0) {
        sys_print_err(strerror(errno));
		return _False;
    }
    return _True;
}

CORE_FILE_FUNC stat_t sys_stat(file_t file) {
	stat_t result = {0};
	struct stat stats;
	int stat_result = fstat(file, &stats);
	if (stat_result == -1) {
		sys_print_err(strerror(errno));
		return result;
	}
	result.created = 0;
#ifdef __MACOS__
	result.modified = stats.st_mtimespec.tv_sec*1000 + (stats.st_mtimespec.tv_nsec/1000000);
#endif
#ifdef __LINUX__
	result.modified = stats.st_mtim.tv_sec*1000 + (stats.st_mtim.tv_nsec/1000000);
#endif
	result.size = stats.st_size;
	if ((stats.st_mode & S_IFMT) == S_IFDIR) {
		result.is_directory = _True;
	}

	return result;
}

CORE_FILE_FUNC void sys_close(file_t file) {
	if(file != -1) {
		close(file);
	}
}

CORE_FILE_FUNC file_t sys_open_dir(char* path) {
	int handle = open(path, O_RDONLY | O_DIRECTORY);
	if (handle == 0) {
		// Note: Somehow we got stdout,
		// dup new handle so 0 can be NULL
		int newFd = dup(handle);
		close(handle);
		handle = newFd;
	}
	if(!_sys_valid_handle(handle)) {
		// if (errno != EACCES) {
		// 	_print_file_error();
		// }
		sys_print_err(strerror(errno));
		return 0;
	}
	return handle;
}

CORE_FILE_FUNC file_t sys_create_dir(char* path) {
	int handle = open(path, O_RDONLY | O_CREAT | O_DIRECTORY);
	if (handle == 0) {
		// Note: Somehow we got stdout,
		// dup new handle so 0 can be NULL
		int newFd = dup(handle);
		close(handle);
		handle = newFd;
	}
	if(!_sys_valid_handle(handle)) {
		// _print_file_error();
		sys_print_err(strerror(errno));
	}
	return handle;
}

CORE_FILE_FUNC int sys_list_dir(char* path, _Bool recursive, stat_t* output, int length) {
	int output_index = 0;

	DIR* dir = opendir(path);
	struct dirent* ent;
	while ((ent = readdir(dir))) {
		if (ent->d_type == DT_DIR) {
			if (recursive) {
				char dirpath[256];
				char* name = ent->d_name;
				snprintf(dirpath, 256, "%s/%s", path, name);
				output_index += sys_list_dir(
					dirpath,
					_True,
					output+output_index,
					length-output_index);
				continue;
			}
		}

		if (output_index < length) {
			stat_t* file = output + output_index++;
			// assert(s_len(ent->d_name) < sizeof(file->filename));
			strncpy(file->filename, ent->d_name, sizeof(file->filename));
			// file->created = find_data.ftCreationTime.dwLowDateTime;
			// file->created |= (u64)find_data.ftCreationTime.dwHighDateTime<<32;
			// file->modified = find_data.ftLastWriteTime.dwLowDateTime;
			// file->modified |= (u64)find_data.ftLastWriteTime.dwHighDateTime<<32;
			// file->size = ((u64)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
		}
	}
	closedir(dir);
	return output_index;
}

CORE_FILE_FUNC char* sys_current_dir(char* output, size_t size) {
	return getcwd(output, size);
}

CORE_FILE_FUNC void sys_change_dir(char* path) {
	chdir(path);
}


// Dynamic libraries
CORE_DYLIB_FUNC dylib_t sys_load_lib(char *file) {
	dylib_t lib;
	char path[256];
// #ifdef __MACOS__
// 	char* format = "%s.dylib";
// #endif
// #ifdef __LINUX__
// 	char* format = "%s.so";
// #endif

	snprintf(path, 255, file, file);

	file_t test = sys_open(path);
	if (!test) {
		// print_error("Unable to open library: %s", path);
		sys_print_err("Unable to open library: ");
		sys_print_err(path);
		sys_print_err("\n");
	} else {
		// print("Found library: %s", path);
		sys_print_err("Found library: ");
		sys_print_err(path);
		sys_print_err("\n");
		sys_close(test);
	}

	lib.handle = dlopen(path, RTLD_LAZY);

	if (!lib.handle) {
		char* error = dlerror();
		// print_error("Error loading library: %s", error);
		sys_print_err("Error loading library: ");
		sys_print_err(error);
		sys_print_err("\n");
	}

	return lib;
}

CORE_DYLIB_FUNC void *sys_load_lib_sym(dylib_t lib, char *proc) {
	return dlsym(lib.handle, proc);
}
