#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "util.h"
#include "sg.h"

#define NUM_POINTS 100
#define BOUND 50
#define GRAV -0.001

sg_image *img;
GLuint tid;

int test();

void frame_mark(u_int fs, u_int fl);

typedef struct point point;

struct point {
  u_int active;

  float p[3];
  float pv[3];

  float r[3];
  float rv[3];

  float s[3];
  float sv[3];
};


point* points[NUM_POINTS];
float rot = 0;

int free_point();
void  delete_point(u_int p);
void  init_point(u_int p);

int free_point() {
  u_int i;

  for (i = 0; i < NUM_POINTS; i++) {
    if (1 != points[i]->active)
      return i;
  }

  return -1;
}

void  delete_point(u_int p) {
  points[p]->active = 0;
}

void  init_point(u_int i) {
  point* p;

  float s;

  p = points[i];

  p->active = 1;
  p->p[0] = 0;
  p->p[1] = 0;
  p->p[2] = 0;
  p->pv[0] = frandom() / 5 - .1;
  p->pv[1] = frandom() / 5 - .1;
  p->pv[2] = frandom() / 5 - .1;

  p->r[0] = 0;
  p->r[1] = 0;
  p->r[2] = frandom() * 360;
  p->rv[0] = 0;
  p->rv[1] = 0;
  p->rv[2] = frandom() * 10 - 5;

  s = frandom() * 4 + 1;

  p->s[0] = s;
  p->s[1] = s;
  p->s[2] = 1;
  p->sv[0] = 0;
  p->sv[1] = 0;
  p->sv[2] = 0;
}

void update_points() {
  u_int i;
  u_int j;
  point* p;

  for (i = 0; i < NUM_POINTS; i++) {
    if (! points[i]->active)
      continue;

    p = points[i];

    for (j = 0; j < 3; j++) {
      p->p[j] += p->pv[j];
      p->r[j] += p->rv[j];
      p->s[j] += p->sv[j];

      if ((p->p[j] > BOUND) ||
	  (p->p[j] < -BOUND)) {
	delete_point(i);
      }
    }

    p->s[0] *= 0.97;
    p->s[1] *= 0.97;
    p->pv[1] += GRAV;
  }

  i = free_point();
  if (-1 != i) {
    init_point(i);
  }

}

void draw_points() {
  u_int i;
  point* p;

  glRotatef(-rot,.2,.3,.5);

  for (i = 0; i < NUM_POINTS; i++) {
    if (! points[i]->active)
      continue;

    p = points[i];

    glPushMatrix();

    glTranslatef(p->p[0], p->p[1], p->p[2]);
    glScalef(p->s[0], p->s[1], p->s[2]);
    glRotatef(p->r[2], 0,0,1);

    glBindTexture(GL_TEXTURE_2D, tid);

    glBegin(GL_QUADS);
    glColor4f(1,1,1,1);
    glTexCoord2f(0,0);
    glVertex3f(-1, -1,0);
    glTexCoord2f(0,1);
    glVertex3f(-1, +1,0);
    glTexCoord2f(1,1);
    glVertex3f(+1, +1,0);
    glTexCoord2f(1,0);
    glVertex3f(+1, -1,0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glPopMatrix();

  }
}

int main(int argc, char* argv[]) {
  u_int i;

  for (i = 0; i < NUM_POINTS; i++) {
    points[i] = (point*) malloc(sizeof(point));
    points[i]->active = 0;
  }

  test();
 
  atexit(SDL_Quit);

  return 0;
}

int test() {
  sg_root* root;
  sg_label* l;
  sg_window* w;

  root = sg_init();

  img = sg_image_load(root, "test_2.png");
  tid = sg_image_texture(img);

  assert(NULL != root);

  w = sg_window_new(root);
  l = (sg_label*) w->subs[SG_WINDOW_SUB_HEAD]->subs[SG_WINDOW_HEAD_SUB_LABEL];

  w->set_geo((sg_element*) w, 20,30,200,100);
  sg_label_set_string(l, "THIS IS A WINDOW");

  sg_root_add_window(root, w);

  root->timing->callback_frame = (fun_timing_mark) frame_mark;

  sg_loop(root);

  return 0;
}

void frame_mark(u_int fs, u_int fl) {
  rot += 0.6;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(-1.5,1.5,-1.5,1.5,1,10);
  glTranslatef(0,0,-3);
  glRotatef(rot,.2,.3,.5);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glBegin(GL_QUADS);
  glColor4f(1,0,0,0.5f);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1, 1,-1);
  glVertex3f( 1, 1,-1);
  glVertex3f( 1,-1,-1);

  glColor4f(0,1,0,0.5f);
  glVertex3f(-1,-1, 1);
  glVertex3f(-1, 1, 1);
  glVertex3f( 1, 1, 1);
  glVertex3f( 1,-1, 1);

  glColor4f(1,1,0,0.5f);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,-1, 1);
  glVertex3f( 1,-1, 1);
  glVertex3f( 1,-1,-1);

  glColor4f(0,0,1,0.5f);
  glVertex3f(-1, 1,-1);
  glVertex3f(-1, 1, 1);
  glVertex3f( 1, 1, 1);
  glVertex3f( 1, 1,-1);

  glColor4f(1,0,1,0.5f);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,-1, 1);
  glVertex3f(-1, 1, 1);
  glVertex3f(-1, 1,-1);

  glColor4f(0,1,1,0.5f);
  glVertex3f(1,-1,-1);
  glVertex3f(1,-1, 1);
  glVertex3f(1, 1, 1);
  glVertex3f(1, 1,-1);
  glEnd();

  update_points();
  draw_points();

  glMatrixMode(GL_PROJECTION);

  glPopMatrix();

}
