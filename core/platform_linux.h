
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <time.h>
#include <string.h>

#define HANDLE_NULL (-1)
#define HANDLE_STDOUT STDOUT_FILENO

typedef int f_handle;
typedef int socket_t;


// Virtual memory
void* core_reserve_virtual_memory(size_t size) {
	return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, HANDLE_NULL, 0);
}

void* core_commit_virtual_memory(void* addr, size_t size) {
	int result = mprotect(addr, size, PROT_READ | PROT_WRITE);
	if (!result) {
		return addr;
	} else {
		return NULL;
	}
}

void* core_allocate_virtual_memory(size_t size) {
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, HANDLE_NULL, 0);
}

void core_free_virtual_memory(void* addr, size_t size) {
	munmap(addr, size);
}

void core_zero_memory(void* addr, size_t size) {
	u8* p = addr;
	u8* end = p+size;
	while(p<end) {
		*p++ = 0;
	}
}

void core_copy_memory(void* dest, void* src, size_t size) {
	u8* out = dest;
	u8* in = src;
	u8* end = out+size;
	while(out<end) {
		*out++ = *in++;
	}
}


// Threading and atomic operations
// TODO TryEnterCriticalSection 
// typedef struct {
// 	CRITICAL_SECTION handle;
// } core_critical_section_t;
typedef b32 core_barrier_t;
// void core_init_critical_section(core_critical_section_t* section) {
// 	InitializeCriticalSection(&section->handle);
// }

// TODO maybe core_atomic_section, enter_atomic_section, exit_atomic_section, atomic_lock
void core_barrier_start(volatile core_barrier_t* barrier) {
	while (!__sync_bool_compare_and_swap((long volatile*)barrier, TRUE, FALSE));
}

void core_barrier_end(volatile core_barrier_t* barrier) {
	__sync_lock_test_and_set((long volatile*)barrier, FALSE);
}

// void core_exit_critical_section(core_critical_section_t* section) {
// 	LeaveCriticalSection(&section->handle);
// }

int atomic_swap32(void *ptr, int swap) {
	return __sync_lock_test_and_set((long volatile*)ptr, swap);
}

b32 atomic_compare_swap32(void *ptr, int cmp, int swap) {
	return __sync_val_compare_and_swap((long volatile*)ptr, swap, cmp) == cmp;
}

int atomic_add32(void *ptr, int value) {
	return __sync_fetch_and_add((long volatile*)ptr, value);
}

int atomic_sub32(void *ptr, int value) {
	return __sync_fetch_and_sub((long volatile*)ptr, value);
}

int atomic_read32(void *ptr) {
	return __sync_fetch_and_add((long volatile*)ptr, 0);
}


// Message Box
void core_message_box(char* msg, char* caption, int type) {
	// TODO
	// Manually create message box window with X11 :(
}


// Time and Dates
timestamp_t core_system_time() {
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
	int result = gettimeofday(&systime, &zone);

	timestamp_t timestamp;
	timestamp.sec = systime.tv_sec;
	timestamp.msec = systime.tv_sec*1000 + systime.tv_usec/1000;

	return timestamp;
}

char* core_format_time(timestamp_t timestamp) {
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
	snprintf("%s, %i %s %i %i:%i:%i GMT",
		short_days[date->tm_wday],
		date->tm_mday,
		short_months[date->tm_mon],
		date->tm_hour,
		date->tm_min,
		date->tm_sec);
}


// Files
#define core_print_file_error()\
	core_error(FALSE, "%s: %s", path, strerror(errno))

f_handle f_open(char* path) {
	int handle = open(path, O_RDWR);
	if(handle == HANDLE_NULL) {
		if (errno != EACCES) {
			core_print_file_error();
		}
		return NULL;
	}
	return handle;
}

f_handle f_create(char* path) {
	int handle = open(path, O_RDWR | O_CREAT);
	if(handle == HANDLE_NULL) {
		core_print_file_error();
		return NULL;
	}
	return handle;
}

f_handle f_open_directory(char* path) {
	int handle = open(path, O_RDONLY | O_DIRECTORY);
	if(handle == HANDLE_NULL) {
		if (errno != EACCES) {
			core_print_file_error();
		}
		return NULL;
	}
	return handle;
}

f_handle f_create_directory(char* path) {
	int handle = open(path, O_RDONLY | O_CREAT | O_DIRECTORY);
	if(handle == HANDLE_NULL) {
		core_print_file_error();
		return NULL;
	}
	return handle;
}

int f_directory_list(char* path, b32 recursive, f_info* output, int length) {
// 	int output_index = 0;
// 	char* wildcard = s_format("%s/*", path);
// 	WIN32_FIND_DATAA find_data;
// 	HANDLE find_handle = FindFirstFileA(wildcard, &find_data);
// 	if (find_handle == INVALID_HANDLE_VALUE) {
// 		core_error_console(FALSE, "Failed to open directory: %s", path);
// 		return 0;
// 	}
// 	do {
// 		if (!s_compare(find_data.cFileName, ".") && !s_compare(find_data.cFileName, "..")) {
// 			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
// 				if (recursive) {
// 					output_index += f_directory_list(
// 						s_format("%s/%s", path, find_data.cFileName),
// 						TRUE,
// 						output+output_index,
// 						length-output_index);
// 				} else {
// 					f_info* file = output + output_index++;
// 					assert(s_len(find_data.cFileName) < sizeof(output->filename));
// 					strcpy(file->filename, find_data.cFileName);
// 					file->created = find_data.ftCreationTime.dwLowDateTime;
// 					file->created |= (u64)find_data.ftCreationTime.dwHighDateTime<<32;
// 					file->modified = find_data.ftLastWriteTime.dwLowDateTime;
// 					file->modified |= (u64)find_data.ftLastWriteTime.dwHighDateTime<<32;
// 					file->size = ((u64)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
// 				}
// 			} else {
// 				// char* filename = s_copy(find_data.cFileName);
// 				if (output_index < length) {
// 					f_info* file = output + output_index++;
// 					assert(s_len(find_data.cFileName) < sizeof(output->filename));
// 					strcpy(file->filename, find_data.cFileName);
// 					file->created = find_data.ftCreationTime.dwLowDateTime;
// 					file->created |= (u64)find_data.ftCreationTime.dwHighDateTime<<32;
// 					file->modified = find_data.ftLastWriteTime.dwLowDateTime;
// 					file->modified |= (u64)find_data.ftLastWriteTime.dwHighDateTime<<32;
// 					file->size = ((u64)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
// 				}
// 			}
// 		}
// 	} while (FindNextFileA(find_handle, &find_data));
// 	FindClose(find_handle);
// 	return output_index;

	int output_index = 0;

	DIR* dir = opendir(path);
	struct dirent* ent;
	while (ent = readdir(dir)) {
		if (ent->d_type == DT_DIR) {
			if (recursive) {
				char dirpath[256];
				char* name = ent->d_name;
				snprintf(dirpath, 256, "%s/%s", path, name);
				output_index += f_directory_list(
					dirpath,
					TRUE,
					output+output_index,
					length-output_index);
				continue;
			}
		}

		if (output_index < length) {
			f_info* file = output + output_index++;
			// assert(s_len(ent->d_name) < sizeof(file->filename));
			strncpy(file->filename, ent->d_name, 256);
			// file->created = find_data.ftCreationTime.dwLowDateTime;
			// file->created |= (u64)find_data.ftCreationTime.dwHighDateTime<<32;
			// file->modified = find_data.ftLastWriteTime.dwLowDateTime;
			// file->modified |= (u64)find_data.ftLastWriteTime.dwHighDateTime<<32;
			// file->size = ((u64)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
		}
	}
	closedir(dir);
}

int f_read(f_handle file, size_t offset, void* buffer, size_t size) {
	int result = pread(file, buffer, size, offset);
	if (result == -1) {
		core_error(FALSE, strerror(errno));
	}
	if (result != size) {
		core_error(FALSE, strerror(errno));
	}
	return result;
}

int f_write(f_handle file, size_t offset, void* buffer, size_t size) {
	int result = pwrite(file, buffer, size, offset);
	if (result == -1) {
		core_error(FALSE, strerror(errno));
	}
	if (result != size) {
		core_error(FALSE, strerror(errno));
	}
	return result;
}

f_info f_stat(f_handle file) {
	f_info result = {0};
	struct stat stats;
	int stat_result = fstat(file, &result);
	if (stat_result == -1) {
		core_error(FALSE, strerror(errno));
	}
	result.created = 0;
	result.modified = stats.st_mtim.tv_sec*1000 + (stats.st_mtim.tv_nsec/1000000);
	result.size = stats.st_size;
	if ((stats.st_mode & S_IFMT) == S_IFDIR) {
		result.is_directory = TRUE;
	}
}

void f_close(f_handle file) {
	if(file) {
		close(file);
	}
}

void f_change_directory(char* path) {
	chdir(path);
}
