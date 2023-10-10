//#define GL_GLEXT_PROTOTYPES
//#include "w:/lib/glext.h"

#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#include <GL/GL.H>

#pragma warning (disable: 4133) // incompatible types

typedef unsigned __int64  uint64_t;
typedef __int64 int64_t;

//typedef WINAPI GLDECL;
#define GLDECL WINAPI
typedef char GLchar;

/*typedef enum {
	SHADER_VERTEX	= (1<<0),
	SHADER_PIXEL	 = (1<<1),
	SHADER_GEOMETRY  = (1<<2),
} ShaderType;*/

typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

typedef GLuint (GLDECL *CreateShaderProc)(GLenum type);
typedef void (GLDECL *ShaderSourceProc)(GLuint shader, GLsizei count, const GLchar * const *string, const GLint *length);
typedef void (GLDECL *CompileShaderProc)(GLuint shader);
typedef GLuint (GLDECL *CreateProgramProc) (void);
typedef void (GLDECL *AttachShaderProc) (GLuint program, GLuint shader);
typedef void (GLDECL *LinkProgramProc) (GLuint program);
typedef void (GLDECL *GetShaderivProc) (GLuint shader, GLenum pname, GLint *params);
typedef void (GLDECL *GetShaderInfoLogProc) (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void (GLDECL *GetProgramivProc) (GLuint program, GLenum pname, GLint *params);
typedef void (GLDECL *GetProgramInfoLogProc) (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void (GLDECL *UseProgramProc) (GLuint program);
typedef void (GLDECL *DeleteProgramProc) (GLuint program);
typedef void (GLDECL *DeleteShaderProc) (GLuint shader);
typedef void (GLDECL *DetachShaderProc)	(GLuint program, GLuint shader);
typedef void (GLDECL *EnableVertexAttribArrayProc) (GLuint index);
typedef void (GLDECL *VertexAttribPointerProc) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
typedef GLint (GLDECL *GetUniformLocationProc) (GLuint program, const GLchar *name);
typedef void (GLDECL *UniformMatrix4fvProc) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

typedef void (GLDECL *Uniform1fProc) (GLint location, GLfloat v0);
typedef void (GLDECL *Uniform2fProc) (GLint location, GLfloat v0, GLfloat v1);
typedef void (GLDECL *Uniform3fProc) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (GLDECL *Uniform4fProc) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (GLDECL *Uniform1fvProc) (GLint location, GLsizei count, const GLfloat *value);
typedef void (GLDECL *Uniform2fvProc) (GLint location, GLsizei count, const GLfloat *value);
typedef void (GLDECL *Uniform3fvProc) (GLint location, GLsizei count, const GLfloat *value);
typedef void (GLDECL *Uniform4fvProc) (GLint location, GLsizei count, const GLfloat *value);
typedef void (GLDECL *Uniform1iProc) (GLint location, int a);
typedef void (GLDECL *Uniform2iProc) (GLint location, int a, int b);
typedef void (GLDECL *Uniform3iProc) (GLint location, int a, int b, int c);
typedef void (GLDECL *Uniform4iProc) (GLint location, int a, int b, int c, int d);

typedef GLint (GLDECL *GetAttribLocationProc) (GLuint program, const GLchar *name);
typedef void (GLDECL *ActiveTextureProc) (GLenum texture);
typedef void (GLDECL *GenFramebuffersProc) (GLsizei n,  GLuint * framebuffers);
typedef void (GLDECL *BindFramebufferProc) (GLenum target,  GLuint framebuffer);
typedef void (GLDECL *FramebufferTextureProc) (GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (GLDECL *FramebufferTexture2DProc) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (GLDECL *DrawBuffersProc) (GLsizei n, const GLenum *bufs);
typedef GLenum (GLDECL *CheckFramebufferStatusProc) (GLenum target);
typedef void (GLDECL *BlitFramebufferProc) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

typedef void (GLDECL *LoadMatrixfProc) (const GLfloat* m);

typedef void (GLDECL *DebugMessageCallbackProc) (GLDEBUGPROC callback, void *userParam);
typedef void (GLDECL *DebugMessageControlProc) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);

#define GL_PROC_LIST \
GLEXT(CreateShader)\
GLEXT(ShaderSource)\
GLEXT(CompileShader)\
GLEXT(CreateProgram)\
GLEXT(AttachShader)\
GLEXT(LinkProgram)\
GLEXT(GetShaderiv)\
GLEXT(GetShaderInfoLog)\
GLEXT(GetProgramiv)\
GLEXT(GetProgramInfoLog)\
GLEXT(UseProgram)\
GLEXT(DeleteProgram)\
GLEXT(DeleteShader)\
GLEXT(DetachShader)\
GLEXT(EnableVertexAttribArray)\
GLEXT(VertexAttribPointer)\
GLEXT(GetUniformLocation)\
GLEXT(UniformMatrix4fv)\
GLEXT(Uniform1f)\
GLEXT(Uniform2f)\
GLEXT(Uniform3f)\
GLEXT(Uniform4f)\
GLEXT(Uniform1fv)\
GLEXT(Uniform2fv)\
GLEXT(Uniform3fv)\
GLEXT(Uniform4fv)\
GLEXT(Uniform1i)\
GLEXT(Uniform2i)\
GLEXT(Uniform3i)\
GLEXT(Uniform4i)\
GLEXT(GetAttribLocation)\
GLEXT(ActiveTexture)\
GLEXT(GenFramebuffers)\
GLEXT(BindFramebuffer)\
GLEXT(FramebufferTexture)\
GLEXT(FramebufferTexture2D)\
GLEXT(DrawBuffers)\
GLEXT(CheckFramebufferStatus)\
GLEXT(BlitFramebuffer)\
GLEXT(DebugMessageCallback)\
GLEXT(DebugMessageControl)\

#define GLEXT(name) name##Proc gl##name;
GL_PROC_LIST
#undef GLEXT

typedef struct {
	void* address;
	int size; //note: 32bit only 
} gfx_memory_block;
typedef struct {
	void* address;
	int size; //note: 32bit only 
	//array_define(memory_block, blocks, 64);
	int stackIndex;
} gfx_memory_stack;

typedef struct {
	uint64_t writeTime;
	int size;
} gfx_file_info;

typedef struct {
	int size;
	void* next;
	void* prev;
} free_block;

typedef struct {
	void* next;
	void* prev;
	GLuint handle;
	GLenum type;
	uint64_t writeTime;
	int path_size; // align size to 8 bytes
	int code_size; // align size to 8 bytes
} gfx_shader;
typedef struct {
	void* next;
	GLuint handle;
	gfx_shader* vertex;
	gfx_shader* fragment;
} gfx_program;

typedef gfx_file_info (*gfx_file_stat_proc)(char* path);
typedef int (*gfx_load_file_proc)(char* path, int size, void* output);

typedef struct {
#define GLEXT(name) name##Proc gl##name;
	GL_PROC_LIST
#undef GLEXT

	gfx_file_stat_proc file_stat;
	gfx_load_file_proc load_file;
	
	void* memory;
	int memorySize;	
	gfx_shader* shaders;
	free_block* freeBlocks;
	
	gfx_memory_stack transient;
	gfx_memory_stack programMemory;
	gfx_memory_stack shaderLinks;
	gfx_program* programs; // note: might not need to be a linked list
} gfx_state;

void load_opengl_extensions(gfx_state* gfx);
void set_gfx_globals(gfx_state* gfx);
gfx_program* gfx_create_shader(char* vs, char* fs);
void gfx_sh(gfx_program* program);
void gfx_ut(char *name, GLuint tex);
void gfx_uf(char *name, float value);
void gfx_uf2(char *name, float a, float b);
void gfx_uf3(char *name, float a, float b, float c);

gfx_state* gfx;

#ifndef _GFX_HEADER

//#include <stdarg.h>
//#include <stdio.h>
//#include <stdlib.h>

#pragma warning(disable: 4838)
#define inline _inline
//#define array_size(arr) (sizeof(arr)/sizeof(arr[0]))

//typedef unsigned __int32 GLuint;
//typedef unsigned __int32 GLsizei;
//typedef unsigned __int32 GLenum;
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_MULTISAMPLE                    0x809D
#define GL_MULTISAMPLE_ARB                0x809D
#define GL_RGB32F                         0x8815
#define GL_LINK_STATUS                    0x8B82

#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7

#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5

#define GL_RGBA32F                        0x8814
#define GL_RGB32F                         0x8815
#define GL_RGBA16F                        0x881A
#define GL_RGB16F                         0x881B

#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9

void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	printf("[id:%i] %s\n", id, message);
}

void set_gfx_globals(gfx_state* state) {
	gfx = state;
#define GLEXT(name) gl##name = gfx->gl##name;
	GL_PROC_LIST
#undef GLEXT
}

void load_opengl_extensions(gfx_state* gfx) {
	//gfx->shader_prog_count = 1;
	
#define GLEXT(name) gfx->gl##name = (name##Proc)wglGetProcAddress("gl" #name);
	GL_PROC_LIST
#undef GLEXT
	
	set_gfx_globals(gfx);

	glDebugMessageCallback(opengl_debug_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}


// Shaders
/*
  TODO:
    Replace crt string functions
    Round up block sizes
    Shader includes
    Dependancy links
    Defragmentation
    Dont reload shader if writeTime hasnt changed, make sure it works
*/
/*
shader memory layout
  header {
    next
    prev
    int program
    filename_size
    code_size
  }
  filename[]
  code[]
*/

/*{
	//char *mem = (char*)res.data;
	size_t size = res.size;
	int str = 0;
	char *inc = "#include";
	int inc_len = strlen(inc);
	{size_t i; for (i=0; i < size - inc_len; ++i) {
			size_t inc_start;
			if (strncmp(mem+str, inc, inc_len) == 0) {
				char name[64];
				int name_len = 0;
				ShaderFile* inc_file;
				char* newmem;
				inc_start = str;
				str += inc_len;
				while (mem[str] != '"') ++str;
				
				++str;
				while (mem[str] != '"') name[name_len++] = mem[str++];
				++str;
				name[name_len] = 0;
				//FileResult inc_file = load_file(name);
				inc_file = process_shader_file(name, sf->index);
				newmem = (char*)realloc(mem, size + inc_file->file_res.size - inc_len);
				memcpy(newmem + inc_start + inc_file->file_res.size, newmem + str, size-str);
				memcpy(newmem + inc_start, inc_file->file_res.data, inc_file->file_res.size);
				size = inc_start + inc_file->file_res.size + (size-str);
				mem = newmem;
			} else {
				++str;
			}
		}}
}*/

#define assert(exp) (exp ? (exp) : (*(int*)0 = 0))
#define strlen asdqwe
#define strcmp asdqwe
#define memset asdqwe
#define memcpy asdqwe

typedef unsigned char byte;

gfx_shader* gfx_load_shader(char* path, GLuint type);

int gfx_align(int n, int stride) {
	return (n/stride + 1)*stride;
}

void zero_memory(byte* address, int size) {
	byte* end = address+size;
	while(address<end){
		*address++ = 0;
	}
}

void copy_memory(byte* dest, byte* src, int size) {
	byte* end = dest+size;
	while(dest<end){
		*dest++ = *src++;
	}
}

int string_length_nt(char* str) { // including null terminator
	int result = 0;
	while(*str) {
		++str;
		++result;
	}
	++result;
	return result;
}

int compare_strings(char* str1, char* str2) {
	while(*str1 == *str2) {
		if(!*str1 && !*str2) {
			return 1;
		}
		++str1;
		++str2;
	}
	return 0;
}

int compare_strings_n(char* str1, char* str2, int size) {
	for(int i=0; i<size; ++i) {
		if(*str1 != *str2) {
			return 0;
		}
		++str1;
		++str2;
	}
	return 1;
}

void gfx_set_file_stat_callback(gfx_file_stat_proc proc) {
	gfx->file_stat = proc;
}

void gfx_set_load_file_callback(gfx_load_file_proc proc) {
	gfx->load_file = proc;
}

void* gfx_push_memory(gfx_memory_stack* arena, int size) {
	assert(arena->stackIndex + size <= arena->size);
	if(arena->stackIndex + size <= arena->size) {
		void* result = (byte*)arena->address+arena->stackIndex;
		arena->stackIndex += size;
		return result;
	}
	return 0;
}

void* gfx_push_and_copy_memory(gfx_memory_stack* arena, byte* src, int size) {
	assert(arena->stackIndex + size <= arena->size);
	if(arena->stackIndex + size <= arena->size) {
		void* result = (byte*)arena->address+arena->stackIndex;
		arena->stackIndex += size;
		copy_memory(result, src, size);
		return result;
	}
	return 0;
}

void gfx_pop_memory(gfx_memory_stack* arena, int size) {
	zero_memory((byte*)arena->address + arena->stackIndex - size, size);
	arena->stackIndex -= size;
}

void gfx_clear_memory(gfx_memory_stack* arena) {
	zero_memory(arena->address, arena->stackIndex);
	arena->stackIndex = 0;
}

int get_shader_size(gfx_shader* shader) {
	return gfx_align(sizeof(gfx_shader) + shader->path_size + shader->code_size, 64);
}

int calculate_shader_size(char* path, int file_size) {
	return gfx_align(sizeof(gfx_shader) + string_length_nt(path) + file_size, 64);
}

gfx_shader* gfx_find_shader(char* filename) {
	gfx_shader* shader = gfx->shaders;
	while(shader) {
		if(compare_strings(filename, (char*)shader + sizeof(gfx_shader))) {
			break;
		}
		
		shader = shader->next;
	}
	
	return shader;
}

//gfx_compile_gl_shader(gfx_shader* shader, GLenum type) {

//}

free_block* gfx_get_free_shader_block(int size) {
	free_block* free = gfx->freeBlocks;
	while(free && free->size < size) {
		free = free->next;
	}
	return free;
}

void add_free_to_list(free_block* free, int size) {
	// todo: defragment
	zero_memory(free, size);
	free->size = size;
	free->next = gfx->freeBlocks;
	if(gfx->freeBlocks) {
		gfx->freeBlocks->prev = free;
	}
	gfx->freeBlocks = free;
}

void remove_free_from_list(free_block* free) {
	if(free->prev) {
		((free_block*)free->prev)->next = free->next;
	} else {
		gfx->freeBlocks = free->next;
	}
	if(free->next) {
		((free_block*)free->next)->prev = free->prev;
	}
}

void split_free_in_list(free_block* free, int offset) {
	free_block* newFree = (byte*)free + offset;
	newFree->size = free->size - offset;
	newFree->next = free->next;
	newFree->prev = free->prev;
	
	if(free->prev) {
		((free_block*)free->prev)->next = newFree;
	} else {
		gfx->freeBlocks = newFree;
	}
	if(free->next) {
		((free_block*)free->next)->prev = newFree;
	}
}

void add_shader_to_list(gfx_shader* shader) {
	shader->next = gfx->shaders;
	if(gfx->shaders) {
		gfx->shaders->prev = shader;
	}
	gfx->shaders = shader;
	shader->prev = 0;
}

void remove_shader_from_list(gfx_shader* shader) {
	if(shader->prev) {
		((gfx_shader*)shader->prev)->next = shader->next;
	} else {
		gfx->shaders = shader->next;
	}
	if(shader->next) {
		((gfx_shader*)shader->next)->prev = shader->prev;
	}	
}

void check_shader_errors(char* path, GLuint handle) {
	int errorSize = 0;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &errorSize);
	if(errorSize) {
		char *error = gfx_push_memory(&gfx->transient, errorSize);
		glGetShaderInfoLog(handle, errorSize, NULL, error);
		printf("%s %s", path, error);
		uiMessage("%s %s", path, error);
		gfx_pop_memory(&gfx->transient, errorSize);
	}	
}

void check_program_errors(GLuint handle) {
	int errorSize;
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &errorSize);
	if(errorSize) {
		char *error = gfx_push_memory(&gfx->transient, errorSize);
		glGetProgramInfoLog(handle, errorSize, NULL, error);
		printf("program: %s", error);
		uiMessage("program: %s", error);
		gfx_pop_memory(&gfx->transient, errorSize);
	}	
}

void* gfx_parse_file(gfx_file_info* info, char* data) {
	// todo: turn into a real tokenizer
	
	int originalSize = info->size;
	char* inc = "#include";
	//char* marker = data;
	char* code = (byte*)gfx->transient.address + gfx->transient.stackIndex;
	char* readCursor = data;
	char* parseCursor = data;
	//int i;
	for(parseCursor; parseCursor<(data+info->size); ++parseCursor) {
		if(compare_strings_n(parseCursor, inc, 8)) {
			gfx_push_and_copy_memory(&gfx->transient, readCursor, parseCursor-readCursor);
			//codeSize += parseCursor-readCursor;
			while (*parseCursor != '"') ++parseCursor;
			++parseCursor;
			char* includePath = parseCursor;
			while (*parseCursor != '"') ++parseCursor;
			*parseCursor = 0;
			++parseCursor;
			readCursor = parseCursor;
			
			gfx_shader* shader = gfx_load_shader(includePath, 0);
			gfx_push_and_copy_memory(&gfx->transient,
									 (byte*)shader + sizeof(gfx_shader) + shader->path_size,
									 shader->code_size-1); // remove null terminator
			//codeSize += shader->code_size;
		}
	}
	*(parseCursor-1) = 0; // null terminate
	gfx_push_and_copy_memory(&gfx->transient, readCursor, parseCursor-readCursor);
	//codeSize += i-codeSize;
	
	info->size = string_length_nt(code);
	return code;
}

gfx_shader* gfx_allocate_shader_in_free_block(char* path, GLenum type,
											  gfx_file_info info, void* fileData) {
	//int sizeNeeded = sizeof(gfx_shader) + string_length_nt(path) + info.size;
	int sizeNeeded = calculate_shader_size(path, info.size);
	free_block* free = gfx_get_free_shader_block(sizeNeeded);
	if(free) {
		gfx_shader* shader = (gfx_shader*)free;

		if(sizeNeeded < free->size) {
			split_free_in_list(free, sizeNeeded);
		} else {
			remove_free_from_list(free);
		}
		
		add_shader_to_list(shader);
		
		shader->writeTime = info.writeTime;
		shader->path_size = string_length_nt(path);
		shader->code_size = info.size;
		byte* mem = shader;
		mem += sizeof(gfx_shader);
		copy_memory(mem, path, string_length_nt(path));
		mem += string_length_nt(path);
		copy_memory(mem, fileData, info.size);
		
		GLuint handle = glCreateShader(type ? type : GL_FRAGMENT_SHADER);
		char* code = (byte*)shader + sizeof(gfx_shader) + shader->path_size;
		glShaderSource(handle, 1, &code, &shader->code_size);
		glCompileShader(handle);
		shader->handle = handle;
		shader->type = type;
		
		check_shader_errors(path, handle);
		
		return shader;
	}
	
	return 0;
}

gfx_shader* gfx_load_shader(char* path, GLuint type) {
	gfx_shader* result = 0;
	gfx_file_info info = gfx->file_stat(path);
	gfx_shader* shader = gfx_find_shader(path);
	if(shader && shader->writeTime == info.writeTime) {
		return shader;
	}
	int originalMemorySize = info.size;
	void* fileData = gfx_push_memory(&gfx->transient, info.size+1);
	if(gfx->load_file(path, info.size, fileData)) {
		info.size += 1; // increase to include null terminator
		
		void* code = gfx_parse_file(&info, fileData);
		
		//gfx_shader* shader = gfx_find_shader(path);
		if(shader) {
			//int oldSize = sizeof(gfx_shader) + shader->path_size + shader->code_size;
			int oldSize = get_shader_size(shader);
			//int sizeNeeded = sizeof(gfx_shader) + string_length_nt(path) + info.size;
			int sizeNeeded = calculate_shader_size(path, info.size);
			if(sizeNeeded > oldSize) {
				printf("reallocating shader block %i -> %i \n", oldSize, sizeNeeded);
				
				// remove old shader
				glDeleteShader(shader->handle);
				remove_shader_from_list(shader);
				
				gfx_shader* newShader = gfx_allocate_shader_in_free_block(path, type, info, code);
				
				add_free_to_list(shader, oldSize);
				
				//gfx_clear_memory(&gfx->transient);
				result = newShader;
			} else {
				// update in place
				zero_memory((byte*)shader + sizeof(gfx_shader) + shader->path_size, shader->code_size);
				copy_memory((byte*)shader + sizeof(gfx_shader) + shader->path_size, code, info.size);
				shader->code_size = info.size;
				shader->writeTime = info.writeTime;
				
				int trim = oldSize - sizeNeeded;
				if(trim > 0 && trim>sizeof(free_block)) {
					//free_block* free = (byte*)shader + sizeof(gfx_shader) + shader->path_size + info.size;
					free_block* free = (byte*)shader + sizeNeeded;
					add_free_to_list(free, trim);
				}
				
				glDeleteShader(shader->handle);
				GLuint handle = glCreateShader(type ? type : GL_FRAGMENT_SHADER);
				char* code = (byte*)shader + sizeof(gfx_shader) + shader->path_size;
				glShaderSource(handle, 1, &code, &shader->code_size);
				glCompileShader(handle);
				shader->handle = handle;
				
				check_shader_errors(path, handle);
				
				//gfx_clear_memory(&gfx->transient);
				result = shader;
			}
		} else {
			// insert new shader
			result = gfx_allocate_shader_in_free_block(path, type, info, code);
		}
		
		gfx_pop_memory(&gfx->transient, info.size);
	} else {
		// todo: FAIL
	}
	
	gfx_pop_memory(&gfx->transient, originalMemorySize);
	return result;
}

gfx_program* gfx_create_shader(char* vertFile, char* fragFile) {
	gfx_shader* vertex = gfx_find_shader(vertFile);
	gfx_shader* fragment = gfx_find_shader(fragFile);
	if(vertex) {
		//gfx_check_shader(vertex); // todo: do we need to check for updates during create?
	} else {
		vertex = gfx_load_shader(vertFile, GL_VERTEX_SHADER);
	}
	if(fragment) {
		//gfx_check_shader(fragment); // todo: do we need to check for updates during create?
	} else {
		fragment = gfx_load_shader(fragFile, GL_FRAGMENT_SHADER);
	}
	
	GLuint program = gfx->glCreateProgram();
	glAttachShader(program, vertex->handle);
	glAttachShader(program, fragment->handle);
	glLinkProgram(program);
	
	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(!linked) {
		check_program_errors(program);
	}
	
	// todo: is this needed?
	glDetachShader(program, vertex->handle);
	glDetachShader(program, fragment->handle);
	//glDeleteShader(vertex->handle);
	//glDeleteShader(fragment->handle);
	
	gfx_program* result = gfx_push_memory(&gfx->programMemory, sizeof(gfx_program));
	if(result) {
		result->handle = program;
		result->vertex = vertex;
		result->fragment = fragment;
		result->next = gfx->programs;
		gfx->programs = result;
	}
	return result;
}

void recompile_program(gfx_program* program) {
	glDeleteProgram(program->handle);
	GLuint handle = gfx->glCreateProgram();
	glAttachShader(handle, program->vertex->handle);
	glAttachShader(handle, program->fragment->handle);
	glLinkProgram(handle);
	
	int linked;
	glGetProgramiv(handle, GL_LINK_STATUS, &linked);
	if(!linked) {
		check_program_errors(handle);
	}
	
	// todo: is this needed?
	glDetachShader(handle, program->vertex->handle);
	glDetachShader(handle, program->fragment->handle);
	//glDeleteShader(vertex->handle);
	//glDeleteShader(fragment->handle);
	
	program->handle = handle;
}

void gfx_update_shaders() {
	gfx_shader* shader = gfx->shaders;
	while(shader) {
		char* path = (byte*)shader + sizeof(gfx_shader);
		assert(string_length_nt(path)>1);
		gfx_file_info info = gfx->file_stat(path);
		if(info.writeTime > shader->writeTime) {
			gfx_shader* newShader = gfx_load_shader(path, shader->type);
			
			gfx_program* program = gfx->programs;
			while(program) {
				if(program->vertex==shader) {
					program->vertex = newShader;
					recompile_program(program);
					break;
				}
				if(program->fragment==shader) {
					program->fragment = newShader;					
					recompile_program(program);
					break;
				}
				program = program->next;
			}
			
			// note: the linked list may have changed, so just do one change per frame
			return;
		}
		
		shader = shader->next;
	}
}


// Rendering
gfx_program* gfx_current_shader = 0;
int gfx_draw_call_tex_stack = 0;

void gfx_sh(gfx_program* program) {
	if(program) {
		GLint linked;
		glGetProgramiv(program->handle, GL_LINK_STATUS, &linked);
		if (linked) {
			gfx_current_shader = program;
			glUseProgram(program->handle);
		} else {
			glUseProgram(0);
			gfx_current_shader = 0;
			// debug_msg("shader %i broken\n", prog_index);
		}
	} else {
		glUseProgram(0);
	}
}

void gfx_ut(char *name, GLuint tex) {
	int u_tex;
	if (gfx_current_shader) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		u_tex = glGetUniformLocation(gfx_current_shader->handle, name); // todo: cache
		glUniform1i(u_tex, 0);
	}
}

void gfx_ui1(char *name, int value) {
	if (gfx_current_shader) {
		int loc = glGetUniformLocation(gfx_current_shader->handle, name); // todo: cache uniform names
		glUniform1i(loc, value);
	}
}

void gfx_uf(char *name, float value) {
	if (gfx_current_shader) {
		int loc = glGetUniformLocation(gfx_current_shader->handle, name); // todo: cache uniform names
		glUniform1f(loc, value);
	}
}

void gfx_uf2(char *name, float a, float b) {
	if (gfx_current_shader) {
		int loc = glGetUniformLocation(gfx_current_shader->handle, name); // todo: cache uniform names
		glUniform2f(loc, a, b);
	}
}

void gfx_uf3(char *name, float a, float b, float c) {
	if (gfx_current_shader) {
		int loc = glGetUniformLocation(gfx_current_shader->handle, name); // todo: cache uniform names
		glUniform3f(loc, a, b, c);
	}
}

void gfx_uf4(char *name, float a, float b, float c, float d) {
	if (gfx_current_shader) {
		int loc = glGetUniformLocation(gfx_current_shader->handle, name); // todo: cache uniform names
		glUniform4f(loc, a, b, c, d);
	}
}

void gfx_uf3v(char *name, int count, float* v) {
	if (gfx_current_shader) {
		int loc = glGetUniformLocation(gfx_current_shader->handle, name); // todo: cache uniform names
		glUniform3fv(loc, count, v);
	}
}

void gfx_um4(char* name, float* values) {
	if (gfx_current_shader) {
		int loc = glGetUniformLocation(gfx_current_shader->handle, name);
		// todo: cache uniform names
		glUniformMatrix4fv(loc, 1, 0, values);
	}
}

#endif