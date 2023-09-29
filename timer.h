
#include "core.h"

typedef struct {
	f32 dt;
	u64 performance_freq;
	u64 start_time;
	u64 last_frame_time;
	u64 last_second_time;
	int frame_counter;
} core_timer_t;

void core_timer(core_timer_t* timer) {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	timer->performance_freq = freq.QuadPart/1000;
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);
	timer->start_time = start.QuadPart;
	timer->last_second_time = timer->start_time;
	timer->last_frame_time = timer->start_time;
	timer->frame_counter = 0;
}

f64 core_time(core_timer_t* timer) { // Milliseconds
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (f64)(counter.QuadPart-timer->start_time) / (f64)timer->performance_freq;
}

f64 core_time_seconds(core_timer_t* timer) { // Seconds
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (f64)(counter.QuadPart-timer->start_time) / (f64)timer->performance_freq / 1000.0;
}

u64 core_time_raw(core_timer_t* timer) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

void core_timer_update(core_timer_t* timer) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	timer->dt = (f64)(counter.QuadPart - timer->last_frame_time) /
			(f64)timer->performance_freq / 1000.0;
	timer->dt = min(timer->dt, 0.1f);
	timer->last_frame_time = counter.QuadPart;

	++timer->frame_counter;
	f32 seconds = (f64)(counter.QuadPart-timer->last_second_time) / (f64)timer->performance_freq / 1000.0;
	if(seconds > 1.0f) {
		// printf("fps %i, dt %f \n", timer->frame_counter, timer->dt);
		timer->last_second_time = counter.QuadPart;
		timer->frame_counter = 0;
	}
}
