//
//  time_win32.c
//  Core
//
//  Created by Matt Hartley on 16/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "time.h"

timer_t create_timer() {
	timer_t timer = {0};
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	timer.performance_freq = freq.QuadPart/1000;
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);
	timer.start_time = start.QuadPart;
	timer.last_second_time = timer.start_time;
	timer.last_frame_time = timer.start_time;
	timer.frame_counter = 0;
	return timer;
}

// Milliseconds
f64 get_time(timer_t* timer) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (f64)(counter.QuadPart-timer->start_time) / (f64)timer->performance_freq;
}

// Seconds
f64 get_time_seconds(timer_t* timer) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (f64)(counter.QuadPart-timer->start_time) / (f64)timer->performance_freq / 1000.0;
}

u64 get_time_raw(timer_t* timer) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

void update_timer(timer_t* timer) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	timer->dt = (f64)(counter.QuadPart - timer->last_frame_time) /
			(f64)timer->performance_freq / 1000.0;
	timer->dt = min(timer->dt, 0.1f);
	timer->last_frame_time = counter.QuadPart;

	++timer->frame_counter;
	f32 seconds = (f64)(counter.QuadPart-timer->last_second_time) / (f64)timer->performance_freq / 1000.0;
	if(seconds > 1.0f) {
		timer->last_second_time = counter.QuadPart;
		timer->fps = timer->frame_counter;
		timer->frame_counter = 0;
	}
}
