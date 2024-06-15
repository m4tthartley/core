
// #include <fileapi.h>
// #include <stdatomic.h>
#include <stdio.h>
// #include <stdatomic.h>
#include <processthreadsapi.h>

#include <core/core.h>

// #include "net.c"

#define VERSION "0.1"

typedef struct {
	core_string_t dll_filename;
	core_string_t dll_path;
	core_string_t dir_path;
} state_t;

typedef void* (*start_proc)(void);
typedef void (*frame_proc)(void* param);

b32 do_reload = FALSE;
HANDLE lib;
start_proc start;
frame_proc frame;

// void core_error(char* err, ...) {
// 	char str[1024];
// 	va_list va;
// 	va_start(va, err);
// 	vsnprintf(str, 1024, err, va);
// 	printf("%s\n", str);
// 	va_end(va);
// }

// char _win32_error_buffer[1024];
// char* win32_error() {
// 	DWORD error = GetLastError();
// 	FormatMessage(
// 		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
// 		NULL,
// 		error,
// 		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
// 		(LPWSTR)_win32_error_buffer,
// 		sizeof(_win32_error_buffer),
// 		NULL);
// 	return _win32_error_buffer;
// }

// void completion_routine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
// 	printf("completion routine \n");
// 	FILE_NOTIFY_INFORMATION* file = file_changes;
// 	while(file) {
// 		printf("%s \n", file->FileName);
// 		// if()
//
// 		file = NULL;
// 		if(file->NextEntryOffset) {
// 			file = (u8*)file + file->NextEntryOffset;
// 		}
// 	}
// }

DWORD dir_listen_thread(void* lp) {
	state_t* state = lp;
#if 0
	FILE_NOTIFY_INFORMATION file_changes[16];

	HANDLE file_handle = CreateFileA(
		state->dir_path,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
		NULL);
	if(file_handle == INVALID_HANDLE_VALUE) {
		printf("\033[91mCreateFileA failed\033[0m \n");
	}

	long bytes;
	int read;

	OVERLAPPED overlapped = {0};
	overlapped.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);

	if(!ReadDirectoryChangesW(
		file_handle,
		file_changes,
		sizeof(file_changes),
		TRUE,
		FILE_NOTIFY_CHANGE_LAST_WRITE,
		&bytes,
		&overlapped,
		NULL)) {
		core_win32_error(0, FALSE, "ReadDirectoryChanges: %s");
	}

	for(;;) {
		// DWORD wait = WaitForSingleObject(overlapped.hEvent, INFINITE);
		
		// if(wait != WAIT_OBJECT_0) {
		// 	printf("wait result is not 0: %i \n", wait);
		// }

		int bytes_transfered = 0;
		GetOverlappedResult(file_handle, &overlapped, &bytes_transfered, TRUE);

		FILE_NOTIFY_INFORMATION* file = file_changes;
		while(file) {
			// TODO maybe this can be a core string function
			char filename[MAX_PATH+1] = {0};
			for(int i=0; i<file->FileNameLength/sizeof(*file->FileName); ++i) {
				filename[i] = file->FileName[i];
			}
			printf("%s \n", filename);
			if(s_compare(filename, state->dll_filename)) {
				printf("RELOAD \n");
				atomic_swap32(&do_reload, TRUE);
			}

			if(file->NextEntryOffset) {
				file = (u8*)file + file->NextEntryOffset;
			} else {
				file = NULL;
			}
		}

		zeroMemory(file_changes, sizeof(file_changes));

		if(!ReadDirectoryChangesW(
			file_handle,
			file_changes,
			sizeof(file_changes),
			TRUE,
			FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytes,
			&overlapped,
			NULL)) {
			core_win32_error(0, FALSE, "ReadDirectoryChanges: %s");
		}
	}
#endif

	core_directory_watcher_t watcher;
	char* dirs[] = {state->dir_path};
	core_watch_directory_changes(&watcher, dirs, 1);

	core_file_change_t changes[8];
	for (;;) {
		int count = core_wait_for_directory_changes(&watcher, changes, 8);
		FOR (i, count) {
			core_print(changes[i].filename);
			if(core_strcmp(changes[i].filename, state->dll_path)) {
				printf("RELOAD \n");
				core_sync_swap32(&do_reload, TRUE);
			}
		}
	}

	return 0;
}

void reload(state_t* state) {
	b32* terminate_threads = (b32*)GetProcAddress(lib, "terminate_threads");
	if (terminate_threads) {
		core_sync_swap32(terminate_threads, TRUE);
	}
	Sleep(1000);
	FreeLibrary(lib);
	core_string_t copy_dll = core_strf("%scopy_%s", state->dir_path, state->dll_filename);
	int copy = CopyFile(state->dll_path, copy_dll, 0);
	lib = LoadLibraryA(copy_dll);
	start = (start_proc*)GetProcAddress(lib, "start");
	frame = (frame_proc*)GetProcAddress(lib, "frame");
}

int main(int argc, char** argv) {
	printf("reloader version %s \n", VERSION);

	if(argc < 2) {
		printf("Arg 1 should be a directory to watch \n");
		exit(1);
	}

	state_t state = {0};

	u8 buffer[PAGE_SIZE];
	// string_pool spool;
	// s_create_pool(&spool, 0, 0);
	// m_reserve(&spool, GB(1), PAGE_SIZE);
	// s_pool(&spool);
	core_allocator_t allocator = core_allocator(buffer, sizeof(buffer));
	core_use_allocator(&allocator);

	HANDLE file = CreateFileA(argv[1], GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(file == INVALID_HANDLE_VALUE) {
		printf("file %s could not be found \n", argv[1]);
		exit(1);
	}

	// server();

	// char dir_buffer[MAX_PATH] = {0};
	// PathCchRemoveFileSpec(dir_buffer, MAX_PATH);
	// printf("dir %s \n", dir_buffer);
	
	char absolute_path[CORE_MAX_PATH_LENGTH];
	GetFullPathNameA(argv[1], sizeof(absolute_path), absolute_path, NULL);
	state.dll_path = core_str(absolute_path);

	char* c = argv[1] + core_strlen(argv[1]);
	while(*c != '/' && c >= argv[1]) {
		// *c = 0;
		--c;
	}
	state.dll_filename = core_str(c+1);
	c[1] = 0;

	// string dll_path = "./test.dll";
	state.dir_path = argv[1];
	if(!core_strlen(state.dir_path)) {
		state.dir_path = "./";
	}
	// printf(dll_path);

	CreateThread(0, 0, dir_listen_thread, &state, 0, 0);

	reload(&state);

	// PULONG low;
	// PULONG high;
	// GetCurrentThreadStackLimits(&low, &high);
	// register void *sp asm ("sp");

	void* user_param = start();
	for(;;) {
		if(core_sync_compare_swap32(&do_reload, TRUE, FALSE)) {
			reload(&state);
			// s_pool_clear(&spool);
		}
		frame(user_param);
	}

	return 0;
}
