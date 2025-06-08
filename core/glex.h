//
//  opengl_extensions.h
//  Core
//
//  Created by Matt Hartley on 02/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#ifndef __CORE_OPENGL_EXTENSIONS_HEADER__
#define __CORE_OPENGL_EXTENSIONS_HEADER__


#include "targetconditionals.h"
#include "sys.h"


#	if defined(__WIN32__) || defined(__LINUX__)


#ifdef __WIN32__
#   include <gl/gl.h>
#endif
#ifdef __LINUX__
#   include <gl/gl.h>
#endif
#ifdef __MACOS__
#	include <TargetConditionals.h>
#	if TARGET_OS_IPHONE
#		include <OpenGLES/ES3/gl.h>
#	else
#		define GL_SILENCE_DEPRECATION
#		include <OpenGL/gl.h>
#	endif
#endif


void _load_opengl_extensions();


#ifndef APIENTRY
#	define APIENTRY
#endif
#ifndef GLDECL
#	define GLDECL APIENTRY
#endif
#ifndef GLAPI
#	define GLAPI extern
#endif
typedef char GLchar;

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

#define GL_RG                             0x8227
#define GL_RGBA32F                        0x8814
#define GL_RGB32F                         0x8815
#define GL_RGBA16F                        0x881A
#define GL_RGB16F                         0x881B

#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT 0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9


#		ifdef CORE_IMPL


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


void _load_opengl_extensions() {

#define GLEXT(name) gl##name = (name##Proc)wglGetProcAddress("gl" #name);
	GL_PROC_LIST
#undef GLEXT
}


#		endif
#	endif
#endif