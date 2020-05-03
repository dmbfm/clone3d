#include <windows.h>
#include <wingdi.h>
#include <gl/gl.h>
#include "win32_opengl.h"

t_glCreateShader *glCreateShader;
t_glShaderSource *glShaderSource;
t_glCompileShader *glCompileShader;
t_glGetShaderiv *glGetShaderiv;
t_glGetShaderInfoLog *glGetShaderInfoLog;
t_glCreateProgram *glCreateProgram;
t_glAttachShader *glAttachShader;
t_glLinkProgram *glLinkProgram;
t_glGetProgramiv *glGetProgramiv;
t_glGetProgramInfoLog *glGetProgramInfoLog;
t_glGenBuffers *glGenBuffers;
t_glBindBuffer *glBindBuffer;
t_glBufferData *glBufferData;
t_glGenVertexArrays *glGenVertexArrays;
t_glBindVertexArray *glBindVertexArray;
t_glVertexAttribPointer *glVertexAttribPointer;
t_glEnableVertexAttribArray *glEnableVertexAttribArray;
t_glUseProgram *glUseProgram;
t_glActiveTexture *glActiveTexture;
t_glUniform1i *glUniform1i;
t_glGetUniformLocation *glGetUniformLocation;

void init_opengl_function_pointers()
{
    glCreateShader = (t_glCreateShader *) wglGetProcAddress("glCreateShader");
    glShaderSource = (t_glShaderSource *) wglGetProcAddress("glShaderSource");
    glShaderSource = (t_glShaderSource *) wglGetProcAddress("glShaderSource");
    glCompileShader = (t_glCompileShader *) wglGetProcAddress("glCompileShader");
    glGetShaderInfoLog = (t_glGetShaderInfoLog *) wglGetProcAddress("glGetShaderInfoLog");
    glGetShaderiv = (t_glGetShaderiv *) wglGetProcAddress("glGetShaderiv");
    glCreateProgram = (t_glCreateProgram *) wglGetProcAddress("glCreateProgram");
    glAttachShader = (t_glAttachShader *) wglGetProcAddress("glAttachShader");
    glLinkProgram = (t_glLinkProgram *) wglGetProcAddress("glLinkProgram");
    glGetProgramiv = (t_glGetProgramiv *) wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (t_glGetProgramInfoLog *) wglGetProcAddress("glGetProgramInfoLog");
    glGenBuffers = (t_glGenBuffers *) wglGetProcAddress("glGenBuffers");
    glBindBuffer = (t_glBindBuffer *) wglGetProcAddress("glBindBuffer");
    glBufferData = (t_glBufferData *) wglGetProcAddress("glBufferData");
    glGenVertexArrays = (t_glGenVertexArrays *) wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (t_glBindVertexArray *) wglGetProcAddress("glBindVertexArray");
    glVertexAttribPointer = (t_glVertexAttribPointer *) wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (t_glEnableVertexAttribArray *) wglGetProcAddress("glEnableVertexAttribArray");
    glUseProgram = (t_glUseProgram *) wglGetProcAddress("glUseProgram");
    glActiveTexture = (t_glActiveTexture *) wglGetProcAddress("glActiveTexture");
    glUniform1i = (t_glUniform1i *) wglGetProcAddress("glUniform1i");
    glGetUniformLocation = (t_glGetUniformLocation *) wglGetProcAddress("glGetUniformLocation");
}
