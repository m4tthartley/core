// #include <windows.h>
// #include <stdio.h>
// #include <locale.h>

#include <core/core.h>

#define VERSION "0.6.0"
#define SIMULATE_MULTIPLE_DIRECTORY 0

typedef struct {
	char filename[64];
	unsigned long long lastWriteTime;
} file_info;

file_info files[256];
int file_count = 0;

typedef struct {
	HANDLE handle;
	HANDLE event_handle;
	OVERLAPPED overlapped;
	char path[CORE_MAX_PATH_LENGTH];
	FILE_NOTIFY_INFORMATION change_buffer[64];
} directory_t;

directory_t directories[64] = {0};
int directory_count = 0;

b32 verbose_mode = FALSE;
char* build_command = "./build.sh";

// HANDLE directoryHandles[64];
// HANDLE ioHandles[64];
// OVERLAPPED directoryOverlapped[64];
// string paths[64];

file_info* getFile(char* filename) {
	for(int i=0; i<file_count; ++i) {
		if(!strcmp(files[i].filename, filename)) {
			return files + i;
		}
	}
	return NULL;
}

void addFile(char* filename, unsigned long long lastWriteTime) {
	printf("adding file %s \n", filename);
	if (file_count < 256) {
		file_info* file = &files[file_count++];
		strncpy(file->filename, filename, 63);
		file->lastWriteTime = lastWriteTime;
	} else {
		core_error(FALSE, "Failed to add file");
	}
}

void clear() {
	system("clear");
}

int build(char* filename) {
	char cwd[MAX_PATH] = {0};
	GetCurrentDirectoryA(MAX_PATH, cwd);
	SetCurrentDirectoryA(directories[0].path);

	// clear();
	// printf(TERM_CLEAR);

	if (verbose_mode) {
		printf("file changed %s \n", filename);
	}

	int result = system(s_format("sh %s", build_command));
	// printf("result %i \n", result);

	if(!result) {
		core_print(TERM_BRIGHT_GREEN_FG "build successful" TERM_RESET);
	} else {
		core_print(TERM_BRIGHT_RED_FG "build failed" TERM_RESET);
	}

	SetCurrentDirectoryA(cwd);
	return result;
}

void handleChange(directory_t* dir) {
	// printf("Handling file changes... \n");
	FILE_NOTIFY_INFORMATION *fileChange = dir->change_buffer;
	int loop_count = 0;
	while(fileChange) {
		// printf("\nFILE_NOTIFY_INFORMATION \n");
		// printf("offset is %i \n", fileChange->NextEntryOffset);
		char rawFileName[MAX_PATH+1] = {0};
		for(int i=0; i<fileChange->FileNameLength/sizeof(*fileChange->FileName); ++i) {
			rawFileName[i] = fileChange->FileName[i];
		}
		string filename = s_create(rawFileName);
		s_prepend(&filename, "/");
		s_prepend(&filename, dir->path);
		// printf("file change %s \n", filename);
		s_free(filename);

		// printf("after filename conversion %i \n", fileChange->NextEntryOffset);

		if( s_find(filename, ".c", 0) ||
			s_find(filename, ".h", 0) ||
			s_find(filename, ".txt", 0) ||
			s_find(filename, ".sh", 0)) {
			file_info* file = getFile(filename);
			HANDLE fileHandle = CreateFileA(
				filename,
				GENERIC_READ,
				FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			// if(fileHandle == INVALID_HANDLE_VALUE) {
			// 	printf("CreateFileA failed with error code %d \n", GetLastError());
			// 	return;
			// }
			FILETIME fileTime = {0};
			GetFileTime(fileHandle, NULL, NULL, &fileTime);

			// the laste write time is the number of 100-nanosecond intervals
			unsigned long long writeTime = ((long long )fileTime.dwHighDateTime)<<32 | fileTime.dwLowDateTime;
			writeTime /= 10 * 1000; // milliseconds

			if(file) {
				// printf("file change %s, diff %li \n", filename, writeTime-file->lastWriteTime);
				// printf("%li, %li (%i, %i) \n", writeTime, file->lastWriteTime, fileTime.dwHighDateTime, fileTime.dwLowDateTime);
				// If more than a second has passed
				if(writeTime-file->lastWriteTime > 1000) {
					// printf("file changed %s time difference %li \n", filename, writeTime-file->lastWriteTime);

					file->lastWriteTime = writeTime;
					
					build(filename);
#if 0
					{
						int next_offset1 = fileChange->NextEntryOffset;
						// printf("before build %i \n", fileChange->NextEntryOffset);

						char cwd[MAX_PATH] = {0};
						GetCurrentDirectoryA(MAX_PATH, cwd);
						SetCurrentDirectoryA(dir->path);

						// printf("during build 1 %i \n", fileChange->NextEntryOffset);

						clear();
						printf("file changed %s \n", filename);

						// printf("during build 2 %i \n", fileChange->NextEntryOffset);

						int result = system("sh ./build.sh");
						// printf("result %i \n", result);

						// printf("during build 3 %i \n", fileChange->NextEntryOffset);

						if(!result) {
							printf("\033[92mbuild successful\033[0m \n");
						} else {
							printf("\033[91mbuild failed\033[0m \n");
						}

						SetCurrentDirectoryA(cwd);

						// printf("after build %i \n", fileChange->NextEntryOffset);
						if (fileChange->NextEntryOffset != next_offset1) {
							int x = 0;
						}
					}
#endif
				}
			} else {
				// printf("file change %s, new \n", filename);
				// printf("before add file %i \n", fileChange->NextEntryOffset);
				addFile(filename, writeTime);
				// printf("before build %i \n", fileChange->NextEntryOffset);
				build(filename);
			}
			CloseHandle(fileHandle);
		}

		if(fileChange->NextEntryOffset) {
			// printf("offsetting by %i \n", fileChange->NextEntryOffset);
			// if (fileChange->NextEntryOffset > 64) {
			// 	int x = 0;
			// }
			fileChange = (char*)fileChange + fileChange->NextEntryOffset;
			++loop_count;
		} else {
			fileChange = 0;
		}
	}
}

void completionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

void dir_read_changes(directory_t* dir) {
	long bytes;
	int read = ReadDirectoryChangesW(
		dir->handle,
		dir->change_buffer,
		sizeof(dir->change_buffer),
		TRUE,
		FILE_NOTIFY_CHANGE_LAST_WRITE,
		&bytes,
		&dir->overlapped,
		&completionRoutine
	);
	if(!read) {
		printf("\033[91mReadDirectoryChangesW failed\033[0m \n");
		core_print(core_win32_error(NULL));
	}
}

void completionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
	directory_t* dir = lpOverlapped->hEvent;

	handleChange(dir);

	memset(dir->change_buffer, 0, sizeof(dir->change_buffer));

	// BOOL readResult = ReadDirectoryChangesW(
	// 	directoryHandles[dirIndex],
	// 	fileChangeBuffer,
	// 	sizeof(fileChangeBuffer),
	// 	TRUE,
	// 	FILE_NOTIFY_CHANGE_LAST_WRITE,
	// 	NULL,
	// 	&directoryOverlapped[dirIndex],
	// 	&completionRoutine);
	// if(!readResult) {
	// 	printf("\033[91mReadDirectoryChangesW failed\033[0m \n");
	// }
	dir_read_changes(dir);
}

void old() {
	for(int i=0; i<directory_count; ++i) {
		directory_t* dir = directories + i;
		// strcpy(dir->path, _argv[i+1]);

		dir->handle = CreateFileA(
			dir->path,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
			NULL);
		if(dir->handle == INVALID_HANDLE_VALUE) {
			printf("\033[91mCreateFileA failed\033[0m \n");
			exit(1);
		}

		dir->event_handle = CreateEventA(NULL, FALSE, FALSE, dir->path);
		dir->overlapped = (OVERLAPPED){0};
		dir->overlapped.hEvent = (void*)dir;

		dir_read_changes(dir);
	}

	HANDLE handles[64];
	FOR (i, directory_count) {
		// handles[i] = directories[i].handle;
		handles[i] = directories[i].event_handle;
	}

	for(;;) {
		DWORD wait_result = WaitForMultipleObjectsEx(directory_count, handles, FALSE, INFINITE, TRUE);
		// if(wait_result != WAIT_IO_COMPLETION) {
		// 	if (wait_result >= WAIT_OBJECT_0 && wait_result < WAIT_OBJECT_0 + directory_count) {
		// 		core_print("WaitForMultipleObjectsEx: WAIT_OBJECT_0 + %i", wait_result - WAIT_OBJECT_0);
		// 	} else if (wait_result >= WAIT_ABANDONED_0 && wait_result < WAIT_ABANDONED_0 + directory_count) {
		// 		core_print("WaitForMultipleObjectsEx: WAIT_ABANDONED_0 + %i", wait_result - WAIT_OBJECT_0);
		// 	} else if (wait_result == WAIT_TIMEOUT) {
		// 		core_print("WaitForMultipleObjectsEx: WAIT_TIMEOUT");
		// 	} else if (wait_result == WAIT_FAILED) {
		// 		core_print("WaitForMultipleObjectsEx: WAIT_FAILED");
		// 	} else {
		// 		core_print("WaitForMultipleObjectsEx: %s", wait_result);
		// 	}
		// }

		// GetOverlappedResult(directoryHandles[dirIndex], &directoryOverlapped[dirIndex], &bytesTransfered, FALSE);
		printf("reuslt %i %i \n", wait_result-WAIT_OBJECT_0, WAIT_IO_COMPLETION);
	}
}

int main(int argc, char **argv) {
	core_print(TERM_BRIGHT_YELLOW_FG "core watch (version %s)" TERM_RESET, VERSION);

	// if(argc < 2) {
	// 	printf("Arg 1 should be a directory to watch \n");
	// 	exit(1);
	// }

	u8 strBuffer[PAGE_SIZE/8];
	string_pool spool;
	s_create_pool(&spool, strBuffer, sizeof(strBuffer));
	s_pool(&spool);

	// HANDLE notificationHandle = FindFirstChangeNotificationA(
	// 	"./*.c",
	// 	TRUE,
	// 	FILE_NOTIFY_CHANGE_LAST_WRITE);
	// if(notificationHandle == INVALID_HANDLE_VALUE) {
	// 	printf("FindFirstChangeNotificationA \n");
	// 	printf("Failed with error code %d \n", GetLastError());
	// }

	for (int i=1; i<argc; ++i) {
		char* arg = argv[i];

		if (arg[0] == '-' && arg[1] == 'D') {
			// Directory
			if (directory_count < array_size(directories)) {
				directory_t* dir = directories + directory_count++;
				s_ncopy(dir->path, argv[i] + 2, array_size(dir->path));
			} else {
				core_error(FALSE, "Too many directories");
			}
		}

		if (arg[0] == '-' && arg[1] == 'B') {
			build_command = arg + 2;
		}
	}

	core_print(TERM_BRIGHT_BLUE_FG"watching: ");
	for(int i=0; i<directory_count; ++i) {
		// if (i) printf(" | ");
		core_print("    %s", directories[i].path);
		// if (i < dirCount-1) printf(", ");
	}
	core_print("build command:");
	core_print("    %s", build_command);
	printf("\n"TERM_RESET);

	// int dirCount = _argc-1;
	// if(dirCount > 64) {
	// 	printf("Too many directories \n");
	// 	exit(1);
	// }


	// char* cwd = paths[0];
	// SetCurrentDirectoryA(cwd);

	// if(dirCount > 1 && SIMULATE_MULTIPLE_DIRECTORY) {
	// 	dirCount = 1;
	// 	s_append(paths, "/..");
	// }

	DWORD filter = 0b11111111; //FILE_NOTIFY_CHANGE_LAST_WRITE
	HANDLE handles[2];
	handles[0] = FindFirstChangeNotification(directories[0].path, TRUE, filter);
	handles[1] = FindFirstChangeNotification(directories[1].path, TRUE, filter);

	if (handles[0] == INVALID_HANDLE_VALUE) {
		core_error(TRUE, "FindFirstChangeNotification");
	}
	if (handles[1] == INVALID_HANDLE_VALUE) {
		core_error(TRUE, "FindFirstChangeNotification");
	}

	for (;;) {
		
		// DWORD wait  = WaitForMultipleObjects(1, &handle, FALSE, INFINITE);
		DWORD wait  = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (wait == WAIT_OBJECT_0 || wait == WAIT_OBJECT_0+1) {

			u8 change_buffer[512] = {0};
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
					wprintf(change->FileName);
					printf(", ");
					if (change->NextEntryOffset) {
						change = (u8*)change + change->NextEntryOffset;
					} else {
						change = NULL;
					}
				}

				printf("\n\n");
			}

		} else {
			core_error(FALSE, "wait %i", wait-WAIT_OBJECT_0);
		}

		// m_zero(change_buffer, sizeof(change_buffer));

		if(!FindNextChangeNotification(handles[wait-WAIT_OBJECT_0])) {
			core_error(TRUE, "FindNextChangeNotification");
		}
	}
}
