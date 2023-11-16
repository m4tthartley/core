//
//  main.c
//  Core Watch
//
//  Created by Matt Hartley on 01/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <core/core.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 6
#define VERSION_PATCH 1
#define VERSION_CREATEB(major, minor, patch) (#major "." #minor "." #patch)
#define VERSION_CREATEA(major, minor, patch) VERSION_CREATEB(major, minor, patch)
#define VERSION VERSION_CREATEA(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)
// #define VERSION "0.6.1"
#define SIMULATE_MULTIPLE_DIRECTORY 0

core_stat_t files[256];
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

core_stat_t* getFile(char* filename) {
	for(int i=0; i<file_count; ++i) {
		if(!strcmp(files[i].filename, filename)) {
			return files + i;
		}
	}
	return NULL;
}

core_stat_t* addFile(char* filename, core_stat_t info) {
	if (file_count < 256) {
		core_stat_t* file = files + file_count++;
		*file = info;
		return file;
	} else {
		core_error("Failed to add file");
	}
	return NULL;
}

void clear() {
	system("clear");
}

int build(char* filename) {
	printf(TERM_CLEAR);

	if (verbose_mode) {
		core_print("file changed %s \n", filename);
	}

	int result = system(core_strf("sh %s", build_command));

	if(!result) {
		core_print(TERM_BRIGHT_GREEN_FG "build successful" TERM_RESET);
	} else {
		core_print(TERM_BRIGHT_RED_FG "build failed" TERM_RESET);
	}

	return result;
}

void file_changes(core_stat_t* files, int count) {
	FOR (i, count) {
		core_stat_t* file = files + i;
		char* filename = files[i].filename;

		if( core_strfind(filename, ".c", 0) ||
			core_strfind(filename, ".h", 0) ||
			core_strfind(filename, ".txt", 0) ||
			core_strfind(filename, ".sh", 0)) {
			core_stat_t* saved_file = getFile(filename);

			if(saved_file) {
				if(file->modified - saved_file->modified < 1000) {
					continue;
				}
				saved_file->modified = file->modified;
			} else {
				addFile(filename, *file);
			}

			build(filename);
		}
	}
}

void print_usage() {
	core_print("");
	core_print("usage: watch [directories] [build script]...");
	core_print("options:");
	core_print("  -D<path>     A directory to watch, up to 64 directories.");
	core_print("  -B<script>   A bash script to run when file changes occur.");
	core_print("");
}

int main(int argc, char **argv) {
	core_print(TERM_BRIGHT_YELLOW_FG "core watch (version %s) \n" TERM_RESET, VERSION);

	// u8 strBuffer[PAGE_SIZE/8];
	// string_pool spool;
	// s_create_pool(&spool, strBuffer, sizeof(strBuffer));
	core_allocator_t arena = core_virtual_allocator(NULL, 0);
	core_use_allocator(&arena);

	for (int i=1; i<argc; ++i) {
		char* arg = argv[i];
		// core_print("directory %s", arg);

		if (arg[0] == '-' && arg[1] == 'D') {
			if (directory_count < array_size(directories)) {
				directory_t* dir = directories + directory_count++;
				if (argv[i][2]) {
					GetFullPathNameA(argv[i] + 2, CORE_MAX_PATH_LENGTH, dir->path, NULL);
				} else {
					++i;
					GetFullPathNameA(argv[i], CORE_MAX_PATH_LENGTH, dir->path, NULL);
				}

				core_handle_t handle = core_open_dir(dir->path);
				if (!handle) {
					core_error("Unable to find directory \"%s\"", dir->path);
					print_usage();
					exit(1);
				}
				core_close(handle);
			} else {
				core_error("too many directories");
				print_usage();
				exit(1);
			}
		}

		if (arg[0] == '-' && arg[1] == 'B') {
			if (arg[2]) {
				build_command = arg + 2;
			} else {
				++i;
				build_command = argv[i];
			}
		}
	}

	if (!directory_count) {
		core_error("no directories specified");
		print_usage();
		exit(1);
	}

	core_print(TERM_BRIGHT_YELLOW_FG"watching: ");
	for(int i=0; i<directory_count; ++i) {
		core_print(TERM_BRIGHT_BLUE_FG"    %s", directories[i].path);
	}
	core_print(TERM_BRIGHT_YELLOW_FG"build command:");
	core_print(TERM_BRIGHT_BLUE_FG"    %s", build_command);
	printf("\n"TERM_RESET);


	// Watch
	char* dirpaths[array_size(directories)];
	FOR (i, directory_count) {
		dirpaths[i] = directories[i].path;
	}

	core_directory_watcher_t watcher;
	core_watch_directory_changes(&watcher, dirpaths, directory_count);
	for (;;) {
		core_stat_t changes[64];
		int count = core_wait_for_directory_changes(&watcher, changes, 64);
		// FOR (i, count) {
		// 	// core_print("%s %llu", changes[i].filename, changes[i].modified);
		// }
		file_changes(changes, count);
	}
}
