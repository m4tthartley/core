
#ifndef __CORE_HEADER__
#define __CORE_HEADER__


#define TRUE 1
#define FALSE 0


#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef CORE_CRT_ASSERT
#	include <assert.h>
#else
#	define assert(exp) if(!(exp)) { printf("Assertion failed (" #exp ") in function \"%s\" \n", __FUNCTION__); fflush(stdout); (*(int*)0 = 0); }
#endif


#undef NULL
#define NULL 0

#define KILOBYTES(n) (n*1024)
#define MEGABYTES(n) (n*1024*1024)
#define GIGABYTES(n) (n*1024*1024*1024)
#define KB KILOBYTES
#define MB MEGABYTES
#define GB GIGABYTES

#define PAGE_SIZE 4096

#define array_size(a) (sizeof(a)/sizeof(a[0]))
#define FOR(index, count) for(int index=0; index<count; ++index)
#define FORSTATIC(index, arr) for(int index=0; index<(sizeof(arr)/sizeof(arr[0])); ++index)
#define FORDYNARR(index, arr) for(int index=0; index<arr.count; ++index)


#include "platform.h"
#include "terminal.h"


// BACKWARDS COMPATIBILITY
// TODO move compatibility stuff into the program
// #define pushMemory m_push
// #define slen s_len
// #define zeroMemory m_zero
// #define copyMemory m_copy

// #define memory_arena m_arena
// #define core_memblock_t core_memblock_t


// STRUCTURES
typedef struct {
	u32 size;
	u32 width;
	u32 height;
	u32 data[];
} bitmap_t;

typedef struct {
	union {
		i16 channels[2];
		struct {
			i16 left;
			i16 right;
		};
	};
} audio_sample_t;

typedef struct {
	int channels;
	int samples_per_second;
	int bytes_per_sample;
	size_t sample_count;
	audio_sample_t data[];
} audio_buffer_t;
typedef audio_buffer_t wave_t;


// PRINTING
void core_print(char* fmt, ...) {
	char str[1024];
	va_list va;
	va_start(va, fmt);
	vsnprintf(str, 1024, fmt, va);
	printf("%s\n", str);
	va_end(va);
}
void core_error(b32 fatal, char* fmt, ...) {
	char str[1024];
	va_list va;
	va_start(va, fmt);
	vsnprintf(str, 1024, fmt, va);
	printf(TERM_RED_FG "%s\n" TERM_RESET, str);
	va_end(va);
	if (fatal) {
		exit(1);
	}
}


// void core_error_console(b32 fatal, char* err, ...) {
// 	char str[1024];
// 	va_list va;
// 	va_start(va, err);
// 	vsnprintf(str, 1024, err, va);
// 	print(REDF "%s\n" RESET, str);
// 	va_end(va);
// 	if (fatal) {
// 		exit(1);
// 	}
// }

// void core_win32_error(DWORD error_code, b32 fatal, char* err, ...) {
// 	if (!error_code) {
// 		error_code = GetLastError();
// 	}
// 	char usrstr[1024];
// 	va_list va;
// 	va_start(va, err);
// 	vsnprintf(usrstr, 1024, err, va);

// 	char* msg;
// 	FormatMessage(
// 		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
// 		NULL,
// 		error_code,
// 		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
// 		&msg,
// 		0,
// 		NULL);

// 	char str[1024];
// 	snprintf(str, 1024, "%s\n%#08X %s\n", usrstr, error_code, msg);

// 	printf(REDF "%s" RESET, str);
// 	MessageBox(NULL, str, NULL, MB_OK);

// 	va_end(va);
// 	LocalFree(msg);
// 	if (fatal) {
// 		exit(1);
// 	}
// }


// MISC
// Power of 2 align
u64 align64(u64 size, u64 align) {
	if(!(size & (align-1))) {
		return size;
	} else {
		return (size & ~(align-1)) + align;
	}
}

// int align_pow2_round_down(int value) {
// 	int l = log2(value);
// 	int target = 0x1 << l;
// 	return target;
// }
//
// int align_pow2_round_up(int value) {
// 	int l = log2(value) + 1;
// 	int target = 0x1 << l;
// 	return target;
// }

f32 r_float() {
	f32 result = (f32)rand() / (f32)RAND_MAX;
	return result;
}
f32 r_float_range(f32 a, f32 b) {
	f32 result = (f32)rand() / (f32)RAND_MAX;
	result = a + (b-a)*result;
	return result;
}
int r_int_range(int min, int max) {
	int result = rand() % (max-min);
	return min + result;
}


// LINKED LISTS
typedef struct list_node list_node;
struct list_node {
	list_node* next;
	list_node* prev;
};
typedef struct {
	list_node* first;
	list_node* last;
} list;
void list_add(list* list, list_node* item) {
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
void list_add_beginning(list* list, list_node* item) {
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
void list_add_after(list* list, list_node* node, list_node* item) {
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
void list_add_before(list* list, list_node* node, list_node* item) {
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
void list_remove(list* list, list_node* item) {
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
// TODO pools
// typedef enum {
// 	ARENA_STATIC = 1,
// 	ARENA_DYNAMIC = 1<<1,
// 	ARENA_RESERVE = 1<<2,
// 	ARENA_STACK = 1<<3,
// 	ARENA_FREELIST = 1<<4,
// } arena_flags;
// typedef struct {
// 	list_node node;
// 	u64 size;
// } core_memblock_t;
// typedef struct {
// 	u8* address;
// 	u64 size;
// 	u64 stack; // TODO some of this can be unionized
// 	u64 commit;
// 	// todo: linked lists | what does that mean?
// 	u64 flags;
// 	list blocks;
// 	list free;
// } m_arena;

typedef struct {
	list_node node;
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
	list blocks;
	list free;
} core_allocator_t;

void core_zero(u8* address, int size);
void core_copy(u8* dest, u8* src, int size);
core_stack_t core_stack(u8* buffer, size_t size);
core_stack_t core_virtual_stack(size_t size, size_t commit);
core_allocator_t core_allocator(u8* buffer, size_t size);
core_allocator_t core_virtual_allocator(size_t size, size_t commit);
void core_use_allocator(core_allocator_t* arena);
// void m_reserve(memory_arena* arena, size_t size, size_t pagesToCommit);

void* core_push_into_virtual(core_stack_t* arena, size_t size);
void* core_push(core_stack_t* arena, size_t size);
void  core_pop(core_stack_t* arena, size_t size);
void  core_pop_and_shift(core_stack_t* arena, size_t offset, size_t size);

void  core_defrag_free_block(core_allocator_t* arena, core_memblock_t* block);
void* _core_alloc_into_free(core_allocator_t* arena, core_memblock_t* free, size_t size);
// void* _core_alloc_into_virtual(core_allocator_t* arena, size_t size);
void _core_virtual_allocator_commit(core_allocator_t* arena, size_t size);
void* core_alloc_in(core_allocator_t* arena, size_t size);
void* core_alloc(size_t size);
void  core_free_in(core_allocator_t* arena, u8* block);
void  core_free(u8* block);
void  core_clear_allocator(core_allocator_t* arena);
void  core_clear_global_allocator();

// memory_arena memoryInit(void* address, u64 size, u64 flags, u64 reserveSize) {
// 	assert(flags&ARENA_STACK || flags&ARENA_FREELIST);
// 	assert(!(flags&ARENA_STACK && flags&ARENA_FREELIST));
// }

core_allocator_t* core_global_allocator = NULL;

#define is_arena_virtual(arena) (arena->commit > 0)

void core_zero(u8* address, int size) {
	core_zero_memory(address, size);
}

void core_copy(u8* dest, u8* src, int size) {
	core_copy_memory(dest, src, size);
}

// Constructors
core_stack_t core_stack(u8* buffer, size_t size) {
	core_stack_t arena = {0};
	arena.address = buffer;
	arena.size = size;
	arena.stack = 0;
	return arena;
}

core_stack_t core_virtual_stack(size_t size, size_t commit) {
	core_stack_t arena = {0};
	arena.size = size;
	arena.commit = align64(commit, PAGE_SIZE);
	arena.address = core_reserve_virtual_memory(arena.size);
	core_commit_virtual_memory(arena.address, arena.commit);
	arena.stack = 0;
	return arena;
}

core_allocator_t core_allocator(u8* buffer, size_t size) {
	core_allocator_t arena = {0};
	arena.address = buffer;
	arena.size = size;

	arena.blocks.first = NULL;
	arena.blocks.last = NULL;
	
	((core_memblock_t*)arena.address)->size = arena.size;
	list_add(&arena.free, arena.address);

	return arena;
}

core_allocator_t core_virtual_allocator(size_t size, size_t commit) {
	core_allocator_t arena = {0};
	arena.size = size;
	arena.commit = align64(commit, PAGE_SIZE);
	arena.address = core_reserve_virtual_memory(arena.size);
	core_commit_virtual_memory(arena.address, arena.commit);

	arena.blocks.first = NULL;
	arena.blocks.last = NULL;
	
	((core_memblock_t*)arena.address)->size = arena.size;
	list_add(&arena.free, arena.address);

	return arena;
}

void core_use_allocator(core_allocator_t* arena) {
	core_global_allocator = arena;
}

// void m_reserve(memory_arena* arena, size_t size, size_t pagesToCommit) {
// 	assert(!(arena->flags & ARENA_STATIC));
// 	arena->size = align64(size, PAGE_SIZE);
// 	arena->address = core_reserve_virtual_memory(arena->size);
// 	arena->commit = align64(pagesToCommit, PAGE_SIZE);
// 	core_commit_virtual_memory(arena->address, arena->commit);
// 	arena->stack = 0;
// 	arena->flags |= ARENA_RESERVE;

// 	if(arena->flags & ARENA_FREELIST) {
// 		((core_memblock_t*)arena->address)->size = arena->commit;
// 		list_add(&arena->free, arena->address);
// 	}
// }

// Stacks
// void* core_push_into_virtual(core_stack_t* arena, size_t size) {
// 	if(arena->stack+size > arena->commit) {
// 		size_t extra_commit = align64(arena->stack+size - arena->commit, PAGE_SIZE);
// 		core_commit_virtual_memory((u8*)arena->address+arena->commit, extra_commit);
// 		arena->commit += extra_commit;
// 	}
// 	arena->stack += size;
// 	return (u8*)arena->address + arena->stack - size;
// }

void* core_push(core_stack_t* arena, size_t size) {
	assert(arena->stack + size <= arena->size);
	if(is_arena_virtual(arena)) {
		// return m_push_into_reserve(arena, size);
		if(arena->stack+size > arena->commit) {
			size_t extra_commit = align64(arena->stack+size - arena->commit, PAGE_SIZE);
			core_commit_virtual_memory((u8*)arena->address+arena->commit, extra_commit);
			arena->commit += extra_commit;
		}
	}

	if(arena->stack + size <= arena->size) {
		void* result = (byte*)arena->address+arena->stack;
		arena->stack += size;
		return result;
	}

	return 0;
}

void core_pop(core_stack_t* arena, size_t size) {
	// TODO: Should we zero this?
	core_zero(arena->address + arena->stack - size, size);
	arena->stack -= size;
}

void core_pop_and_shift(core_stack_t* arena, size_t offset, size_t size) {
	assert(arena->stack >= offset + size);
	core_copy(arena->address + offset, arena->address + offset + size, arena->stack - (offset+size));
	core_pop(arena, size);
}

// Allocators
void core_defrag_free_block(core_allocator_t* arena, core_memblock_t* block) {
	core_memblock_t* free = arena->free.first;
	while(free) {
		core_memblock_t* next_free = ((list_node*)free)->next;

		if (free != block) {
			if ((u8*)free == (u8*)block + block->size) {
				block->size += free->size;
				list_remove(&arena->free, free);
			}
			if((u8*)free + free->size == (u8*)block) {
				free->size += block->size;
				list_remove(&arena->free, block);
				block = free;
			}
		}

		free = next_free;
	}
}

void* _core_alloc_into_free(core_allocator_t* arena, core_memblock_t* free, size_t size) {
	if(free->size > size) {
		core_memblock_t* newFree = (u8*)free + size;
		newFree->size = free->size - size;
		list_add(&arena->free, newFree);
	}
	list_remove(&arena->free, free);
	free->size = size;
	list_add(&arena->blocks, free);
	return free + 1;
}

// void* _core_alloc_into_virtual(core_allocator_t* arena, size_t size) {
// 	core_memblock_t* free = arena->free.first;
// 	while(free) {
// 		if(free->size >= size) {
// 			return _core_alloc_into_free(arena, free, size);
// 		}
// 		free = ((list_node*)free)->next;
// 	}

// 	u64 commit = align64(size, PAGE_SIZE);
// 	core_memblock_t* new_memory = core_commit_virtual_memory((u8*)arena->address+arena->commit, commit);
// 	arena->commit += commit;
// 	new_memory->size = commit;
// 	list_add(&arena->free, new_memory);
// 	core_defrag_free_block(arena, new_memory);
// 	return _core_alloc_into_free(arena, new_memory, size);
// }

void _core_virtual_allocator_commit(core_allocator_t* arena, size_t size) {
	u64 commit = align64(size, PAGE_SIZE);
	core_memblock_t* new_memory = core_commit_virtual_memory((u8*)arena->address+arena->commit, commit);
	arena->commit += commit;
	new_memory->size = commit;
	list_add(&arena->free, new_memory);
	core_defrag_free_block(arena, new_memory);
}

void* core_alloc_in(core_allocator_t* arena, size_t size) {
	if (!arena) {
		core_error(FALSE, "malloc");
		return malloc(size);
	}
	assert(arena->address);
	// This shouldnt be needed for alloc?
	// It will be caught at the bottom
	// assert(arena->stack + size <= arena->size);
	size_t required_size = size + sizeof(core_memblock_t);
	// if(is_arena_virtual(arena)) {
	// 	return _core_alloc_into_virtual(arena, size);
	// } else {
		
	// }

	core_memblock_t* free = arena->free.first;
	while(free) {
		if(free->size >= required_size) {
			return _core_alloc_into_free(arena, free, required_size);
		}
		free = ((list_node*)free)->next;
	}

	if(is_arena_virtual(arena)) {
		_core_virtual_allocator_commit(arena, required_size);
		core_alloc_in(arena, required_size);
	}

#ifdef CORE_CRASHING_ASSERTS
	assert(!"Failed to find a free block large enough");
#else
	core_error(FALSE, "Failed to find a free block large enough");
#endif
	return NULL;
}

void* core_alloc(size_t size) {
	return core_alloc_in(core_global_allocator, size);
}

void core_free_in(core_allocator_t* arena, u8* block) {
	if (!arena) {
		// core_print("free");
		return free(block);
	}
	assert(block);
	assert(block >= arena->address && block < arena->address+arena->size);
	block -= sizeof(core_memblock_t);
	// arena->stack -= ((core_memblock_t*)block)->size;
	list_remove(&arena->blocks, block);
	list_add(&arena->free, block);

	core_defrag_free_block(arena, block);
}

void core_free(u8* block) {
	return core_free_in(core_global_allocator, block);
}

void core_clear_allocator(core_allocator_t* arena) {
	arena->blocks = (list){0};
	arena->free = (list){0};
	((core_memblock_t*)arena->address)->size = arena->commit;
	list_add(&arena->free, arena->address);
}

void core_clear_global_allocator() {
	core_clear_allocator(core_global_allocator);
}

// void m_clear(m_arena* arena) {
// 	if (arena->flags & ARENA_RESERVE) {
// 		m_zero(arena->address, arena->stack);
// 		arena->stack = 0;
// 		arena->blocks = (list){0};

// 	} else {
// 		m_zero(arena->address, arena->stack);
// 		arena->stack = 0;
// 	}

// 	if(arena->flags & ARENA_FREELIST) {
// 		arena->blocks = (list){0};
// 		arena->free = (list){0};
// 		((core_memblock_t*)arena->address)->size = arena->commit;
// 		list_add(&arena->free, arena->address);
// 	}
// }

// void* pushRollingMemory(memory_arena* arena, int size) {
// 	if(arena->stack+size > arena->size) {
// 		arena->stack = 0;
// 	}
// 	assert(arena->stack+size <= arena->size);
// 	if(arena->stack+size <= arena->size) {
// 		void* result = (byte*)arena->address+arena->stack;
// 		arena->stack += size;
// 		return result;
// 	}
// 	return 0;
// }

// void* pushAndCopyMemory(memory_arena* arena, byte* src, int size) {
// 	void* result = pushMemory(arena, size);
// 	if(result) {
// 		copyMemory(result, src, size);
// 	}
// 	return result;
// }
// void* pushAndCopyRollingMemory(memory_arena* arena, byte* src, int size) {
// 	void* result = pushRollingMemory(arena, size);
// 	if(result) {
// 		copyMemory(result, src, size);
// 	}
// 	return result;
// }

// void popMemory(memory_arena* arena, int size) {
// 	zeroMemory((byte*)arena->address + arena->stack - size, size);
// 	arena->stack -= size;
// }

// void clearMemoryArena(memory_arena* arena) {
// 	zeroMemory(arena->address, arena->stack);
// 	arena->stack = 0;
// }

// #define pushStruct(arena, a)\
//     pushAndCopyMemory(arena, &a, sizeof(a))

void print_core_memblock_t(core_memblock_t* block) {
	for(int i=0; i< block->size-sizeof(core_memblock_t); ++i) {
		char c = *((u8*)(block+1) + i);
		if(c) {
			if(c == '\n') {
				printf("\\n");
			} else {
				printf("%c", c);
			}
		} else {
			printf("_");
		}
	}
}

void core_print_arena(core_allocator_t* arena) {
	u64 index = 0;
	u64 arena_size = is_arena_virtual(arena) ? arena->commit : arena->size;
	while(index < arena_size) {
		core_memblock_t* b = arena->blocks.first;
		while(b) {
			if(arena->address+index == (void*)b) {
				printf(TERM_RESET TERM_INVERTED);
				char size[32];
				snprintf(size, sizeof(size), "block %li", b->size);
				printf(size);
				for(int i=0; i<sizeof(core_memblock_t)-s_len(size); ++i) {
					printf(" ");
				}
				printf(TERM_RESET TERM_BLUE_BG);
				print_core_memblock_t(b);
				index += b->size;
				goto next;
			}
			b = ((list_node*)b)->next;
		}

		core_memblock_t* f = arena->free.first;
		while(f) {
			if(arena->address+index == (void*)f) {
				printf(TERM_RESET TERM_INVERTED);
				char size[32];
				snprintf(size, sizeof(size), "free %li", f->size);
				printf(size);
				for(int i=0; i<sizeof(core_memblock_t)-s_len(size); ++i) {
					printf(" ");
				}
				printf(TERM_RESET TERM_GREEN_BG);
				print_core_memblock_t(f);
				index += f->size;
				goto next;
			}
			f = ((list_node*)f)->next;
		}

		printf(TERM_RED_BG "FATAL ERROR");
		exit(1);
next:
	}

	assert(index == arena_size);
	printf(TERM_RESET TERM_YELLOW_BG "END" TERM_RESET "\n");
}


// DYNAMIC ARRAY
typedef struct {
	core_stack_t arena;
	int stride;
	int max;
	int count;
} dynarr_t;

dynarr_t m_dynarr_static(u8* buffer, size_t size, int stride) {
	dynarr_t result;
	result.arena = core_stack(buffer, size);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

dynarr_t dynarr_reserve(size_t size, size_t pages_to_commit, int stride) {
	dynarr_t result;
	result.arena = core_virtual_stack(size, pages_to_commit);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

dynarr_t dynarr(int stride) {
	return dynarr_reserve(GB(1), PAGE_SIZE, stride);
}

void dynarr_push(dynarr_t* arr, void* item) {
	void* result = core_push(&arr->arena, arr->stride);
	core_copy(result, item, arr->stride);
	++arr->count;
}

void dynarr_pop(dynarr_t* arr, int index) {
	core_pop_and_shift(&arr->arena, index*arr->stride, arr->stride);
	--arr->count;
}

void* dynarr_get(dynarr_t* arr, int index) {
	return arr->arena.address + (arr->stride * index);
}

// void dynarr_clear(dynarr_t* arr) {
// 	m_clear
// }


// STRINGS
// TODO string pools
// TODO string functions
/*
	s_create()					DONE
	s_len()						DONE
	s_format(char*fmt, ...)	    DONE
	s_copy()					DONE
	s_find()					DONE
	s_compare()					DONE
	s_append()					DONE
	s_prepend()					DONE
	s_insert()					DONE
	s_split()
	s_trim()
	s_replace()					DONE
	s_replaceSingle()			DONE
	s_lower()					DONE
	s_upper()					DONE

	s_size()					X
	s_free()					DONE
	s_slice()
*/
// typedef struct {
// 	char* str;
// 	u32 len;
// } string;
typedef char* string;
// typedef struct {
// 	u8 buffer[1024*1024];
// } string_pool;
// typedef memory_arena string_pool;
// string_pool* _s_active_pool = NULL;

// void s_create_pool(string_pool* pool, u8* buffer, u64 size) {
// 	m_freelist(pool, buffer, size);
// }

// void s_pool(string_pool* pool) {
// 	_s_active_pool = pool;
// }

// void s_pool_clear(string_pool* pool) {
// 	m_clear(pool);
// }

char* core_allocate_string(size_t len) {
	char* result = core_alloc(align64(len+1 + sizeof(core_memblock_t), 64));
	return result;
}

u32 s_len(char* str) {
	u32 len = 0;
	while(*str++) ++len;
	return len;
}

// core_str
string s_create(char* str) {
	// assert(_s_active_pool);
	u64 len = s_len(str);
	char* result = core_allocate_string(len);
	if (result) {
		core_copy(result, str, len+1);
	}
	return result;
}

void s_free(string str) {
	core_free(str);
}

char* s_format(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(0, 0, fmt, args) + 1;
	va_end(args);
	char* result = core_allocate_string(len);
	va_list args2;
	va_start(args2, fmt);
	vsnprintf(result, len, fmt, args2);
	va_end(args2);
	return result;
}

char* core_convert_wide_string(wchar_t* str) {
	int wlen = 0;
	while (str[wlen]) wlen++;

	char* result = core_allocate_string(wlen);
	for(int i=0; i<wlen+1; ++i) {
		result[i] = str[i];
	}

	return result;
}

// char* s_copy(char* str) {
// 	return s_create(str);
// }

void s_copy(char* dest, char* src) {
	while (*src) {
		*dest++ = *src++;
	}
	*dest = *src;
}

void s_ncopy(char* dest, char* src, int n) {
	while (*src && n > 1) {
		*dest++ = *src++;
		--n;
	}
	*dest = *src;
}

b32 s_compare(char* a, char* b) {
	if(s_len(a) != s_len(b)) return FALSE;
	while(*a==*b) {
		if(*a==0) return TRUE;
		++a;
		++b;
	}
	if(*a==0 || *b==0) return TRUE;
	return FALSE;
}

b32 s_ncompare(char* a, char* b, u64 n) {
	for(int i=0; i<n; ++i) {
		if(a[i] != b[i]) return FALSE;
	}
	return TRUE;
}

b32 s_find(char* str, char* find, char** out) {
	int len = s_len(str);
	int findLen = s_len(find);
	for(int i=0; i<len-findLen+1; ++i) {
		if(s_ncompare(str+i, find, findLen)) {
			if(out) *out = str+i;
			return TRUE;
		}
	}
	return FALSE;
}

int s_findn(string str, char* find) {
	int result = 0;
	int len = s_len(str);
	int findLen = s_len(find);
	for(int i=0; i<len-findLen+1; ++i) {
		if(s_ncompare(str+i, find, findLen)) {
			++result;
		}
	}
	return result;
}

// TODO in string functions that create new allocations,
// 		first check str is actually allocated in the current pool
void s_append(string* str, char* append) {
	u64 len = s_len(*str);
	u64 len2 = s_len(append);
	u64 alen = align64(len+1, 64);
	if(len + 1 + len2 > alen) {
		string newStr = core_allocate_string(len + len2);
		core_copy(newStr, *str, len);
		core_free(*str);
		*str = newStr;
	}
	core_copy(*str + len, append, len2+1);
}

void s_prepend(string* str, char* prepend) {
	u64 len = s_len(*str);
	u64 len2 = s_len(prepend);
	core_memblock_t* block = (core_memblock_t*)*str - 1;
	u64 newLen = len + len2 + 1;
	if(newLen > block->size) {
		string newStr = core_allocate_string(newLen);
		core_copy(newStr+len2, *str, len+1);
		core_free(*str);
		*str = newStr;
	} else {
		core_copy(*str+len2, *str, len+1);
	}
	core_copy(*str, prepend, len2);
}

void s_insert(string* str, u64 index, char* insert) {
	u64 len = s_len(*str);
	assert(index < len);
	u64 len2 = s_len(insert);
	core_memblock_t* block = (core_memblock_t*)*str - 1;
	u64 newLen = len + len2 + 1;
	if(newLen > block->size) {
		string newStr = core_allocate_string(newLen);
		core_copy(newStr+index+len2, *str+index, len+1);
		core_free(*str);
		*str = newStr;
	} else {
		core_copy(*str+index+len2, *str+index, len+1);
	}
	core_copy(*str+index, insert, len2);
}
// s_split()
// s_trim()
void s_replace(string* str, char* find, char* replace) {
	core_memblock_t* block = (core_memblock_t*)*str - 1;
	int num = s_findn(*str, find);
	int flen = s_len(find);
	int rlen = s_len(replace);
	u64 newSize = s_len(*str) + num*(rlen-s_len(find)) + 1;
	string newStr = core_allocate_string(newSize);
	char* s = *str;
	char* o = newStr;
	while(*s) {
		if(s_ncompare(s, find, flen)) {
			core_copy(o, replace, rlen);
			o += rlen;
			s += flen;
		} else {
			*o = *s;
			++o;
			++s;
		}
	}
	*o = 0;
	core_free(*str);
	*str = newStr;
}

void s_replace_single(string* str, char* find, char* replace) {
	core_memblock_t* block = (core_memblock_t*)*str - 1;
	int len = s_len(*str);
	int flen = s_len(find);
	int rlen = s_len(replace);
	u64 newSize = s_len(*str) + (rlen-s_len(find)) + 1;
	string newStr = core_allocate_string(newSize);
	char* s = *str;
	char* o = newStr;
	int i = 0;
	while(*s) {
		if(s_ncompare(s, find, flen)) {
			core_copy(newStr, *str, i);
			core_copy(o, replace, rlen);
			s += flen;
			o += rlen;
			i += flen;
			core_copy(o, s, len-i+1);
			break;
		}
		++s;
		++o;
		++i;
	}
	core_free(*str);
	*str = newStr;
}

void s_lower(string* str) {
	char* s = *str;
	while(*s) {
		if(*s >= 'A' && *s <= 'Z') {
			*s += 32;
		}
		++s;
	}
}

void s_upper(string* str) {
	char* s = *str;
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
	u32 len = s_len((char*)key);

	u32 c1 = 0xcc9e2d51;
	u32 c2 = 0x1b873593;
	u32 r1 = 15;
	u32 r2 = 13;
	u32 m = 5;
	u32 n = 0xe6546b64;

	u32 hash = seed;
	u32 k;

	for(int i = len>>2; i; --i) {
		core_copy(&k, key, sizeof(u32));
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


#endif

