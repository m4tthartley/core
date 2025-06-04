//
//  time_win32.c
//  Core
//
//  Created by Matt Hartley on 16/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "time.h"
#include <winnt.h>

u64 _win32_time_freq;
u64 _start_time;

static inline void time_load_win32_timebase() {
	if (!_win32_time_freq) {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		_win32_time_freq = freq.QuadPart/1000;
	}
	if (!_start_time) {
		LARGE_INTEGER start;
		QueryPerformanceCounter(&start);
		_start_time = start.QuadPart;
	}
}

frametimer_t frametimer_init() {
	time_load_win32_timebase();

	u64 time = time_get_raw();
	frametimer_t timer = {0};
	timer.start_time = time;
	timer.last_frame_time = time;
	timer.last_second_time = time;
	return timer;
}

void frametimer_update(frametimer_t* timer) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	timer->dt = (f64)(counter.QuadPart - timer->last_frame_time) /
			(f64)_win32_time_freq / 1000.0;
	timer->dt = min(timer->dt, 0.1f);
	timer->last_frame_time = counter.QuadPart;

	++timer->frame_counter;
	f32 seconds = (f64)(counter.QuadPart-timer->last_second_time) / (f64)_win32_time_freq / 1000.0;
	if(seconds > 1.0f) {
		timer->last_second_time = counter.QuadPart;
		timer->fps = timer->frame_counter;
		timer->frame_counter = 0;
	}
}

// Milliseconds
f64 time_get_ms() {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (f64)(counter.QuadPart-_start_time) / (f64)_win32_time_freq;
}

// Seconds
f64 time_get_seconds() {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (f64)(counter.QuadPart-_start_time) / (f64)_win32_time_freq / 1000.0;
}

u64 time_get_raw() {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}
