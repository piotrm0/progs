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
lmap2d* map;
int* coord_from;
double* coord_to;

double max_mu = 1.0f;
double temp_max_mu = 1.0f;

void mandel(double x, double y, float* cols) {
  double x0 = x;
  double y0 = y;

  double xtemp;

  double mod;
  double mu;

  double rad = 200.0f;

  float col;

  double mag_old;
  double mag_new;

  u_int i = 0;
  u_int max = 200;

  mag_old = x*x + y*y;
  
  while(i < 100) {
    mag_old = x * x + y * y;
    xtemp = x * x - y * y + x0;
    y = 2*x*y + y0;
    x = xtemp;
    i++;
  }

  mag_new = x * x + y * y;

  cols[0] = 0;
  cols[1] = 0;
  cols[2] = 0;

  if (mag_new > mag_old) {
    cols[0] = (mag_new / mag_old) / 2.0;
  } else {
    cols[1] = (mag_old / mag_new) / 2.0;
  }

  return;

  while (x*x + y*y < (rad*rad) && i < max) {
    xtemp = x * x - y * y + x0;
    y = 2*x*y + y0;
    x = xtemp;
    i++;
  }

  
    xtemp = x * x - y * y + x0;
    y = 2*x*y + y0;
    x = xtemp;
    i++;
    xtemp = x * x - y * y + x0;
    y = 2*x*y + y0;
    x = xtemp;
    i++;
  

    mod = sqrt(x * x + y * y);
    mu = (double) i - log(log(mod)) / log(2.0);
    //mu = i - log(log(mod)) / log(2.0);
  
    //  col = (double) mu / (double) (max+2);    
    col = mu / max_mu;

    if ((mu > temp_max_mu) && (! (x == 0 || y == 0))) {
      //printf("%f, %f\n", x,y);
      temp_max_mu = mu;
    }

    if (col < 0.0) { col = 0.0; }
    if (col > 1.0) { col = 1.0; }

  cols[0] = 0;
  cols[1] = 0;
  cols[2] = 0;

  //cols[0] = (sin(sqrt(col)*M_PI*2+M_PI*1.5) + 1) * 0.5;
  //cols[1] = (cos(col * M_PI) + 1) * 0.5;
  //cols[2] = (sin(sqrt(sqrt(col))*M_PI*2+M_PI*1.5) + 1) * 0.5;

  if (mu > 0.0) {
    cols[0] = col;
  }
}

GLvoid init_scene(GLvoid) {
  view_main = view_new(0,
		       VIEW_NONE,
		       GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		       0, 0,
		       1, 1);
  view_main->draw_func = draw_scene;

  from = box2d_new(0,0, (double) w_width, (double) w_height);
  to   = box2d_new(-2.5,-1.5,4,3);

  map = lmap2d_new(from, to);
  //box2d_zoom_fixed(to, 0.001643721971153, 0.822467633298876,4);

  lmap2d_comp(map);

  coord_from = (int*) calloc(2, sizeof(int));
  coord_to   = (double*) calloc(2, sizeof(double));

  capture_init();
  cap = capture_new("test.mpg", 0, 0, w_width, w_height);
}

GLvoid update_world() {
  sample_world();
  draw_gl();
}

GLvoid sample_world() {
}

GLvoid update_scene() {
}

GLvoid draw_scene(GLvoid) {
  u_int x;
  u_int y;

  max_mu = temp_max_mu;
  temp_max_mu = 1.0f;

  printf("max_mu = %f\n", max_mu);

  float* col;
  col = (float*) calloc(3, sizeof(float));

  glEnable2D();

  for (x = 0; x < w_width; x++) {
    for (y = 0; y < w_height; y++) {
      coord_from[0] = x;
      coord_from[1] = y;
      lmap2d_map_intd_doubled(map, coord_from, coord_to);
      mandel(coord_to[0], coord_to[1], col);

      glBegin(GL_POINTS);
      glColor3f(col[0], col[1], col[2]);
      glVertex2d(x, y);
      glEnd();

    }
  }
  
  free(col);

  glFlush();

  glReadPixels(0,0,w_width,w_height,GL_RGB,GL_BYTE,cap->picture_buf_rgb);
  capture_snap(cap);

  glDisable2D();

  box2d_zoom_fixed(to, 0.001643721971153, 0.822467633298876, 1.2);
  lmap2d_comp(map);
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
