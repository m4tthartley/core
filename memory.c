
#define COREAPI

typedef unsigned char byte;

typedef struct {
	void* address;
	int size; //note: 32bit only 
	struct memory_block* next;
	struct memory_block* prev;
} memory_block;
typedef struct {
	void* address;
	int size; //note: 32bit only 
	int stack;
	// todo: linked lists
} memory_arena;

#define assert(exp) (exp ? (exp) : (*(int*)0 = 0))

COREAPI void zeroMemory(byte* address, int size) {
	byte* end = address+size;
	while(address<end){
		*address++ = 0;
	}
}

COREAPI void copyMemory(byte* dest, byte* src, int size) {
	byte* end = dest+size;
	while(dest<end){
		*dest++ = *src++;
	}
}

// arenas
COREAPI void* pushMemory(memory_arena* arena, int size) {
	assert(arena->stack + size <= arena->size);
	if(arena->stack + size <= arena->size) {
		void* result = (byte*)arena->address+arena->stack;
		arena->stack += size;
		return result;
	}
	return 0;
}
COREAPI void* pushRollingMemory(memory_arena* arena, int size) {
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

COREAPI void* pushAndCopyMemory(memory_arena* arena, byte* src, int size) {
	void* result = pushMemory(arena, size);
	if(result) {
		copyMemory(result, src, size);
	}
	return result;
}
COREAPI void* pushAndCopyRollingMemory(memory_arena* arena, byte* src, int size) {
	void* result = pushRollingMemory(arena, size);
	if(result) {
		copyMemory(result, src, size);
	}
	return result;
}

COREAPI void popMemory(memory_arena* arena, int size) {
	zeroMemory((byte*)arena->address + arena->stack - size, size);
	arena->stack -= size;
}

COREAPI void clearMemoryArena(memory_arena* arena) {
	zeroMemory(arena->address, arena->stack);
	arena->stack = 0;
}

#define pushStruct(arena, a)\
    pushAndCopyMemory(arena, &a, sizeof(a))

// strings
int stringLength(char* str) {
	int result = 0;
	while(*str) {
		++str;
		++result;
	}
	return result;
}
int ntStringLength(char* str) { // including null terminator
	int result = 0;
	while(*str) {
		++str;
		++result;
	}
	++result;
	return result;
}

int compareStrings(char* str1, char* str2) {
	while(*str1 == *str2) {
		if(!*str1 && !*str2) {
			return 1;
		}
		++str1;
		++str2;
	}
	return 0;
}

int nCompareStrings(char* str1, char* str2, int size) {
	for(int i=0; i<size; ++i) {
		if(*str1 != *str2) {
			return 0;
		}
		++str1;
		++str2;
	}
	return 1;
}

memory_arena* _strBuffer;
char* _string;
void strBuffer(memory_arena* ptr) {
	_strBuffer = ptr;
}

void string(char* string) {
	_string = pushAndCopyMemory(_strBuffer, string, ntStringLength(string));
}

char* getString() {
	return _string;
}

void catf(float f) {
	//char* input = "F \0";
	char buffer[64];
	gcvt(f, 6, buffer);
	int len = stringLength(buffer);
	//buffer[len] = 'F';
	buffer[len+0] = ' ';
	buffer[len+1] = 0;
	popMemory(_strBuffer, 1);
	pushAndCopyMemory(_strBuffer, buffer, ntStringLength(buffer));
}

void cats(char* string) {
	popMemory(_strBuffer, 1);
	pushAndCopyMemory(_strBuffer, string, ntStringLength(string));
}

void stringClear() {
	clearMemoryArena(_strBuffer);
}

char* stringFormat(char* text, ...) {
	va_list args;
	va_start(args, text);
	int length = vsnprintf(0, 0, text, args) + 1;
	char* buffer = pushMemory(_strBuffer, length);
	vsnprintf(buffer, length, text, args);
	_string = buffer;
	return buffer;
}