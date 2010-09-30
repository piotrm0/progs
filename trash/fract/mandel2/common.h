#ifndef __common_h__
#define __common_h__

#include <stdlib.h>
#include <stdio.h>
#include "igl.h"
#include "viewport.h"

#define W_WIDTH 800
#define W_HEIGHT 600

#define rot_int 2.0f

/*****  these are global variables:  *****/
extern GLfloat rot_x, rot_y, rot_z, zoom;

extern GLint w_width;
extern GLint w_height;
extern GLfloat w_aspect;

GLvoid init_gl(GLvoid);
GLvoid draw_gl(GLvoid);
GLvoid resize_gl(int w, int h);
GLvoid idle_gl(GLvoid);
GLvoid handle_keyboard(unsigned char key, int x, int y);
GLvoid handle_special(int key, int x, int y);

void glEnable2D();
void glDisable2D();

extern view* view_main;

extern GLvoid init_scene(GLvoid);
extern GLvoid draw_scene(GLvoid);
extern GLvoid draw_extras(GLvoid);
extern GLvoid update_scene(GLvoid);
extern GLvoid custom_keyboard(unsigned char key, int x, int y);
extern GLvoid custom_special(int key, int x, int y);

#endif /* __common_h__ */
