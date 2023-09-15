typedef GLuint (GLDECL *CreateShaderProc)(GLenum type);
typedef void (GLDECL *ShaderSourceProc)(GLuint shader, GLsizei count, const GLchar * const *string, const GLint *length);
typedef void (GLDECL *CompileShaderProc)(GLuint shader);
typedef GLuint (GLDECL *CreateProgramProc) (void);
typedef void (GLDECL *AttachShaderProc) (GLuint program, GLuint shader);
typedef void (GLDECL *LinkProgramProc) (GLuint program);
typedef void (GLDECL *GetShaderInfoLogProc) (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void (GLDECL *UseProgramProc) (GLuint program);
typedef void (GLDECL *GetShaderivProc) (GLuint shader, GLenum pname, GLint *params);
typedef void (GLDECL *DeleteProgramProc) (GLuint program);
typedef void (GLDECL *DeleteShaderProc) (GLuint shader);
typedef void (GLDECL *DetachShaderProc)	(GLuint program, GLuint shader);
typedef void (GLDECL *GetProgramivProc) (GLuint program, GLenum pname, GLint *params);
typedef void (GLDECL *EnableVertexAttribArrayProc) (GLuint index);
typedef void (GLDECL *VertexAttribPointerProc) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
typedef GLint (GLDECL *GetUniformLocationProc) (GLuint program, const GLchar *name);
typedef void (GLDECL *UniformMatrix4fvProc) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (GLDECL *Uniform1fProc) (GLint location, GLfloat v0);
typedef void (GLDECL *Uniform2fProc) (GLint location, GLfloat v0, GLfloat v1);
typedef void (GLDECL *Uniform3fProc) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (GLDECL *Uniform4fProc) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (GLDECL *Uniform1iProc) (GLint location, int i);
typedef GLint (GLDECL *GetAttribLocationProc) (GLuint program, const GLchar *name);
typedef void (GLDECL *ActiveTextureProc) (GLenum texture);
typedef void (GLDECL *GenFramebuffersProc) (GLsizei n,  GLuint * framebuffers);
typedef void (GLDECL *BindFramebufferProc) (GLenum target,  GLuint framebuffer);
typedef void (GLDECL *FramebufferTextureProc) (GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (GLDECL *FramebufferTexture2DProc) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (GLDECL *DrawBuffersProc) (GLsizei n, const GLenum *bufs);
typedef GLenum (GLDECL *CheckFramebufferStatusProc) (GLenum target);
typedef void (GLDECL *BlitFramebufferProc) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void (GLDECL *DebugMessageCallbackProc) (GLDEBUGPROC callback, void *userParam);
typedef void (GLDECL *DebugMessageControlProc) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, 


GLEXT(CreateShader)\
GLEXT(ShaderSource)\
GLEXT(CompileShader)\
GLEXT(CreateProgram)\
GLEXT(AttachShader)\
GLEXT(LinkProgram)\
GLEXT(GetShaderInfoLog)\
GLEXT(UseProgram)\
GLEXT(GetShaderiv)\
GLEXT(DeleteProgram)\
GLEXT(DeleteShader)\
GLEXT(DetachShader)\
GLEXT(GetProgramiv)\
GLEXT(EnableVertexAttribArray)\
GLEXT(VertexAttribPointer)\
GLEXT(GetUniformLocation)\
GLEXT(UniformMatrix4fv)\
GLEXT(Uniform1f)\
GLEXT(Uniform2f)\
GLEXT(Uniform3f)\
GLEXT(Uniform4f)\
GLEXT(Uniform1i)\
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