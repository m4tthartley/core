#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// #include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef __int64  i64;
typedef unsigned __int64  u64;
typedef __int32  i32;
typedef unsigned __int32  u32;
typedef __int16  i16;
typedef unsigned __int16  u16;
typedef __int8  i8;
typedef unsigned __int8  u8;

typedef u32 b32;
typedef u8 byte;
typedef u64 size_t;

#define TRUE 1
#define FALSE 0

// #define assert(exp) ((exp) ? (exp) : (*(int*)0 = 0))
#define assert(exp) if(!(exp)) { printf("Assertion failed (" #exp ") in function \"%s\" \n", __FUNCTION__); fflush(stdout); (*(int*)0 = 0); } //(*(int*)0 = 0);
#define array_size(a) (sizeof(a)/sizeof(a[0]))

#define KILOBYTES(n) (n*1024)
#define MEGABYTES(n) (n*1024*1024)
#define GIGABYTES(n) (n*1024*1024*1024)
#define KB KILOBYTES
#define MB MEGABYTES
#define GB GIGABYTES

#define PAGE_SIZE 4096


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
	u64 stack;
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

// Power of 2 align
u64 align64(u64 size, u64 align) {
	if(!(size & (align-1))) {
		return size;
	} else {
		return (size & ~(align-1)) + align;
	}
}

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

void zeroMemory(byte* address, int size) {
	byte* end = address+size;
	while(address<end) {
		*address++ = 0;
	}
}

void copyMemory(byte* dest, byte* src, int size) {
	byte* end = dest+size;
	while(dest<end) {
		*dest++ = *src++;
	}
}

void* m_push_into_reserve(memory_arena* arena, size_t size) {
	assert(arena->flags & ARENA_RESERVE);
	assert(arena->stack + size <= arena->size);
	if(arena->stack+size > arena->commit) {
		size_t pagesToCommit = align64(arena->stack+size - arena->commit, PAGE_SIZE);
		VirtualAlloc((u8*)arena->address+arena->commit, pagesToCommit, MEM_COMMIT, PAGE_READWRITE);
		arena->commit += pagesToCommit;
	}
	arena->stack += size;
	return (u8*)arena->address + arena->stack - size;
}

#define pushMemory m_push
void* m_push(memory_arena* arena, int size) {
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

void* _m_alloc_into_free(memory_arena* arena, memory_block* free, size_t size) {
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

void* m_alloc(memory_arena* arena, size_t size) {
	assert(arena->address);
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
	list_remove(&arena->blocks, block);
	list_add(&arena->free, block);
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


// STRINGS
// TODO string pools
// TODO string functions
/*
	s_intern()
	s_size() probably a header structure
	s_format(char*fmt, ...)
	s_replace()
	s_replaceSingle()
	s_split()
	s_append()
	s_prepend()
	s_insert()
	s_copy()
	s_find()
	s_compare()
	s_copy()
	s_trim() ?

	toLowerCase()
	toUpperCase()

	delete()
*/
// typedef struct {
// 	char* str;
// 	u32 len;
// } string;
typedef char* string;
typedef struct {
	u8 buffer[1024*1024];
} string_pool;

u32 slen(char* str) {
	u32 len = 0;
	while(*str++) ++len;
	return len;
}

b32 scompare(char* a, char* b) {
	while(*a==*b) {
		if(*a==0) return TRUE;
		++a;
		++b;
	}
	return FALSE;
}

b32 sfind(char* str, char* find) {
	int len = slen(str);
	int findLen = slen(find);
	for(int i=0; i<len-findLen+1; ++i) {
		if(scompare(str+i, find)) return TRUE;
	}
	return FALSE;
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