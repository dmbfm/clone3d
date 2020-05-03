#if !defined(WIN32_OPENGL_H)
#define WIN32_OPENGL_H

#include <windows.h>
#include <wingdi.h>
#include <gl/gl.h>

// https://github.com/KhronosGroup/OpenGL-Registry/blob/master/api/GL/glcorearb.h
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VERTEX_ARRAY                   0x8074
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1

typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;

typedef GLuint WINAPI t_glCreateShader(GLenum shaderType);
typedef void WINAPI t_glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void WINAPI t_glCompileShader(GLuint shader);
typedef void WINAPI t_glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
typedef void WINAPI t_glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLuint WINAPI t_glCreateProgram(void);
typedef void WINAPI t_glAttachShader(GLuint program, GLuint shader);
typedef void WINAPI t_glLinkProgram(GLuint program);
typedef void WINAPI t_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef void WINAPI t_glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void WINAPI t_glGenBuffers(GLsizei n, GLuint *buffers);
typedef void WINAPI t_glBindBuffer(GLenum target, GLuint buffer);
typedef void WINAPI t_glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void WINAPI t_glGenVertexArrays(GLsizei n, GLuint *arrays);
typedef void WINAPI t_glBindVertexArray(GLuint array);
typedef void WINAPI t_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void WINAPI t_glEnableVertexAttribArray(GLuint index);
typedef void WINAPI t_glUseProgram(GLuint program);
typedef void WINAPI t_glActiveTexture(GLenum texture);
typedef void WINAPI t_glUniform1i(GLint location, GLint v0);
typedef GLint WINAPI t_glGetUniformLocation(GLuint program, const GLchar *name);

extern t_glCreateShader *glCreateShader;
extern t_glShaderSource *glShaderSource;
extern t_glCompileShader *glCompileShader;
extern t_glGetShaderiv *glGetShaderiv;
extern t_glGetShaderInfoLog *glGetShaderInfoLog;
extern t_glCreateProgram *glCreateProgram;
extern t_glAttachShader *glAttachShader;
extern t_glLinkProgram *glLinkProgram;
extern t_glGetProgramiv *glGetProgramiv;
extern t_glGetProgramInfoLog *glGetProgramInfoLog;
extern t_glGenBuffers *glGenBuffers;
extern t_glBindBuffer *glBindBuffer;
extern t_glBufferData *glBufferData;
extern t_glGenVertexArrays *glGenVertexArrays;
extern t_glBindVertexArray *glBindVertexArray;
extern t_glVertexAttribPointer *glVertexAttribPointer;
extern t_glEnableVertexAttribArray *glEnableVertexAttribArray;
extern t_glUseProgram *glUseProgram;
extern t_glActiveTexture *glActiveTexture;
extern t_glUniform1i *glUniform1i;
extern t_glGetUniformLocation *glGetUniformLocation;

void init_opengl_function_pointers();

#endif
