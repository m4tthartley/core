/*
	Created by Matt Hartley on 24/10/2025.
	Copyright 2024 GiantJelly. All rights reserved.
*/

#include <stdint.h>
#include <time.h>


double time_get_seconds() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	// uint64_t ns = (ts.tv_sec * 1000000000) + ts.tv_nsec;
	// double result = (double)ns / 1000000000;

	double result = (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);

	return result;
}

double time_get_ms() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	// uint64_t ns = (ts.tv_sec * 1000000000) + ts.tv_nsec;
	// double result = (double)ns / 1000000000;

	double result = (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000.0);

	return result;
}
