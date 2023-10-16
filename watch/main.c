#include <windows.h>
#include <stdio.h>
#include <locale.h>

#include <core/core.h>

#define VERSION "0.5.0"
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
	char path[MAX_PATH];
	FILE_NOTIFY_INFORMATION change_buffer[64];
} directory_t;

directory_t directories[64] = {0};
int directory_count = 0;

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
		core_error_console(FALSE, "Failed to add file");
	}
}

void clear() {
	// system("cls");
}

int build(char* filename) {
	char cwd[MAX_PATH] = {0};
	GetCurrentDirectoryA(MAX_PATH, cwd);
	SetCurrentDirectoryA(directories[0].path);

	clear();
	printf("file changed %s \n", filename);

	int result = system("sh ./build.sh");
	// printf("result %i \n", result);

	if(!result) {
		printf("\033[92mbuild successful\033[0m \n");
	} else {
		printf("\033[91mbuild failed\033[0m \n");
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
					
					// build(filename);
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
			&completionRoutine);
	if(!read) {
		printf("\033[91mReadDirectoryChangesW failed\033[0m \n");
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

int main(int _argc, char **_argv) {
	printf("\033[93mcore watch version %s\033[0m \n", VERSION);

	if(_argc < 2) {
		printf("Arg 1 should be a directory to watch \n");
		exit(1);
	}

	u8 strBuffer[PAGE_SIZE];
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

	int dirCount = _argc-1;
	if(dirCount > 64) {
		printf("Too many directories \n");
		exit(1);
	}


	// char* cwd = paths[0];
	// SetCurrentDirectoryA(cwd);

	// if(dirCount > 1 && SIMULATE_MULTIPLE_DIRECTORY) {
	// 	dirCount = 1;
	// 	s_append(paths, "/..");
	// }

	printf("\033[93mwatching:\033[0m ");
	for(int i=0; i<dirCount; ++i) {
		printf("\033[93m%s\033[0m ", _argv[i+1]);
		// if (i < dirCount-1) printf(", ");
	}
	printf("\n");
	
	for(int i=0; i<dirCount; ++i) {
		directory_t* dir = directories + directory_count;
		strcpy(dir->path, _argv[i+1]);

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
		}

		dir->event_handle = CreateEventA(NULL, FALSE, FALSE, dir->path);
		dir->overlapped = (OVERLAPPED){0};
		dir->overlapped.hEvent = (void*)dir;

		dir_read_changes(dir);

		directory_count++;
	}

	HANDLE handles[64];
	FOR (i, directory_count) {
		handles[i] = directories[i].handle;
	}

	for(;;) {
		DWORD wait_result = WaitForMultipleObjectsEx(directory_count, handles, FALSE, INFINITE, TRUE);
		if(wait_result != WAIT_IO_COMPLETION) {
			// printf("\033[91mWaitForMultipleObjectsEx returned something other than WAIT_IO_COMPLETION: %i\033[0m \n", waitResult);

			if (wait_result >= WAIT_OBJECT_0 && wait_result < WAIT_OBJECT_0 + directory_count) {
				core_print("WaitForMultipleObjectsEx: WAIT_OBJECT_0 + %i", wait_result - WAIT_OBJECT_0);
			} else if (wait_result >= WAIT_ABANDONED_0 && wait_result < WAIT_ABANDONED_0 + directory_count) {
				core_print("WaitForMultipleObjectsEx: WAIT_ABANDONED_0 + %i", wait_result - WAIT_OBJECT_0);
			} else if (wait_result == WAIT_TIMEOUT) {
				core_print("WaitForMultipleObjectsEx: WAIT_TIMEOUT");
			} else if (wait_result == WAIT_FAILED) {
				core_print("WaitForMultipleObjectsEx: WAIT_FAILED");
			} else {
				core_print("WaitForMultipleObjectsEx: %s", wait_result);
			}
		}
		// TODO what is waitResult now?
		// printf("waitResult %i \n", waitResult);

		// if(waitResult >= WAIT_OBJECT_0+argc-1) {
			// printf("\033[91mwait error %i\033[0m \n", waitResult);
			// exit(1);
		// }

		// int dirIndex = waitResult-WAIT_OBJECT_0;

		// long bytesTransfered;
		// GetOverlappedResult(directoryHandles[dirIndex], &directoryOverlapped[dirIndex], &bytesTransfered, FALSE);
	}
}
