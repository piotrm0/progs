#include "canvas.h"

#ifndef __canvas_c__
#define __canvas_c__

canvas* canvas_new(u_int w, u_int h) {
  canvas* c = (canvas*) malloc(sizeof(canvas));

  c->w = w;
  c->h = h;

  c->val = (float*) calloc(CANVAS_VALUES * w * h, sizeof(float));
  c->col = (u_int*) calloc(w * h, sizeof(u_int[CANVAS_COLORS]));
  
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

float canvas_min_value(canvas* c, u_int index) {
  u_int i;
  float val;
  float best = 99999999; // get the actual max here

  for (i = 0; i < c->w * c->h; i++) {
    val = c->val[i*CANVAS_VALUES];
    if (val < best) best = val;
  }

  return best;
}

float canvas_max_value(canvas* c, u_int index) {
  u_int i;
  float val;
  float best = -99999999; // get the actual min here

  for (i = 0; i < c->w * c->h; i++) {
    val = c->val[i*CANVAS_VALUES];
    if (val > best) best = val;
  }

  return best;
}

void canvas_values_to_colors(canvas* c, canvas_color_func col) {
  u_int x, y;

  for (y = 0; y < c->h; y++) {
    for (x = 0; x < c->w; x++) {
      col(c, x, y, CANVAS_VALUE(c, x, y));
      canvas_point(c, x, y);
    }
  }
}

void canvas_set_value(canvas* c, u_int x, u_int y, u_int index, float value) {
  (CANVAS_VALUE(c, x, y))[index] = value;
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

void canvas_print(canvas* c) {
  u_int x,y;
  u_int* color;

  printf("canvas %d x %d\n", c->w, c->h);

  for (y = 0; y < c->h; y++) {
    for (x = 0; x < c->w; x++) {
      color = CANVAS_COLOR(c,x,y);

      printf("%d/%d/%d/%d ", color[0], color[1], color[2], color[3]);
    }
    printf("\n");
  }
}

void canvas_write_png(canvas * c, char* filename) {
  // http://www.libgd.org/Basics

  u_int x, y;

  gdImagePtr im;

  u_int* color;
  int gdcolor;

  FILE *pngout;

  im = gdImageCreateTrueColor(c->w, c->h);

  for (y = 0; y < c->h; y++) {
    for (x = 0; x < c->w; x++) {
      color = CANVAS_COLOR(c,x,y);
      gdcolor = gdImageColorAllocate(im, color[0], color[1], color[2]);
      gdImageSetPixel(im, x, y, gdcolor);
    }
  }
  
  pngout = fopen(filename, "wb");

  gdImagePng(im, pngout);

  fclose(pngout);

  gdImageDestroy(im);
}

#endif /* __canvas_c__ */
