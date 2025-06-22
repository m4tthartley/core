//
//  macos.c
//  Builder
//
//  Created by Matt Hartley on 14/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#include <CoreServices/CoreServices.h>

#define CORE_IMPL
#include <core/core.h>
#include <core/time.h>


#define VERSION_MAJOR 2
#define VERSION_MINOR 2
#define VERSION_PATCH 0
#define VERSION_CREATEB(major, minor, patch) (#major "." #minor "." #patch)
#define VERSION_CREATEA(major, minor, patch) VERSION_CREATEB(major, minor, patch)
#define VERSION VERSION_CREATEA(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)


CFStringRef directories[64] = {0};
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
	// print_inline(TERM_CLEAR);

	file_t file = sys_open(filename);
	stat_t stat = sys_stat(file);
	uint64_t fileTime = stat.modified;
	// print("file modified  : %lu \n", fileTime);
	// print("last build time: %lu \n", lastBuildTime);
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
	// gfree_memory(cmd);

	float time = (end-start);
	if(!result) {
		escape_color(ESCAPE_GREEN);
		print("\nbuild successful (%.2fs) \n", time);
		escape_mode(ESCAPE_RESET);
	} else {
		escape_color(ESCAPE_RED);
		print("\nbuild failed (%.2fs) \n", time);
		escape_mode(ESCAPE_RESET);
	}

	lastBuildTime = sys_time();

	return result;
}

void callback(
    ConstFSEventStreamRef streamRef,
    void* client_callback_info,
    size_t num_events,
    void* event_paths,
    const FSEventStreamEventFlags event_flags[],
    const FSEventStreamEventId event_ids[]
) {
    char** files = (char**)event_paths;
    FOR (i, num_events) {
		char* parts[8];
		int numParts = strsplit(parts, 8, files[i], ".");
		char* ext = parts[numParts-1];
        // if(
        //     strcompare(ext, "c") ||
		// 	strcompare(ext, "h") ||
		// 	strcompare(ext, "txt") ||
		// 	strcompare(ext, "sh") ||
		// 	strcompare(ext, "m") ||
		// 	strcompare(ext, "vert") ||
		// 	strcompare(ext, "frag") ||
		// 	strcompare(ext, "glsl")
        // ) {
        //     build(files[i]);
        // }
		_Bool doBuild = _False;
		for (int i=0; i<extCount; ++i) {
			if (strcompare(ext, exts[i])) {
				doBuild = _True;
				break;
			}
		}

		if (doBuild) {
			build(files[i]);
		}

		// FOR(si, numParts) {
		// 	str_free(parts[si]);
		// }
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

FSEventStreamRef stream;
// dispatch_queue_t dispatch_queue;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        FSEventStreamStop(stream);
        FSEventStreamInvalidate(stream);
        FSEventStreamRelease(stream);
        // dispatch_release(dispatch_queue);
        exit(0);
    }
}

int main(int argc, char** argv) {

	strsetallocproc(alloc, NULL);

	escape_color(escape_basic_color(ESCAPE_YELLOW, 1));
    print("Builder %s \n\n", VERSION);
	escape_mode(ESCAPE_RESET);

    // u8 buffer[1024];
    // allocator_t allocator = heap_allocator(buffer, sizeof(buffer));
    // use_allocator(&allocator);
	// str_set_allocator(&allocator);

    for (int i=1; i<argc; ++i) {
		char* arg = argv[i];

		if (arg[0] == '-' && arg[1] == 'D') {
			if (directory_count < array_size(directories)) {
				CFStringRef* dir = directories + directory_count++;
                char* path = arg + 2;
				// if (argv[i][2]) {
				// 	GetFullPathNameA(argv[i] + 2, MAX_PATH_LENGTH, dir->path, NULL);
				// } else {
				// 	++i;
				// 	GetFullPathNameA(argv[i], CORE_MAX_PATH_LENGTH, dir->path, NULL);
				// }

                *dir = CFStringCreateWithCString(NULL, path, kCFStringEncodingUTF8);

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
			// if (arg[2]) {
				build_command = arg + 2;
			// } else {
			// 	++i;
			// 	build_command = arg;
			// }
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

	// char* extSplit[64];
	extCount = strsplit(exts, 64, ext_list, ",");
	for (int i=0; i<extCount; ++i) {
		exts[i] = strstore(exts[i]);
	}

	// print(TERM_BRIGHT_YELLOW_FG"watching: ");
	// for(int i=0; i<directory_count; ++i) {
	// 	print(TERM_BRIGHT_BLUE_FG"    %s", directories[i]);
	// }
	// print(TERM_BRIGHT_YELLOW_FG"build command:");
	escape_basic_color(ESCAPE_BLUE, 1);
	print("{%s} \n\n\n", build_command);
	escape_mode(ESCAPE_RESET);

    // Start File System Events loop
    // CFStringRef dirs[] = {
    //     CFStringCreateWithCString(NULL, "../core", kCFStringEncodingUTF8),
    //     CFStringCreateWithCString(NULL, "../watch", kCFStringEncodingUTF8),
    // };

	lastBuildTime = sys_time();
    
    CFArrayRef dir_array = CFArrayCreate(NULL, (const void**)directories, directory_count, NULL);
    FSEventStreamContext context = {0, NULL, NULL, NULL, NULL};

    stream = FSEventStreamCreate(
        NULL,
        callback,
        &context,
        dir_array,
        kFSEventStreamEventIdSinceNow,
        0.5,
        kFSEventStreamCreateFlagFileEvents
    );

    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    // dispatch_queue = dispatch_queue_create("fsevents_queue", NULL);
    // FSEventStreamSetDispatchQueue(stream, dispatch_queue);

    FSEventStreamStart(stream);
    
    signal(SIGINT, signal_handler);
    CFRunLoopRun();
    // dispatch_main();

    return 0;
}