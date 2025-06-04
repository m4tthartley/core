//
//  time.h
//  Core
//
//  Created by Matt Hartley on 27/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//


#ifndef __CORE_TIME_HEADER__
#define __CORE_TIME_HEADER__


#include "core.h"


typedef struct {
	u64 performance_freq;

	f32 dt;
	u64 start_time;
	u64 last_frame_time;
	u64 last_second_time;
	int frame_counter;
	int fps;
} frametimer_t;

typedef struct {
	uint64_t startTime;
	uint64_t endTime;
	uint64_t cycles;
	f64 milliseconds;
} timeblock_t;

// timer_t create_timer();
// f64 get_time(timer_t* timer);
// f64 get_time_seconds(timer_t* timer);
// u64 get_time_raw(timer_t* timer);
// void update_timer(timer_t* timer);

frametimer_t    frametimer_init();
void 			frametimer_update(frametimer_t* time);
f64 			time_get_ms();
f64 			time_get_seconds();
u64 			time_get_raw();
float64_t		time_raw_to_milliseconds(uint64_t time);
float64_t 		time_raw_to_seconds(uint64_t time);

timeblock_t 	time_start_block();
void 			time_end_block(timeblock_t* block);


#	ifdef CORE_IMPL


#if defined(__WIN32__)
#	include "time_win32.c"
#elif defined(__LINUX__)
#	include "time_linux.c"
#elif defined(__MACOS__)
#	include "time_macos.c"
#else
#   error "NO PLATFORM IS DEFINED"
#endif


#	endif
#endif