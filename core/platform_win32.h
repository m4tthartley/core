
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// #include <mmeapi.h>

#define HANDLE_NULL INVALID_HANDLE_VALUE
#define HANDLE_STDOUT stdout

// #define INVALID_SOCKET  (SOCKET)(~0)
// #define SOCKET_ERROR            (-1)

typedef void* f_handle; // HANDLE
typedef SOCKET socket_t;


// Errors
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
	// snprintf(str, 1024, "%s\n%#08X %s\n", usrstr, error_code, msg);

	char* result = s_create(msg);
	LocalFree(msg);

	return result;
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
	ZeroMemory(arena, sizeof(memory_arena));
}

void core_copy_memory(void* dest, void* source, size_t size) {
	CopyMemory(dest, source, size);
}


// Threading and atomic operations
// TODO TryEnterCriticalSection 
typedef struct {
	CRITICAL_SECTION handle;
} core_critical_section_t;

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

int atomic_swap32(void *ptr, int swap) {
	return _InterlockedExchange((long volatile*)ptr, swap);
}

b32 atomic_compare_swap32(void *ptr, int cmp, int swap) {
	return _InterlockedCompareExchange((long volatile*)ptr, swap, cmp) == cmp;
}

int atomic_add32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, value);
}

int atomic_sub32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, -value);
}

int atomic_read32(void *ptr) {
	return _InterlockedExchangeAdd((long volatile*)ptr, 0);
}


// Message Box
void core_message_box(char* msg, char* caption, int type) {
	MessageBox(NULL, msg, caption, MB_OK);
}


// Time and Dates
timestamp_t core_system_time() {
	// number of 100-nanosecond intervals
	FILETIME time;
	GetSystemTimeAsFileTime(&time);
	u64 result2 = ((u64)time.dwHighDateTime << 32) | time.dwLowDateTime;
	u64 nsec100 = *(u64*)&time;
	timestamp_t result = {
		nsec100 / 10000
	};
	return result;
}

char* core_format_time(timestamp_t time) {
	char d[64];
	char t[64];
	if (!time.msec) {
		GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "ddd, dd MMM yyyy", d, 64);
		GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "HH:mm:ss", t, 64);
	} else {
		u64 nsec100 = time.msec * 10000;
		FILETIME ft = *(FILETIME*)&nsec100;
		SYSTEMTIME st;
		FileTimeToSystemTime(&ft, &st);
		GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "ddd, dd MMM yyyy", d, 64);
		GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, &st, "HH:mm:ss", t, 64);
	}
	char* result = s_format("%s %s GMT", d, t);
	return result;
}


// Files
f_handle f_open(char* path) {
	assert(sizeof(HANDLE)<=sizeof(f_handle));
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, OPEN_EXISTING, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		// DWORD error = GetLastError();
		// LPTSTR msg;
		// FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
		// 			  FORMAT_MESSAGE_FROM_SYSTEM|
		// 			  FORMAT_MESSAGE_IGNORE_INSERTS,
		// 			  NULL, error,
		// 			  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// 			  (LPTSTR)&msg, 0, NULL);
		core_error_console(FALSE, "Failed to open file %s", path);
		return NULL_HANDLE;
	}
	return handle;
}

f_handle f_create(char* path) {
	assert(sizeof(HANDLE)<=sizeof(f_handle));
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, CREATE_ALWAYS, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		// DWORD error = GetLastError();
		// LPTSTR msg;
		// FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
		// 			  FORMAT_MESSAGE_FROM_SYSTEM|
		// 			  FORMAT_MESSAGE_IGNORE_INSERTS,
		// 			  NULL, error,
		// 			  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// 			  (LPTSTR)&msg, 0, NULL);
		core_win32_error(0, FALSE, "Failed to create file %s", path);
		return 0;
	}
	return handle;
}

f_handle f_open_directory(char* path) {
	assert(sizeof(HANDLE)<=sizeof(f_handle));
	
	DWORD attributes = GetFileAttributesA(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		core_error_console(FALSE, "Failed to open directory %s", path);
		return NULL_HANDLE;
	}
	if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
		core_error_console(FALSE, "Path is not a directory %s", path);
		return NULL_HANDLE;
	}

	HANDLE handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
								0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		core_error_console(FALSE, "Failed to open directory %s", path);
		return NULL_HANDLE;
	}
	return handle;
}

void f_create_directory(char* path) {
	BOOL success = CreateDirectoryA(path, NULL);
	if(!success) {
		DWORD err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS) {
			core_error_console(FALSE, "Directory already exists");
		} else if (err == ERROR_PATH_NOT_FOUND) {
			core_error_console(FALSE, "Directory path not found");
		} else {
			core_win32_error(0, FALSE, "Failed to create directory %s", path);
		}
	}
}

int f_directory_list(char* path, b32 recursive, f_info* output, int length) {
	int output_index = 0;
	char* wildcard = s_format("%s/*", path);
	WIN32_FIND_DATAA find_data;
	HANDLE find_handle = FindFirstFileA(wildcard, &find_data);
	if (find_handle == INVALID_HANDLE_VALUE) {
		core_error_console(FALSE, "Failed to open directory: %s", path);
		return 0;
	}
	do {
		if (!s_compare(find_data.cFileName, ".") && !s_compare(find_data.cFileName, "..")) {
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (recursive) {
					output_index += f_directory_list(
						s_format("%s/%s", path, find_data.cFileName),
						TRUE,
						output+output_index,
						length-output_index);
				} else {
					f_info* file = output + output_index++;
					assert(s_len(find_data.cFileName) < sizeof(output->filename));
					strcpy(file->filename, find_data.cFileName);
					file->created = find_data.ftCreationTime.dwLowDateTime;
					file->created |= (u64)find_data.ftCreationTime.dwHighDateTime<<32;
					file->modified = find_data.ftLastWriteTime.dwLowDateTime;
					file->modified |= (u64)find_data.ftLastWriteTime.dwHighDateTime<<32;
					file->size = ((u64)find_data.nFileSizeHigh<<32) | find_data.nFileSizeLow;
				}
			} else {
				// char* filename = s_copy(find_data.cFileName);
				if (output_index < length) {
					f_info* file = output + output_index++;
					assert(s_len(find_data.cFileName) < sizeof(output->filename));
					strcpy(file->filename, find_data.cFileName);
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

int f_read(f_handle file, size_t offset, void* output, size_t size) {
	DWORD bytesRead;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = ReadFile(file, output, size, &bytesRead, &overlapped);
	if(!result || bytesRead!=size) {
		char path[64];
		GetFinalPathNameByHandleA(file, path, 64, FILE_NAME_OPENED);
		MessageBox(NULL, s_format("Failed to read file: %s", path), "File Error", MB_OK);
		return 0;
	} else {
		return 1;
	}
}

void f_write(f_handle file, size_t offset, void* data, size_t size) {
	DWORD bytesWritten;
	OVERLAPPED overlapped = {0};
	overlapped.Offset = offset;
	int result = WriteFile(file, data, size, &bytesWritten, &overlapped);
	if(!result || bytesWritten!=size) {
		MessageBox(NULL, "Failed to write file", "Error", MB_OK);
	}
}

f_info f_stat(f_handle file) {
	f_info result = {0};
	BY_HANDLE_FILE_INFORMATION info = {0};
	if(GetFileInformationByHandle(file, &info)) {
		result.created = info.ftCreationTime.dwLowDateTime;
		result.created |= (u64)info.ftCreationTime.dwHighDateTime<<32;
		result.modified = info.ftLastWriteTime.dwLowDateTime;
		result.modified |= (u64)info.ftLastWriteTime.dwHighDateTime<<32;
		result.size = info.nFileSizeLow;
		result.is_directory = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	} else {
		MessageBox(NULL, "Failed to stat file", "Error", MB_OK);
	}
	return result;
}

void f_close(f_handle file) {
	if(file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
	}
}

void f_change_directory(char* path) {
	SetCurrentDirectoryA(argv[1]);
}