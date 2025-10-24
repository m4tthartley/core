/*
	Created by Matt Hartley on 23/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/

#include "core/sys.h"
#include <linux/limits.h>
#include <sys/inotify.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <stdlib.h>
#include <core/print.h>


int fd;

void Run(char** dirs, int count, void (*callback)(char* file))
{
	fd = inotify_init1(0);
	if (fd == -1) {
		print_err("Error initializing inotify: %i", errno);
		exit(1);
	}

	int watchFds[64];
	for (int i=0; i<count; ++i) {
		int addfd = inotify_add_watch(fd, dirs[i], IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
		if (addfd == -1) {
			print_err("Error adding directory to inotify: %i", errno);
		}
		watchFds[i] = addfd;
	}

	uint8_t buffer[4096] __attribute__((aligned(8)));
	struct inotify_event* event;
	for (;;) {
		ssize_t readSize = read(fd, buffer, sizeof(buffer));

		event = (struct inotify_event*)buffer;
		while ((uintptr_t)event < (uintptr_t)buffer+readSize) {
			char path[MAX_PATH_LENGTH];
			for (int w=0; w<count; ++w) {
				if (event->wd == watchFds[w]) {
					sprint(path, MAX_PATH_LENGTH, "%s/%s", dirs[w], event->name);
					break;
				}
			}
			
			callback(path);

			event += sizeof(struct inotify_event) + event->len;
		}
	}
}

void Stop()
{
	close(fd);
}
