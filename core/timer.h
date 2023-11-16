//
//  timer.h
//  Core
//
//  Created by Matt Hartley on 27/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "core.h"

typedef struct {
	f32 dt;
	u64 performance_freq;
	u64 start_time;
	u64 last_frame_time;
	u64 last_second_time;
	int frame_counter;
	int fps;
} core_timer_t;

void core_timer(core_timer_t* timer);
f64 core_time(core_timer_t* timer);
f64 core_time_seconds(core_timer_t* timer);
u64 core_time_raw(core_timer_t* timer);
void core_timer_update(core_timer_t* timer);
