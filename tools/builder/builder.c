//
//  macos.c
//  Builder
//
//  Created by Matt Hartley on 14/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#define CORE_IMPL
#include <core/core.h>
#include <core/time.h>

#ifdef __APPLE__
#	include "macos.c"
#endif
#ifdef __LINUX__
#	include "linux.c"
#endif


#define VERSION_MAJOR 3
#define VERSION_MINOR 2
#define VERSION_PATCH 0
#define VERSION_CREATEB(major, minor, patch) (#major "." #minor "." #patch)
#define VERSION_CREATEA(major, minor, patch) VERSION_CREATEB(major, minor, patch)
#define VERSION VERSION_CREATEA(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)


char* directories[64] = {0};
int directory_count = 0;
char* build_command = "./build.sh";
char* ext_list = "c,h,cpp,hpp,sh,bat";
uint64_t lastBuildTime;

char* exts[64];
int extCount = 0;

void* alloc(void* state, size_t size) {
	return malloc(size);
}

int build(char* filename) {
	file_t file = sys_open(filename);
	stat_t stat = sys_fstat(file);
	uint64_t fileTime = stat.modified;
	sys_close(file);

	if (fileTime < lastBuildTime) {
		return 0;
	}
	
	escape_basic_color(ESCAPE_BLUE, 1);
	print("\n[%s] \n\n", filename);
	escape_mode(ESCAPE_RESET);

	char* cmd = strformat("sh %s", build_command);
	f64 start = time_get_seconds();
	int result = system(cmd);
	f64 end = time_get_seconds();

	float time = (end-start);
	if(!result) {
		escape_color(ESCAPE_GREEN);
		print("\nbuild successful (%fs) \n", time);
		escape_mode(ESCAPE_RESET);
	} else {
		escape_color(ESCAPE_RED);
		print("\nbuild failed (%fs) \n", time);
		escape_mode(ESCAPE_RESET);
	}

	lastBuildTime = sys_timeofday();

	return result;
}

// void callback(char** files, int count) {
// 	// char** files = (char**)event_paths;
// 	FOR (i, count) {
// 		char* parts[8];
// 		int numParts = strsplit(parts, 8, files[i], ".");
// 		char* ext = parts[numParts-1];
// 		// if(
// 		//     strcompare(ext, "c") ||
// 		// 	strcompare(ext, "h") ||
// 		// 	strcompare(ext, "txt") ||
// 		// 	strcompare(ext, "sh") ||
// 		// 	strcompare(ext, "m") ||
// 		// 	strcompare(ext, "vert") ||
// 		// 	strcompare(ext, "frag") ||
// 		// 	strcompare(ext, "glsl")
// 		// ) {
// 		//     build(files[i]);
// 		// }
// 		_Bool doBuild = _False;
// 		for (int i=0; i<extCount; ++i) {
// 			if (strcompare(ext, exts[i])) {
// 				doBuild = _True;
// 				break;
// 			}
// 		}

// 		if (doBuild) {
// 			build(files[i]);
// 		}

// 		// FOR(si, numParts) {
// 		// 	str_free(parts[si]);
// 		// }
// 	}
// }

void callback(char* file) {

	char* parts[8];
	int numParts = strsplit(parts, 8, file, ".");
	char* ext = parts[numParts-1];

	_Bool doBuild = _False;
	for (int i=0; i<extCount; ++i) {
		if (strcompare(ext, exts[i])) {
			doBuild = _True;
			break;
		}
	}

	if (doBuild) {
		build(file);
	}
}

void print_usage() {
	print(" \n");
	print("usage: watch [directories] [build script]... \n");
	print("options: \n");
	print("  -D<path>     A directory to watch, up to 64 directories. \n");
	print("  -B<script>   A bash script to run when file changes occur. \n");
	print(" \n");
}

void signal_handler(int signal) {
	if (signal == SIGINT) {
		Stop();
		exit(0);
	}
}

int main(int argc, char** argv) {

	strsetallocproc(alloc, NULL);

	escape_color(escape_basic_color(ESCAPE_YELLOW, 1));
	print("Builder %s \n\n", VERSION);
	escape_mode(ESCAPE_RESET);

	for (int i=1; i<argc; ++i) {
		char* arg = argv[i];

		if (arg[0] == '-' && arg[1] == 'D') {
			if (directory_count < array_size(directories)) {
				char* path = arg + 2;
				directories[directory_count++] = path;
				file_t handle = sys_open_dir(path);
				if (!handle) {
					print_err("Unable to find directory \"%s\" \n", path);
					print_usage();
					exit(1);
				}
				sys_close(handle);

				escape_basic_color(ESCAPE_GREEN, _True);
				print("[%s] \n", path);
			} else {
				print_err("too many directories \n");
				print_usage();
				exit(1);
			}
		}

		if (arg[0] == '-' && arg[1] == 'B') {
			build_command = arg + 2;
		}

		if (arg[0] == '-' && arg[1] == 'E') {
			ext_list = arg + 2;
		}
	}

	if (!directory_count) {
		print_err("no directories specified \n");
		print_usage();
		exit(1);
	}

	extCount = strsplit(exts, 64, ext_list, ",");
	for (int i=0; i<extCount; ++i) {
		exts[i] = strstore(exts[i]);
	}

	escape_basic_color(ESCAPE_BLUE, 1);
	print("{%s} \n\n\n", build_command);
	escape_mode(ESCAPE_RESET);

	signal(SIGINT, signal_handler);
	lastBuildTime = sys_timeofday();

	Run(directories, directory_count, callback);

	return 0;
}