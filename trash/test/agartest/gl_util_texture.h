#ifndef __gl_util_texture_h__
#define __gl_util_texture_h__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <png.h>
#include <sys/types.h>
#include "util.h"
#include "igl.h"

GLuint gl_load_texture_raw(const char* filename, u_int width, u_int height, u_int alpha);
GLuint gl_load_texture_png(const char* filename, u_int alpha);
GLuint gl_load_texture_from_data(char* data, u_int width, u_int height, u_int size);
char* texture_deinterlace(char** data, u_int width, u_int height, u_int size);

#endif /* __gl_util_texture_h__ */
