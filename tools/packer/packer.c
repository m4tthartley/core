//
//  packer.c
//  Core Packer
//
//  Created by Matt Hartley on 28/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <core.h>
#include <stdio.h>

typedef void* f_handle;
typedef struct {
	u64 created;
	u64 modified;
	size_t size;
} f_info;

f_handle f_open(char* path) {
	assert(sizeof(HANDLE)<=sizeof(f_handle));
	
	HANDLE handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
								0, /*OPEN_EXISTING*/OPEN_ALWAYS, 0, 0);
	if(handle==INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		LPTSTR msg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
					  FORMAT_MESSAGE_FROM_SYSTEM|
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL, error,
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR)&msg, 0, NULL);
		// uiMessage("%i, %s", GetLastError(), msg);
		return 0;
	}
	return handle;
}

int f_read(f_handle file, int offset, void* output, size_t size) { // 32bit only?
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

void f_write(f_handle file, int offset, void* data, size_t size) { // 32bit only?
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

typedef struct {
	char code[4];
	size_t size;
	size_t list;
	size_t file_count;
} directory_header_t;

typedef struct {
	char name[MAX_PATH];
	size_t offset; // from top
	size_t size;
	// char name[];
} file_header_t;

int main(int argc, char** argv) {
	char* exe_file = argv[1];
	char* ext[16];
	int extc = 0;
	FOR (i, argc-2) {
		if (extc < 16) {
			ext[extc++] = argv[i+2];
		}
	}

	string_pool spool;
	s_create_pool(&spool, 0, 0);
	m_reserve(&spool, GB(1), PAGE_SIZE);
	s_pool(&spool);
	directory_header_t dir;
	// dir.code = { 'C', 'D', 'I', 'R' };
	dir.code[0] = 'C';
	dir.code[1] = 'D';
	dir.code[2] = 'I';
	dir.code[3] = 'R';
	dir.size = 53;

	// FILE* wfile = fopen("../reloader/build/test.exe", "w");
	// fseek(wfile, 0, SEEK_END);
	// fwrite(&dir, 1, sizeof(directory_header_t), wfile);
	// fclose(wfile);
	//
	// FILE* file = fopen("../reloader/build/test.exe", "r");
	// if(!file) {
	// 	printf("cant open file\n");
	// }
	// fseek(file, 0, SEEK_END);
	// int size = ftell(file);
	// void* data = malloc(size+1);
	// memset(data, 0, size+1);
	// rewind(file);
	// fread(data, 1, size, file);
	// fclose(file);
	//
	// directory_header_t* header = data + (size - sizeof(directory_header_t));
	// printf("header \n");
	
	m_arena mem;
	m_stack(&mem, 0, 0);
	m_reserve(&mem, GB(1), PAGE_SIZE);

	dynarr_t files = dynarr(sizeof(file_header_t));

	WIN32_FIND_DATA find_data;
	HANDLE find = FindFirstFile("*.*", &find_data);
	while (FindNextFile(find, &find_data)) {
		int x = 0;
		printf("file %s \n", find_data.cFileName);
		if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			FOR (i, extc) {
				if (s_find(find_data.cFileName, ext[i], NULL)) {
					printf("packing %s...", find_data.cFileName);

					HANDLE handle = f_open(find_data.cFileName);
					size_t size = ((size_t)find_data.nFileSizeHigh << 32) | find_data.nFileSizeLow;
					file_header_t header;
					header.offset = mem.stack;
					header.size = size;
					memcpy(header.name, find_data.cFileName, s_len(find_data.cFileName)+1);
					dynarr_push(&files, &header);
					void* data = m_push(&mem, size);
					f_read(handle, 0, data, size);
					f_close(handle);

					break;
				}
			}
		}
	}
	FindClose(find);

	dir.list = mem.stack;
	dir.file_count = files.count;
	FORDYNARR (i, files) {
		memcpy(
			m_push(&mem, sizeof(file_header_t)),
			dynarr_get(&files, i),
			sizeof(file_header_t)
		);
	}

	dir.size = mem.stack + sizeof(directory_header_t);
	memcpy(
		m_push(&mem, sizeof(directory_header_t)),
		&dir,
		sizeof(directory_header_t)
	);

	f_handle input = f_open(exe_file);
	f_info stat = f_stat(input);
	void* input_data = m_push(&mem, stat.size);
	f_read(input, 0, input_data, stat.size);
	f_close(input);
	f_handle output = f_open(s_format("%s.packed", exe_file));
	f_write(output, 0, input_data, stat.size);
	f_write(output, stat.size, mem.address, dir.size);
	f_close(output);

	printf("done \n");
}
