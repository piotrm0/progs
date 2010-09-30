#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <assert.h>

#include <util.h>
#include <m_util.h>

#include "capture.h"
#include "canvas.h"

#define CW 256
#define CH 256

void color(canvas* c, u_int x, u_int y, float* val) {
  canvas_color4d(c, val[0], val[1], val[2], 255);
}

int main(int argc, char** argv) {
  canvas* c;
  u_int x,y;

  c = canvas_new(CW, CH);

  for (x = 0; x < CW; x++) {
    for (y = 0; y < CH; y++) {
      //canvas_color4d(c, x, y, 0, 255);
      canvas_set_value(c, x, y, 0, x + 1000);
      //canvas_point(c, x, y);
    }
  }

  //  canvas_print(c);

  canvas_values_to_colors(c, & color);

  canvas_write_png(c, "i1.png");

  printf("min = %f, max = %f\n", canvas_min_value(c, 0), canvas_max_value(c, 0));

  return 0;
}
