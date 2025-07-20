//
//  Created by Matt Hartley on 22/03/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


#ifndef __CORE_PRINT_HEADER__
#define __CORE_PRINT_HEADER__


#include <stdint.h>
#include <stdarg.h>


void strbinsert(char* dest, int index, char* src, int buf_size);


// SPRINTING
int sprint_u64(char* buf, int len, uint64_t num);
int sprint_u64_display(char* buf, int len, uint64_t num);
int sprint_i64(char* buf, int len, int64_t num);
int sprint_u32(char* buf, int len, uint32_t num);
int sprint_int(char* buf, int len, int32_t num);
int sprint_f64(char* buf, int len, double input, int precision);
int sprint_float(char* buf, int len, float input, int precision);
int sprint_str(char* buf, int len, char* str);
int vsprint(char* buf, int len, char* fmt, va_list args);
int sprint(char* buf, int len, char* fmt, ...);


// ESCAPE CODES
enum {
	ESCAPE_BLACK = 0,
	ESCAPE_RED = 1,
	ESCAPE_GREEN = 2,
	ESCAPE_YELLOW = 3,
	ESCAPE_BLUE = 4,
	ESCAPE_MAGENTA = 5,
	ESCAPE_CYAN = 6,
	ESCAPE_WHITE = 7,
};
enum {
	ESCAPE_RESET = (1<<0),
	ESCAPE_BOLD = (1<<1),
	ESCAPE_DIM = (1<<2),
	ESCAPE_ITALIC = (1<<3),
	ESCAPE_UNDERLINE = (1<<4),
	ESCAPE_BLINK = (1<<5),
	ESCAPE_INVERTED = (1<<7),
	ESCAPE_HIDDEN = (1<<8),
	ESCAPE_STRIKETHROUGH = (1<<9),
	ESCAPE_END = (1<<10),
};

uint8_t escape_basic_color(uint8_t color, _Bool bright);
uint8_t escape_256_color(uint8_t r, uint8_t g, uint8_t b);
void escape_color(int color);
void escape_color_bg(int color);
void escape_mode(int attrs);

#define CORE_ERR_COLOR escape_256_color(5, 1, 1)


// OUTPUTTING
void print(char* fmt, ...);
void print_err(char* fmt, ...);


#	ifdef CORE_IMPL


#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h> // TODO: Remove

#include "imath.h"
#include "sys.h"


int _print_strlen(char* str) {
	int len = 0;
	while(*str++) ++len;
	return len;
}


// SPRINTING
// TODO: Pull out setting buf chars into fucntion with len check
#define _sprint_write(c) if (buf && ci<len-1) buf[ci] = (c);

int sprint_u64(char* buf, int len, uint64_t num) {
	int ci = 0;
	if (!num) {
		// if (buf) {
		// 	if (ci<len-1) {
		// 		buf[ci++] = '0';
		// 	}
		// 	buf[ci] = 0;
		// } else {
		// 	++ci;
		// }
		_sprint_write('0');
		++ci;
		return ci;
	}
	
	int l = ilog10_64(num);

	// if (buf) {
	// 	if (ci+l+1 < len-1) {
	// 		for (int i=0; i<l+1; ++i) {
	// 			buf[ci + (l-i)] = num%10 + '0';
	// 			num /= 10;
	// 		}
	// 		ci += l+1;
	// 	}
	// 	buf[ci] = 0;
	// } else {
	// 	ci += l+1;
	// }

	// for (int i=l; i>=0; --i) {
	// 	uint64_t div = ipow64(10, i);
	// 	uint64_t tmp = i ? num / div : num;
	// 	// buf[ci + (l-i)] = num%10 + '0';
	// 	_sprint_write(tmp%10 + '0');
	// 	++ci;
	// 	// num /= 10;
	// }

	// TODO: Better
	ci += l;
	for (int i=l; i>=0; --i) {
		// uint64_t div = ipow64(10, i);
		// uint64_t tmp = i ? num / div : num;
		// buf[ci + (l-i)] = num%10 + '0';
		_sprint_write(num%10 + '0');
		--ci;
		num /= 10;
	}
	ci += l + 2;

	_sprint_write(0);

	return ci;
}

int sprint_u64_display(char* buf, int len, uint64_t num) {
	int size = sprint_u64(buf, len, num);
	int cursor = size-3;
	while (cursor > 0) {
		strbinsert(buf, cursor, ",", len);
		cursor -= 3;
	}
	return size;
}

int sprint_i64(char* buf, int len, int64_t num) {
	int ci = 0;
	if (num < 0) {
		// if (buf) {
		// 	buf[ci++] = '-';
		// } else {
		// 	++ci;
		// }
		_sprint_write('-');
		++ci;
	}
	num = llabs(num);
	// if (buf) {
	// 	ci += sprint_u64(buf? buf+ci : buf, buf ? len-ci : len, num);
	// } else {
	// 	ci += sprint_u64(buf, len, num);
	// }
	ci += sprint_u64(buf? buf+ci : buf, buf ? len-ci : len, num);
	return ci;
}

int sprint_u32(char* buf, int len, uint32_t num) {
	return sprint_u64(buf, len, num);
}

int sprint_int(char* buf, int len, int32_t num) {
	int ci = 0;
	if (num < 0) {
		// if (buf) {
		// 	buf[ci++] = '-';
		// } else {
		// 	++ci;
		// }
		_sprint_write('-');
		++ci;
	}
	num = abs(num);
	// if (buf) {
	// 	ci += sprint_u64(buf+ci, len-ci, num);
	// } else {
	// 	ci += sprint_u64(buf, len, num);
	// }
	ci += sprint_u64(buf? buf+ci : buf, buf ? len-ci : len, num);
	return ci;
}

int sprint_f64(char* buf, int len, double input, int precision) {
	double value = fabs(input);
	int ci = 0;
	if (input < 0.0f) {
		// if (buf) {
		// 	buf[ci++] = '-';
		// } else {
		// 	++ci;
		// }
		_sprint_write('-');
		++ci;
	}

	int64_t wholePart = value;
	double fractPart = value - (double)wholePart;
	// if (buf) {
	// 	ci += sprint_i64(buf+ci, len-ci, wholePart);
	// } else {
	// 	ci += sprint_i64(buf, len, wholePart);
	// }
	ci += sprint_i64(buf? buf+ci : buf, buf ? len-ci : len, wholePart);

	// if (buf) {
	// 	if (ci<len-1) {
	// 		buf[ci++] = '.';
	// 	}
	// } else {
	// 	++ci;
	// }
	_sprint_write('.');
	++ci;

	// if (buf) {
	// 	for (int i=0; i<precision && ci<len-1; ++i) {
	// 		fractPart = fmodf(fractPart*10, 10);
	// 		int fractInt = fractPart;
	// 		buf[ci++] = '0' + fractInt;
	// 	}
	// 	buf[ci] = 0;
	// } else {
	// 	ci += len-1;
	// }

	// Using 50 as roughly the max string length of a float
	// TODO: Maybe implement nice algorithm like Dragon4 or Ryu
	int maxDigits = precision ? precision : 20;
	// for (int i=0; i<maxDigits; ++i) {
	// 	fractPart = fmodf(fractPart*10, 10);
	// 	int fractInt = fractPart;
	// 	if (fractInt) {
	// 		lastNonZero = i;
	// 	}
	// }
	int cistack = ci;
	for (int i=0; i<maxDigits; ++i) {
		fractPart = fmodf(fractPart*10, 10);
		int fractInt = fractPart;

		_sprint_write('0' + fractInt);
		++ci;

		if (!i || fractInt) {
			cistack = ci;
		}
	}

	ci = cistack;
	// ci -= zeroDigits;
	// buf[ci] = 0;
	_sprint_write(0);

	return ci;
}

int sprint_float(char* buf, int len, float input, int precision) {
	return sprint_f64(buf, len, input, precision);
}

int sprint_str(char* buf, int len, char* str) {
	int ci = 0;
	while (*str) {
		// if (buf) {
		// 	buf[ci++] = *str;
		// } else {
		// 	++ci;
		// }
		_sprint_write(*str);
		++ci;
		++str;
	}
	
	// if (buf) {
	// 	buf[ci] = 0;
	// }
	// _sprint_write(0);

	return ci;
}

int vsprint(char* buf, int len, char* fmt, va_list args) {
	int ci = 0;

	while (*fmt) {
		if (fmt[0] == '%') {
			char* subBuf = buf ? buf+ci : buf;
			int subLen = buf ? len-ci : len;

			if (fmt[1] == 'i') {
				int intlen = sprint_int(subBuf, subLen, va_arg(args, int32_t));
				fmt += 2;
				ci += intlen;
				continue;
			}
			else if (fmt[1] == 'u') {
				int intlen = sprint_u32(subBuf, subLen, va_arg(args, uint32_t));
				fmt += 2;
				ci += intlen;
				continue;
			}
			else if (fmt[1] == 'l' && fmt[2] == 'i') {
				int intlen = sprint_i64(subBuf, subLen, va_arg(args, int64_t));
				ci += intlen;
				fmt += 3;
				continue;
			}
			else if (fmt[1] == 'l' && fmt[2] == 'u') {
				int intlen = sprint_u64(subBuf, subLen, va_arg(args, uint64_t));
				ci += intlen;
				fmt += 3;
				continue;
			}
			// else if (fmt[1] == 'f') {
			// 	int insertLen = sprint_float(subBuf, subLen, va_arg(args, double), 3);
			// 	ci += insertLen;
			// 	fmt += 2;
			// 	continue;
			// }
			// else if (fmt[1] == 'l' && fmt[2] == 'f') {
			// 	int insertLen = sprint_f64(subBuf, subLen, va_arg(args, double), 3);
			// 	ci += insertLen;
			// 	fmt += 3;
			// 	continue;
			// }
			else if (fmt[1] == 'f') {
				int insertLen = sprint_f64(subBuf, subLen, va_arg(args, double), 3);
				ci += insertLen;
				fmt += 2;
				continue;
			}
			else if (fmt[1] == 's') {
				ci += sprint_str(subBuf, subLen, va_arg(args, char*));
				fmt += 2;
				continue;
			}
			else if (fmt[1] == 'c') {
				// ci += sprint_str(subBuf, subLen, va_arg(args, char*));
				char c = (char)va_arg(args, int);
				_sprint_write(c);
				++ci;
				fmt += 2;
				continue;
			}
		}

		// if (buf) {
		// 	buf[ci++] = fmt[0];
		// } else {
		// 	++ci;
		// }
		_sprint_write(fmt[0]);
		++ci;
		++fmt;
	}

	// if (buf) {
	// 	buf[ci] = 0;
	// }
	if (buf && ci<len) buf[ci] = 0; 
	return ci;
}

int sprint(char* buf, int len, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ci = vsprint(buf, len, fmt, args);
	va_end(args);
	return ci;
}


// ESCAPE CODES
void escape_print_color(int color) {
	char buffer[16] = {0};

	if (color > -1) {
		snprintf(buffer, 16, "\x1B[38;5;%im", color);
	} else {
		snprintf(buffer, 16, "\x1B[39m");
	}

	sys_print(buffer);
}
void escape_print_color_bg(int color) {
	char buffer[16] = {0};

	if (color > -1) {
		snprintf(buffer, 16, "\x1B[48;5;%im", color);
	} else {
		snprintf(buffer, 16, "\x1B[49m");
	}

	sys_print(buffer);
}

uint8_t escape_basic_color_encode(uint8_t color, _Bool bright) {
	if (bright) {
		color += 8;
	}
	return color;
}
uint8_t escape_basic_color(uint8_t color, _Bool bright) {
	uint8_t result = escape_basic_color_encode(color, bright);
	escape_print_color(result);
	return result;
}
uint8_t escape_basic_color_bg(uint8_t color, _Bool bright) {
	uint8_t result = escape_basic_color_encode(color, bright);
	escape_print_color_bg(result);
	return result;
}

uint8_t escape_256_color_encode(uint8_t r, uint8_t g, uint8_t b) {
	return 16 + (r*36) + (g*6) + (b);
}
uint8_t escape_256_color(uint8_t r, uint8_t g, uint8_t b) {
	uint8_t result = escape_256_color_encode(r, g, b);
	escape_print_color(result);
	return result;
}
uint8_t escape_256_color_bg(uint8_t r, uint8_t g, uint8_t b) {
	uint8_t result = escape_256_color_encode(r, g, b);
	escape_print_color_bg(result);
	return result;
}

void escape_color(int color) {
	escape_print_color(color);
}

void escape_color_bg(int color) {
	escape_print_color_bg(color);
}

void escape_mode(int attrs) {
	char buffer[64] = {0};
	snprintf(buffer, 64, "\x1B[");

	for (int i=0; i<32; ++i) {
		if (attrs & (1 << i)) {
			if (buffer[_print_strlen(buffer)-1] != '[') {
				buffer[_print_strlen(buffer)+1] = 0;
				buffer[_print_strlen(buffer)+0] = ';';
			}
			snprintf(buffer+_print_strlen(buffer), 64-_print_strlen(buffer), "%i", i);
		}
	}

	snprintf(buffer+_print_strlen(buffer), 64-_print_strlen(buffer), "m");
	sys_print(buffer);
}


// OUTPUTTING
// void print_init_log_file(char* filename) {
// 	int logFile = open(filename, O_WRONLY | O_CREAT | OAPPEND, 0644);
// }
// void _print_with_info(const char* filename, const char* function, int line, char* fmt, ...) {
// 	char headerStr[256];
// 	snprintf(headerStr, sizeof(headerStr), "[%s:%i] ", function, line);
	
// 	char str[1024];
// 	va_list va;
// 	va_start(va, fmt);
// 	vsnprintf(str, 1024, fmt, va);
// 	va_end(va);

// 	// fputs(headerStr, stdout);
// 	puts(str);
// }
// void _print_inline(char* fmt, ...) {
// 	char str[1024];
// 	va_list va;
// 	va_start(va, fmt);
// 	vsnprintf(str, 1024, fmt, va);
// 	fputs(str, stdout);
// 	va_end(va);
// }
void print(char* fmt, ...) {
	if (!fmt) {
		return;
	}

	char buffer[4096];
	va_list va;
	va_start(va, fmt);

	vsprint(buffer, sizeof(buffer), fmt, va);
	sys_print(buffer);
	
	va_end(va);
}

void print_err(char* fmt, ...) {
	if (!fmt) {
		return;
	}
	
	char buffer[4096];
	va_list va;
	va_start(va, fmt);

	vsprint(buffer, sizeof(buffer), fmt, va);
	sys_print_err(buffer);
	
	va_end(va);
}
// void _print_error(char* fmt, ...) {
// 	assert(fmt > (char*)1); // Might be using old format with boolean as first parameter
// 	char str[1024];
// 	va_list va;
// 	va_start(va, fmt);
// 	vsnprintf(str, 1024, fmt, va);
// 	// print(TERM_RED_FG "%s" TERM_RESET, str);
// 	escape_color(CORE_ERR_COLOR);
// 	escape_mode(ESCAPE_INVERTED | ESCAPE_BOLD);
// 	print("%s", str);
// 	escape_mode(ESCAPE_RESET);
// 	va_end(va);
// }
// int print_to_buffer(char* buffer, size_t len, char* fmt, ...) {
// 	va_list va;
// 	va_start(va, fmt);
// 	int result = vsnprintf(buffer, len, fmt, va);
// 	va_end(va);
// 	return result;
// }
// int print_to_buffer_va(char* buffer, size_t len, char* fmt, va_list args) {
// 	int result = vsnprintf(buffer, len, fmt, args);
// 	return result;
// }


#	endif
#endif
