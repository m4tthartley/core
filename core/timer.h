//
//  timer.h
//  Core
//
//  Created by Matt Hartley on 27/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_TIME_HEADER__
#define __CORE_TIME_HEADER__


#include "core.h"

typedef struct {
	f32 dt;
	u64 performance_freq;
	u64 start_time;
	u64 last_frame_time;
	u64 last_second_time;
	int frame_counter;
	int fps;
} timer_t;

timer_t create_timer();
f64 time(timer_t* timer);
f64 time_seconds(timer_t* timer);
u64 time_raw(timer_t* timer);
void update_timer(timer_t* timer);


#endif