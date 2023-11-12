// General system platform specific api 

#ifndef __CORE_PLATFORM_HEADER__
#define __CORE_PLATFORM_HEADER__


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
	char filename[CORE_MAX_PATH_LENGTH];
} f_info;


// Platform functions
// void core_zero(byte* address, int size);
// void core_copy(byte* dest, byte* src, int size);
// void core_print(char* fmt, ...);
// void core_error(b32 fatal, char* fmt, ...);
// u32 s_len(char* str);
// void s_free(char* str);
// char* s_format(char* fmt, ...);
// char* core_convert_wide_string(wchar_t* str);
// void s_copy(char* dest, char* src);
// void s_ncopy(char* dest, char* src, int n);
// b32 s_compare(char* a, char* b);


// Threading
typedef b32 core_barrier_t;

void core_barrier_start(volatile core_barrier_t* barrier) {
	int num = 0;
	while (!__sync_bool_compare_and_swap((long volatile*)barrier, TRUE, FALSE)) {
		++num;
	}
	core_print("SPIN LOCK %i", num);
}

void core_barrier_end(volatile core_barrier_t* barrier) {
	__sync_lock_test_and_set((long volatile*)barrier, FALSE);
}


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
