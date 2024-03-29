//
//  platform_win32.c
//  Core
//
//  Created by Matt Hartley on 13/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform.h"

// Errors
// This needs to be freed manually with LocalFree, if you care
char* core_win32_error(DWORD error_code) {
	if (!error_code) {
		error_code = GetLastError();
	}

	char* msg;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		&msg,
		0,
		NULL);

	// char str[1024];
	// core_print_to_buffer(str, 1024, "%s\n%#08X %s\n", usrstr, error_code, msg);

	return msg;
}

void core_message_box(char* msg, char* caption, int type) {
	MessageBox(NULL, msg, caption, MB_OK);
}


// Virtual memory
void* core_reserve_virtual_memory(size_t size) {
	return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

void* core_commit_virtual_memory(void* addr, size_t size) {
	return VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
}

void* core_allocate_virtual_memory(size_t size) {
	return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void core_free_virtual_memory(void* addr, size_t size) {
	VirtualFree(addr, size, MEM_RELEASE);
}

void core_zero_memory(void* addr, size_t size) {
	ZeroMemory(addr, size);
}

void core_copy_memory(void* dest, void* source, size_t size) {
	CopyMemory(dest, source, size);
}


// Threading and atomic operations
// TODO TryEnterCriticalSection 
void core_init_critical_section(core_critical_section_t* section) {
	InitializeCriticalSection(&section->handle);
}

// TODO maybe core_atomic_section, enter_atomic_section, exit_atomic_section, atomic_lock
void core_enter_critical_section(core_critical_section_t* section) {
	// if (!) {
		// TODO I assume this is unsafe as you could have a 
		// race condition on the initialization?
		// InitializeCriticalSection(&section->win32_section);
	// }
	EnterCriticalSection(&section->handle);
}

void core_exit_critical_section(core_critical_section_t* section) {
	LeaveCriticalSection(&section->handle);
}

int core_sync_swap32(void *ptr, int swap) {
	return _InterlockedExchange((long volatile*)ptr, swap);
}

b32 core_sync_compare_swap32(void *ptr, int cmp, int swap) {
	return _InterlockedCompareExchange((long volatile*)ptr, swap, cmp) == cmp;
}

int core_sync_add32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, value);
}

int core_sync_sub32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, -value);
}

int core_sync_read32(void *ptr) {
	return _InterlockedExchangeAdd((long volatile*)ptr, 0);
}


// Time and Dates
core_time_t core_system_time() {
	// number of 100-nanosecond intervals
	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	u64 result2 = ((u64)time.dwHighDateTime << 32) | time.dwLowDateTime;
	u64 nsec100 = *(u64*)&time;
	core_time_t result = nsec100/10000;
	// result.sec = nsec100 / 10000000;
	// result.msec = (nsec100/10000) - (result.sec*1000);
	return result;
}

char* core_format_time(core_time_t time) {
	char d[64];
	char t[64];
	if (!time) {
		GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "ddd, dd MMM yyyy", d, 64);
		GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "HH:mm:ss", t, 64);
	} else {
		u64 nsec100 = time * 10000;
		FILETIME ft = *(FILETIME*)&nsec100;
		SYSTEMTIME st;
		FileTimeToSystemTime(&ft, &st);
		GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "ddd, dd MMM yyyy", d, 64);
		GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "HH:mm:ss", t, 64);
	}
	char* result = strdf("%s %s GMT", d, t);
	return result;
}


// Files
core_handle_t core_open(char* path) {
	assert(sizeof(HANDLE)<=sizeof(core_handle_t));

	// TODO: do this for all functions
	if (str_len(path) >= MAX_PATH) {
		core_error("Sorry, we don't support paths longer than %i", (int)MAX_PATH);
		return NULL;
	}
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, OPEN_EXISTING, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		if (error != ERROR_FILE_NOT_FOUND) {
			char* err = core_win32_error(error);
			core_error("%s: %s", path, err);
			LocalFree(err);
		}
		return NULL;
	}
	return handle;
}

core_handle_t core_create(char* path) {
	assert(sizeof(HANDLE)<=sizeof(core_handle_t));
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, OPEN_ALWAYS, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		char* err = core_win32_error(NULL);
		core_error("%s: %s", path, err);
		LocalFree(err);
		return NULL;
	}
	return handle;
}

// core_handle_t core_open_or_create(char* path) {
// 	assert(sizeof(HANDLE)<=sizeof(core_handle_t));

// 	if (str_len(path) >= MAX_PATH) {
// 		core_error("Sorry, we don't support paths longer than %i", (int)MAX_PATH);
// 		return NULL;
// 	}
	
// 	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
// 	if(handle==INVALID_HANDLE_VALUE) {
// 		return NULL;
// 	}
// 	return handle;
// }

core_handle_t core_open_dir(char* path) {
	assert(sizeof(HANDLE)<=sizeof(core_handle_t));
	
	DWORD attributes = GetFileAttributesA(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		core_error("Failed to open directory %s", path);
		return NULL;
	}
	if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
		core_error("Path is not a directory %s", path);
		return NULL;
	}

	HANDLE handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
								0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		char* err = core_win32_error(NULL);
		core_error("%s: %s", path, err);
		LocalFree(err);
		return NULL;
	}
	return handle;
}

void core_create_dir(char* path) {
	BOOL success = CreateDirectoryA(path, NULL);
	if(!success) {
		DWORD err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS) {
			// don't think it matters
			// core_error("Directory already exists");
		} else if (err == ERROR_PATH_NOT_FOUND) {
			core_error("Directory path not found");
		} else {
			char* err = core_win32_error(NULL);
			core_error("%s: %s", path, err);
			LocalFree(err);
		}
	}
}

int core_dir_list(char* path, b32 recursive, core_stat_t* output, int length) {
	int output_index = 0;
	char* wildcard = strdf("%s/*", path);
	WIN32_FIND_DATAA find_data;
	HANDLE find_handle = FindFirstFileA(wildcard, &find_data);
	if (find_handle == INVALID_HANDLE_VALUE) {
		core_error("Failed to open directory: %s", path);
		return 0;
	}
	do {
		if (!str_compare(find_data.cFileName, ".") && !str_compare(find_data.cFileName, "..")) {
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (recursive) {
					output_index += core_dir_list(
						strdf("%s/%s", path, find_data.cFileName),
						TRUE,
						output+output_index,
						length-output_index);
				} else {
					core_stat_t* file = output + output_index++;
					assert(str_len(find_data.cFileName) < sizeof(output->filename));
					str_copy(file->filename, find_data.cFileName, sizeof(file->filename));
					file->created = find_data.ftCreationTime.dwLowDateTime;
					file->created |= (u64)find_data.ftCreationTime.dwHighDateTime<<32;
					file->modified = find_data.ftLastWriteTime.dwLowDateTime;
					file->modified |= (u64)find_data.ftLastWriteTime.dwHighDateTime<<32;
					file->size = ((u64)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
				}
			} else {
				// char* filename = s_copy(find_data.cFileName);
				if (output_index < length) {
					core_stat_t* file = output + output_index++;
					assert(str_len(find_data.cFileName) < sizeof(output->filename));
					str_copy(file->filename, find_data.cFileName, sizeof(file->filename));
					file->created = find_data.ftCreationTime.dwLowDateTime;
					file->created |= (u64)find_data.ftCreationTime.dwHighDateTime<<32;
					file->modified = find_data.ftLastWriteTime.dwLowDateTime;
					file->modified |= (u64)find_data.ftLastWriteTime.dwHighDateTime<<32;
					file->size = ((u64)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
				}
			}
		}
	} while (FindNextFileA(find_handle, &find_data));
	FindClose(find_handle);
	return output_index;
}

b32 core_read(core_handle_t file, size_t offset, void* output, size_t size) {
	DWORD bytesRead;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = ReadFile(file, output, size, &bytesRead, &overlapped);
	if(!result || bytesRead!=size) {
		char path[CORE_MAX_PATH_LENGTH];
		GetFinalPathNameByHandleA(file, path, CORE_MAX_PATH_LENGTH, FILE_NAME_OPENED);
		char* err = core_win32_error(NULL);
		core_error("%s: %s", path, err);
		LocalFree(err);
		return FALSE;
	}
	return TRUE;
}

b32 core_write(core_handle_t file, size_t offset, void* data, size_t size) {
	DWORD bytesWritten;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = WriteFile(file, data, size, &bytesWritten, &overlapped);
	if(!result || bytesWritten!=size) {
		char path[CORE_MAX_PATH_LENGTH];
		GetFinalPathNameByHandleA(file, path, CORE_MAX_PATH_LENGTH, FILE_NAME_OPENED);
		char* err = core_win32_error(NULL);
		core_error("%s: %s", path, err);
		LocalFree(err);
		return FALSE;
	}
	return TRUE;
}

core_stat_t core_stat(core_handle_t file) {
	core_stat_t result = {0};
	BY_HANDLE_FILE_INFORMATION info = {0};
	if(GetFileInformationByHandle(file, &info)) {
		// FILETIME is the number of 100-nanosecond intervals
		// I'm storing file times in milliseconds
		result.created = info.ftCreationTime.dwLowDateTime;
		result.created |= (u64)info.ftCreationTime.dwHighDateTime<<32;
		result.created /= 10000;
		result.modified = info.ftLastWriteTime.dwLowDateTime;
		result.modified |= (u64)info.ftLastWriteTime.dwHighDateTime<<32;
		result.modified /= 10000;
		result.size = info.nFileSizeLow;
		result.is_directory = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	} else {
		char path[CORE_MAX_PATH_LENGTH];
		GetFinalPathNameByHandleA(file, path, CORE_MAX_PATH_LENGTH, FILE_NAME_OPENED);
		char* err = core_win32_error(NULL);
		core_error("%s: %s", path, err);
		LocalFree(err);
	}
	return result;
}

void core_close(core_handle_t file) {
	if(file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
	}
}

void core_current_dir(char* output, size_t size) {
	GetCurrentDirectoryA(size, output);
}

void core_change_dir(char* path) {
	SetCurrentDirectoryA(path);
}


// Watching directory changes
DWORD WINAPI core_watcher_thread_proc(core_watcher_thread_t* thread) {
	core_directory_watcher_t* watcher = thread->watcher;

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
		core_error("Failed to open directory %s", thread->path);
		return NULL;
	}

	for (;;) {
		// WaitForSingleObject(thread->handle, INFINITE);

		u8 change_buffer[512] = {0};
		int bytes;
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
			core_error("ReadDirectoryChangesW failed: %s", core_win32_error(NULL));
			continue;
		}

		if (!bytes) {
			continue;
		}

		WaitForSingleObject(watcher->semaphore, INFINITE);

		FILE_NOTIFY_INFORMATION *change = change_buffer;
		while (change) {
			char filename[CORE_MAX_PATH_LENGTH];
			str_wide_to_char(filename, change->FileName, CORE_MAX_PATH_LENGTH);

			if (watcher->result_count < array_size(watcher->results)) {
				core_file_change_t* result = watcher->results + watcher->result_count++;

				char fullpath_buffer[CORE_MAX_PATH_LENGTH] = {0};
				str_copy(fullpath_buffer, thread->path, CORE_MAX_PATH_LENGTH);
				str_append(fullpath_buffer, "/", CORE_MAX_PATH_LENGTH);
				str_append(fullpath_buffer, filename, CORE_MAX_PATH_LENGTH);

 				// char* fullpath = core_strf("%s/%s", thread->path, filename);
				GetFullPathNameA(fullpath_buffer, sizeof(result->filename), result->filename, NULL);
				// core_strncpy(result->filename, fullpath, CORE_MAX_PATH_LENGTH);
				// core_strfree(fullpath);

				DWORD attr = GetFileAttributesA(result->filename);

				// if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
				// 	Sleep(100);
				// 	core_handle_t file = core_open(result->filename);
				// 	int loopcount = 0;
				// 	while (!file && loopcount < 10) {
				// 		++loopcount;
				// 		Sleep(100);
				// 		file = core_open(result->filename);
				// 	}
					
				// 	if (file) {
				// 		core_stat_t info = core_stat(file);
				// 		core_strncpy(info.filename, result->filename, CORE_MAX_PATH_LENGTH);
				// 		*result = info;
				// 		core_close(file);
				// 	}
				// }

				// core_handle_t file = core_open(result->filename);
				// if (file) {
					// core_stat_t info = core_stat(file);
					// core_close(file);
					core_time_t time = core_system_time();
					result->modified = time;
				// }
			} else {
				break;
			}
			
			core_free(filename);

			if (change->NextEntryOffset) {
				change = (u8*)change + change->NextEntryOffset;
			} else {
				change = NULL;
			}
		}

		ReleaseSemaphore(watcher->semaphore, 1, NULL);
		SetEvent(watcher->ready_event);
	}
}

b32 core_watch_directory_changes(core_directory_watcher_t* watcher, char** dir_paths, int dir_count) {
	if (dir_count > array_size(watcher->threads)) {
		core_error("Maximum of %i directories", array_size(watcher->threads));
		return FALSE;
	}
	
	core_zero(watcher, sizeof(core_directory_watcher_t));
	watcher->filter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	// watcher->filter = 0b11111111;
	watcher->directory_count = dir_count;

	watcher->semaphore = CreateSemaphoreA(NULL, 1, 1, "DirectoryWatcherSemaphore");
	watcher->ready_event = CreateEventA(NULL, TRUE, FALSE, "DirectoryWatcherEvent");
	
	FOR (i, dir_count) {
		core_watcher_thread_t* thread = watcher->threads + i;

		GetFullPathNameA(dir_paths[i], CORE_MAX_PATH_LENGTH, thread->path, NULL);

		thread->watcher = watcher;
		CreateThread(NULL, 0, core_watcher_thread_proc, thread, 0, NULL);
	}

	return TRUE;
}

int core_wait_for_directory_changes(core_directory_watcher_t* watcher, core_file_change_t* output, int output_size) {
	assert(output_size > 0);
	WaitForSingleObject(watcher->ready_event, INFINITE);
	ResetEvent(watcher->ready_event);
	
	// Write out results
	WaitForSingleObject(watcher->semaphore, INFINITE);
	core_copy(output, watcher->results, min(watcher->result_count, output_size)*sizeof(*output));
	// TODO check output size first
	core_zero(watcher->results, sizeof(watcher->results));
	int result = watcher->result_count;
	watcher->result_count = 0;
	ReleaseSemaphore(watcher->semaphore, 1, NULL);
	return result;
}
