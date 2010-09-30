#ifndef __sg_type_h__
#define __sg_type_h__

#include "sg.h"

struct sg_font {
  u_int ptsize;
  sg_root *root;
  TTF_Font *font;
};

sg_font* sg_font_load(sg_root *root, char *filename, u_int ptsize);

struct sg_type {
  sg_font *font;
  vi2 *size;
  v2 *rsize;
  char *string;
  GLuint tid;
};

sg_type* sg_type_new_render(sg_font* font, char* string);
void sg_type_set_string(sg_type* e, char* string);
void sg_type_draw(sg_type* e, recti *view);

#endif /* __sg_type_h__ */
