#ifndef __gl_util_h__
#define __gl_util_h__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <png.h>
#include <sys/types.h>
#include "util.h"
#include "igl.h"

#define CIRCLE_INT 0.3141592653f

GLvoid gl_print(GLfloat, GLfloat, GLfloat, char*);
GLvoid gl_circle_vertices(GLfloat radius);
GLvoid gl_circle_vertices_texture(GLfloat radius);
GLvoid gl_cylinder_vertices(GLfloat radius, GLfloat height);

#endif /* __gl_util_h__ */

