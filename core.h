
#ifndef __CORE_HEADER__
#define __CORE_HEADER__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmeapi.h>
#include <stdio.h>
// #include <string.h>

#include "terminal.h"

typedef __int64  i64;
typedef unsigned __int64  u64;
typedef __int32  i32;
typedef unsigned __int32  u32;
typedef __int16  i16;
typedef unsigned __int16  u16;
typedef __int8  i8;
typedef unsigned __int8  u8;

typedef float f32;
typedef double f64;

typedef u32 b32;
typedef u8 byte;
typedef u64 size_t;

#define TRUE 1
#define FALSE 0

#define KILOBYTES(n) (n*1024)
#define MEGABYTES(n) (n*1024*1024)
#define GIGABYTES(n) (n*1024*1024*1024)
#define KB KILOBYTES
#define MB MEGABYTES
#define GB GIGABYTES

#define PAGE_SIZE 4096

// #define assert(exp) ((exp) ? (exp) : (*(int*)0 = 0))
#define assert(exp) if(!(exp)) { printf("Assertion failed (" #exp ") in function \"%s\" \n", __FUNCTION__); fflush(stdout); (*(int*)0 = 0); } //(*(int*)0 = 0);
#define array_size(a) (sizeof(a)/sizeof(a[0]))

#define FOR(index, count) for(int index=0; index<count; ++index)
#define FORSTATIC(index, arr) for(int index=0; index<(sizeof(arr)/sizeof(arr[0])); ++index)
#define FORDYNARR(index, arr) for(int index=0; index<arr.count; ++index)


// BACKWARDS COMPATIBILITY
#define pushMemory m_push
#define slen s_len
#define zeroMemory m_zero
#define copyMemory m_copy


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
	// size_t num_bytes;
	audio_sample_t data[];
} audio_buffer_t;
typedef audio_buffer_t wave_t;


// ERRORS
void core_error(b32 fatal, char* err, ...) {
	char str[1024];
	va_list va;
	va_start(va, err);
	vsnprintf(str, 1024, err, va);
	print(REDF "%s\n" RESET, str);
	MessageBox(NULL, str, NULL, MB_OK);
	va_end(va);
	if (fatal) {
		exit(1);
	}
}

void core_win32_error(DWORD error_code, b32 fatal, char* err, ...) {
	if (!error_code) {
		error_code = GetLastError();
	}
	char str[1024];
	va_list va;
	va_start(va, err);
	vsnprintf(str, 1024, err, va);
	char* msg;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		&msg,
		0,
		NULL);
	print(REDF "%s\n%s\n" RESET, str, msg);
	MessageBox(NULL, str, NULL, MB_OK);
	va_end(va);
	LocalFree(msg);
	if (fatal) {
		exit(1);
	}
}

// #define core_error_exit(...)\
// 	core_error(__VA_ARGS__);\
// 	exit(1);

// char* _win32_hresult_string(HRESULT hresult) {
// 	FormatMessage(
// 		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
// 		NULL,
// 		hresult,
// 		0,
// 		// (LPWSTR)_win32_error_buffer,
// 		// sizeof(_win32_error_buffer),
// 		NULL,
// 		0,
// 		NULL);
// 	return _win32_error_buffer;
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
// void addToListBack(ll_node** head, ll_node* item) {
// 	item->next = 0;
// 	if(*head) {
// 		ll_node* w = *head;
// 		while(w->next) {
// 			w = w->next;
// 		}
// 		w->next = item;
// 		item->prev = w;
// 	} else {
// 		*head = item;
// 	}
// }
// void removeFromList(ll_node **head, ll_node* item) {
// 	if(item->next) {
// 		item->next->prev = item->prev;
// 	}
// 	if(item->prev) {
// 		item->prev->next = item->next;
// 	} else {
// 		*head = item->next;
// 	}
// }


// MEMORY
// TODO pools
// TODO freelist
// TODO dynamic arrays

#define m_arena memory_arena
#define m_block memory_block

typedef enum {
	ARENA_STATIC = 1,
	ARENA_DYNAMIC = 1<<1,
	ARENA_RESERVE = 1<<2,
	ARENA_STACK = 1<<3,
	ARENA_FREELIST = 1<<4,
} arena_flags;
typedef struct {
	list_node node;
	// void* address;
	u64 size;
	// struct memory_block* next;
	// struct memory_block* prev;
} memory_block;
typedef struct {
	u8* address;
	u64 size;
	u64 stack; // TODO some of this can be unionized
	u64 commit;
	// todo: linked lists | what does that mean?
	u64 flags;
	list blocks;
	list free;
} memory_arena;

// memory_arena memoryInit(void* address, u64 size, u64 flags, u64 reserveSize) {
// 	assert(flags&ARENA_STACK || flags&ARENA_FREELIST);
// 	assert(!(flags&ARENA_STACK && flags&ARENA_FREELIST));
// }

void m_stack(memory_arena* arena, u8* buffer, size_t size) {
	*arena = (memory_arena){};
	arena->address = buffer;
	arena->size = size;
	arena->stack = 0;
	arena->flags = ARENA_STACK;
	if(buffer) {
		arena->flags |= ARENA_STATIC;
	}
}

void m_freelist(memory_arena* arena, u8* buffer, size_t size) {
	ZeroMemory(arena, sizeof(memory_arena));
	arena->address = buffer;
	arena->size = size;
	arena->stack = 0;
	arena->flags = ARENA_FREELIST;
	if(buffer) {
		arena->flags |= ARENA_STATIC;
	}

	arena->blocks.first = 0;
	arena->blocks.last = 0;
	// arena->free.first = arena->address;
	// arena->free.last = arena->address;
	if(buffer) {
		((memory_block*)arena->address)->size = arena->size;
		list_add(&arena->free, arena->address);
	}
}

void m_reserve(memory_arena* arena, size_t size, size_t pagesToCommit) {
	assert(!(arena->flags & ARENA_STATIC));
	arena->size = align64(size, PAGE_SIZE);
	arena->address = VirtualAlloc(0, arena->size, MEM_RESERVE, PAGE_READWRITE);
	arena->commit = align64(pagesToCommit, PAGE_SIZE);
	VirtualAlloc(arena->address, arena->commit, MEM_COMMIT, PAGE_READWRITE);
	arena->stack = 0;
	arena->flags |= ARENA_RESERVE;

	if(arena->flags & ARENA_FREELIST) {
		((memory_block*)arena->address)->size = arena->commit;
		list_add(&arena->free, arena->address);
	}
}

void m_zero(byte* address, int size) {
	byte* end = address+size;
	while(address<end) {
		*address++ = 0;
	}
}

void m_copy(byte* dest, byte* src, int size) {
	byte* end = dest+size;
	while(dest<end) {
		*dest++ = *src++;
	}
}

void* m_push_into_reserve(memory_arena* arena, size_t size) {
	assert(arena->flags & ARENA_RESERVE);
	if(arena->stack+size > arena->commit) {
		size_t pagesToCommit = align64(arena->stack+size - arena->commit, PAGE_SIZE);
		VirtualAlloc((u8*)arena->address+arena->commit, pagesToCommit, MEM_COMMIT, PAGE_READWRITE);
		arena->commit += pagesToCommit;
	}
	arena->stack += size;
	return (u8*)arena->address + arena->stack - size;
}

void* m_push(memory_arena* arena, size_t size) {
	assert(arena->stack + size <= arena->size);
	if(arena->flags & ARENA_RESERVE) {
		return m_push_into_reserve(arena, size);
	} else {
		assert(arena->stack + size <= arena->size);
		if(arena->stack + size <= arena->size) { // todo pointless?
			void* result = (byte*)arena->address+arena->stack;
			arena->stack += size;
			return result;
		}
		return 0;
	}
}

void m_pop(m_arena* arena, size_t size) {
	zeroMemory(arena->address + arena->stack - size, size);
	arena->stack -= size;
}

void m_pop_and_shift(m_arena* arena, size_t offset, size_t size) {
	assert(arena->stack >= offset + size);
	memcpy(arena->address + offset, arena->address + offset + size, arena->stack - (offset+size));
	m_pop(arena, size);
	// arena->stack -= size;
}

void* _m_alloc_into_free(memory_arena* arena, memory_block* free, size_t size) {
	arena->stack += size;
	if(free->size > size) {
		memory_block* newFree = (u8*)free + size;
		newFree->size = free->size - size;
		list_add(&arena->free, newFree);
	}
	list_remove(&arena->free, free);
	free->size = size;
	list_add(&arena->blocks, free);
	return free+1;
}

void*_m_alloc_into_virtual(memory_arena* arena, size_t size) {
	memory_block* free = arena->free.first;
	while(free) {
		if(free->size >= size) {
			return _m_alloc_into_free(arena, free, size);
		}
		free = ((list_node*)free)->next;
	}

	u64 commit = align64(size, PAGE_SIZE);
	memory_block* newMemory = VirtualAlloc((u8*)arena->address+arena->commit, commit, MEM_COMMIT, PAGE_READWRITE);
	arena->commit += commit;
	newMemory->size = commit;
	list_add(&arena->free, newMemory);
	return _m_alloc_into_free(arena, newMemory, size);
}

// TODO maybe always use ->commit for size and rename ->size
void* m_alloc(memory_arena* arena, size_t size) {
	assert(arena->address);
	assert(arena->stack + size <= arena->size);
	size += sizeof(memory_block);
	if(arena->flags & ARENA_RESERVE) {
		return _m_alloc_into_virtual(arena, size);
	} else {
		memory_block* free = arena->free.first;
		while(free) {
			if(free->size >= size) {
				return _m_alloc_into_free(arena, free, size);
			}
			free = ((list_node*)free)->next;
		}
	}
	return 0;
}

void m_free(memory_arena* arena, u8* block) {
	assert(block >= arena->address && block < arena->address+arena->size);
	block -= sizeof(memory_block);
	arena->stack -= ((memory_block*)block)->size;
	list_remove(&arena->blocks, block);
	list_add(&arena->free, block);
}

void m_clear(m_arena* arena) {
	if (arena->flags & ARENA_RESERVE) {
		m_zero(arena->address, arena->commit);
		arena->stack = 0;
		arena->blocks = (list){0};

	} else {
		m_zero(arena->address, arena->commit);
		arena->stack = 0;
	}

	if(arena->flags & ARENA_FREELIST) {
		((memory_block*)arena->address)->size = arena->commit;
		list_add(&arena->free, arena->address);
	}
}

// TODO m_defrag

void* pushRollingMemory(memory_arena* arena, int size) {
	if(arena->stack+size > arena->size) {
		arena->stack = 0;
	}
	assert(arena->stack+size <= arena->size);
	if(arena->stack+size <= arena->size) {
		void* result = (byte*)arena->address+arena->stack;
		arena->stack += size;
		return result;
	}
	return 0;
}

void* pushAndCopyMemory(memory_arena* arena, byte* src, int size) {
	void* result = pushMemory(arena, size);
	if(result) {
		copyMemory(result, src, size);
	}
	return result;
}
void* pushAndCopyRollingMemory(memory_arena* arena, byte* src, int size) {
	void* result = pushRollingMemory(arena, size);
	if(result) {
		copyMemory(result, src, size);
	}
	return result;
}

void popMemory(memory_arena* arena, int size) {
	zeroMemory((byte*)arena->address + arena->stack - size, size);
	arena->stack -= size;
}

void clearMemoryArena(memory_arena* arena) {
	zeroMemory(arena->address, arena->stack);
	arena->stack = 0;
}

#define pushStruct(arena, a)\
    pushAndCopyMemory(arena, &a, sizeof(a))


// DYNAMIC ARRAY
typedef struct {
	m_arena arena;
	int stride;
	int max;
	int count;
} dynarr_t;

dynarr_t m_dynarr_static(u8* buffer, size_t size, int stride) {
	dynarr_t result;
	m_stack(&result.arena, buffer, size);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

dynarr_t dynarr_reserve(size_t size, size_t pages_to_commit, int stride) {
	dynarr_t result;
	m_stack(&result.arena, 0, 0);
	m_reserve(&result.arena, size, pages_to_commit);
	result.stride = stride;
	result.max = size / stride;
	result.count = 0;
	return result;
}

dynarr_t dynarr(int stride) {
	return dynarr_reserve(GB(1), PAGE_SIZE, stride);
}

void dynarr_push(dynarr_t* arr, void* item) {
	void* result = m_push(&arr->arena, arr->stride);
	memcpy(result, item, arr->stride);
	++arr->count;
}

void dynarr_pop(dynarr_t* arr, int index) {
	m_pop_and_shift(&arr->arena, index*arr->stride, arr->stride);
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

	s_intern()
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
typedef memory_arena string_pool;
string_pool* _s_active_pool = NULL;

void s_create_pool(string_pool* pool, u8* buffer, u64 size) {
	m_freelist(pool, buffer, size);
}

void s_pool(string_pool* pool) {
	_s_active_pool = pool;
}

void s_pool_clear(string_pool* pool) {
	m_clear(pool);
}

u32 s_len(char* str) {
	u32 len = 0;
	while(*str++) ++len;
	return len;
}

string s_create(char* str) {
	assert(_s_active_pool);
	u64 len = s_len(str);
	char* result = m_alloc(_s_active_pool, align64(len+1, 64));
	memcpy(result, str, len+1);
	return result;
}

void s_free(string str) {
	m_free(_s_active_pool, str);
}

char* s_format(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(0, 0, fmt, args) + 1;
	char* result = m_alloc(_s_active_pool, align64(len+1, 64));
	vsnprintf(result, len, fmt, args);
	return result;
}

char* s_copy(char* str) {
	return s_create(str);
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
	int len = slen(str);
	int findLen = slen(find);
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
	int len = slen(str);
	int findLen = slen(find);
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
		string newStr = m_alloc(_s_active_pool, align64(len + len2 + 1, 64));
		memcpy(newStr, *str, len);
		m_free(_s_active_pool, *str);
		*str = newStr;
	}
	memcpy(*str + len, append, len2+1);
}

void s_prepend(string* str, char* prepend) {
	u64 len = s_len(*str);
	u64 len2 = s_len(prepend);
	memory_block* block = (memory_block*)*str - 1;
	u64 newLen = len + len2 + 1;
	if(newLen > block->size) {
		string newStr = m_alloc(_s_active_pool, align64(newLen, 64));
		memcpy(newStr+len2, *str, len+1);
		m_free(_s_active_pool, *str);
		*str = newStr;
	} else {
		memcpy(*str+len2, *str, len+1);
	}
	memcpy(*str, prepend, len2);
}

void s_insert(string* str, u64 index, char* insert) {
	u64 len = s_len(*str);
	assert(index < len);
	u64 len2 = s_len(insert);
	memory_block* block = (memory_block*)*str - 1;
	u64 newLen = len + len2 + 1;
	if(newLen > block->size) {
		string newStr = m_alloc(_s_active_pool, align64(newLen, 64));
		memcpy(newStr+index+len2, *str+index, len+1);
		m_free(_s_active_pool, *str);
		*str = newStr;
	} else {
		memcpy(*str+index+len2, *str+index, len+1);
	}
	memcpy(*str+index, insert, len2);
}
// s_split()
// s_trim()
void s_replace(string* str, char* find, char* replace) {
	memory_block* block = (memory_block*)*str - 1;
	int num = s_findn(*str, find);
	int flen = s_len(find);
	int rlen = s_len(replace);
	u64 newSize = s_len(*str) + num*(rlen-s_len(find)) + 1;
	string newStr = m_alloc(_s_active_pool, align64(newSize, 64));
	char* s = *str;
	char* o = newStr;
	while(*s) {
		if(s_ncompare(s, find, flen)) {
			memcpy(o, replace, rlen);
			o += rlen;
			s += flen;
		} else {
			*o = *s;
			++o;
			++s;
		}
	}
	*o = 0;
	m_free(_s_active_pool, *str);
	*str = newStr;
}

void s_replace_single(string* str, char* find, char* replace) {
	memory_block* block = (memory_block*)*str - 1;
	// int num = s_findn(*str, find);
	int len = s_len(*str);
	int flen = s_len(find);
	int rlen = s_len(replace);
	u64 newSize = s_len(*str) + (rlen-s_len(find)) + 1;
	string newStr = m_alloc(_s_active_pool, align64(newSize, 64));
	char* s = *str;
	char* o = newStr;
	int i = 0;
	while(*s) {
		if(s_ncompare(s, find, flen)) {
			memcpy(newStr, *str, i);
			memcpy(o, replace, rlen);
			s += flen;
			o += rlen;
			i += flen;
			memcpy(o, s, len-i+1);
			// newStr[newSize-1] = '|';
			break;
		}
		++s;
		++o;
		++i;
	}
	m_free(_s_active_pool, *str);
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
	u32 len = strlen((char*)key);

	u32 c1 = 0xcc9e2d51;
	u32 c2 = 0x1b873593;
	u32 r1 = 15;
	u32 r2 = 13;
	u32 m = 5;
	u32 n = 0xe6546b64;

	u32 hash = seed;
	u32 k;

	for(int i = len>>2; i; --i) {
		memcpy(&k, key, sizeof(u32));
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


// THREADS AND ATOMICS
// TODO TryEnterCriticalSection 
typedef struct {
	CRITICAL_SECTION win32_section;
} core_critical_section_t;

void core_init_critical_section(core_critical_section_t* section) {
	InitializeCriticalSection(&section->win32_section);
}

// TODO maybe core_atomic_section, enter_atomic_section, exit_atomic_section, atomic_lock
void core_enter_critical_section(core_critical_section_t* section) {
	// if (!) {
		// TODO I assume this is unsafe as you could have a 
		// race condition on the initialization?
		// InitializeCriticalSection(&section->win32_section);
	// }
	EnterCriticalSection(&section->win32_section);
}

void core_exit_critical_section(core_critical_section_t* section) {
	LeaveCriticalSection(&section->win32_section);
}

int atomic_swap32(void *ptr, int swap) {
	return _InterlockedExchange((long volatile*)ptr, swap);
}

b32 atomic_compare_swap32(void *ptr, int cmp, int swap) {
	return _InterlockedCompareExchange((long volatile*)ptr, swap, cmp) == cmp;
}

int atomic_add32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, value);
}

int atomic_sub32(void *ptr, int value) {
	return _InterlockedExchangeAdd((long volatile*)ptr, -value);
}

int atomic_read32(void *ptr) {
	return _InterlockedExchangeAdd((long volatile*)ptr, 0);
}

#endif

