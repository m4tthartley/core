/*
	Created by Matt Hartley on 23/10/2025.
	Copyright 2025 GiantJelly. All rights reserved.
*/


#include <CoreServices/CoreServices.h>

FSEventStreamRef stream;
// dispatch_queue_t dispatch_queue;

void (*__callback)(char* file) = 0;

void MacosCallback(
	ConstFSEventStreamRef streamRef,
	void* client_callback_info,
	size_t num_events,
	void* event_paths,
	const FSEventStreamEventFlags event_flags[],
	const FSEventStreamEventId event_ids[]
)
{
	for (int i=0; i<num_events; ++i) {
		__callback(((char**)event_paths)[i]);
	}
}

void Run(char* dirs, int count, void (*callback)(char* file))
{
	__callback = callback;

	CFStringRef directories[64] = {0};
	for (int i=0; i<count; ++i) {
		directories[i] = CFStringCreateWithCString(NULL, dirs[i], kCFStringEncodingUTF8);
	}

	CFArrayRef dir_array = CFArrayCreate(NULL, (const void**)directories, directory_count, NULL);
    FSEventStreamContext context = {0, NULL, NULL, NULL, NULL};

    stream = FSEventStreamCreate(
        NULL,
        MacosCallback,
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
    
    CFRunLoopRun();
    // dispatch_main();
}

void Stop()
{
	FSEventStreamStop(stream);
	FSEventStreamInvalidate(stream);
	FSEventStreamRelease(stream);
	// dispatch_release(dispatch_queue);
}
