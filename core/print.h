//
//  Created by Matt Hartley on 22/03/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef __CORE_PRINT_HEADER__
#define __CORE_PRINT_HEADER__

#include <stdint.h>


int sprint_int(char* buf, int len, int32_t num);
int sprint_float(char* buf, int len, float input, int precision);


#endif


#ifdef CORE_IMPL
#	ifndef __CORE_PRINT_HEADER_IMPL__
#	define __CORE_PRINT_HEADER_IMPL__

#ifndef __EDITOR__
#	ifndef __CORE_HEADER__
#		error "core.h must be included before wave.h"
#	endif
#else
#	include "math.h"
#	include "imath.h"
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <math.h>


int sprint_u64(char* buf, int len, uint64_t num) {
	int ci = 0;
	if (!num) {
		if (ci<len-1) {
			buf[ci++] = '0';
		}
		buf[ci] = 0;
		return ci;
	}
	
	int l = ilog10_64(num);
	if (ci+l+1 < len-1) {
		for (int i=0; i<l+1; ++i) {
			buf[ci + (l-i)] = num%10 + '0';
			num /= 10;
		}
		ci += l+1;
	}

	buf[ci] = 0;
	return ci;
}

int sprint_i64(char* buf, int len, int64_t num) {
	int ci = 0;
	if (num < 0) {
		buf[ci++] = '-';
	}
	num = labs(num);
	ci += sprint_u64(buf+ci, len-ci, num);
	return ci;
}

int sprint_u32(char* buf, int len, uint32_t num) {
	return sprint_u64(buf, len, num);
}

int sprint_int(char* buf, int len, int32_t num) {
	int ci = 0;
	if (num < 0) {
		buf[ci++] = '-';
	}
	num = abs(num);
	ci += sprint_u64(buf+ci, len-ci, num);
	return ci;
}

int sprint_f64(char* buf, int len, double input, int precision) {
	double value = fabs(input);
	int ci = 0;
	if (input < 0.0f) {
		buf[ci++] = '-';
	}

	int64_t wholePart = value;
	double fractPart = value - (double)wholePart;
	ci += sprint_i64(buf+ci, len-ci, wholePart);

	if (ci<len-1) {
		buf[ci++] = '.';
	}

	for (int i=0; i<precision && ci<len-1; ++i) {
		fractPart = fmodf(fractPart*10, 10);
		int fractInt = fractPart;
		buf[ci++] = '0' + fractInt;
	}

	buf[ci] = 0;
	return ci;
}

int sprint_float(char* buf, int len, float input, int precision) {
	return sprint_f64(buf, len, input, precision);
}

int sprint_str(char* buf, int len, char* str) {
	int ci = 0;
	while (*str && ci < len-1) {
		buf[ci++] = *str;
		++str;
	}

	buf[ci] = 0;
	return ci;
}

int sprint(char* buf, int len, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	int ci = 0;
	while (*fmt && ci < len-1) {
		if (fmt[0] == '%') {
			if (fmt[1] == 'i') {
				int intlen = sprint_int(buf+ci, len-ci, va_arg(args, int32_t));
				fmt += 2;
				ci += intlen;
				continue;
			}
			else if (fmt[1] == 'u') {
				int intlen = sprint_u32(buf+ci, len-ci, va_arg(args, uint32_t));
				fmt += 2;
				ci += intlen;
				continue;
			}
			else if (fmt[1] == 'l' && fmt[2] == 'i') {
				int intlen = sprint_i64(buf+ci, len-ci, va_arg(args, int64_t));
				ci += intlen;
				fmt += 3;
				continue;
			}
			else if (fmt[1] == 'l' && fmt[2] == 'u') {
				int intlen = sprint_u64(buf+ci, len-ci, va_arg(args, uint64_t));
				ci += intlen;
				fmt += 3;
				continue;
			}
			// else if (fmt[1] == 'f') {
			// 	int insertLen = sprint_float(buf+ci, len-ci, va_arg(args, double), 3);
			// 	ci += insertLen;
			// 	fmt += 2;
			// 	continue;
			// }
			// else if (fmt[1] == 'l' && fmt[2] == 'f') {
			// 	int insertLen = sprint_f64(buf+ci, len-ci, va_arg(args, double), 3);
			// 	ci += insertLen;
			// 	fmt += 3;
			// 	continue;
			// }
			else if (fmt[1] == 'f') {
				int insertLen = sprint_f64(buf+ci, len-ci, va_arg(args, double), 3);
				ci += insertLen;
				fmt += 2;
				continue;
			}
			else if (fmt[1] == 's') {
				ci += sprint_str(buf+ci, len-ci, va_arg(args, char*));
				fmt += 2;
				continue;
			}
		}

		buf[ci++] = fmt[0];
		++fmt;
	}

	va_end(args);
	buf[ci] = 0;
	return ci;
}


#	endif
#endif
