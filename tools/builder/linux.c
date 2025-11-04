/*
	Created by Matt Hartley on 23/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include <linux/limits.h>
#include <sys/inotify.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <stdlib.h>
#include <core/sys.h>
#include <core/core.h>
#include <core/print.h>


int fd;
stat_t watchFds[64];

void AddWatch(char* path)
{
	int addfd = inotify_add_watch(fd, path, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
	if (addfd == -1) {
		print_err("Error adding directory to inotify: %i", errno);
		return;
	}
	assert(addfd < array_size(watchFds));
	stat_t info = sys_stat(path);
	assert(info.is_directory);
	watchFds[addfd] = info;
	print("%s -> %i \n", path, addfd);

	sys_listing_t listing = sys_listing(path);
	while (sys_next(&listing)) {
		if (listing.file.is_directory) {
			AddWatch(strformat("%s/%s", path, listing.file.filename));
		}
	}
}

void Run(char** dirs, int count, void (*callback)(char* file))
{
	fd = inotify_init1(0);
	if (fd == -1) {
		print_err("Error initializing inotify: %i", errno);
		exit(1);
	}

	for (int i=0; i<count; ++i) {
		AddWatch(dirs[i]);
	}

	uint8_t buffer[4096] __attribute__((aligned(8)));
	for (;;) {
		ssize_t readSize = read(fd, buffer, sizeof(buffer));

		uint8_t* eventPtr = buffer;
		while ((uintptr_t)eventPtr < (uintptr_t)buffer+readSize) {
			struct inotify_event* event = (struct inotify_event*)eventPtr;
			assert(event->wd < array_size(watchFds));
			if (event->wd < array_size(watchFds)) {
				char path[MAX_PATH_LENGTH];
				sprint(path, MAX_PATH_LENGTH, "%s/%s", watchFds[event->wd].filename, event->name);

				if (event->mask & IN_CREATE) {
					stat_t info = sys_stat(path);
					if (info.is_directory) {
						print("Directory created and added \n");
						AddWatch(path);
					}
				}
				
				callback(path);
			} else {
				print_err("Invalid WD: %i \n", event->wd);
			}

			eventPtr += sizeof(struct inotify_event) + event->len;
		}
	}
}

void Stop()
{
	close(fd);
}
