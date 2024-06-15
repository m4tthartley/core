//
//  macos.c
//  Builder
//
//  Created by Matt Hartley on 14/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#include <CoreServices/CoreServices.h>

#include <core/core.h>
#include <core/core.c>

#define CORE_IMPL
#include <core/time.h>

#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define VERSION_CREATEB(major, minor, patch) (#major "." #minor "." #patch)
#define VERSION_CREATEA(major, minor, patch) VERSION_CREATEB(major, minor, patch)
#define VERSION VERSION_CREATEA(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)

CFStringRef directories[64] = {0};
int directory_count = 0;
char* build_command = "./build.sh";

int build(char* filename) {
	print_inline(TERM_CLEAR);

    print("[%s] \n", filename);

	char* cmd = str_format("sh %s", build_command);
	f64 start = time_get_seconds();
	int result = system(cmd);
	f64 end = time_get_seconds();
	free_memory(cmd);

	float time = (end-start);
	if(!result) {
		print(TERM_BRIGHT_GREEN_FG "build successful (%.2fs)" TERM_RESET, time);
	} else {
		print(TERM_BRIGHT_RED_FG "build failed (%.2fs)" TERM_RESET, time);
	}

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
        if(
            str_find(files[i], ".c") ||
			str_find(files[i], ".h") ||
			str_find(files[i], ".txt") ||
			str_find(files[i], ".sh")
        ) {
            // print("File: %s", files[i]);
            build(files[i]);
        }
    }
}

void print_usage() {
	print("");
	print("usage: watch [directories] [build script]...");
	print("options:");
	print("  -D<path>     A directory to watch, up to 64 directories.");
	print("  -B<script>   A bash script to run when file changes occur.");
	print("");
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

    print(TERM_BRIGHT_YELLOW_FG "Builder %s \n" TERM_RESET, VERSION);

    u8 buffer[1024];
    allocator_t allocator = create_allocator(buffer, sizeof(buffer));
    use_allocator(&allocator);

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

				file_t handle = file_open_dir(path);
				if (!handle) {
					print_error("Unable to find directory \"%s\"", path);
					print_usage();
					exit(1);
				}
				file_close(handle);

                print(TERM_BRIGHT_GREEN_FG "[%s]", path);
			} else {
				print_error("too many directories");
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
	}

    if (!directory_count) {
		print_error("no directories specified");
		print_usage();
		exit(1);
	}

	// print(TERM_BRIGHT_YELLOW_FG"watching: ");
	// for(int i=0; i<directory_count; ++i) {
	// 	print(TERM_BRIGHT_BLUE_FG"    %s", directories[i]);
	// }
	// print(TERM_BRIGHT_YELLOW_FG"build command:");
	print(TERM_BRIGHT_BLUE_FG "{%s}", build_command);
	print("\n"TERM_RESET);

    // Start File System Events loop
    // CFStringRef dirs[] = {
    //     CFStringCreateWithCString(NULL, "../core", kCFStringEncodingUTF8),
    //     CFStringCreateWithCString(NULL, "../watch", kCFStringEncodingUTF8),
    // };
    
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