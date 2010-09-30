#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <assert.h>

#include <viewport.h>
#include "common.h"
#include <util.h>
#include <gl_util.h>
#include <m_util.h>

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

GLuint fb;
GLuint fbd;
GLuint texture_content;

capture_context* cap;

box2d* from, *to;
lmap2d* map, *rmap;
int* coord_from;
double* coord_to;

double max_mu = 1.0f;
double temp_max_mu = 1.0f;

float* vals;

u_int c_max = 1000;
u_int* c_maxd;

#define RW 512
#define RH 256

float mandel(double x, double y, float* vals) {
  double xtemp;
  double x0 = x;
  double y0 = y;

  //printf("mandel: [x,y] = [%f, %f] counts = [%d,%d]\n", x, y, counts[0], counts[1]);

  double rad = 2.0f;

  u_int i = 0;

  double mag;

  mag = x * x + y * y;

  while (mag < (rad*rad) && i < c_max) {
    xtemp = x * x - y * y + x0;
    y = 2*x*y + y0;
    x = xtemp;
    i++;
    mag = x * x + y * y;
  }

  return (float) i / (float) c_max;
}

GLvoid init_scene(GLvoid) {
  glGenTextures(1, & texture_content);

  glBindTexture(GL_TEXTURE_2D, texture_content);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RW, RH, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

  glGenFramebuffersEXT(1, & fb);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			    GL_COLOR_ATTACHMENT0_EXT,
			    GL_TEXTURE_2D,
			    texture_content,
			    0);

  glGenFramebuffersEXT(1, & fbd);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbd);

  GLenum status;
  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch (status) {
  case GL_FRAMEBUFFER_COMPLETE_EXT:
    printf("framebuffers are good\n"); break;
  default:
    pexit(1, "init_scene: frame buffer issues: %d", status);
  }

  view_main = view_new(0,
		       VIEW_BG,
		       GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		       0, 0,
		       1, 1);

  view_main->color_bg[0] = 0.0f;
  view_main->color_bg[2] = 0.0f;

  view_main->draw_func = draw_scene;

  from = box2d_new(0,0, (double) RW - 1, (double) RH - 1);
  to   = box2d_new(-2.2,-1.5,4,3);

  map = lmap2d_new(from, to);
  rmap = lmap2d_new(to, from);

  box2d_zoom_fixed(to, 0.001643721971153, 0.822467633298876,4);

  lmap2d_comp(map);
  lmap2d_comp(rmap);

  coord_from = (int*)    calloc(2, sizeof(int));
  coord_to   = (double*) calloc(2, sizeof(double));

  capture_init();
  cap = capture_new_video(CAPTURE_VIDEO_MPEG, "test.mpg", 0, 0, w_width, w_height);

  assert(vals = (float*) calloc(RW * RH, sizeof(float)));
}

GLvoid update_world() {
  sample_world();
  draw_gl();
}

GLvoid sample_world() {
}

GLvoid update_scene() {
}

u_int cycles = 10;
u_int cycle  = 0;

float temp_rot = 0.0f;

GLvoid draw_scene_content() {
  u_int x;
  u_int y;

  float* col;
  col = (float*) calloc(3, sizeof(float));

  col[0] = 0;
  col[1] = 0;
  col[2] = 0;

  for (x = 0; x < RW; x++) {
    for (y = 0; y < RH; y++) {
      coord_from[0] = x;
      coord_from[1] = y;
      lmap2d_map_intd_doubled(map, coord_from, coord_to);
      vals[x * RH + y] = mandel(coord_to[0], coord_to[1], vals);
    }
  }

  u_int temp_i;

  glBegin(GL_POINTS);
  for (x = 0; x < RW; x++) {
    for (y = 0; y < RH; y++) {
      temp_i = x * RH + y;
      //col[0] = (double) countsd[0][temp_i] / (double) max_countsd[0];
      //col[1] = (double) countsd[1][temp_i] / (double) max_countsd[1];
      //col[2] = (double) countsd[2][temp_i] / (double) max_countsd[2];
      //col[0] = (double) vals[0][temp_i];
      //col[1] = (double) vals[1][temp_i];
      //col[2] = (double) vals[2][temp_i];

      col[0] = col[1] = col[2] = vals[temp_i];

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

  free(col);
}

#define BOX 20

GLvoid draw_scene(GLvoid) {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable2Dspecific(RW, RH);

  glDisable(GL_TEXTURE_2D);
  draw_scene_content();
  glFlush();

  glEnable(GL_TEXTURE_2D);

  // when texture area is small, bilinear filter the closest mipmap
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		   GL_LINEAR_MIPMAP_NEAREST );
  // when texture area is large, bilinear filter the original
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // the texture wraps over at the edges (repeat)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //draw_scene_content();

  glFlush();

  glBindTexture(GL_TEXTURE_2D, texture_content);
  glGenerateMipmapEXT(GL_TEXTURE_2D);

  glEnable2D();

  glPushMatrix();

  //glRotatef(temp_rot += 0.5, 0,0,1);

  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_QUADS);
  
  glTexCoord2d(0,0); glVertex2f(BOX,BOX);
  glTexCoord2d(1,0); glVertex2f(W_WIDTH-BOX,BOX);
  glTexCoord2d(1,1); glVertex2f(W_WIDTH-BOX,W_HEIGHT-BOX);
  glTexCoord2d(0,1); glVertex2f(BOX,W_HEIGHT-BOX);

  glEnd();

  glFlush();

  glPopMatrix();

  glDisable2D();
  
  capture_snap(cap);

  box2d_zoom_fixed(to, 0.001643721971153, 0.822467633298876, 1.1);
  lmap2d_comp(map);

}

GLvoid finish() {
  glDeleteTextures(1, &texture_content);
  glDeleteRenderbuffersEXT(1, &fbd);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glDeleteFramebuffersEXT(1, &fb);

  capture_end(cap);
  exit(0);

}

GLvoid custom_keyboard(unsigned char key, int x, int y) {
  switch(key) {
  case '\t':
    break;
  case 13: // carriage return
    break;
  case 27: // escape
    finish();
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
