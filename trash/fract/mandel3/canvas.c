#include "canvas.h"

#ifndef __canvas_c__
#define __canvas_c__

canvas* canvas_new(u_int w, u_int h) {
  canvas* c = (canvas*) malloc(sizeof(canvas));

  c->w = w;
  c->h = h;

  c->val = (float*) calloc(w * h, sizeof(float));
  c->col = (u_int*) calloc(CANVAS_COLORS * w * h, sizeof(u_int[4]));
  
  c->current_color[0] = 0;
  c->current_color[1] = 0;
  c->current_color[2] = 0;
  c->current_color[3] = 255;

  assert(c->val);
  assert(c->col);

  return c;
}

void canvas_free(canvas* c) {
  free(c->val);
  free(c->col);
  free(c);
}

void canvas_color4d(canvas* c, CANVAS_URGBA(r,g,b,a)) {
  c->current_color[0] = r;
  c->current_color[1] = g;
  c->current_color[2] = b;
  c->current_color[3] = a;
}

void canvas_color4f(canvas* c, CANVAS_FRGBA(r,g,b,a)) {
  c->current_color[0] = (u_int) (255.0f * r);
  c->current_color[1] = (u_int) (255.0f * g);
  c->current_color[2] = (u_int) (255.0f * b);
  c->current_color[3] = (u_int) (255.0f * a);
}

void canvas_point(canvas* c, u_int x, u_int y) {
  memcpy(CANVAS_COLOR(c, x, y),
	 c->current_color,
	 sizeof(c->current_color));
}

#endif /* __canvas_c__ */
