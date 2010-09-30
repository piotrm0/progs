#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>

//#include "viewport.h"
//#include "common.h"

#include "util.h"
//#include "gl_util.h"
#include "m_util.h"
#include "org.h"

//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//#include <OpenGL/glext.h>

#include <GLUT/glut.h>

#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/opengl.h>

float scale_1 = 0.1f;
float rate_1 = 0.01f;

#define WAVICLES 201
#define FLOW_R 0.1f
#define DAMP_V 0.9999f
#define RAND_SCALE 100.0f
#define RAND_RATE 0.0f

org_world* world = NULL;
#define ORGS 20

float phase1 = 0;

u_int sim_paused = 0;

AG_Button* pause_button;
AG_GLView* glv;

void sample_world();

typedef struct _wavicle {
  v1* c;
  v1* v;
} wavicle;

wavicle ws[WAVICLES];

GLvoid init_scene(GLvoid) {
  u_int i;

  world = org_world_new();
  org_world_populate(world, ORGS);

  for (i = 1; i < WAVICLES-1; i++) {
    //ws[i].c = v1_new(10.0f * frandom() - 5.0f);
    ws[i].c = v1_new(0.0f);
    //ws[i].v = v1_new(2.0f * frandom() - 1.0f);
    ws[i].v = v1_new(0.0f);
  }
  ws[0].c = v1_new(0.0f);
  ws[0].v = v1_new(0.0f);
  ws[WAVICLES-1].c = v1_new(0.0f);
  ws[WAVICLES-1].v = v1_new(0.0f);

}

GLvoid sample_world() {
  u_int i;
  float f;

  org_world_mark(world, rate_1);

  if (frandom() < RAND_RATE) {
    f = RAND_SCALE * frandom() - RAND_SCALE / 2.0f;
    i = 2 + irandom(WAVICLES - 4);
    ws[i].v->v[0] += f;
  }

  ws[50].v->v[0] = 1.0f * scale_1 * sin(phase1);
  ws[49].v->v[0] = 0.7f * scale_1 * sin(phase1);
  ws[51].v->v[0] = 0.7f * scale_1 * sin(phase1);
  phase1 += rate_1;

  for (i = 1; i < WAVICLES - 1; i++) {
    ws[i].v->v[0] += (ws[i-1].c->v[0] - ws[i].c->v[0]) * 0.5 * FLOW_R +
      (ws[i+1].c->v[0] - ws[i].c->v[0]) * 0.5 * FLOW_R;
    ws[i].v->v[0] *= DAMP_V;
  }
  for (i = 1; i < WAVICLES - 1; i++) {
    ws[i].c->v[0] += ws[i].v->v[0];
  }
}

static void gl_scale(AG_Event *event) {
  GLdouble xMin, xMax, yMin, yMax;
	
  glLoadIdentity();

  GLdouble aspect = (GLdouble) AGWIDGET(glv)->w / (GLdouble) AGWIDGET(glv)->h;

  yMax =  1.0f;
  yMin = -1.0f;
  xMin = yMin * aspect;
  xMax = yMax * aspect;
  glFrustum(xMin, xMax, yMin, yMax, 2.0, 10.0);

}

float ls = 0;
float rot = 0;


static void gl_draw(AG_Event *event) {
  glLoadIdentity();
	
  //  glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushAttrib(GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glShadeModel(GL_SMOOTH);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClearDepth(1.0f);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  //glEnable(GL_LIGHTING);
  //glEnable(GL_LIGHT0);

  glDepthFunc(GL_LESS);

  glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);

  glHint(GL_LINE_SMOOTH_HINT |
	 GL_PERSPECTIVE_CORRECTION_HINT |
	 GL_POINT_SMOOTH_HINT |
	 GL_POLYGON_SMOOTH_HINT, 
	 GL_FASTEST);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  sample_world();

  u_int i;

  glPushMatrix(); glTranslatef(0.0f, 0.0f, -5.0f);

  glRotatef(rot += 0.1, 0, 1, 0);

  glLineWidth(1.0f);
  glColor3f(0.0f, 0.0f, 0.0f);

  geo_space_draw_gl(world->space);

  glPopMatrix();

  glPopAttrib();
}

static void ui_toggle_pause(AG_Event* event) {
  sim_paused = ! sim_paused;

  AG_ButtonText(pause_button, (sim_paused ? "unpause" : "pause"));
}

int main(int argc, char *argv[]) {
  init_scene();

  if (AG_InitCore("agar test 1", 0) == -1 ||
      AG_InitVideo(320, 240, 32, AG_VIDEO_RESIZABLE | AG_VIDEO_OPENGL) == -1) {
    fprintf(stderr, "%s\n", AG_GetError());
    return (1);
  }

  AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);
  AG_BindGlobalKey(SDLK_F8, KMOD_NONE, AG_ViewCapture);

  AG_Window* win;

  AG_Pane *pane;
  AG_Box *div1, *div2;

  win = AG_WindowNew(AG_WINDOW_PLAIN);

  pane = AG_PaneNew(win, AG_PANE_HORIZ, AG_PANE_EXPAND);
  div1 = pane->div[0];
  div2 = pane->div[1];

  AG_LabelNew(div1, 0, "Controls");

  AG_Numerical *num_rate;
  AG_Numerical *num_scale;

  num_rate = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "rate: ");
  AG_WidgetBindFloat(num_rate, "value", &rate_1);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.0001);

  num_scale = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "scale: ");
  AG_WidgetBindFloat(num_scale, "value", &scale_1);
  AG_NumericalSetRangeFlt(num_scale, 0, 100);
  AG_NumericalSetIncrement(num_scale, 0.1);

  pause_button = AG_ButtonNewFn(div1, 0, "pause", ui_toggle_pause, "%s", "help caption");

  glv = AG_GLViewNew(div2, AG_GLVIEW_EXPAND);

  AG_GLViewScaleFn(glv, gl_scale, NULL);
  AG_GLViewDrawFn(glv, gl_draw, NULL);

  AG_WindowShow(win);
  AG_WindowMaximize(win);

  AG_EventLoop();
  AG_Destroy();

  return (0);
}
