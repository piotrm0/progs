#ifndef __canvas_h__
#define __canvas_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <assert.h>
#include <sys/types.h>
#include <gd.h>

#define CANVAS_COLORS 4
#define CANVAS_VALUES 3

typedef struct _canvas {
  u_int w, h;

  float* val;
  u_int* col;

  u_int current_color[4];

} canvas;

#define CANVAS_URGBA(r,g,b,a) u_int r, u_int g, u_int b, u_int a
#define CANVAS_FRGBA(r,g,b,a) float r, float g, float b, float a

#define CANVAS_INDEX(c, x, y) x * c->h * CANVAS_COLORS + y * CANVAS_COLORS
#define CANVAS_COLOR(c, x, y) c->col + x * c->h * CANVAS_COLORS + y * CANVAS_COLORS
#define CANVAS_VALUE(c, x, y) c->val + x * c->h * CANVAS_VALUES + y * CANVAS_VALUES

#define CANVAS_FOREACH(c, x, y, content)		\
  for (x = 0; x < c->w; x++) {				\
    for (y = 0; y < c->h; y++) {			\
      content;						\
    }							\
  }

void canvas_set_value(canvas* c, u_int x, u_int y, u_int index, float value);

typedef void canvas_color_func(canvas* c, u_int x, u_int y, float* val);

void canvas_values_to_colors(canvas* c, canvas_color_func col);

float canvas_min_value(canvas* c, u_int index);
float canvas_max_value(canvas* c, u_int index);

void canvas_color4d(canvas* c, CANVAS_URGBA(r,g,b,a));
void canvas_color4f(canvas* c, CANVAS_FRGBA(r,g,b,a));

void canvas_point(canvas* c, u_int x, u_int y);
  
void canvas_print(canvas* c);

canvas* canvas_new(u_int w, u_int h);
void canvas_free(canvas* c);

void canvas_write_png(canvas * c, char* filename);

#endif /* __canvas_h__ */
