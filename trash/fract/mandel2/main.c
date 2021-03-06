#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#include <libavcodec/avcodec.h>

#include "viewport.h"
#include "common.h"
#include "util.h"
#include "gl_util.h"
#include "m_util.h"

#include "capture.h"

GLvoid init_scene(GLvoid);
GLvoid draw_scene(GLvoid);
GLvoid draw_extras(GLvoid);
GLvoid update_scene(GLvoid);
GLvoid custom_keyboard(unsigned char key, int x, int y);
GLvoid custom_special(int key, int x, int y);

GLvoid capture_buffer();

void update_world();
void sample_world();

capture* cap;
box2d* from, *to;
lmap2d* map, *rmap;
int* coord_from;
double* coord_to;

double max_mu = 1.0f;
double temp_max_mu = 1.0f;

u_int max_counts;
u_int* max_countsd;

u_int c_max;
u_int* c_maxd;

u_int** countsd;

void mandel(double x, double y, float* cols, u_int* counts) {
  double x0 = x;
  double y0 = y;

  double xtemp;

  // double mod;
  // double mu;

  double rad = 2.0f;

  //  float col;

  double* coord1, *coord2;
  coord1 = (double*) calloc(2, sizeof(double));
  coord2 = (double*) calloc(2, sizeof(double));

  u_int i = 0;
  u_int max = c_max;

  u_int temp_x;
  u_int temp_y;

  u_int temp_i;

  double mag;

  mag = x * x + y * y;

  //  while (mag < (rad*rad) && mag > 0.5 && i < max) {
  while (mag < (rad*rad) && i < max) {
    xtemp = x * x - y * y + x0;
    y = 2*x*y + y0;
    x = xtemp;
    i++;
    mag = x * x + y * y;
  }

  if (i < max) {
    i = 0;
    x = x0;
    y = y0;
    while (i < max && x * x + y * y < 9.0) {
      xtemp = x * x - y * y + x0;
      y = 2*x*y + y0;
      x = xtemp;
      i++;

      coord1[0] = x;
      coord1[1] = y;
      lmap2d_map_doubled_doubled(rmap, coord1, coord2);
      temp_x = coord2[0];
      temp_y = coord2[1];
      temp_i = temp_y * w_width + temp_x;
      if ((temp_x >= w_width) ||
	  (temp_x < 0) ||
	  (temp_y >= w_height) ||
	  (temp_y < 0)) {
      } else {
	counts[temp_i]++;
	if (counts[temp_i] > max_counts) {
	  max_counts = counts[temp_i];
	}
      }
    }
  }

  /*
  while (x*x + y*y < (rad*rad) && i < max) {
    xtemp = x * x - y * y + x0;
    y = 2*x*y + y0;
    x = xtemp;
    i++;
  }
  if (i == max) {
    i = 0;
    x = x0;
    y = y0;
    while (i < max) {
      xtemp = x * x - y * y + x0;
      y = 2*x*y + y0;
      x = xtemp;
      i++;

      coord1[0] = x;
      coord1[1] = y;
      lmap2d_map_doubled_doubled(rmap, coord1, coord2);
      temp_x = coord2[0];
      temp_y = coord2[1];
      temp_i = temp_y * w_width + temp_x;
      if ((temp_x >= w_width) ||
	  (temp_x < 0) ||
	  (temp_y >= w_height) ||
	  (temp_y < 0)) {
	// printf("bad rmap: %f,%f -> %d,%d\n", x,y,temp_x,temp_y);
      } else {
	counts[temp_i]++;
	if (counts[temp_i] > max_counts) {
	  max_counts = counts[temp_i];
	}
      }
    }
  }
  */

  free(coord1);
  free(coord2);

}

GLvoid init_scene(GLvoid) {
  u_int layer;
  
  view_main = view_new(0,
		       VIEW_NONE,
		       GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		       0, 0,
		       1, 1);
  view_main->draw_func = draw_scene;

  from = box2d_new(0,0, (double) w_width - 1, (double) w_height - 1);
  to   = box2d_new(-2.2,-1.5,4,3);

  map = lmap2d_new(from, to);
  rmap = lmap2d_new(to, from);
  //box2d_zoom_fixed(to, 0.001643721971153, 0.822467633298876,4);

  lmap2d_comp(map);
  lmap2d_comp(rmap);

  coord_from = (int*) calloc(2, sizeof(int));
  coord_to   = (double*) calloc(2, sizeof(double));

  capture_init();
  cap = capture_new("test.mpg", 0, 0, w_width, w_height);

  max_countsd = (u_int*) calloc(3, sizeof(u_int));
  countsd = (u_int**) calloc(3, sizeof(uint*));
  for (layer = 0; layer < 3; layer++) {
    max_countsd[layer] = 0;
    countsd[layer] = (u_int*) calloc(w_width * w_height, sizeof(u_int));
  }

  c_maxd = (u_int*) calloc(3, sizeof(u_int));
  c_maxd[0] = 24;
  c_maxd[1] = 22;
  c_maxd[2] = 20;
}

GLvoid update_world() {
  sample_world();
  draw_gl();
}

GLvoid sample_world() {
}

GLvoid update_scene() {
}

u_int cycles = 10000;
u_int cycle = 0;

GLvoid draw_scene(GLvoid) {
  u_int x;
  u_int y;
  u_int layer;

  float col_base = 0;

  if (cycle == cycles) {
    for (layer = 0; layer < 3; layer++) {
      for (x = 0; x < w_width * w_height ; x++) {
	countsd[layer][x] = 0;
      }
      //  c_max = (u_int) ((float) c_max * 1.1);
      c_maxd[layer]++;
      printf("layer %d, c_max=%d\n", layer, c_maxd[layer]);
      max_countsd[layer] = 0;
      cycle = 0;
    }
  }
  cycle++;

  float* col;
  col = (float*) calloc(3, sizeof(float));

  col[0] = 0;
  col[1] = 0;
  col[2] = 0;

  glEnable2D();

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  /*
  for (x = 0; x < w_width; x++) {
    for (y = 0; y < w_height; y++) {
      coord_from[0] = x;
      coord_from[1] = y;
      lmap2d_map_intd_doubled(map, coord_from, coord_to);
      mandel(coord_to[0], coord_to[1], col, counts);
    }
  }
  */

  for (layer = 0; layer < 3; layer++) {

    u_int iters = 1000000;
    u_int percent = iters / 10;

    c_max = c_maxd[layer];
    //counts = countds[layer];
    max_counts = max_countsd[layer];

    for (x = 0; x < iters; x++) {
      if (x % percent == 0) {
	printf("layer %d: %0.2f done\n", layer, 100 * (float) x / (float) iters);
      }
      // coord_from[0] = drand48() * (double) w_width;
      // coord_from[1] = drand48() * (double) w_height;
      // lmap2d_map_intd_doubled(map, coord_from, coord_to);
      coord_to[0] = drand48() * 6 - 3;
      coord_to[1] = drand48() * 6 - 3;
      mandel(coord_to[0], coord_to[1], col, countsd[layer]);
    }

    max_countsd[layer] = max_counts;

    printf("max_counts[%d] = %d\n", layer, max_countsd[layer]);

  }

  u_int temp_i;

  glBegin(GL_POINTS);
  for (x = 0; x < w_width; x++) {
    for (y = 0; y < w_height; y++) {
      temp_i = y * w_width + x;
      col[0] = (double) countsd[0][temp_i] / (double) max_countsd[0];
      col[1] = (double) countsd[1][temp_i] / (double) max_countsd[1];
      col[2] = (double) countsd[2][temp_i] / (double) max_countsd[2];
      //col_base = (double) counts[temp_i] / (double) max_counts;
      //col[2] = col_base;
      //col[1] = col[2] * col[2];
      //col[0] = col[1] * col[1];
      //col[0] = (sin(col_base * M_PI - 0.75 * M_PI) + 1) * 0.5;
      //col[1] = (sin(col_base * M_PI - 0.5 * M_PI) + 1) * 0.5;
      //col[2] = (sin(col_base * M_PI - 0.25 * M_PI) + 1) * 0.5;
      //col[1] = (sin(sqrt(col_base)*M_PI*2+M_PI*1.5) + 1) * 0.5;
      //col[2] = (sin(sqrt(sqrt(col_base))*M_PI*2+M_PI*1.5) + 1) * 0.5;
      glColor3f(col[0], col[1], col[2]);
      glVertex2d(x, y);
    }
  }
  glEnd();
  glFlush();

  free(col);

  if (cycle == cycles) {
    glReadPixels(0,0,w_width,w_height,GL_RGB,GL_BYTE,cap->picture_buf_rgb);
    capture_snap(cap);
    glDisable2D();
    //box2d_zoom_fixed(to, 0.001643721971153, 0.822467633298876, 1.2);
    lmap2d_comp(map);
  }
}

GLvoid custom_keyboard(unsigned char key, int x, int y) {
  switch(key) {
  case '\t':
    break;
  case 13: // carriage return
    break;
  case 27: // escape
    capture_end(cap);
    exit(0);
    break;
  }   
}

GLvoid custom_special(int key, int x, int y) {
  switch(key) {
  case GLUT_KEY_LEFT:
    break;
  case GLUT_KEY_UP:
    break;
  case GLUT_KEY_RIGHT:
    break;
  case GLUT_KEY_DOWN:
    break;
  }
}
