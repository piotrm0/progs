#ifndef __viewport_h__
#define __viewport_h__

//#include <stdlib.h>
//#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include "util.h"
#include "igl.h"

enum {VIEW_NONE,
      VIEW_ASPECT,
      VIEW_CROP,
      VIEW_FG,
      VIEW_BG};

typedef void (*draw_func_p)(void);

typedef struct _view {
  u_int flags;
  GLint flags_gl_clear;

  GLfloat alpha;
  GLfloat x1, y1, x2, y2;
  GLfloat aspect;
  u_int num_subviews;
  struct _view** subviews;

  GLfloat* color_bg;
  GLfloat* color_fg;

  draw_func_p draw_func;
} view;

view* view_new(u_int subviews, u_int flags, u_int clear_flags, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
void view_set_bounds(view* v, GLint x1, GLint y1, GLint x2, GLint y2);
void view_gl_setup(view* v, GLint x, GLint y, GLint width, GLint height);
void view_draw(view* v);
void view_draw_sub(view* v, GLint x, GLint y, GLint width, GLint height);

#endif /* __viewport_h__ */
