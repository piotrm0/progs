#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#include <ffmpeg/avcodec.h>

#include "viewport.h"
#include "common.h"
#include "util.h"
#include "gl_util.h"
#include "m_util.h"

#include "capture.h"

#define PARTICLES 100

#define RADIUS 2.0f
#define LOSS_WALL 50.0f
#define LOSS_COL 15.0f

#define LOSS_V 0.99f

GLvoid init_scene(GLvoid);
GLvoid draw_scene(GLvoid);
GLvoid draw_extras(GLvoid);
GLvoid update_scene(GLvoid);
GLvoid custom_keyboard(unsigned char key, int x, int y);
GLvoid custom_special(int key, int x, int y);

GLvoid capture_buffer();

#define SPS 100  // samples per second
#define SPF 5    // samples per frame
float dt = (float) (SPF) / (float) (SPS);
int TI = (int) (SPF * 1000000 / SPS); // target interval for rendering in us
int CI = (int) (SPF * 1000000 / SPS); // current delay
timer* timer_frames;
int    mutex_frames;

v3* grav_vector;
v3* gr;

int state_paused = 1;

float gv;         //
float GR = 0.80f; // geometric averaging mechanism
float GQ = 0.20f; //

#define G 9.8f
double g = G / SPS; // g per sample

#define D 3     // dimension
#define B 25.0f // space [-B,B]^D

typedef struct _particle {
  v3* c;
  v3* v;
} particle;


particle ps[PARTICLES];

timer* timer_bounce;

void update_world();
void sample_world();

capture* cap;

GLvoid init_scene(GLvoid) {
  u_int i;
  // u_int d;

  grav_vector = v3_new(0,-g,0);
  gr = v3_copy(grav_vector);

  glutTimerFunc(100, update_world, 0);

  view_main = view_new(0,
		       VIEW_NONE,
		       GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		       0, 0,
		       1, 1);
  view_main->draw_func = draw_scene;
  for (i = 0; i < PARTICLES; i++) {
            ps[i].c = v3_new(2 * B * frandom() - B,
			     2 * B * frandom() - B,
			     2 * B * frandom() - B);
	    
	            ps[i].v = v3_new((frandom() * 2 - 1),
	        		     (frandom() * 2 - 1),
	        		     (frandom() * 2 - 1));
	    //  ps[i].v = v3_new(0,0,0);
	    
  // ps[i].c = v3_new(0,//frandom() * 0.001 - 0.0005,
  //	   B - RADIUS - i * RADIUS * 2 * 1.01,
  //	   0);//frandom() * 0.001 - 0.0005);
  //	    
  }
  /*
    ps[0].v = v3_new(3,0,0);
    ps[1].v = v3_new(-3,0,0);
    ps[0].c = v3_new(-B + RADIUS, -B * 0 + RADIUS, 0);
    ps[1].c = v3_new(B - RADIUS, -B * 0 + RADIUS, 0);
  */

  timer_frames = timer_new();
  timer_bounce = timer_new();

  mutex_frames = 0;

  timer_start(timer_frames);
  timer_start(timer_bounce);

  capture_init();
  cap = capture_new("test.mpg", 0, 0, w_width, w_height);
}

GLvoid update_world() {
  u_int dt;
  u_int i;

  if (++mutex_frames > 1) {
    --mutex_frames;
    printf("update already occuring\n");
    return;
  }
  mutex_frames = 1;

  dt = timer_mark(timer_frames);

    gv = gv * GR + GQ * (float) dt;

    //  printf("target: %d / elapsed: %d [avg: %0.3f] / CI: %d\n", TI, dt, gv, CI);

    CI += TI - ((int) gv);
    if (CI <= 0) CI = 1000;

  if (! state_paused) { 
    for (i = 0; i < SPF; i++) {
      sample_world();
    }
  }

  draw_gl();

  glutTimerFunc(CI/1000, update_world, 0);

  mutex_frames = 0;
}

GLvoid sample_world() {
  //  int timing;
  u_int i,j,d;
  //  float t0,t1,t2;

  v3* temp;
  v3* disp;
  //  v3* disp2;
  float dot;
  float dm;

  // char* temp_s1;
  // char* temp_s2;

  for (i = 0; i < PARTICLES; i++) {
    //    printf("adding g %d,%f\n", i,ps[i].v[1]);
    v3_add(ps[i].v, gr);
    //    ps[i].v->v[1] -= g;

    v3_mult(ps[i].v, LOSS_V);
    for (d = 0; d < D; d++) {
      v3_mult_add(ps[i].c, ps[i].v, 1 / (float) SPS); //  ps[i].c[d] += ps[i].v[d] / (float) SPS;
      if (ps[i].c->v[d] < -B + RADIUS) {

	disp = v3_new(0,0,0);
	disp->v[d] = ps[i].c->v[d] + B;
	dm = 1 / (1 - (RADIUS - v3_mag(disp)) / RADIUS);
	v3_unit(disp);

	v3_mult_add(ps[i].v, disp, dm * LOSS_WALL * dt);

	//	ps[i].c->v[d] = -B + RADIUS;
	//	ps[i].v->v[d] *= -1 * LOSS_WALL;

      }
      if (ps[i].c->v[d] > B - RADIUS) {

	disp = v3_new(0,0,0);
	disp->v[d] = ps[i].c->v[d] - B;
	//	dm = (RADIUS - v3_mag(disp)) / RADIUS;
	dm = 1 / (1 - (RADIUS - v3_mag(disp)) / RADIUS);
	v3_unit(disp);

	v3_mult_add(ps[i].v, disp, dm * LOSS_WALL * dt);

	//	ps[i].c->v[d] = B - RADIUS;
	//	ps[i].v->v[d] *= -1 * LOSS_WALL;
      }
    }
  }
  for (i = 0; i < PARTICLES; i++) {
    for (j = 0; j < PARTICLES; j++) {
      if (i == j) continue;
      if (v3_dist(ps[i].c, ps[j].c) >= RADIUS * 2) continue;

      /*      temp_s1 = v3_str(ps[i].c);
      temp_s2 = v3_str(ps[j].c);
      printf("collision pos: %s %s\n", temp_s1, temp_s2);
      free(temp_s1);
      free(temp_s2);*/

      /*
      temp_s1 = v3_str(ps[i].v);
      temp_s2 = v3_str(ps[j].v);
      printf("collision vel: %s %s\n", temp_s1, temp_s2);
      free(temp_s1);
      free(temp_s2);
      */

      temp = v3_copy(ps[i].v);
      v3_sub(temp, ps[j].v);

      /*
      temp_s1 = v3_str(temp);
      printf("temp = %s\n", temp_s1);
      free(temp_s1);
      */

      disp = v3_copy(ps[i].c);
      v3_sub(disp, ps[j].c);

      //      disp2 = v3_copy(disp);

      dm = RADIUS - v3_mag(disp) / 2;

      /*
      temp_s1 = v3_str(disp);
      printf("disp = %s\n", temp_s1);
      free(temp_s1);
      */

      v3_unit(disp);

      /*
      temp_s1 = v3_str(disp);
      printf("disp unit = %s\n", temp_s1);
      free(temp_s1);
      */

      dot = v3_dot(temp, disp);

      /*
      printf("dot = %f\n", dot);
      */

      v3_mult_add(ps[i].v, disp, dm*LOSS_COL*dt);
      v3_mult_sub(ps[j].v, disp, dm*LOSS_COL*dt);

      //      v3_mult_add(ps[i].c, disp, dm);
      //      v3_mult_sub(ps[j].c, disp, dm);

      v3_del(disp);
      v3_del(temp);
    }
  }
}

GLvoid update_scene() {

}

GLvoid draw_scene(GLvoid) {
  u_int i;

  float rotfv[3];
  rotfv[0] = -rot_x * 3.14159 / 180;
  rotfv[1] = -rot_y * 3.14159 / 180;
  rotfv[2] = -rot_z * 3.14159 / 180;

  v3_del(gr);
  gr = v3_copy(grav_vector);
  v3_rot(gr, rotfv);

  glTranslatef(0.0f, 0.0f, -20.0f);

  gl_v3(gr);

  glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
  glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
  glRotatef(rot_z, 0.0f, 0.0f, 1.0f);

  glScalef(zoom, zoom, zoom);

  glutWireCube(2 * B);

  for (i = 0; i < PARTICLES; i++) {
    glPushMatrix();
      gl_translate_v3(ps[i].c);
      glutSolidSphere(RADIUS, 15, 15);
      gl_v3(ps[i].v);
    glPopMatrix();
  }

  glFlush();

  glReadPixels(0,0,w_width,w_height,GL_RGB,GL_BYTE,cap->picture_buf_rgb);
  capture_snap(cap);
}

GLvoid custom_keyboard(unsigned char key, int x, int y) {
  switch(key) {
  case 'p':
    state_paused = ! state_paused;
    break;
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
