// General system platform specific api 

#ifndef __CORE_PLATFORM_HEADER__
#define __CORE_PLATFORM_HEADER__

// Define standard platform defs
#ifdef _WIN32
#	undef __WIN32__
#	define __WIN32__
#endif
#ifdef __linux__
#	define __LINUX__
#endif
#ifdef __APPLE__
#	define __MACOS__
#endif

// Types
#ifdef __WIN32__
typedef __int64  i64;
typedef unsigned __int64  u64;
typedef __int32  i32;
typedef unsigned __int32  u32;
typedef __int16  i16;
typedef unsigned __int16  u16;
typedef __int8  i8;
typedef unsigned __int8  u8;
#endif

#ifdef __LINUX__
typedef signed long int  i64;
typedef unsigned long int  u64;
typedef signed int  i32;
typedef unsigned int  u32;
typedef signed short int  i16;
typedef unsigned short int  u16;
typedef signed char  i8;
typedef unsigned char  u8;
#endif

#ifdef __MACOS__

#endif

typedef float f32;
typedef double f64;

typedef u32 b32;
typedef u8 byte;
// typedef u64 size_t;

#define CORE_MAX_PATH_LENGTH 256

// Structures
enum {
	CORE_MB_OK = (1<<0),
	CORE_MB_YES_NO = (1<<1), // TODO
};

typedef struct {
	u64 sec;
	u64 msec;
} timestamp_t;

typedef struct {
	u64 created;
	u64 modified;
	size_t size;
	b32 is_directory;
	char filename[64];
} f_info;

// Platform functions
void core_print(char* fmt, ...);
void core_error(b32 fatal, char* fmt, ...);
u32 s_len(char* str);
void s_free(char* str);
char* s_format(char* fmt, ...);
char* core_convert_wide_string(wchar_t* str);
void s_copy(char* dest, char* src);
void s_ncopy(char* dest, char* src, int n);
b32 s_compare(char* a, char* b);

#ifdef __WIN32__
#	include "platform_win32.h"
#endif

#ifdef __LINUX__
#	include "platform_linux.h"
#endif

#ifdef __MACOS__
#	include "platform_macos.h"
#endif


#endif
