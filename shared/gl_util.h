#ifndef __gl_util_h__
#define __gl_util_h__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <png.h>
#include <sys/types.h>
#include "util.h"
#include "m_util.h"
#include "igl.h"

#define CIRCLE_INT 0.3141592653f

GLvoid gl_print(GLfloat, GLfloat, GLfloat, char*);
GLvoid gl_circle_vertices(GLfloat radius);
GLvoid gl_circle_vertices_texture(GLfloat radius);
GLvoid gl_cylinder_vertices(GLfloat radius, GLfloat height);

/*
GLuint gl_load_texture_raw(const char* filename, u_int width, u_int height, u_int alpha);
GLuint gl_load_texture_png(const char* filename, u_int alpha);
GLuint gl_load_texture_from_data(char* data, u_int width, u_int height, u_int size);
char* texture_deinterlace(char** data, u_int width, u_int height, u_int size);
*/

void gl_normal_v3(v3*);
void gl_vertex_v3(v3*);
void gl_translate_v3(v3*);
void gl_v3(v3*);

#endif /* __gl_util_h__ */
