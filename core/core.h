//
//  Created by Matt Hartley on 02/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

/*
  Contains:
	- Types
	- General macros
	- Print API
	- Alignment API
	- Linked list API
	- Memory allocation API
	- Dynamic array API
	- String API
	- Hashing API
*/

// TODO
// Window fullscreen flag and toggle
// Window resizeable flag
// Remove CRT file api from bmp and wav loading

// More print options
// Convert print functions to using posix functions
// Use use dup2 to print to stdout and log file

// allocator memory pools
// Strings refresh


#ifndef __CORE_HEADER__
#define __CORE_HEADER__


#include "targetconditionals.h"
#include "sys.h"
#include "print.h"


#ifdef __POSIX__
#	include <stdarg.h>
#	include <stdlib.h>
#	include <stdint.h>
#	include <stdbool.h>
#endif
#ifdef __MACOS__
#	include <time.h>
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
typedef int64_t  i64;
typedef uint64_t  u64;
typedef int32_t  i32;
typedef uint32_t  u32;
typedef int16_t  i16;
typedef uint16_t  u16;
typedef int8_t  i8;
typedef uint8_t  u8;
#endif

typedef float f32;
typedef double f64;
typedef float float32_t;
typedef double float64_t;

typedef u32 b32;
typedef u8 byte;
// typedef u64 size_t;


// General constants and macros
#undef NULL
#undef TRUE
#undef FALSE
#define NULL (0)
#define NULLPTR ((void*)0)
#define TRUE ((int)1)
#define FALSE ((int)0)
#undef PAGE_SIZE
#define PAGE_SIZE 4096
#undef _True
#undef _False
#define _True ((_Bool)1)
#define _False ((_Bool)0)

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
#   undef assert
#	define assert(exp) if(!(exp)) { printf("Assertion failed (" #exp ") in function \"%s\" \n", __FUNCTION__); fflush(stdout); (*(volatile int*)0 = 0); }
#endif
#define min(a, b) (a<b ? a : b)
#define max(a, b) (a>b ? a : b)
#define TOSTRING(a) __STRING(a)


#define CORE_ALWAYS_INLINE __attribute__((always_inline))
#define CORE_API
#define CORE_PRINT_FUNC
#define CORE_ALIGN_FUNC


// Linked list definitions
typedef struct llnode_t llnode_t;
struct llnode_t {
	llnode_t* next;
	llnode_t* prev;
};
typedef struct {
	llnode_t* first;
	llnode_t* last;
} llist_t;

void list_add(llist_t* list, llnode_t* item);
void list_add_beginning(llist_t* list, llnode_t* item);
void list_add_after(llist_t* list, llnode_t* node, llnode_t* item);
void list_add_before(llist_t* list, llnode_t* node, llnode_t* item);
void list_remove(llist_t* list, llnode_t* item);


// Memory definitions
typedef struct {
	llnode_t node;
	u64 size;
    u32 debug_id; // This is set to something specific to debug check if a memory block is kosher
} allocator_block_t;
// typedef struct {
// 	void* address;
// 	u64 size;
// 	u64 stack;
// 	u64 commit;
// } memstack_t;
typedef enum {
	ALLOCATOR_HEAP,
	ALLOCATOR_BUMP,
} allocator_type_t;
typedef struct {
	allocator_type_t type;
	void* address;
	u64 size;
	u64 commit;
	union {
		struct {
			llist_t blocks;
			llist_t free;
		};
		struct {
			u64 stackptr;
		};
	};
} allocator_t;

CORE_API allocator_t bump_allocator(u8* buffer, size_t size);
CORE_API allocator_t virtual_bump_allocator(size_t size, size_t commit);
CORE_API allocator_t heap_allocator(u8* buffer, size_t size);
CORE_API allocator_t virtual_heap_allocator(size_t size, size_t commit);
CORE_API void use_allocator(allocator_t* arena);
CORE_API void* push_memory(allocator_t* arena, size_t size);
CORE_API void pop_memory(allocator_t* arena, size_t size);
CORE_API void pop_memory_and_shift(allocator_t* arena, size_t offset, size_t size);
CORE_API void* alloc_memory(allocator_t* arena, size_t size);
CORE_API void* galloc_memory(size_t size);
CORE_API void free_memory(allocator_t* arena, void* block);
CORE_API void gfree_memory(void* block);
CORE_API void clear_allocator(allocator_t* arena);
CORE_API void clear_global_allocator();

CORE_API void print_arena(allocator_t* arena);


// Dynarr definitions
typedef struct {
	allocator_t arena;
	int stride;
	int max;
	int count;
} dynarr_t;

dynarr_t dynarr_static(u8* buffer, size_t size, int stride);
dynarr_t dynarr_virtual(size_t size, size_t pages_to_commit, int stride);
dynarr_t dynarr(int stride);
void dynarr_push(dynarr_t* arr, void* item);
void dynarr_pop(dynarr_t* arr, int index);
void* dynarr_get(dynarr_t* arr, int index);
void dynarr_clear(dynarr_t* arr);


// TODO: Remove this?
// Pool definitions
#define pool_add(array, item) \
	FOR (i, array_size(array)) {\
		if (!(array)[i].used) {\
			(item).used = TRUE;\
			(array)[i] = (item);\
			break;\
		}\
	}

#define pool_for(array, code) \
	FOR (i, array_size(array)) {\
		if ((array)[i].used) {\
			{code}\
		}\
	}


// String definitions
// #ifndef CORE_STRING_PROC_PREFIX
// #	define CORE_STRING_PROC_PREFIX core_
// #endif
// #define CORE_STR_PROC(name) CORE_STRING_PROC_PREFIX##name

// #define CORE_STRING_TYPE_NAME CORE_STR_PROC(str_t)
// typedef char* core_string_t;
// typedef core_string_t CORE_STRING_TYPE_NAME;

// typedef char* core_string_t;

// void core_strcpy(core_string_t dest, core_string_t src);

typedef char* core_string_t;
typedef char* str_t;

void str_set_allocator(allocator_t* allocator);

int 			str_len(char* str);

core_string_t 	str_create(char* str);
core_string_t 	str_format(char* fmt, ...);
void 			str_free(core_string_t str);

void 			char_wide_to_char(core_string_t dest, wchar_t* str, int n);
core_string_t 	str_wide_to_char(wchar_t* str);

void 			char_copy(char* dest, char* src, int buf_size);
void 			str_copy(core_string_t* dest, core_string_t src);

b32				str_compare(core_string_t a, core_string_t b);
b32				str_ncompare(core_string_t a, core_string_t b, u64 n);

char*			str_find(core_string_t str, core_string_t find);
int				str_find_num(core_string_t str, core_string_t find);

void			char_append(char* dest, char* src, int buf_size);
void			str_append(core_string_t* str, core_string_t append);

void			char_prepend(char* dest, char* src, int buf_size);
void			str_prepend(core_string_t* str, core_string_t prepend);

void			char_insert(char* str, int index, char* insert, int buf_size);
void			str_insert(core_string_t* str, int index, core_string_t insert);

// void core_strncpy(core_string_t dest, core_string_t src, int n);

void 			char_replace(char* str, char* find, char* replace, int buf_size);
void 			str_replace(core_string_t* str, core_string_t find, core_string_t replace);

void 			char_replace_first(char* str, char* find, char* replace, int buf_size);
void 			str_replace_first(core_string_t* str, core_string_t find, core_string_t replace);

int 			char_split(char** buffer, size_t size, char* str, char* by);
int 			str_split(core_string_t* buffer, size_t size, core_string_t str, core_string_t by);

void			char_substr(char*buffer, size_t buf_size, char* str, int start, int len);
core_string_t	str_substr(core_string_t str, int start, int len);

void 			char_trim(char* str);
void 			str_trim(core_string_t* str);

void 			str_strip(char* str, int start, int n);

void 			str_lower(char* str);
void 			str_upper(char* str);


// Murmur hash definitions
u32 murmur3(u8* key);


#	ifdef CORE_IMPL


// LINKED LISTS
void list_add(llist_t* list, llnode_t* item) {
	item->next = 0;
	item->prev = 0;
	if(list->last) {
		list->last->next = item;
		item->prev = list->last;
		list->last = item;
	} else {
		list->first = item;
		list->last = item;
	}
}
void list_add_beginning(llist_t* list, llnode_t* item) {
	item->next = 0;
	item->prev = 0;
	if(list->first) {
		list->first->prev = item;
		item->next = list->first;
		list->first = item;
	} else {
		list->first = item;
		list->last = item;
	}
}
void list_add_after(llist_t* list, llnode_t* node, llnode_t* item) {
	item->prev = node;
	if(node->next) {
		item->next = node->next;
		node->next->prev = item;
		node->next = item;
	} else {
		list->last = item;
		item->next = 0;
	}
}
void list_add_before(llist_t* list, llnode_t* node, llnode_t* item) {
	item->next = node;
	if(node->prev) {
		item->prev = node->prev;
		node->prev->next = item;
		node->prev = item;
	} else {
		list->first = item;
		item->prev = 0;
	}
}
void list_remove(llist_t* list, llnode_t* item) {
	if(!item->prev) {
		list->first = item->next;
	} else {
		item->prev->next = item->next;
	}
	if(!item->next) {
		list->last = item->prev;
	} else {
		item->next->prev = item->prev;
	}
}


// MEMORY
allocator_t* _global_allocator = NULL;
#define _is_arena_virtual(arena) (arena->commit > 0)

uint64_t _allocator_align(uint64_t size, uint64_t align) {
	if(!(size & (align-1))) {
		return size;
	} else {
		return (size & ~(align-1)) + align;
	}
}

// Constructors
CORE_API allocator_t bump_allocator(u8* buffer, size_t size) {
	allocator_t arena = {0};
	arena.type = ALLOCATOR_BUMP;
	arena.address = buffer;
	arena.size = size;
	arena.stackptr = 0;
	return arena;
}

CORE_API allocator_t virtual_bump_allocator(size_t size, size_t commit) {
	if (!commit) {
		commit = size;
	}
	assert(size >= commit);
	allocator_t arena = {0};
	arena.type = ALLOCATOR_BUMP;
	arena.size = size;
	arena.commit = _allocator_align(commit, PAGE_SIZE);
	arena.address = sys_reserve_memory(arena.size);
	sys_commit_memory(arena.address, arena.commit);
	arena.stackptr = 0;
	return arena;
}

CORE_API allocator_t heap_allocator(u8* buffer, size_t size) {
	allocator_t arena = {0};
	arena.type = ALLOCATOR_HEAP;
	if (buffer) {
		arena.address = buffer;
	} else {
		arena.address = sys_alloc_memory(size);
	}
	arena.size = size;

	arena.blocks.first = NULL;
	arena.blocks.last = NULL;
	
	((allocator_block_t*)arena.address)->size = arena.size;
	list_add(&arena.free, (llnode_t*)arena.address);

	return arena;
}

CORE_API allocator_t virtual_heap_allocator(size_t size, size_t commit) {
	if (!commit) {
		commit = size;
	}
	assert(size >= commit);
	allocator_t arena = {0};
	arena.type = ALLOCATOR_HEAP;
	arena.size = size;
	arena.commit = _allocator_align(commit, PAGE_SIZE);
	arena.address = sys_reserve_memory(arena.size);
	sys_commit_memory(arena.address, arena.commit);

	arena.blocks.first = NULL;
	arena.blocks.last = NULL;
	
	((allocator_block_t*)arena.address)->size = arena.commit;
	list_add(&arena.free, (llnode_t*)arena.address);

	return arena;
}

CORE_API void use_allocator(allocator_t* arena) {
	_global_allocator = arena;
}

// Stacks
CORE_API void* push_memory(allocator_t* arena, size_t size) {
	assert(arena->type == ALLOCATOR_BUMP);
	assert(arena->stackptr + size <= arena->size);
	if(_is_arena_virtual(arena)) {
		// return m_push_into_reserve(arena, size);
		if(arena->stackptr+size > arena->commit) {
			size_t extra_commit = _allocator_align(arena->stackptr+size - arena->commit, PAGE_SIZE);
			sys_commit_memory((u8*)arena->address+arena->commit, extra_commit);
			arena->commit += extra_commit;
		}
	}

	if(arena->stackptr + size <= arena->size) {
		void* result = (byte*)arena->address+arena->stackptr;
		arena->stackptr += size;
		return result;
	}

	return 0;
}

CORE_API void pop_memory(allocator_t* arena, size_t size) {
	assert(arena->type == ALLOCATOR_BUMP);
	// TODO: Should we zero this?
	sys_zero_memory(arena->address + arena->stackptr - size, size);
	arena->stackptr -= size;
}

CORE_API void pop_memory_and_shift(allocator_t* arena, size_t offset, size_t size) {
	assert(arena->type == ALLOCATOR_BUMP);
	assert(arena->stackptr >= offset + size);
	sys_copy_memory(arena->address + offset, arena->address + offset + size, arena->stackptr - (offset+size));
	pop_memory(arena, size);
}

// Allocators
void defrag_free_block(allocator_t* arena, allocator_block_t* block) {
	assert(arena->type == ALLOCATOR_HEAP);
	allocator_block_t* free = (allocator_block_t*)arena->free.first;
	while(free) {
		allocator_block_t* next_free = (allocator_block_t*)((llnode_t*)free)->next;

		if (free != block) {
			if ((u8*)free == (u8*)block + block->size) {
				block->size += free->size;
				list_remove(&arena->free, (llnode_t*)free);
			}
			if((u8*)free + free->size == (u8*)block) {
				free->size += block->size;
				list_remove(&arena->free, (llnode_t*)block);
				block = free;
			}
		}

		free = next_free;
	}
}

void* _alloc_into_free(allocator_t* arena, allocator_block_t* free, size_t size) {
	assert(arena->type == ALLOCATOR_HEAP);
	if(free->size > size) {
		allocator_block_t* newFree = (allocator_block_t*)((u8*)free + size);
		newFree->size = free->size - size;
		list_add(&arena->free, (llnode_t*)newFree);
	}
	list_remove(&arena->free, (llnode_t*)free);
	free->size = size;
	list_add(&arena->blocks, (llnode_t*)free);
    free->debug_id = 0xDeadBeef;
	return free + 1;
}

void _virtual_allocator_commit(allocator_t* arena, size_t size) {
	assert(arena->type == ALLOCATOR_HEAP);
	assert(arena->commit < arena->size);
	u64 commit = _allocator_align(size, PAGE_SIZE);
	allocator_block_t* new_memory = sys_commit_memory((u8*)arena->address+arena->commit, commit);
	arena->commit += commit;
	new_memory->size = commit;
	list_add(&arena->free, (llnode_t*)new_memory);
	defrag_free_block(arena, new_memory);
}

CORE_API void* alloc_memory(allocator_t* arena, size_t size) {
	assert(arena->type == ALLOCATOR_HEAP);
	assert(arena);
	assert(arena->address);
	assert(size);

	size_t required_size = size + sizeof(allocator_block_t);

	allocator_block_t* free = (allocator_block_t*)arena->free.first;
	while(free) {
		// If the free block's size isn't exactly the same,
		// it will need to split the free block.
		// The new split free block must be at least the size of allocator_block_t
		// to store the block header.
		if(free->size == required_size
			|| free->size >= (required_size+sizeof(allocator_block_t))) {
			return _alloc_into_free(arena, free, required_size);
		}
		free = (allocator_block_t*)((llnode_t*)free)->next;
	}

	if(_is_arena_virtual(arena)) {
		_virtual_allocator_commit(arena, required_size);
		return alloc_memory(arena, required_size);
	}

#ifdef CRASHING_ASSERTS
	assert(!"Failed to find a free block large enough");
#else
	print_err("Failed to find a free block large enough");
#endif
	return NULL;
}

CORE_API void* galloc_memory(size_t size) {
	return alloc_memory(_global_allocator, size);
}

CORE_API void free_memory(allocator_t* arena, void* block) {
	assert(arena->type == ALLOCATOR_HEAP);
	assert(arena);
	assert(block);
	if (block >= arena->address && block < arena->address+arena->size) {
		block -= sizeof(allocator_block_t);
		// arena->stack -= ((allocator_block_t*)block)->size;
        assert(((allocator_block_t*)block)->debug_id == 0xDeadBeef);
		list_remove(&arena->blocks, (llnode_t*)block);
		list_add(&arena->free, (llnode_t*)block);

		defrag_free_block(arena, (allocator_block_t*)block);
	} else {
		print_err("Attempt to free block outside of allocator space");
	}
}

CORE_API void gfree_memory(void* block) {
	free_memory(_global_allocator, block);
}

CORE_API void clear_allocator(allocator_t* arena) {
	// assert(arena->type == ALLOCATOR_HEAP);
	if (arena->type == ALLOCATOR_HEAP) {
		arena->blocks = (llist_t){0};
		arena->free = (llist_t){0};
		((allocator_block_t*)arena->address)->size = arena->commit ? arena->commit : arena->size;
		list_add(&arena->free, (llnode_t*)arena->address);
	}
	if (arena->type == ALLOCATOR_BUMP) {
		arena->stackptr = 0;
	}
}

CORE_API void clear_global_allocator() {
	clear_allocator(_global_allocator);
}

#ifdef __ALLOCATOR_BLOCK_DEBUG__
void print_allocator_block_t(allocator_block_t* block) {
	for(int i=0; i< block->size-sizeof(allocator_block_t); ++i) {
		char c = *((u8*)(block+1) + i);
		if(c) {
			if(c == '\n') {
				print("\\n");
			} else {
				print("%c", c);
			}
		} else {
			print("_");
		}
	}
}

CORE_API void print_arena(allocator_t* arena) {
	u64 index = 0;
	u64 arena_size = _is_arena_virtual(arena) ? arena->commit : arena->size;

next:
	while(index < arena_size) {
		allocator_block_t* b = (allocator_block_t*)arena->blocks.first;
		while(b) {
			if(arena->address+index == (void*)b) {
				// print(TERM_RESET TERM_INVERTED);
				escape_mode(ESCAPE_RESET | ESCAPE_INVERTED);
				char size[32];
				sprint(size, sizeof(size), "block %li", b->size);
				print(size);
				for(int i=0; i<sizeof(allocator_block_t)-str_len(size); ++i) {
					print(" ");
				}
				// print(TERM_RESET TERM_BLUE_BG);
				escape_mode(ESCAPE_RESET);
				escape_color_bg(escape_basic_color(4, _True));
				print_allocator_block_t(b);
				index += b->size;
				goto next;
			}
			b = (allocator_block_t*)((llnode_t*)b)->next;
		}

		allocator_block_t* f = (allocator_block_t*)arena->free.first;
		while(f) {
			if(arena->address+index == (void*)f) {
				// print(TERM_RESET TERM_INVERTED);
				escape_mode(ESCAPE_RESET | ESCAPE_INVERTED);
				char size[32];
				sprint(size, sizeof(size), "free %li", f->size);
				print(size);
				for(int i=0; i<sizeof(allocator_block_t)-str_len(size); ++i) {
					print(" ");
				}
				// print(TERM_RESET TERM_GREEN_BG);
				escape_mode(ESCAPE_RESET);
				escape_color_bg(escape_basic_color(2, _True));
				print_allocator_block_t(f);
				index += f->size;
				goto next;
			}
			f = (allocator_block_t*)((llnode_t*)f)->next;
		}

		escape_color_bg(escape_basic_color(1, _True));
		print("FATAL ERROR");
		exit(1);
	}

	assert(index == arena_size);
	escape_mode(ESCAPE_RESET);
	escape_color_bg(escape_basic_color(3, _True));
	print("END\n");
	escape_mode(ESCAPE_RESET);
}
#endif


// DYNAMIC ARRAY
dynarr_t dynarr_static(u8* buffer, size_t size, int stride) {
	dynarr_t result;
	result.arena = bump_allocator(buffer, size);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

dynarr_t dynarr_virtual(size_t size, size_t pages_to_commit, int stride) {
	dynarr_t result;
	result.arena = virtual_bump_allocator(size, pages_to_commit);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

dynarr_t dynarr(int stride) {
	return dynarr_virtual(GB(1), PAGE_SIZE, stride);
}

void dynarr_push(dynarr_t* arr, void* item) {
	void* result = push_memory(&arr->arena, arr->stride);
	sys_copy_memory(result, item, arr->stride);
	++arr->count;
}

void dynarr_pop(dynarr_t* arr, int index) {
	pop_memory_and_shift(&arr->arena, index*arr->stride, arr->stride);
	--arr->count;
}

void* dynarr_get(dynarr_t* arr, int index) {
	return arr->arena.address + (arr->stride * index);
}

void dynarr_clear(dynarr_t* arr) {
	arr->arena.stackptr = 0;
	arr->count = 0;
}


// STRINGS
allocator_t* _global_str_allocator = NULL;

void str_set_allocator(allocator_t* allocator) {
    _global_str_allocator = allocator;
}

int str_get_aligned_size(int size) {
	// NOTE: don't think adding sizeof memblock at this point is needed
	return _allocator_align(size+1 /*+ sizeof(core_allocator_block_t)*/, 64);
}

core_string_t _allocate_string(size_t len) {
	core_string_t result = NULL;
	if (_global_str_allocator->type == ALLOCATOR_HEAP) {
		result = alloc_memory(_global_str_allocator, str_get_aligned_size(len));
	}
	if (_global_str_allocator->type == ALLOCATOR_BUMP) {
		result = push_memory(_global_str_allocator, str_get_aligned_size(len));
	}
	return result;
}

void str_check_inside_allocator(core_string_t str) {
	assert((u8*)str >= (u8*)_global_allocator->address &&
			(u8*)str < ((u8*)_global_allocator->address+_global_allocator->size));
}

int str_len(char* str) {
	int len = 0;
	while(*str++) ++len;
	return len;
}

core_string_t str_create(char* str) {
	u64 len = str_len(str);
	core_string_t result = _allocate_string(len);
	if (result) {
		sys_copy_memory(result, str, len+1);
	}
	return result;
}
core_string_t str_format(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int len = vsprint(0, 0, fmt, args) + 1;
	va_end(args);
	core_string_t result = _allocate_string(len);
	va_list args2;
	va_start(args2, fmt);
	vsprint(result, len, fmt, args2);
	va_end(args2);
	return result;
}
void str_free(core_string_t str) {
	free_memory(_global_str_allocator, str);
}

void char_wide_to_char(char* dest, wchar_t* str, int n) {
	int wlen = 0;
	while (wlen < (n-1) && str[wlen]) wlen++;

	// core_string_t result = _allocate_string(wlen);
	for(int i=0; i<wlen+1; ++i) {
		dest[i] = str[i];
	}
}
core_string_t str_wide_to_char(wchar_t* str) {
	int wlen = 0;
	while (str[wlen]) wlen++;

	core_string_t result = _allocate_string(wlen);
	for(int i=0; i<wlen+1; ++i) {
		result[i] = str[i];
	}

	return result;
}

// TODO should these write a null terminator?
// void core_strcpy(core_string_t dest, core_string_t src) {
// 	while (*src) {
// 		*dest++ = *src++;
// 	}
// 	*dest = *src;
// }
void char_copy(char* dest, char* src, int buf_size) {
	while (*src && buf_size > 1) {
		*dest++ = *src++;
		--buf_size;
	}
	*dest = NULL;
}
void str_copy(core_string_t* dest, core_string_t src) {
	str_check_inside_allocator(*dest);

	u64 destlen = str_len(*dest);
	u64 srclen = str_len(src);
	u64 alen = str_get_aligned_size(destlen);
	if(srclen + 1 > alen) {
		core_string_t newStr = _allocate_string(srclen);
		sys_copy_memory(newStr, src, srclen+1);
		str_free(*dest);
		*dest = newStr;
	} else {
		sys_copy_memory(*dest, src, srclen+1);
	}
}

b32 str_compare(core_string_t a, core_string_t b) {
	if(str_len(a) != str_len(b)) return FALSE;
	while(*a==*b) {
		if(*a==0) return TRUE;
		++a;
		++b;
	}
	if(*a==0 || *b==0) return TRUE;
	return FALSE;
}
b32 str_ncompare(core_string_t a, core_string_t b, u64 n) {
	for(int i=0; i<n; ++i) {
		if(a[i] != b[i]) return FALSE;
	}
	return TRUE;
}

char* str_find(core_string_t str, core_string_t find) {
	int len = str_len(str);
	int findLen = str_len(find);
	for(int i=0; i<len-findLen+1; ++i) {
		if(str_ncompare(str+i, find, findLen)) {
			// if(out) *out = str+i;
			// return TRUE;
			return str+i;
		}
	}
	return NULL;
}
int str_find_num(core_string_t str, core_string_t find) {
	int result = 0;
	int len = str_len(str);
	int findLen = str_len(find);
	for(int i=0; i<len-findLen+1; ++i) {
		if(str_ncompare(str+i, find, findLen)) {
			++result;
		}
	}
	return result;
}

// TODO in string functions that create new allocations,
// 		first check str is actually allocated in the current pool
void char_append(char* dest, char* src, int buf_size) {
	while (*dest && buf_size > 1) {
		--buf_size;
		++dest;
	}
	while (*src && buf_size > 1) {
		--buf_size;
		*dest++ = *src++;
	}
	*dest = NULL;
}
void str_append(core_string_t* str, core_string_t append) {
	u64 len = str_len(*str);
	u64 len2 = str_len(append);
	u64 alen = _allocator_align(len+1, 64);
	if(len + 1 + len2 > alen) {
		core_string_t newStr = _allocate_string(len + len2);
		sys_copy_memory(newStr, *str, len);
		gfree_memory(*str);
		*str = newStr;
	}
	sys_copy_memory(*str + len, append, len2+1);
}

void char_prepend(char* dest, char* src, int buf_size) {
	int destlen = str_len(dest);
	int srclen = str_len(src);
	int end = min(destlen+srclen, buf_size-1);
	dest[end--] = NULL;

	while (destlen) {
		dest[end--] = dest[destlen-- -1];
	}
	sys_copy_memory(dest, src, srclen);

	// while (*src && buf_size > 1) {
	// 	--buf_size;
	// 	++dest;
	// }
	// while (*src && buf_size > 1) {
	// 	--buf_size;
	// 	*dest++ = *src++;
	// }
	// *dest = NULL;
}
void str_prepend(core_string_t* str, core_string_t prepend) {
	// TODO Check str is within allocator, or always reallocate
	// dunno what that means
	u64 len = str_len(*str);
	u64 len2 = str_len(prepend);
	allocator_block_t* block = (allocator_block_t*)*str - 1;
	u64 newLen = len + len2 + 1;
	if(newLen > block->size) {
		core_string_t newStr = _allocate_string(newLen);
		sys_copy_memory(newStr+len2, *str, len+1);
		gfree_memory(*str);
		*str = newStr;
	} else {
		sys_copy_memory(*str+len2, *str, len+1);
	}
	sys_copy_memory(*str, prepend, len2);
}

void char_insert(char* dest, int index, char* src, int buf_size) {
	int destlen = str_len(dest);
	int srclen = str_len(src);
	int end = min(destlen+srclen, buf_size-1);
	dest[end--] = NULL;

	while (destlen >= index) {
		dest[end--] = dest[destlen-- -1];
	}
	sys_copy_memory(dest+index, src, srclen);
}
void str_insert(core_string_t* str, int index, core_string_t insert) {
	u64 len = str_len(*str);
	u64 len2 = str_len(insert);
	u64 result_len = len + len2;
	assert(index < len);

	core_string_t result = _allocate_string(result_len);
	sys_copy_memory(result, *str, index);
	sys_copy_memory(result+index, insert, len2);
	sys_copy_memory(result+index+len2, *str+index, len-index);
	result[result_len] = NULL;

	gfree_memory(*str);
	*str = result;
}

void char_replace(char* str, char* find, char* replace, int buf_size) {
	// core_allocator_block_t* block = (core_allocator_block_t*)*str - 1;
	// int num = core_strfindn(*str, find);
	// int flen = str_len(find);
	// int rlen = str_len(replace);
	// int end = min(str_len(str) + num*(rlen-str_len(find)), buf_size-1);
	// src[end--] = NULL;
	
	// char* s = *str;
	// char* o = newStr;
	// while(*s) {
	// 	if(core_strncmp(s, find, flen)) {
	// 		copy_memory(o, replace, rlen);
	// 		o += rlen;
	// 		s += flen;
	// 	} else {
	// 		*o = *s;
	// 		++o;
	// 		++s;
	// 	}
	// }
	// *o = 0;
	// free_memory(*str);
	// *str = newStr;
	print_err("Function not implemented");
	assert(FALSE);
}
void str_replace(core_string_t* str, core_string_t find, core_string_t replace) {
	allocator_block_t* block = (allocator_block_t*)*str - 1;
	int num = str_find_num(*str, find);
	int flen = str_len(find);
	int rlen = str_len(replace);
	u64 newSize = str_len(*str) + num*(rlen-str_len(find)) + 1;
	core_string_t newStr = _allocate_string(newSize);
	core_string_t s = *str;
	core_string_t o = newStr;
	while(*s) {
		if(str_ncompare(s, find, flen)) {
			sys_copy_memory(o, replace, rlen);
			o += rlen;
			s += flen;
		} else {
			*o = *s;
			++o;
			++s;
		}
	}
	*o = 0;
	gfree_memory(*str);
	*str = newStr;
}

void char_replace_first(char* str, char* find, char* replace, int buf_size) {
	print_err("Function not implemented");
	assert(FALSE);
}
void str_replace_first(core_string_t* str, core_string_t find, core_string_t replace) {
	allocator_block_t* block = (allocator_block_t*)*str - 1;
	int len = str_len(*str);
	int flen = str_len(find);
	int rlen = str_len(replace);
	u64 newSize = len + (rlen-flen) + 1;
	core_string_t newStr = _allocate_string(newSize);
	core_string_t s = *str;
	core_string_t o = newStr;
	int i = 0;
	while(*s) {
		if(str_ncompare(s, find, flen)) {
			sys_copy_memory(newStr, *str, i);
			sys_copy_memory(o, replace, rlen);
			s += flen;
			o += rlen;
			i += flen;
			sys_copy_memory(o, s, len-i+1);
			break;
		}
		++s;
		++o;
		++i;
	}
	gfree_memory(*str);
	*str = newStr;
}

// TODO maybe do 1 allocation and store all parts in it
int char_split(char** buffer, size_t size, char* str, char* by) {
	print_err("Function not implemented");
	assert(FALSE);
	return 0;
}
int str_split(core_string_t* buffer, size_t size, core_string_t str, core_string_t by) {
	int len = str_len(str);
	int by_len = str_len(by);
	core_string_t str1 = str;
	core_string_t str2 = str;
	int num_results = 0;

	while (*str2) {
		if (str_ncompare(str2, by, by_len)) {
			int chunk_size = str2-str1;
			if (size && chunk_size > 0) {
				core_string_t result = _allocate_string(chunk_size);
				sys_copy_memory(result, str1, chunk_size);
				result[chunk_size] = NULL;
				*buffer = result;
				++buffer;
				--size;
				++num_results;
			}
			str2 += by_len;
			str1 = str2;
		} else {
			++str2;
		}
	}

	int chunk_size = str2-str1;
	if (size && chunk_size > 0) {
		core_string_t result = _allocate_string(chunk_size);
		sys_copy_memory(result, str1, chunk_size);
		result[chunk_size] = NULL;
		*buffer = result;
		++buffer;
		--size;
		++num_results;
	}

	return num_results;
}

void char_substr(char*buffer, size_t buf_size, char* str, int start, int len) {
	print_err("Function not implemented");
	assert(FALSE);
}
core_string_t str_substr(core_string_t str, int start, int len) {
	core_string_t result = _allocate_string(len);
	sys_copy_memory(result, str+start, len);
	result[len] = NULL;
	return result;
}

void char_trim(char* str) {
	print_err("Function not implemented");
	assert(FALSE);
}
void str_trim(core_string_t* str) {
	int len = str_len(*str);
	core_string_t result = _allocate_string(len);
	core_string_t start = *str;
	core_string_t end = *str + len-1;
	while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n') ++start;
	while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') --end;
	len = end-start + 1;
	sys_copy_memory(result, start, len);
	result[len] = NULL;
	gfree_memory(*str);
	*str = result;
}

void str_strip(char* str, int start, int n) {
	int len = str_len(str);
	int i;
	for (i=0; i<len-n; ++i) {
		if (start+n+i > len) break;
		str[start+i] = str[start+n+i];
	}
	str[start+i] = NULL;
}

void str_lower(char* str) {
	char* s = str;
	while(*s) {
		if(*s >= 'A' && *s <= 'Z') {
			*s += 32;
		}
		++s;
	}
}

void str_upper(char* str) {
	char* s = str;
	while(*s) {
		if(*s >= 'a' && *s <= 'z') {
			*s -= 32;
		}
		++s;
	}
}


// HASHING
u32 seed = 0x46ab6950;

u32 murmur3_scramble(u32 k) {
	k *= 0xcc9e2d51;
	k = (k << 15) | (k >> 17);
	k *= 0x1b873593;
	return k;
}

// https://en.wikipedia.org/wiki/MurmurHash
u32 murmur3(u8* key) {
	u32 len = str_len((char*)key);

	u32 c1 = 0xcc9e2d51;
	u32 c2 = 0x1b873593;
	u32 r1 = 15;
	u32 r2 = 13;
	u32 m = 5;
	u32 n = 0xe6546b64;

	u32 hash = seed;
	u32 k;

	for(int i = len>>2; i; --i) {
		sys_copy_memory(&k, key, sizeof(u32));
		key += sizeof(u32);
		hash ^= murmur3_scramble(k);
		hash = (hash << 13) | (hash >> 19);
		hash *= 5 + 0xe6546b64;
	}

	k=0;
	for(int i = len&3; i; --i) {
		k <<= 8;
		k |= key[i-1];
	}

	hash ^= murmur3_scramble(k);
	hash ^= len;
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;
	return hash;
}


#	endif
#endif
