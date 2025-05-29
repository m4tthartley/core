//
//  Created by Matt Hartley on 26/10/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//


#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "core.h"
// #include "platform.h"
#include "sys.h"


#define _strcmp strcmp
#define _strlen strlen
#define _strncpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
// (size)-1
#define _strncat(dst, src, size) strncat_s(dst, size, src, _TRUNCATE)

void _wide_to_char(char* dest, wchar_t* str, int n) {
	int wlen = 0;
	while (wlen < (n-1) && str[wlen]) wlen++;

	// core_string_t result = _allocate_string(wlen);
	for(int i=0; i<wlen+1; ++i) {
		dest[i] = str[i];
	}
}

file_t logFileDescriptor = INVALID_HANDLE_VALUE;


void sys_init_log(char* filename) {
	logFileDescriptor = CreateFileA(filename, FILE_APPEND_DATA , FILE_SHARE_READ,
								0, CREATE_ALWAYS, 0, 0);
}


void sys_print(char* str) {
	DWORD bytesWritten;
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), str, strlen(str), &bytesWritten, NULL);
	if (logFileDescriptor != INVALID_HANDLE_VALUE) {
		WriteFile(logFileDescriptor, str, strlen(str), &bytesWritten, NULL);
	}
}

void sys_print_err(char* str) {
	DWORD bytesWritten;
	char* esc_red = "\x1B[1;91m";
	char* esc_reset = "\x1B[0m";
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), esc_red, strlen(esc_red), &bytesWritten, NULL);
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), str, strlen(str), &bytesWritten, NULL);
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), esc_reset, strlen(esc_reset), &bytesWritten, NULL);
	if (logFileDescriptor != INVALID_HANDLE_VALUE) {
		WriteFile(logFileDescriptor, esc_red, strlen(esc_red), &bytesWritten, NULL);
		WriteFile(logFileDescriptor, str, strlen(str), &bytesWritten, NULL);
		WriteFile(logFileDescriptor, esc_reset, strlen(esc_reset), &bytesWritten, NULL);
	}
}


// Errors
// This needs to be freed manually with LocalFree, if you care
char* _win32_error(DWORD error_code) {
	if (!error_code) {
		error_code = GetLastError();
	}

	char* msg;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&msg,
		0,
		NULL);

	// char str[1024];
	// core_print_to_buffer(str, 1024, "%s\n%#08X %s\n", usrstr, error_code, msg);

	return msg;
}

void message_box(char* msg, char* caption, int type) {
	MessageBox(NULL, msg, caption, MB_OK);
}


// Virtual memory
void* sys_reserve_memory(size_t size) {
	return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

void* sys_commit_memory(void* addr, size_t size) {
	return VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
}

void* sys_alloc_memory(size_t size) {
	return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void sys_free_memory(void* addr, size_t size) {
	VirtualFree(addr, size, MEM_RELEASE);
}

void sys_zero_memory(void* addr, size_t size) {
	ZeroMemory(addr, size);
}

void sys_copy_memory(void* dest, void* source, size_t size) {
	CopyMemory(dest, source, size);
}


// Threading and atomic operations
// TODO TryEnterCriticalSection 
void init_critical_section(critical_section_t* section) {
	InitializeCriticalSection(&section->handle);
}

// TODO maybe core_atomic_section, enter_atomic_section, exit_atomic_section, atomic_lock
void enter_critical_section(critical_section_t* section) {
	// if (!) {
		// TODO I assume this is unsafe as you could have a 
		// race condition on the initialization?
		// InitializeCriticalSection(&section->win32_section);
	// }
	EnterCriticalSection(&section->handle);
}

void exit_critical_section(critical_section_t* section) {
	LeaveCriticalSection(&section->handle);
}

int sync_swap32(void *ptr, int swap) {
	return _InterlockedExchange((long volatile*)ptr, swap);
}

_Bool sync_compare_swap32(void *ptr, int cmp, int swap) {
	return _InterlockedCompareExchange((long volatile*)ptr, swap, cmp) == cmp;
}

int sync_add32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, value);
}

int sync_sub32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, -value);
}

int sync_read32(void *ptr) {
	return _InterlockedExchangeAdd((long volatile*)ptr, 0);
}


// Time and Dates
time_t sys_time() {
	// number of 100-nanosecond intervals
	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	uint64_t result2 = ((uint64_t)time.dwHighDateTime << 32) | time.dwLowDateTime;
	uint64_t nsec100 = *(uint64_t*)&time;
	time_t result = nsec100/10000;
	// result.sec = nsec100 / 10000000;
	// result.msec = (nsec100/10000) - (result.sec*1000);
	return result;
}

char* sys_format_time(time_t time) {
	char d[64];
	char t[64];
	if (!time) {
		GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "ddd, dd MMM yyyy", d, 64);
		GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "HH:mm:ss", t, 64);
	} else {
		uint64_t nsec100 = time * 10000;
		FILETIME ft = *(FILETIME*)&nsec100;
		SYSTEMTIME st;
		FileTimeToSystemTime(&ft, &st);
		GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "ddd, dd MMM yyyy", d, 64);
		GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "HH:mm:ss", t, 64);
	}

	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%s %s GMT", d, t);
	char* result = malloc(strlen(buffer));
	strncpy_s(result, sizeof(buffer), buffer, sizeof(buffer)-1);

	return result;
}


// Files
file_t sys_open(char* path) {
	assert(sizeof(HANDLE)<=sizeof(file_t));

	// TODO: do this for all functions
	// NOTE: Why?
	// if (str_len(path) >= MAX_PATH) {
	// 	print_err("Sorry, we don't support paths longer than %i", (int)MAX_PATH);
	// 	return NULL;
	// }
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, OPEN_EXISTING, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		// if (error != ERROR_FILE_NOT_FOUND) {
		// }
		char* err = _win32_error(error);
		sys_print_err(err);
		sys_print_err(": ");
		sys_print_err(path);
		sys_print_err("\n");
		LocalFree(err);
		return NULL;
	}
	return handle;
}

file_t sys_create(char* path) {
	assert(sizeof(HANDLE)<=sizeof(file_t));
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, OPEN_ALWAYS, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		char* err = _win32_error(0);
		sys_print_err(err);
		sys_print_err(": ");
		sys_print_err(path);
		sys_print_err("\n");
		LocalFree(err);
		return NULL;
	}
	return handle;
}

size_t sys_read(file_t file, size_t offset, void* output, size_t size) {
	DWORD bytesRead;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = ReadFile(file, output, size, &bytesRead, &overlapped);
	if(!result || bytesRead!=size) {
		char path[MAX_PATH_LENGTH];
		GetFinalPathNameByHandleA(file, path, MAX_PATH_LENGTH, FILE_NAME_OPENED);
		char* err = _win32_error(0);
		sys_print_err(err);
		sys_print_err(": ");
		sys_print_err(path);
		sys_print_err("\n");
		LocalFree(err);
	}

	return bytesRead;
}

size_t sys_write(file_t file, size_t offset, void* data, size_t size) {
	DWORD bytesWritten;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = WriteFile(file, data, size, &bytesWritten, &overlapped);
	if(!result || bytesWritten!=size) {
		char path[MAX_PATH_LENGTH];
		GetFinalPathNameByHandleA(file, path, MAX_PATH_LENGTH, FILE_NAME_OPENED);
		char* err = _win32_error(0);
		sys_print_err(err);
		sys_print_err(": ");
		sys_print_err(path);
		sys_print_err("\n");
		LocalFree(err);
	}

	return bytesWritten;
}

_Bool sys_truncate(file_t file, size_t size) {
	LARGE_INTEGER lsize;
	lsize.QuadPart = size;

	if (!SetFilePointerEx(file, lsize, NULL, FILE_BEGIN)) {
		sys_print_err("sys_truncate(): SetFilePointerEx failed \n");
		return _False;
	}
	if (!SetEndOfFile(file)) {
		sys_print_err("sys_truncate(): SetEndOfFile failed \n");
		return _False;
	}
	
    return _True;
}

stat_t sys_stat(file_t file) {
	stat_t result = {0};
	BY_HANDLE_FILE_INFORMATION info = {0};
	if(GetFileInformationByHandle(file, &info)) {
		// FILETIME is the number of 100-nanosecond intervals
		// I'm storing file times in milliseconds
		result.created = info.ftCreationTime.dwLowDateTime;
		result.created |= (uint64_t)info.ftCreationTime.dwHighDateTime<<32;
		result.created /= 10000;
		result.modified = info.ftLastWriteTime.dwLowDateTime;
		result.modified |= (uint64_t)info.ftLastWriteTime.dwHighDateTime<<32;
		result.modified /= 10000;
		result.size = info.nFileSizeLow;
		result.is_directory = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	} else {
		char path[MAX_PATH_LENGTH];
		GetFinalPathNameByHandleA(file, path, MAX_PATH_LENGTH, FILE_NAME_OPENED);
		char* err = _win32_error(0);
		sys_print_err(err);
		sys_print_err(": ");
		sys_print_err(path);
		sys_print_err("\n");
		LocalFree(err);
	}
	return result;
}

void sys_close(file_t file) {
	if(file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
	}
}

// handle_t core_open_or_create(char* path) {
// 	assert(sizeof(HANDLE)<=sizeof(handle_t));

// 	if (str_len(path) >= MAX_PATH) {
// 		print_error("Sorry, we don't support paths longer than %i", (int)MAX_PATH);
// 		return NULL;
// 	}
	
// 	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
// 	if(handle==INVALID_HANDLE_VALUE) {
// 		return NULL;
// 	}
// 	return handle;
// }

file_t sys_open_dir(char* path) {
	assert(sizeof(HANDLE)<=sizeof(file_t));
	
	DWORD attributes = GetFileAttributesA(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		sys_print_err("Failed to open directory: ");
		sys_print_err(path);
		sys_print_err("\n");
		return NULL;
	}
	if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
		sys_print_err("Path is not a directory: ");
		sys_print_err(path);
		sys_print_err("\n");
		return NULL;
	}

	HANDLE handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
								0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		char* err = _win32_error(0);
		sys_print_err(err);
		sys_print_err(": ");
		sys_print_err(path);
		sys_print_err("\n");
		LocalFree(err);
		return NULL;
	}
	return handle;
}

file_t sys_create_dir(char* path) {
	BOOL success = CreateDirectoryA(path, NULL);
	if(!success) {
		DWORD err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS) {
			// don't think it matters
			// print_error("Directory already exists");
		} else if (err == ERROR_PATH_NOT_FOUND) {
			sys_print_err("Directory path not found: ");
			sys_print_err(path);
			sys_print_err("\n");
			return NULL;
		} else {
			char* err = _win32_error(0);
			sys_print_err(err);
			sys_print_err(": ");
			sys_print_err(path);
			sys_print_err("\n");
			LocalFree(err);
			return NULL;
		}
	}

	return sys_open_dir(path);
}

int sys_list_dir(char* path, _Bool recursive, stat_t* output, int length) {
	int output_index = 0;
	char wildcard[MAX_PATH_LENGTH];
	// char* wildcard = str_format("%s/*", path);
	int pathlen = strlen(path);
	if (pathlen > MAX_PATH_LENGTH-3) {
		sys_print_err("sys_list_dir(): Path is longer than MAX_PATH_LENGTH \n");
		return 0;
	}
	sys_copy_memory(wildcard, path, pathlen);
	wildcard[pathlen] = '/';
	wildcard[pathlen+1] = '*';
	wildcard[pathlen+2] = 0;

	WIN32_FIND_DATAA find_data;
	HANDLE find_handle = FindFirstFileA(wildcard, &find_data);
	if (find_handle == INVALID_HANDLE_VALUE) {
		sys_print_err("Failed to open directory: ");
		sys_print_err(path);
		sys_print_err("\n");
		return 0;
	}
	do {
		//if (!str_compare(find_data.cFileName, ".") && !str_compare(find_data.cFileName, "..")) {
		if (!_strcmp(find_data.cFileName, ".") && !_strcmp(find_data.cFileName, "..")) {
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (recursive) {
					char filePath[MAX_PATH_LENGTH];
					snprintf(filePath, sizeof(filePath), "%s/%s", path, find_data.cFileName);
					output_index += sys_list_dir(
						filePath,
						_True,
						output+output_index,
						length-output_index);
				} else {
					stat_t* file = output + output_index++;
					assert(_strlen(find_data.cFileName) < sizeof(output->filename));
					_strncpy(file->filename, find_data.cFileName, sizeof(file->filename));
					file->created = find_data.ftCreationTime.dwLowDateTime;
					file->created |= (uint64_t)find_data.ftCreationTime.dwHighDateTime<<32;
					file->modified = find_data.ftLastWriteTime.dwLowDateTime;
					file->modified |= (uint64_t)find_data.ftLastWriteTime.dwHighDateTime<<32;
					file->size = ((uint64_t)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
				}
			} else {
				// char* filename = s_copy(find_data.cFileName);
				if (output_index < length) {
					stat_t* file = output + output_index++;
					assert(_strlen(find_data.cFileName) < sizeof(output->filename));
					_strncpy(file->filename, find_data.cFileName, sizeof(file->filename));
					file->created = find_data.ftCreationTime.dwLowDateTime;
					file->created |= (uint64_t)find_data.ftCreationTime.dwHighDateTime<<32;
					file->modified = find_data.ftLastWriteTime.dwLowDateTime;
					file->modified |= (uint64_t)find_data.ftLastWriteTime.dwHighDateTime<<32;
					file->size = ((uint64_t)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
				}
			}
		}
	} while (FindNextFileA(find_handle, &find_data));
	FindClose(find_handle);
	return output_index;
}

void sys_current_dir(char* output, size_t size) {
	GetCurrentDirectoryA(size, output);
}

void sys_change_dir(char* path) {
	SetCurrentDirectoryA(path);
}


// Dynamic libraries
dylib_t sys_load_lib(char *file) {
	dylib_t lib;
	char path[MAX_PATH];
	snprintf(path, MAX_PATH-1, "%s.dll", file);
	lib.handle = LoadLibraryA(path);
	return lib;
}

void *sys_load_lib_sym(dylib_t lib, char *proc) {
	return GetProcAddress(lib.handle, proc);
}


// Watching directory changes
DWORD WINAPI watcher_thread_proc(watcher_thread_t* thread) {
	directory_watcher_t* watcher = thread->watcher;

	thread->handle = CreateFileA(
		thread->path,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);
	if (thread->handle == INVALID_HANDLE_VALUE) {
		sys_print_err("Failed to open directory: ");
		sys_print_err(thread->path);
		sys_print_err("\n");
		return 0;
	}

	for (;;) {
		// WaitForSingleObject(thread->handle, INFINITE);

		uint8_t change_buffer[512] = {0};
		DWORD bytes;
		BOOL rdc = ReadDirectoryChangesW(
			thread->handle,
			change_buffer,
			sizeof(change_buffer),
			TRUE,
			watcher->filter,
			&bytes,
			NULL,
			NULL
		);
		if (!rdc) {
			char* err = _win32_error(0);
			sys_print_err("ReadDirectoryChangesW failed: ");
			sys_print_err(": ");
			sys_print_err(err);
			sys_print_err("\n");
			LocalFree(err);
			continue;
		}

		if (!bytes) {
			continue;
		}

		WaitForSingleObject(watcher->semaphore, INFINITE);

		FILE_NOTIFY_INFORMATION* change = (FILE_NOTIFY_INFORMATION*)change_buffer;
		while (change) {
			char filename[MAX_PATH_LENGTH];
			_wide_to_char(filename, change->FileName, MAX_PATH_LENGTH);

			if (watcher->result_count < sizeof(watcher->results)/sizeof(watcher->results[0])) {
				file_change_t* result = watcher->results + watcher->result_count++;

				char fullpath_buffer[MAX_PATH_LENGTH] = {0};
				_strncpy(fullpath_buffer, thread->path, MAX_PATH_LENGTH);
				_strncat(fullpath_buffer, "/", MAX_PATH_LENGTH);
				_strncat(fullpath_buffer, filename, MAX_PATH_LENGTH);

 				// char* fullpath = core_strf("%s/%s", thread->path, filename);
				GetFullPathNameA(fullpath_buffer, sizeof(result->filename), result->filename, NULL);
				// core_strncpy(result->filename, fullpath, MAX_PATH_LENGTH);
				// core_strfree(fullpath);

				DWORD attr = GetFileAttributesA(result->filename);

				// if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
				// 	Sleep(100);
				// 	handle_t file = core_open(result->filename);
				// 	int loopcount = 0;
				// 	while (!file && loopcount < 10) {
				// 		++loopcount;
				// 		Sleep(100);
				// 		file = core_open(result->filename);
				// 	}
					
				// 	if (file) {
				// 		stat_t info = core_stat(file);
				// 		core_strncpy(info.filename, result->filename, MAX_PATH_LENGTH);
				// 		*result = info;
				// 		core_close(file);
				// 	}
				// }

				// handle_t file = core_open(result->filename);
				// if (file) {
					// stat_t info = core_stat(file);
					// core_close(file);
					time_t time = sys_time();
					result->modified = time;
				// }
			} else {
				break;
			}
			
			// sys_free_memory(filename);

			if (change->NextEntryOffset) {
				change = (FILE_NOTIFY_INFORMATION*)((uint8_t*)change + change->NextEntryOffset);
			} else {
				change = NULL;
			}
		}

		ReleaseSemaphore(watcher->semaphore, 1, NULL);
		SetEvent(watcher->ready_event);
	}
}

_Bool watch_directory_changes(directory_watcher_t* watcher, char** dir_paths, int dir_count) {
	if (dir_count > sizeof(watcher->threads)/sizeof(watcher->threads[0])) {
		// print_error("Maximum of %i directories", array_size(watcher->threads));
		sys_print_err("watch_directory_changes(): Maximum directory count exceeded \n");
		return FALSE;
	}
	
	sys_zero_memory(watcher, sizeof(directory_watcher_t));
	watcher->filter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	// watcher->filter = 0b11111111;
	watcher->directory_count = dir_count;

	watcher->semaphore = CreateSemaphoreA(NULL, 1, 1, "DirectoryWatcherSemaphore");
	watcher->ready_event = CreateEventA(NULL, TRUE, FALSE, "DirectoryWatcherEvent");
	
	for (int i=0; i<dir_count; ++i) {
		watcher_thread_t* thread = watcher->threads + i;

		GetFullPathNameA(dir_paths[i], MAX_PATH_LENGTH, thread->path, NULL);

		thread->watcher = watcher;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)watcher_thread_proc, thread, 0, NULL);
	}

	return TRUE;
}

int wait_for_directory_changes(directory_watcher_t* watcher, file_change_t* output, int output_size) {
	assert(output_size > 0);
	WaitForSingleObject(watcher->ready_event, INFINITE);
	ResetEvent(watcher->ready_event);
	
	// Write out results
	WaitForSingleObject(watcher->semaphore, INFINITE);
	sys_copy_memory(output, watcher->results, min(watcher->result_count, output_size)*sizeof(*output));
	// TODO check output size first
	sys_zero_memory(watcher->results, sizeof(watcher->results));
	int result = watcher->result_count;
	watcher->result_count = 0;
	ReleaseSemaphore(watcher->semaphore, 1, NULL);
	return result;
}
