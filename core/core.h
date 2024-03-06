//
//  core.h
//  Core
//
//  Created by Matt Hartley on 02/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#ifndef __CORE_HEADER__
#define __CORE_HEADER__


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


#ifdef __LINUX__
#	include <stdint.h>
#	include <stdarg.h>
#	include <stdlib.h>
#endif
#include <stddef.h>
#include <stdio.h>
#ifdef CORE_CRT_ASSERT
#	include <assert.h>
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


// General constants and macros
#undef NULL
#undef TRUE
#undef FALSE
#define NULL 0
#define TRUE ((int)1)
#define FALSE ((int)0)
#define PAGE_SIZE 4096

#define KILOBYTES(n) (n*1024)
#define MEGABYTES(n) (n*1024*1024)
#define GIGABYTES(n) (n*1024*1024*1024)
#define KB KILOBYTES
#define MB MEGABYTES
#define GB GIGABYTES

#define array_size(a) (sizeof(a)/sizeof(a[0]))
#define FOR(index, count) for(int index=0; index<count; ++index)
#define FORSTATIC(index, arr) for(int index=0; index<(sizeof(arr)/sizeof(arr[0])); ++index)
#define FORDYNARR(index, arr) for(int index=0; index<arr.count; ++index)
#ifndef CORE_CRT_ASSERT
#	define assert(exp) if(!(exp)) { printf("Assertion failed (" #exp ") in function \"%s\" \n", __FUNCTION__); fflush(stdout); (*(int*)0 = 0); }
#endif


// Printing definitions
void core_print_inline(char* fmt, ...);
void core_print(char* fmt, ...);
void core_error(char* fmt, ...);
int core_print_to_buffer(char* buffer, size_t len, char* fmt, ...);
int core_print_to_buffer_va(char* buffer, size_t len, char* fmt, va_list args);

// Misc definitions
int valign(int n, int stride);
u64 align64(u64 size, u64 align);
u32 align32(u32 size, u32 align);
f32 core_randf();
f32 core_randfr(f32 a, f32 b);
int core_rand(int min, int max);

// Linked list definitions
typedef struct core_node_t core_node_t;
struct core_node_t {
	core_node_t* next;
	core_node_t* prev;
};
typedef struct {
	core_node_t* first;
	core_node_t* last;
} core_list_t;

void list_add(core_list_t* list, core_node_t* item);
void list_add_beginning(core_list_t* list, core_node_t* item);
void list_add_after(core_list_t* list, core_node_t* node, core_node_t* item);
void list_add_before(core_list_t* list, core_node_t* node, core_node_t* item);
void list_remove(core_list_t* list, core_node_t* item);

// Memory definitions
typedef struct {
	core_node_t node;
	u64 size;
} core_memblock_t;
typedef struct {
	u8* address;
	u64 size;
	u64 stack;
	u64 commit;
} core_stack_t;
typedef struct {
	u8* address;
	u64 size;
	u64 commit;
	core_list_t blocks;
	core_list_t free;
} core_allocator_t;

void core_zero(u8* address, int size);
void core_copy(u8* dest, u8* src, int size);
core_stack_t core_stack(u8* buffer, size_t size);
core_stack_t core_virtual_stack(size_t size, size_t commit);
core_allocator_t core_allocator(u8* buffer, size_t size);
core_allocator_t core_virtual_allocator(size_t size, size_t commit);
void core_use_allocator(core_allocator_t* arena);

void* core_push_into_virtual(core_stack_t* arena, size_t size);
void* core_push(core_stack_t* arena, size_t size);
void  core_pop(core_stack_t* arena, size_t size);
void  core_pop_and_shift(core_stack_t* arena, size_t offset, size_t size);

void  core_defrag_free_block(core_allocator_t* arena, core_memblock_t* block);
void* _core_alloc_into_free(core_allocator_t* arena, core_memblock_t* free, size_t size);
void _core_virtual_allocator_commit(core_allocator_t* arena, size_t size);
void* core_alloc_in(core_allocator_t* arena, size_t size);
void* core_alloc(size_t size);
void  core_free_in(core_allocator_t* arena, u8* block);
void  core_free(u8* block);
void  core_clear_allocator(core_allocator_t* arena);
void  core_clear_global_allocator();

void core_print_arena(core_allocator_t* arena);

// Dynarr definitions
typedef struct {
	core_stack_t arena;
	int stride;
	int max;
	int count;
} core_dynarr_t;

core_dynarr_t core_dynarr_static(u8* buffer, size_t size, int stride);
core_dynarr_t core_dynarr_virtual(size_t size, size_t pages_to_commit, int stride);
core_dynarr_t core_dynarr(int stride);
void core_dynarr_push(core_dynarr_t* arr, void* item);
void core_dynarr_pop(core_dynarr_t* arr, int index);
void* core_dynarr_get(core_dynarr_t* arr, int index);
void core_dynarr_clear(core_dynarr_t* arr);

// Pool definitions
#define core_pool_add(array, item) \
	FOR (i, array_size(array)) {\
		if (!(array)[i].used) {\
			(item).used = TRUE;\
			(array)[i] = (item);\
			break;\
		}\
	}

#define core_pool_for(array, code) \
	FOR (i, array_size(array)) {\
		if ((array)[i].used) {\
			{code}\
		}\
	}

// String definitions
#ifndef CORE_STRING_PROC_PREFIX
#	define CORE_STRING_PROC_PREFIX core_
#endif
#define CORE_STR_PROC(name) CORE_STRING_PROC_PREFIX##name

// #define CORE_STRING_TYPE_NAME CORE_STR_PROC(str_t)
// typedef char* core_string_t;
// typedef core_string_t CORE_STRING_TYPE_NAME;

// typedef char* core_string_t;

// void core_strcpy(core_string_t dest, core_string_t src);

typedef char* core_string_t;
typedef char* str_t;

int 			str_len(char* str);

core_string_t 	strd(char* str);
core_string_t 	strdf(char* fmt, ...);
void 			strd_free(core_string_t str);

void 			str_wide_to_char(core_string_t dest, wchar_t* str, int n);
core_string_t 	strd_wide_to_char(wchar_t* str);

void 			str_copy(char* dest, char* src, int buf_size);
void 			strd_copy(core_string_t* dest, core_string_t src);

b32				str_compare(core_string_t a, core_string_t b);
b32				str_ncompare(core_string_t a, core_string_t b, u64 n);

char*			str_find(core_string_t str, core_string_t find);
int				str_find_num(core_string_t str, core_string_t find);

void			str_append(char* dest, char* src, int buf_size);
void			strd_append(core_string_t* str, core_string_t append);

void			str_prepend(char* dest, char* src, int buf_size);
void			strd_prepend(core_string_t* str, core_string_t prepend);

void			str_insert(char* str, int index, char* insert, int buf_size);
void			strd_insert(core_string_t* str, int index, core_string_t insert);

// void core_strncpy(core_string_t dest, core_string_t src, int n);

void 			str_replace(char* str, char* find, char* replace, int buf_size);
void 			strd_replace(core_string_t* str, core_string_t find, core_string_t replace);

void 			str_replace_first(char* str, char* find, char* replace, int buf_size);
void 			strd_replace_first(core_string_t* str, core_string_t find, core_string_t replace);

int str_split(char** buffer, size_t size, char* str, char* by);
int strd_split(core_string_t* buffer, size_t size, core_string_t str, core_string_t by);

void			str_substr(char*buffer, size_t buf_size, char* str, int start, int len);
core_string_t	strd_substr(core_string_t str, int start, int len);

void str_trim(char* str);
void strd_trim(core_string_t* str);

void str_strip(char* str, int start, int n);

void str_lower(char* str);
void str_upper(char* str);

// Murmur hash definitions
u32 murmur3(u8* key);


#include "platform.h"
#include "terminal.h"

#endif