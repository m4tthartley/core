//
//  core.c
//  Core
//
//  Created by Matt Hartley on 13/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "core.h"


#ifdef __WIN32__
#	include "platform_win32.c"
#endif

#ifdef __LINUX__
#	include "platform_linux.c"
#endif

#ifdef __MACOS__
#	include "platform_macos.c"
#endif


// PRINTING
void core_print_inline(char* fmt, ...) {
	char str[1024];
	va_list va;
	va_start(va, fmt);
	vsnprintf(str, 1024, fmt, va);
	fputs(str, stdout);
	va_end(va);
}
void core_print(char* fmt, ...) {
	char str[1024];
	va_list va;
	va_start(va, fmt);
	vsnprintf(str, 1024, fmt, va);
	puts(str);
	va_end(va);
}
void core_error(char* fmt, ...) {
	assert(fmt > (char*)TRUE); // Might be using old format with boolean as first parameter
	char str[1024];
	va_list va;
	va_start(va, fmt);
	vsnprintf(str, 1024, fmt, va);
	core_print(TERM_RED_FG "%s" TERM_RESET, str);
	va_end(va);
}
int core_print_to_buffer(char* buffer, size_t len, char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	int result = vsnprintf(buffer, len, fmt, va);
	va_end(va);
	return result;
}
int core_print_to_buffer_va(char* buffer, size_t len, char* fmt, va_list args) {
	int result = vsnprintf(buffer, len, fmt, args);
	return result;
}


// MISC
f32 core_randf() {
	f32 result = (f32)rand() / (f32)RAND_MAX;
	return result;
}
f32 core_randfr(f32 a, f32 b) {
	f32 result = (f32)rand() / (f32)RAND_MAX;
	result = a + (b-a)*result;
	return result;
}
int core_rand(int min, int max) {
	int result = rand() % (max-min);
	return min + result;
}


// ALIGNMENT
// non power of 2 align
int valign(int n, int stride) {
	return (n/stride + 1)*stride;
}
/*int roundUp(int n, int stride) {
	return (n/stride + 1)*stride;
}
int roundDown(int n, int stride) {
	return (n/stride)*stride;
}*/

// Power of 2 align
u64 align64(u64 size, u64 align) {
	if(!(size & (align-1))) {
		return size;
	} else {
		return (size & ~(align-1)) + align;
	}
}
u32 align32(u32 size, u32 align) {
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
// int align_pow2_round_up(int value) {
// 	int l = log2(value) + 1;
// 	int target = 0x1 << l;
// 	return target;
// }


// LINKED LISTS
void list_add(core_list_t* list, core_node_t* item) {
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
void list_add_beginning(core_list_t* list, core_node_t* item) {
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
void list_add_after(core_list_t* list, core_node_t* node, core_node_t* item) {
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
void list_add_before(core_list_t* list, core_node_t* node, core_node_t* item) {
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
void list_remove(core_list_t* list, core_node_t* item) {
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
	
	((core_memblock_t*)arena.address)->size = arena.commit;
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
		core_memblock_t* next_free = ((core_node_t*)free)->next;

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
	// if (!arena) {
	// 	core_error("malloc");
	// 	return malloc(size);
	// }
	assert(arena);
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
		// If the free block's size isn't exactly the same,
		// it will need to split the free block.
		// The new split free block must be at least the size of core_memblock_t
		// to store the block header.
		if(free->size == required_size
			|| free->size >= (required_size+sizeof(core_memblock_t))) {
			return _core_alloc_into_free(arena, free, required_size);
		}
		free = ((core_node_t*)free)->next;
	}

	if(is_arena_virtual(arena)) {
		_core_virtual_allocator_commit(arena, required_size);
		return core_alloc_in(arena, required_size);
	}

#ifdef CORE_CRASHING_ASSERTS
	assert(!"Failed to find a free block large enough");
#else
	core_error("Failed to find a free block large enough");
#endif
	return NULL;
}

void* core_alloc(size_t size) {
	return core_alloc_in(core_global_allocator, size);
}

void core_free_in(core_allocator_t* arena, u8* block) {
	// if (!arena) {
	// 	// core_print("free");
	// 	return free(block);
	// }
	assert(arena);
	assert(block);
	// assert(block >= arena->address && block < arena->address+arena->size);
	if (block >= arena->address && block < arena->address+arena->size) {
		block -= sizeof(core_memblock_t);
		// arena->stack -= ((core_memblock_t*)block)->size;
		list_remove(&arena->blocks, block);
		list_add(&arena->free, block);

		core_defrag_free_block(arena, block);
	}
}

void core_free(u8* block) {
	return core_free_in(core_global_allocator, block);
}

void core_clear_allocator(core_allocator_t* arena) {
	arena->blocks = (core_list_t){0};
	arena->free = (core_list_t){0};
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
				core_print_inline("\\n");
			} else {
				core_print_inline("%c", c);
			}
		} else {
			core_print_inline("_");
		}
	}
}

void core_print_arena(core_allocator_t* arena) {
	u64 index = 0;
	u64 arena_size = is_arena_virtual(arena) ? arena->commit : arena->size;

next:
	while(index < arena_size) {
		core_memblock_t* b = arena->blocks.first;
		while(b) {
			if(arena->address+index == (void*)b) {
				core_print_inline(TERM_RESET TERM_INVERTED);
				char size[32];
				core_print_to_buffer(size, sizeof(size), "block %li", b->size);
				core_print_inline(size);
				for(int i=0; i<sizeof(core_memblock_t)-core_strlen(size); ++i) {
					core_print_inline(" ");
				}
				core_print_inline(TERM_RESET TERM_BLUE_BG);
				print_core_memblock_t(b);
				index += b->size;
				goto next;
			}
			b = ((core_node_t*)b)->next;
		}

		core_memblock_t* f = arena->free.first;
		while(f) {
			if(arena->address+index == (void*)f) {
				core_print_inline(TERM_RESET TERM_INVERTED);
				char size[32];
				core_print_to_buffer(size, sizeof(size), "free %li", f->size);
				core_print_inline(size);
				for(int i=0; i<sizeof(core_memblock_t)-core_strlen(size); ++i) {
					core_print_inline(" ");
				}
				core_print_inline(TERM_RESET TERM_GREEN_BG);
				print_core_memblock_t(f);
				index += f->size;
				goto next;
			}
			f = ((core_node_t*)f)->next;
		}

		core_print_inline(TERM_RED_BG "FATAL ERROR");
		exit(1);
	}

	assert(index == arena_size);
	core_print_inline(TERM_RESET TERM_YELLOW_BG "END" TERM_RESET "\n");
}


// DYNAMIC ARRAY
core_dynarr_t core_dynarr_static(u8* buffer, size_t size, int stride) {
	core_dynarr_t result;
	result.arena = core_stack(buffer, size);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

core_dynarr_t core_dynarr_virtual(size_t size, size_t pages_to_commit, int stride) {
	core_dynarr_t result;
	result.arena = core_virtual_stack(size, pages_to_commit);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

core_dynarr_t core_dynarr(int stride) {
	return core_dynarr_virtual(GB(1), PAGE_SIZE, stride);
}

void core_dynarr_push(core_dynarr_t* arr, void* item) {
	void* result = core_push(&arr->arena, arr->stride);
	core_copy(result, item, arr->stride);
	++arr->count;
}

void core_dynarr_pop(core_dynarr_t* arr, int index) {
	core_pop_and_shift(&arr->arena, index*arr->stride, arr->stride);
	--arr->count;
}

void* core_dynarr_get(core_dynarr_t* arr, int index) {
	return arr->arena.address + (arr->stride * index);
}

void core_dynarr_clear(core_dynarr_t* arr) {
	arr->arena.stack = 0;
	arr->count = 0;
}


// STRINGS
int str_get_aligned_size(int size) {
	// NOTE: don't think adding sizeof memblock at this point is needed
	return align64(size+1 /*+ sizeof(core_memblock_t)*/, 64);
}

core_string_t core_allocate_string(size_t len) {
	core_string_t result = core_alloc(str_get_aligned_size(len));
	return result;
}

void str_check_inside_allocator(core_string_t str) {
	assert(str >= core_global_allocator->address &&
			str < (core_global_allocator->address+core_global_allocator->size));
}

int str_len(char* str) {
	int len = 0;
	while(*str++) ++len;
	return len;
}

core_string_t strasd(char* str) {
	u64 len = core_strlen(str);
	core_string_t result = core_allocate_string(len);
	if (result) {
		core_copy(result, str, len+1);
	}
	return result;
}
core_string_t strf(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int len = core_print_to_buffer_va(0, 0, fmt, args) + 1;
	va_end(args);
	core_string_t result = core_allocate_string(len);
	va_list args2;
	va_start(args2, fmt);
	core_print_to_buffer_va(result, len, fmt, args2);
	va_end(args2);
	return result;
}
void str_free(core_string_t str) {
	core_free(str);
}

void str_wide_to_char(char* dest, wchar_t* str, int n) {
	int wlen = 0;
	while (wlen < (n-1) && str[wlen]) wlen++;

	// core_string_t result = core_allocate_string(wlen);
	for(int i=0; i<wlen+1; ++i) {
		dest[i] = str[i];
	}
}
core_string_t strd_wide_to_char(wchar_t* str) {
	int wlen = 0;
	while (str[wlen]) wlen++;

	core_string_t result = core_allocate_string(wlen);
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
void str_copy(char* dest, char* src, int buf_size) {
	while (*src && buf_size > 1) {
		*dest++ = *src++;
		--buf_size;
	}
	*dest = NULL;
}
void strd_copy(core_string_t* dest, core_string_t src) {
	str_check_inside_allocator(*dest);

	u64 destlen = str_len(*dest);
	u64 srclen = str_len(src);
	u64 alen = str_get_aligned_size(destlen);
	if(srclen + 1 > alen) {
		core_string_t newStr = core_allocate_string(srclen);
		core_copy(newStr, src, srclen+1);
		str_free(*dest);
		*dest = newStr;
	} else {
		core_copy(*dest, src, srclen+1);
	}
}

b32 str_compare(core_string_t a, core_string_t b) {
	if(core_strlen(a) != core_strlen(b)) return FALSE;
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

char* str_find(core_string_t str, core_string_t find, core_string_t* out) {
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
	int len = core_strlen(str);
	int findLen = core_strlen(find);
	for(int i=0; i<len-findLen+1; ++i) {
		if(core_strncmp(str+i, find, findLen)) {
			++result;
		}
	}
	return result;
}

// TODO in string functions that create new allocations,
// 		first check str is actually allocated in the current pool
void str_append(char* dest, char* src, int buf_size) {
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
void strd_append(core_string_t* str, core_string_t append) {
	u64 len = core_strlen(*str);
	u64 len2 = core_strlen(append);
	u64 alen = align64(len+1, 64);
	if(len + 1 + len2 > alen) {
		core_string_t newStr = core_allocate_string(len + len2);
		core_copy(newStr, *str, len);
		core_free(*str);
		*str = newStr;
	}
	core_copy(*str + len, append, len2+1);
}

void str_prepend(char* dest, char* src, int buf_size) {
	int destlen = str_len(dest);
	int srclen = str_len(src);
	int end = min(destlen+srclen, buf_size-1);
	dest[end--] = NULL;

	while (destlen) {
		dest[end--] = dest[destlen-- -1];
	}
	core_copy(dest, src, srclen);

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
void strd_prepend(core_string_t* str, core_string_t prepend) {
	// TODO Check str is within allocator, or always reallocate
	// dunno what that means
	u64 len = core_strlen(*str);
	u64 len2 = core_strlen(prepend);
	core_memblock_t* block = (core_memblock_t*)*str - 1;
	u64 newLen = len + len2 + 1;
	if(newLen > block->size) {
		core_string_t newStr = core_allocate_string(newLen);
		core_copy(newStr+len2, *str, len+1);
		core_free(*str);
		*str = newStr;
	} else {
		core_copy(*str+len2, *str, len+1);
	}
	core_copy(*str, prepend, len2);
}

void str_insert(char* dest, int index, char* src, int buf_size) {
	int destlen = str_len(dest);
	int srclen = str_len(src);
	int end = min(destlen+srclen, buf_size-1);
	dest[end--] = NULL;

	while (destlen >= index) {
		dest[end--] = dest[destlen-- -1];
	}
	core_copy(dest+index, src, srclen);
}
void strd_insert(core_string_t* str, int index, core_string_t insert) {
	u64 len = core_strlen(*str);
	u64 len2 = core_strlen(insert);
	u64 result_len = len + len2;
	assert(index < len);

	core_string_t result = core_allocate_string(result_len);
	core_copy(result, *str, index);
	core_copy(result+index, insert, len2);
	core_copy(result+index+len2, *str+index, len-index);
	result[result_len] = NULL;

	core_free(*str);
	*str = result;
}

void core_strreplace(core_string_t* str, core_string_t find, core_string_t replace) {
	core_memblock_t* block = (core_memblock_t*)*str - 1;
	int num = core_strfindn(*str, find);
	int flen = core_strlen(find);
	int rlen = core_strlen(replace);
	u64 newSize = core_strlen(*str) + num*(rlen-core_strlen(find)) + 1;
	core_string_t newStr = core_allocate_string(newSize);
	core_string_t s = *str;
	core_string_t o = newStr;
	while(*s) {
		if(core_strncmp(s, find, flen)) {
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

void core_strreplace_single(core_string_t* str, core_string_t find, core_string_t replace) {
	core_memblock_t* block = (core_memblock_t*)*str - 1;
	int len = core_strlen(*str);
	int flen = core_strlen(find);
	int rlen = core_strlen(replace);
	u64 newSize = core_strlen(*str) + (rlen-core_strlen(find)) + 1;
	core_string_t newStr = core_allocate_string(newSize);
	core_string_t s = *str;
	core_string_t o = newStr;
	int i = 0;
	while(*s) {
		if(core_strncmp(s, find, flen)) {
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

// TODO maybe do 1 allocation and store all parts in it
int core_strsplit(core_string_t* buffer, size_t size, core_string_t str, core_string_t by) {
	int len = core_strlen(str);
	int by_len = core_strlen(by);
	core_string_t str1 = str;
	core_string_t str2 = str;
	int num_results = 0;

	while (*str2) {
		if (core_strncmp(str2, by, by_len)) {
			int chunk_size = str2-str1;
			if (size && chunk_size > 0) {
				core_string_t result = core_allocate_string(chunk_size);
				core_copy(result, str1, chunk_size);
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
		core_string_t result = core_allocate_string(chunk_size);
		core_copy(result, str1, chunk_size);
		result[chunk_size] = NULL;
		*buffer = result;
		++buffer;
		--size;
		++num_results;
	}

	return num_results;
}

core_string_t core_substr(core_string_t str, int start, int len) {
	core_string_t result = core_allocate_string(len);
	core_copy(result, str+start, len);
	result[len] = NULL;
	return result;
}

void core_strtrim(core_string_t* str) {
	int len = core_strlen(*str);
	core_string_t result = core_allocate_string(len);
	core_string_t start = *str;
	core_string_t end = *str + len-1;
	while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n') ++start;
	while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') --end;
	len = end-start + 1;
	core_copy(result, start, len);
	result[len] = NULL;
	core_free(*str);
	*str = result;
}

void core_strlower(core_string_t* str) {
	core_string_t s = *str;
	while(*s) {
		if(*s >= 'A' && *s <= 'Z') {
			*s += 32;
		}
		++s;
	}
}

void core_strupper(core_string_t* str) {
	core_string_t s = *str;
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
	u32 len = core_strlen((char*)key);

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
