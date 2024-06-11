//
//  time_macos.c
//  Core
//
//  Created by Matt Hartley on 08/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#include "time.h"

#include <mach/mach_time.h>

struct mach_timebase_info mach_timebase = {0};
u64 _start_time = 0;

static inline void time_load_mach_timebase() {
	if (!mach_timebase.denom) {
		mach_timebase_info(&mach_timebase);
	}
	if (!_start_time) {
		_start_time = mach_absolute_time();
	}
}

gametime_t time_init() {
    time_load_mach_timebase();

    u64 time = mach_absolute_time();

	gametime_t state = {0};
	state.start_time = time;
	state.last_frame_time = time;
	state.last_second_time = time;
	return state;
}

void time_update(gametime_t* time) {
    time_load_mach_timebase();

    u64 ticks = mach_absolute_time();

	u64 freq = mach_timebase.denom * 1000000000 / mach_timebase.numer;

	time->dt = (f64)(ticks - time->last_frame_time) / (f64)freq;
	time->dt = min(time->dt, 0.1f);
	time->last_frame_time = ticks;

	++time->frame_counter;
	f32 seconds = (f64)(ticks-time->last_second_time) / (f64)freq;
	if(seconds > 1.0f) {
		time->last_second_time = ticks;
		time->fps = time->frame_counter;
		time->frame_counter = 0;
	}
}

// Milliseconds
f64 time_get_ms() {
    time_load_mach_timebase();

	u64 freq_ms = mach_timebase.denom * 1000000 / mach_timebase.numer;

    u64 ticks = mach_absolute_time();
    f64 ms = (f64)(ticks - _start_time) / (f64)freq_ms;
    return ms;
}

// Seconds
f64 time_get_seconds() {
    time_load_mach_timebase();

	u64 freq = mach_timebase.denom * 1000000000 / mach_timebase.numer;

    u64 ticks = mach_absolute_time();
    f64 seconds = (f64)(ticks - _start_time) / (f64)freq;
    return seconds;
}

u64 time_get_raw() {
    u64 ticks = mach_absolute_time();
	return ticks;
}
