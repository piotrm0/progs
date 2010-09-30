#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#include "viewport.h"
#include "common.h"
#include "util.h"
#include "gl_util.h"
#include "m_util.h"

#define WAVICLES 100
#define FLOW_R 0.05f
#define DAMP_V 0.99f
#define RAND_SCALE 1000.0f
#define RAND_RATE 0.001

GLvoid init_scene(GLvoid);
GLvoid draw_scene(GLvoid);
GLvoid draw_extras(GLvoid);
GLvoid update_scene(GLvoid);
GLvoid custom_keyboard(unsigned char key, int x, int y);
GLvoid custom_special(int key, int x, int y);

#define SPS 100  // samples per second
#define SPF 5    // samples per frame
float dt = (float) (SPF) / (float) (SPS);
int TI = (int) (SPF * 1000000 / SPS); // target interval for rendering in us
int CI = (int) (SPF * 1000000 / SPS); // current delay
timer* timer_frames;
int    mutex_frames;

int state_paused = 1;

float gv = 1.0f;  //
float GR = 0.80f; // geometric averaging mechanism
float GQ = 0.20f; //

typedef struct _wavicle {
  v1* c;
  v1* v;
} wavicle;

wavicle ws[WAVICLES];

void update_world();
void sample_world();

GLvoid init_scene(GLvoid) {
  u_int i;

  glutTimerFunc(100, update_world, 0);

  view_main = view_new(0,
		       VIEW_NONE,
		       GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		       0, 0,
		       1, 1);
  view_main->draw_func = draw_scene;

  for (i = 1; i < WAVICLES-1; i++) {
    //ws[i].c = v1_new(10.0f * frandom() - 5.0f);
    ws[i].c = v1_new(0.0f);
    //    ws[i].v = v1_new(2.0f * frandom() - 1.0f);
    ws[i].v = v1_new(0.0f);
  }
  ws[0].c = v1_new(0.0f);
  ws[0].v = v1_new(0.0f);
  ws[WAVICLES-1].c = v1_new(0.0f);
  ws[WAVICLES-1].v = v1_new(0.0f);

  timer_frames = timer_new();

  mutex_frames = 0;

  timer_start(timer_frames);
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
  u_int i;
  float f;

  if (frandom() < RAND_RATE) {
    f = RAND_SCALE * frandom() - RAND_SCALE / 2.0f;
    i = 2 + irandom(WAVICLES - 4);
    ws[i].v->v[0] += f;
    //ws[i-1].v->v[0] += f;
    //ws[i+1].v->v[0] += f;
  }


  for (i = 1; i < WAVICLES - 1; i++) {
    ws[i].v->v[0] += (ws[i-1].c->v[0] - ws[i].c->v[0]) * 0.5 * FLOW_R +
                     (ws[i+1].c->v[0] - ws[i].c->v[0]) * 0.5 * FLOW_R;
    ws[i].v->v[0] *= DAMP_V;
  }
  for (i = 1; i < WAVICLES - 1; i++) {
    ws[i].c->v[0] += ws[i].v->v[0];
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

  glTranslatef(0.0f, 0.0f, -20.0f);

  glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
  glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
  glRotatef(rot_z, 0.0f, 0.0f, 1.0f);

  glScalef(zoom, zoom, zoom);

  glPushMatrix();
  glTranslatef(- (float) WAVICLES / 2 + 0.5f, 0.0f, 0.0f);

  glLineWidth(2.0f);

  glBegin(GL_LINE_STRIP);
  for (i = 0; i < WAVICLES; i++) {
    glColor3f(abs(ws[i].c->v[0]) / 20.0f, 0.0f, 0.0f);
    glVertex3f((float) i, ws[i].c->v[0], 0.0f);
  }
  glEnd();

  glLineWidth(1.0f);
  glColor3f(0.0f, 0.0f, 0.0f);


  glBegin(GL_LINES);
  for (i = 0; i < WAVICLES; i++) {
    glVertex3f((float) i, ws[i].c->v[0], 0.0f);
    glVertex3f((float) i, ws[i].c->v[0] + ws[i].v->v[0], 0.0f);
  }
  glEnd();

  glPopMatrix();

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
