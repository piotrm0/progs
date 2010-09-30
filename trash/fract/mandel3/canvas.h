#ifndef __canvas_h__
#define __canvas_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <assert.h>
#include <sys/types.h>

#define CANVAS_COLORS 4

typedef struct _canvas {
  u_int w, h;

  float* val;
  u_int* col;

  u_int current_color[4];

} canvas;

#define CANVAS_URGBA(r,g,b,a) u_int r, u_int g, u_int b, u_int a
#define CANVAS_FRGBA(r,g,b,a) float r, float g, float b, float a

#define CANVAS_INDEX(c, x, y) x * c->h + y
#define CANVAS_COLOR(c, x, y) c->col + x * c->h + y

#define CANVAS_FOREACH(c, x, y, content)		\
  for (x = 0; x < c->w; x++) {				\
    for (y = 0; y < c->h; y++) {			\
      content;						\
    }							\
  }
  
canvas* canvas_new(u_int w, u_int h);
void canvas_free(canvas* c);

#endif /* __canvas_h__ */
