//
//  platform_posix.c
//  Core
//
//  Created by Matt Hartley on 26/10/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "core.h"
#include "platform.h"

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

#define HANDLE_NULL (-1)
#define HANDLE_STDOUT STDOUT_FILENO


char* error_with_source_location(char* errstr, char* file, int line_number) {
	char* buffer = malloc(64);
	snprintf(buffer, 64, "%s (%s:%i)", errstr, file, line_number);
	return buffer;
}
#define strerror(err)\
	error_with_source_location(strerror(errno), __FILE__, __LINE__)


// Virtual memory
void* reserve_virtual_memory(size_t size) {
	return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, HANDLE_NULL, 0);
}

void* commit_virtual_memory(void* addr, size_t size) {
	int result = mprotect(addr, size, PROT_READ | PROT_WRITE);
	if (!result) {
		return addr;
	} else {
		return NULL;
	}
}

void* allocate_virtual_memory(size_t size) {
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, HANDLE_NULL, 0);
}

void free_virtual_memory(void* addr, size_t size) {
	munmap(addr, size);
}

void zero_memory(void* addr, size_t size) {
	u8* p = addr;
	u8* end = p+size;
	while(p<end) {
		*p++ = 0;
	}
}

void copy_memory(void* dest, void* src, size_t size) {
	u8* out = dest;
	u8* in = src;
	u8* end = out+size;
	while(out<end) {
		*out++ = *in++;
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

b32 sync_compare_swap32(void *ptr, int cmp, int swap) {
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
time_t system_time() {
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

char* format_time(time_t timestamp) {
	// char d[64];
	// char t[64];
	// if (!time.msec) {
	// 	GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "ddd, dd MMM yyyy", d, 64);
	// 	GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "HH:mm:ss", t, 64);
	// } else {
	// 	u64 nsec100 = time.msec * 10000;
	// 	FILETIME ft = *(FILETIME*)&nsec100;
	// 	SYSTEMTIME st;
	// 	FileTimeToSystemTime(&ft, &st);
	// 	GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "ddd, dd MMM yyyy", d, 64);
	// 	GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "HH:mm:ss", t, 64);
	// }
	// char* result = s_format("%s %s GMT", d, t);
	// return result;

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

	char* result = malloc(str_len(buffer));
	strcpy(result, buffer);
	return result;
}


// Files
#define _print_file_error()\
	print_error("%s: %s", path, strerror(errno))

file_t file_open(char* path) {
	int handle = open(path, O_RDWR);
	if(handle == HANDLE_NULL) {
		if (errno != EACCES) {
			_print_file_error();
		}
		return NULL;
	}
	return handle;
}

file_t file_create(char* path) {
	int handle = open(path, O_RDWR | O_CREAT);
	if(handle == HANDLE_NULL) {
		_print_file_error();
		return NULL;
	}
	return handle;
}

b32 file_read(file_t file, size_t offset, void* buffer, size_t size) {
	int result = pread(file, buffer, size, offset);
	if (result == -1) {
		print_error(strerror(errno));
		return FALSE;
	}
	if (result != size) {
		print_error(strerror(errno));
		return FALSE;
	}
	return result;
}

b32 file_write(file_t file, size_t offset, void* buffer, size_t size) {
	int result = pwrite(file, buffer, size, offset);
	if (result == -1) {
		print_error(strerror(errno));
		return FALSE;
	}
	if (result != size) {
		print_error(strerror(errno));
		return FALSE;
	}
	return result;
}

b32 file_truncate(file_t file, size_t size) {
    int result = ftruncate(file, size);
    if (result != 0) {
        print_error(strerror(errno));
		return FALSE;
    }
    return TRUE;
}

stat_t file_stat(file_t file) {
	stat_t result = {0};
	struct stat stats;
	int stat_result = fstat(file, &stats);
	if (stat_result == -1) {
		print_error(strerror(errno));
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
		result.is_directory = TRUE;
	}

	return result;
}

void file_close(file_t file) {
	if(file) {
		close(file);
	}
}

file_t file_open_dir(char* path) {
	int handle = open(path, O_RDONLY | O_DIRECTORY);
	if(handle == HANDLE_NULL) {
		if (errno != EACCES) {
			_print_file_error();
		}
		return NULL;
	}
	return handle;
}

file_t file_create_dir(char* path) {
	int handle = open(path, O_RDONLY | O_CREAT | O_DIRECTORY);
	if(handle == HANDLE_NULL) {
		_print_file_error();
		return NULL;
	}
	return handle;
}

int file_list_dir(char* path, b32 recursive, stat_t* output, int length) {
	int output_index = 0;

	DIR* dir = opendir(path);
	struct dirent* ent;
	while ((ent = readdir(dir))) {
		if (ent->d_type == DT_DIR) {
			if (recursive) {
				char dirpath[256];
				char* name = ent->d_name;
				snprintf(dirpath, 256, "%s/%s", path, name);
				output_index += file_list_dir(
					dirpath,
					TRUE,
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

char* file_current_dir(char* output, size_t size) {
	return getcwd(output, size);
}

void file_change_dir(char* path) {
	chdir(path);
}


// Dynamic libraries
dylib_t load_dynamic_library(char *file) {
	dylib_t lib;
	char path[256];
// #ifdef __MACOS__
// 	char* format = "%s.dylib";
// #endif
// #ifdef __LINUX__
// 	char* format = "%s.so";
// #endif

	snprintf(path, 255, file, file);

	file_t test = file_open(path);
	if (!test) print_error("Unable to open library: %s", path);
	else {
		print("Found library: %s", path);
		file_close(test);
	}

	lib.handle = dlopen(path, RTLD_LAZY);

	if (!lib.handle) {
		char* error = dlerror();
		print_error("Error loading library: %s", error);
	}

	return lib;
}

void *load_library_proc(dylib_t lib, char *proc) {
	return dlsym(lib.handle, proc);
}
