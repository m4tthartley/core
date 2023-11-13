
// void core_watcher_handle_change(core_directory_watcher_t* watcher, u8* change_buffer) {
// 	printf("ReadDirectoryChangesW bytes %i %lu, ", directory_index, bytes);
// 	if (!bytes) {
// 		printf("\n\n");
// 		continue;
// 	}

// 	WaitForSingleObject(watcher->semaphore, INFINITE);

// 	FILE_NOTIFY_INFORMATION *change = change_buffer;
// 	while (change) {
// 		char* filename = core_convert_wide_string(change->FileName);
// 		printf(filename);
// 		printf(", ");

// 		// if (watcher->result.count < output_size) {
// 		// 	s_ncopy(watcher->result.files[watcher->result.count].filename, filename, array_size(output[0].filename));
// 		// 	watcher->result.count++;
// 		// } else {
// 		// 	break;
// 		// }
		
// 		s_free(filename);

// 		if (change->NextEntryOffset) {
// 			change = (u8*)change + change->NextEntryOffset;
// 		} else {
// 			change = NULL;
// 		}
// 	}

// 	printf("\n\n");

// 	ReleaseSemaphore(watcher->semaphore, 1, NULL);
// }

// DWORD WINAPI core_watcher_thread_proc(core_watcher_thread_t* thread) {
// 	// int directory_index = ((core_directory_watcher_thread_t*)param)->directory_index;
// 	core_directory_watcher_t* watcher = thread->watcher;

// 	for (;;) {
// 		// WaitForSingleObject(watcher->handles[directory_index], INFINITE);

// 		u8 change_buffer[512] = {0};
// 		int bytes;
// 		core_print("Reading %i...", directory_index);
// 		BOOL rdc = ReadDirectoryChangesW(
// 			watcher->handles[directory_index],
// 			change_buffer,
// 			sizeof(change_buffer),
// 			TRUE,
// 			watcher->filter,
// 			&bytes,
// 			NULL,
// 			NULL
// 		);
// 		if (!rdc) {
// 			core_error("ReadDirectoryChangesW failed");
// 			continue;
// 		}

// 		printf("ReadDirectoryChangesW bytes %i %lu, ", directory_index, bytes);
// 		if (!bytes) {
// 			printf("\n\n");
// 			continue;
// 		}

// 		WaitForSingleObject(watcher->semaphore, INFINITE);

// 		FILE_NOTIFY_INFORMATION *change = change_buffer;
// 		while (change) {
// 			char* filename = core_convert_wide_string(change->FileName);
// 			printf(filename);
// 			printf(", ");

// 			// if (watcher->result.count < output_size) {
// 			// 	s_ncopy(watcher->result.files[watcher->result.count].filename, filename, array_size(output[0].filename));
// 			// 	watcher->result.count++;
// 			// } else {
// 			// 	break;
// 			// }
			
// 			s_free(filename);

// 			if (change->NextEntryOffset) {
// 				change = (u8*)change + change->NextEntryOffset;
// 			} else {
// 				change = NULL;
// 			}
// 		}

// 		printf("\n\n");

// 		ReleaseSemaphore(watcher->semaphore, 1, NULL);
// 	}
// }


#ifdef CORE_DIRECTORY_WATCHER_SINGLE_THREADED_COMPLETION_ROUTINE
b32 core_watch_directory_changes(core_directory_watcher_t* watcher, char** dir_paths, int dir_count) {
    DWORD filter = FILE_NOTIFY_CHANGE_LAST_WRITE; //0b11111111;
	HANDLE handles[2];
	handles[0] = FindFirstChangeNotification(directories[0].path, TRUE, filter);
	handles[1] = FindFirstChangeNotification(directories[1].path, TRUE, filter);

	if (handles[0] == INVALID_HANDLE_VALUE) {
		core_error("FindFirstChangeNotification");
	}
	if (handles[1] == INVALID_HANDLE_VALUE) {
		core_error("FindFirstChangeNotification");
	}

	for (;;) {
		DWORD wait  = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (wait == WAIT_OBJECT_0 || wait == WAIT_OBJECT_0+1) {

			u8 change_buffer[512] = {0};
			char filenames[64][CORE_MAX_PATH_LENGTH];
			int file_count = 0;
			int bytes;
			if (!ReadDirectoryChangesW(
				handles[wait-WAIT_OBJECT_0],
				change_buffer,
				sizeof(change_buffer),
				TRUE,
				filter,
				&bytes,
				NULL,
				NULL
			)) {
				printf("ReadDirectoryChangesW failed \n");
			} else {
				printf("ReadDirectoryChangesW bytes %i %lu, ", wait-WAIT_OBJECT_0, bytes);

				FILE_NOTIFY_INFORMATION *change = change_buffer;
				while (change) {
					char* filename = core_convert_wide_string(change->FileName);
					printf(filename);
					printf(", ");

					if (file_count < array_size(filenames)) {
						s_copy(filenames[file_count], filename);
						++file_count;
					} else {
						break;
					}
					// file_changes(filename);

					if (change->NextEntryOffset) {
						change = (u8*)change + change->NextEntryOffset;
					} else {
						change = NULL;
					}
				}

				printf("\n\n");

				file_changes(filenames, file_count);
			}

		} else {
			core_error("wait %i", wait-WAIT_OBJECT_0);
		}

		// m_zero(change_buffer, sizeof(change_buffer));

		if(!FindNextChangeNotification(handles[wait-WAIT_OBJECT_0])) {
			core_error("FindNextChangeNotification");
		}
	}
}
#endif


#ifdef CORE_DIRECTORY_WATCHER_MULTI_THREADED_COMPLETION_ROUTINE
// typedef struct {
// 	int directory_index;
// 	struct core_directory_watcher_t* watcher;
// } core_directory_watcher_thread_t;

typedef struct {
	HANDLE handle;
	HANDLE event_handle;
	OVERLAPPED overlapped;
	char path[CORE_MAX_PATH_LENGTH];
	FILE_NOTIFY_INFORMATION change_buffer[64];
	struct core_directory_watcher_t* watcher;
} core_watcher_thread_t;

typedef struct {
	HANDLE semaphore;
	HANDLE ready_event;
	HANDLE handles[64];
	core_watcher_thread_t threads[64];
	int directory_count;
	DWORD filter;
	// struct {
	// 	f_info* files;
	// 	int count;
	// 	int directory_index;
	// } result;

	// struct {
	// 	char filename[CORE_MAX_PATH_LENGTH];
	// 	int directory_index;
	// } results[64];
	// int result_count;

	f_info results[64];
	int result_count;
} core_directory_watcher_t;

void completion_routine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

void dir_read_changes(core_watcher_thread_t* thread) {
	m_zero(thread->change_buffer, sizeof(thread->change_buffer));

	long bytes;
	int rdc = ReadDirectoryChangesW(
		thread->handle,
		thread->change_buffer,
		sizeof(thread->change_buffer),
		TRUE,
		FILE_NOTIFY_CHANGE_LAST_WRITE,
		NULL,
		&thread->overlapped,
		completion_routine
	);
	if (!rdc) {
		core_error("ReadDirectoryChangesW failed");
	}
}

void completion_routine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
	core_watcher_thread_t* thread = lpOverlapped->hEvent;
	core_directory_watcher_t* watcher = thread->watcher;
	
	// printf("ReadDirectoryChangesW bytes %lu, ", dwNumberOfBytesTransfered);
	if (!dwNumberOfBytesTransfered) {
		// printf("\n");
		return;
	}

	WaitForSingleObject(watcher->semaphore, INFINITE);

	FILE_NOTIFY_INFORMATION *change = thread->change_buffer;
	while (change) {
		char* filename = core_convert_wide_string(change->FileName);
		// printf(filename);
		// printf(", ");

		if (watcher->result_count < array_size(watcher->results)) {
			f_info* result = watcher->results + watcher->result_count++;
			s_ncopy(result->filename, thread->path, CORE_MAX_PATH_LENGTH);
			int len = s_len(result->filename);
			result->filename[len] = '/';
			++len;
			s_ncopy(result->filename+len, filename, CORE_MAX_PATH_LENGTH);
			
			FOR (i, s_len(result->filename)) {
				if (result->filename[i] == '\\') {
					result->filename[i] = '/';
				}
			}

			// f_handle file = f_open(result->filename);
			// f_info info = f_stat(file);
			// s_ncopy(info.filename, result->filename, CORE_MAX_PATH_LENGTH);
			// *result = info;
			// f_close(file);
		} else {
			break;
		}
		
		s_free(filename);

		if (change->NextEntryOffset) {
			change = (u8*)change + change->NextEntryOffset;
		} else {
			change = NULL;
		}
	}

	// printf("\n");

	ReleaseSemaphore(watcher->semaphore, 1, NULL);

	SetEvent(watcher->ready_event);
}

DWORD WINAPI core_watcher_thread_proc(core_watcher_thread_t* thread) {
	// int directory_index = ((core_directory_watcher_thread_t*)param)->directory_index;
	core_directory_watcher_t* watcher = thread->watcher;

	// thread->handle = FindFirstChangeNotificationA(thread->path, TRUE, watcher->filter);
	thread->handle = CreateFileA(
		thread->path,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
		NULL
	);
	if (thread->handle == INVALID_HANDLE_VALUE) {
		core_error("Failed to open directory %s", thread->path);
		return NULL;
	}
	// thread->handle = watcher->handles[i];
	// thread->event_handle = CreateEventA(NULL, FALSE, FALSE, thread->path);
	thread->overlapped = (OVERLAPPED){0};
	thread->overlapped.hEvent = (void*)thread;
	thread->watcher = watcher;

	dir_read_changes(thread);

	for (;;) {
		DWORD wait_result = WaitForSingleObjectEx(thread->handle, INFINITE, TRUE);
		if(wait_result != WAIT_IO_COMPLETION) {
			if (wait_result == WAIT_OBJECT_0) {
				core_print("WaitForMultipleObjectsEx: WAIT_OBJECT_0 + %i", wait_result - WAIT_OBJECT_0);
			} else if (wait_result == WAIT_ABANDONED_0) {
				core_print("WaitForMultipleObjectsEx: WAIT_ABANDONED_0 + %i", wait_result - WAIT_OBJECT_0);
			} else if (wait_result == WAIT_TIMEOUT) {
				core_print("WaitForMultipleObjectsEx: WAIT_TIMEOUT");
			} else if (wait_result == WAIT_FAILED) {
				core_print("WaitForMultipleObjectsEx: WAIT_FAILED");
			} else {
				core_print("WaitForMultipleObjectsEx: %s", wait_result);
			}
		}

		dir_read_changes(thread);
	}
}

b32 core_watch_directory_changes(core_directory_watcher_t* watcher, char** dir_paths, int dir_count) {
	if (dir_count > array_size(watcher->handles)) {
		core_error("You can't have more than %i directories", array_size(watcher->handles));
		return FALSE;
	}
	
	watcher->filter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	// watcher->filter = 0b11111111;
	watcher->directory_count = dir_count;

	watcher->semaphore = CreateSemaphoreA(NULL, 1, 1, "DirectoryWatcherSemaphore");
	watcher->ready_event = CreateEventA(NULL, TRUE, FALSE, "DirectoryWatcherEvent");
	
	FOR (i, dir_count) {
		
		core_watcher_thread_t* thread = watcher->threads + i;
		// thread->directory_index = i;
		thread->watcher = watcher;
		// s_copy(thread->path, dir_paths[i]);
		GetFullPathNameA(dir_paths[i], CORE_MAX_PATH_LENGTH, thread->path, NULL);
		core_print(thread->path);

		CreateThread(NULL, 0, core_watcher_thread_proc, thread, 0, NULL);
	}
}

int core_wait_for_directory_changes(core_directory_watcher_t* watcher, f_info* output, int output_size) {
	// ReleaseSemaphore(watcher=>semaphore, 1, NULL);

	WaitForSingleObject(watcher->ready_event, INFINITE);
	ResetEvent(watcher->ready_event);

	// Write out results
	WaitForSingleObject(watcher->semaphore, INFINITE);
	m_copy(output, watcher->results, min(watcher->result_count, output_size)*sizeof(f_info));
	// TODO check output size first
	m_zero(watcher->results, sizeof(watcher->results));
	int result = watcher->result_count;
	watcher->result_count = 0;
	ReleaseSemaphore(watcher->semaphore, 1, NULL);
	return result;
}
#endif


// This version didn't work well because sometimes
// WaitForMultipleObjects would trigger but then ReadDirectoryChangesW
// would block and wait again instead of just reading the changes.
// I think it's meant to work this way, but who knows.
// Windows is confusing.
#if CORE_DIRECTORY_WATCHER_SINGLE_THREADED
typedef struct {
	HANDLE handles[64];
	int directory_count;
	DWORD filter;
	struct {
		f_info* files;
		int count;
		int directory_index;
	} result;
} core_directory_watcher_t;

b32 core_watch_directory_changes(core_directory_watcher_t* watcher, char** dir_paths, int dir_count) {
	if (dir_count > array_size(watcher->handles)) {
		core_error("You can't have more than %i directories", array_size(watcher->handles));
		return FALSE;
	}
	
	watcher->filter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	watcher->directory_count = dir_count;
	
	FOR (i, dir_count) {
		watcher->handles[i] = FindFirstChangeNotification(dir_paths[i], TRUE, watcher->filter);
		if (watcher->handles[i] == INVALID_HANDLE_VALUE) {
			core_error("Failed to open directory %s", dir_paths[i]);
			return NULL;
		}
	}
}

void core_wait_for_directory_changes(core_directory_watcher_t* watcher, f_info* output, int output_size) {
	watcher->result.files = output;
	watcher->result.directory_index = 0;
	watcher->result.count = 0;

	core_print("Waiting...");
	DWORD wait  = WaitForMultipleObjects(watcher->directory_count, watcher->handles, FALSE, INFINITE);

	if (wait < WAIT_OBJECT_0 || wait >= WAIT_OBJECT_0+watcher->directory_count) {
		core_error("wait %i", wait-WAIT_OBJECT_0);
	}

	u8 change_buffer[512] = {0};
	int bytes;
	core_print("Reading...");
	BOOL rdc = ReadDirectoryChangesW(
		watcher->handles[wait-WAIT_OBJECT_0],
		change_buffer,
		sizeof(change_buffer),
		TRUE,
		watcher->filter,
		&bytes,
		NULL,
		NULL
	);
	if (!rdc) {
		core_error("ReadDirectoryChangesW failed");
		return;
	}

	printf("ReadDirectoryChangesW bytes %i %lu, ", wait-WAIT_OBJECT_0, bytes);
	if (!bytes) {
		return;
	}

	FILE_NOTIFY_INFORMATION *change = change_buffer;
	while (change) {
		char* filename = core_convert_wide_string(change->FileName);
		printf(filename);
		printf(", ");

		if (watcher->result.count < output_size) {
			s_ncopy(watcher->result.files[watcher->result.count].filename, filename, array_size(output[0].filename));
			watcher->result.count++;
		} else {
			break;
		}
		
		s_free(filename);

		if (change->NextEntryOffset) {
			change = (u8*)change + change->NextEntryOffset;
		} else {
			change = NULL;
		}
	}

	printf("\n\n");

	if(!FindNextChangeNotification(watcher->handles[wait-WAIT_OBJECT_0])) {
		core_error("FindNextChangeNotification");
	}
}
#endif
