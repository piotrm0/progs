#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>

#include <util.h>
#include <gl_util.h>
#include <m_util.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

#include <GLUT/glut.h>

#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/opengl.h>

float scale_1 = 10.0f;
float rate_1  = 0.6f;
float rate_2  = 0.8f;
float damp_1  = 0.99f;

//#define DRAW_NORMALS = 1

#define RAND_RATE 1.0f

#define WW 200
#define WH 200

#define SIER_ITER 0

#define DIFFUSE_MAG 0.2f

//#define NOISE 1
#define NOISE_W 3
#define NOISE_H 3
#define NOISE_SHARP .1

#define BUMP_RADIUS 20
#define BUMP_SHARP  0.1

//#define SLITS 1
//#define WAVE_GEN 1

//#define WRAP 1

//#define TILE
#define TILES_X 3
#define TILES_Y 3

GLuint dlist1;

GLfloat ambientLight[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat diffuseLight[]  = { DIFFUSE_MAG, DIFFUSE_MAG, DIFFUSE_MAG, 1.0f };
GLfloat specularLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat position[] = { 10.0f, 100.0f, 10.0f, 1.0f };

float phase_1 = 0;

u_int sim_paused = 0;

AG_Button* pause_button;
AG_Button* blip_button;
AG_GLView* glv;

void sample_world();
void reset_c();
void reset_v();

typedef struct _wavicle {
  v3* p;

  v3* norm;
  v3* norm1;
  v3* norm2;

  float c;
  float v;
  float d;
} wavicle;

typedef wavicle* wavicle_ptr;

wavicle ws[WW][WH];

#define WAVICLE_FOREACH(w,do) {		\
    u_int x,y;				\
    wavicle* w;				\
    for (x = 0; x < WW; x++) {		\
      for (y = 0; y < WH; y++) {	\
	w = & ws[x][y];			\
	do;				\
      }					\
    }					\
  }

void ui_flatten() {
  reset_c();
  reset_v();
}

void reset_c() {
  WAVICLE_FOREACH(w, {
      w->c = 0;
      w->p->v[2] = 0;
    });
}

void reset_v() {
  WAVICLE_FOREACH(w, {
      w->v = 0;
    });
}

void fill_noise() {
  int x,y;
  int nx, ny, nxf, nyf;

  float n[NOISE_W+1][NOISE_H+1];

  double dist[NOISE_W+1][NOISE_H+1];

  double tdist;

  float n2b_w = (float) WW / (float) NOISE_W;
  float n2b_h = (float) WH / (float) NOISE_H;

  for (x = 0; x < NOISE_W+1; x++) {
    for (y = 0; y < NOISE_H+1; y++) {
      n[x][y] = frandom();
    }
  }

  for (x = 0; x < WW; x++) {
    for (y = 0; y < WH; y++) {

      tdist = 0;

      for (nx = 0; nx < NOISE_W+1; nx++) {
	for (ny = 0; ny < NOISE_H+1; ny++) {
	  nxf = (float) nx * n2b_w;
	  nyf = (float) ny * n2b_h;

	  dist[nx][ny] = pow(M_E, - sqrt(pow(nxf - x, 2) + pow(nyf - y, 2)) * NOISE_SHARP);

	  tdist += dist[nx][ny];
	}
      }

      ws[x][y].d = 0;

      for (nx = 0; nx < NOISE_W+1; nx++) {
	for (ny = 0; ny < NOISE_H+1; ny++) {
	  ws[x][y].d += n[nx][ny] * dist[nx][ny] / tdist;
	}
      }

    }
  }
}

void color_me(float e, float d) {
  float c[4] = {0,0,0,1.0f};

  if (e > 0) {
    c[1] = e;
  } else {
    c[0] = -e;
  }
  c[2] = d;
  //c[3] = d;

  glColor4fv(c);
}

GLvoid init_scene(GLvoid) {
  u_int y,x;
  float yf,xf, ww, wh, wwm, whm;
  u_int i;

  v3* p[3];

  v3 *p0, *p1, *p2, *pc;

  ww = WW;
  wh = WH;

  wwm = ww/2;
  whm = wh/2;

  float tf;

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      ws[x][y].p = v3_new((float) x - WW / 2, (float) y - WW / 2, 0);
      ws[x][y].norm = v3_new(0,0,0);
      ws[x][y].norm1 = v3_new(0,0,0);
      ws[x][y].norm2 = v3_new(0,0,0);

      yf = y;
      xf = x;

      ws[x][y].c = 0.0f;
      ws[x][y].v = 0.0f;
      ws[x][y].d = 1.0f;

      //      if (x > WW / 2) {
      //	ws[x][y].d = 0.5f;
      //      } else {
      //	ws[x][y].d = 0.25f;
      //      }
      //      ws[x][y].d = ((float) x / (float) WW);
      //      ws[x][y].d = 0.25f;
      //      ws[x][y].d = 0.5 + frandom() * 0.5;

      tf = sqrt(pow(x-wwm, 2) + pow(y-whm,2));

      #define INNER_RADIUS 2
      #define OUTER_RADIUS 100

      if (tf < INNER_RADIUS) {
	ws[x][y].d = 0;
      } else if (tf > OUTER_RADIUS) {
	ws[x][y].d = 1.0;
      } else {
	ws[x][y].d = (tf - (INNER_RADIUS)) / (OUTER_RADIUS - INNER_RADIUS);
      }

      //ws[x][y].d = (sin((float) x/10.f) + 1.0f) * 0.5f;

      //if (x >= wwm - wwm / 3 && x <= wwm + wwm / 3) ws[x][y].d = 0.25f;

#ifdef SLITS
      if (x == wwm && ((y < whm - 40) || (y > whm + 40))) {
	ws[x][y].d = 0;
      }
      if ((x == wwm) && (y > whm - 35) && (y < whm + 35)) {
	ws[x][y].d = 0.0f;
      }
#endif

    }
  }
  
#ifdef NOISE
  fill_noise();
#endif

  p0 = v3_new(0.0, 0.0, 0.0);
  p1 = v3_new(1.0, 0.0, 0.0);
  p2 = v3_new(1.0, 1.0, 0.0);

  pc = v3_new(0.0, 0.0, 0.0);

  p[0] = p0;
  p[1] = p1;
  p[2] = p2;

  for (i = 0; i < SIER_ITER; i++) {
    v3_add(pc, p[irandom(3)]);
    v3_mult(pc, 0.5);

    x = pc->v[0] * (float) WW;
    y = pc->v[1] * (float) WH;

    ws[x][y].d += 1.0f;
    ws[x][y].d *= 0.5;
    //ws[x][y].d = 1.0f;
  }

  /*
  for (y = 0; y < WH-1; y++) {
    ws[0][y].d = 0.0f;
    ws[WW-1][y].d = 0.0f;
  }

  for (x = 0; x < WW-1; x++) {
    ws[x][0].d = 0.0f;
    ws[x][WH-1].d = 0.0f;
  }
  */

  v3_free(p0);
  v3_free(p1);
  v3_free(p2);
  v3_free(pc);
}

float lp = 0;

GLvoid update_light() {
  lp += 0.05;
  position[0] = sin(lp) * WW / 1.5;
  position[1] = cos(lp) * WH / 1.5;
  position[2] = 10.0f;

  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glPushMatrix();

  glTranslatef(position[0], position[1], position[2]);
  glutSolidSphere(1.0f, 10, 10);

  glPopMatrix();
}

GLvoid draw_space() {
  int y,x;
  wavicle *w00, *w01, *w10, *w11;

  v3 *norm1, *norm2;

  norm1 = v3_new(0,0,0);
  norm2 = v3_new(0,0,0);

#ifdef WRAP
  for (x = 0; x < WW; x++) {
    for (y = 0; y < WH; y++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][(y+1) % WH]);
      w10 = & (ws[(x+1) % WW][y]);
      w11 = & (ws[(x+1) % WW][(y+1) % WH]);
#else
  for (x = 0; x < WW-1; x++) {
    for (y = 0; y < WH-1; y++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][y+1]);
      w10 = & (ws[x+1][y]);
      w11 = & (ws[x+1][y+1]);
#endif

      v3_copy_from(norm1, w10->p);
      v3_sub(norm1, w11->p);
      v3_copy_from(norm2, w11->p);
      v3_sub(norm2, w00->p);

      v3_cross(norm1, norm2);
      v3_unit(norm1);

      v3_copy_from(w00->norm1, norm1);

      v3_copy_from(norm1, w00->p);
      v3_sub(norm1, w11->p);
      v3_copy_from(norm2, w11->p);
      v3_sub(norm2, w01->p);

      v3_cross(norm1, norm2);
      v3_unit(norm1);

      v3_copy_from(w00->norm2, norm1);
    }
  }

  v3* n;

#ifdef WRAP
  for (x = 0; x < WW; x++) {
    for (y = 0; y < WH; y++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][(y-1+WH) % WH]);
      w10 = & (ws[(x-1+WW) % WW][y]);
      w11 = & (ws[(x-1+WW) % WW][(y-1+WH) % WH]);
#else
  for (x = 1; x < WW-1; x++) {
    for (y = 1; y < WH-1; y++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][y-1]);
      w10 = & (ws[x-1][y]);
      w11 = & (ws[x-1][y-1]);
#endif

      n = w00->norm;

      v3_set(n, 0);

      v3_add(n, w00->norm1);
      v3_add(n, w00->norm2);

      v3_add(n, w01->norm2);
      v3_add(n, w10->norm1);

      v3_add(n, w11->norm1);
      v3_add(n, w11->norm2);

      v3_mult(n, 1.0f / 6.0f);
    }
  }

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  float s = 5.0f / (float) WW;

#ifdef TILE

  glNewList(dlist1, GL_COMPILE);

#else 

  glPushMatrix();

  glScalef(s,s,s);

  update_light();

#endif

  for (y = 0; y < WH-1; y++) {
    for (x = 0; x < WW-1; x++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][y+1]);
      w10 = & (ws[x+1][y]);
      w11 = & (ws[x+1][y+1]);

      glBegin(GL_TRIANGLES);

      gl_normal_v3(w10->norm);
      color_me(w10->c, w10->d);
      gl_vertex_v3(w10->p);

      gl_normal_v3(w11->norm);
      color_me(w11->c, w11->d);
      gl_vertex_v3(w11->p);

      gl_normal_v3(w00->norm);
      color_me(w00->c, w00->d);
      gl_vertex_v3(w00->p);

      //gl_normal_v3(w00->norm);
      //color_me(w00->c, w00->d);
      gl_vertex_v3(w00->p);

      gl_normal_v3(w11->norm);
      color_me(w11->c, w11->d);
      gl_vertex_v3(w11->p);

      gl_normal_v3(w01->norm);
      color_me(w01->c, w01->d);
      gl_vertex_v3(w01->p);

      glEnd();

      #ifdef DRAW_NORMALS
      v3_copy_from(norm1, w10->norm);
      v3_add(norm1, w10->p);

      glColor4f(1.0,1,1,1);

      glBegin(GL_LINES);
      gl_vertex_v3(w10->p);
      gl_vertex_v3(norm1);
      glEnd();
      #endif

    }
  }

#ifdef TILE

  glEndList();

#endif

  v3_free(norm1);
  v3_free(norm2);

#ifdef TILE

  glPushMatrix();

  glScalef(s,s,s);

  update_light();

  for (x = 0; x < TILES_X; x++) {
    for (y = 0; y < TILES_Y; y++) {
      glPushMatrix();

      glTranslatef((double) (WW-1) * ((double) x - (double) TILES_X / 2.0f + 0.5),
		   (double) (WH-1) * ((double) y - (double) TILES_Y / 2.0f + 0.5),
		   0);

      glCallList(dlist1);

      glPopMatrix();
    }
  }

#else

#endif
  
  glPopMatrix();
}

GLvoid add_to_v(int x, int y, double amount) {
  if (x >= 1 && y >= 1 && x < WW-1 && y < WH-1) {
    ws[x][y].c += amount;
  }
}

GLvoid place_bump() {
  int x = irandom(WW - 2) + 1;
  int y = irandom(WH - 2) + 1;

  float xi, yi;

  float s = 1.0f;

  float a;

  if (frandom() > 0.5) s = -1.0f;

  for (xi = - BUMP_RADIUS; xi <= BUMP_RADIUS; xi++) {
    for (yi = - BUMP_RADIUS; yi <= BUMP_RADIUS; yi++) {

      a = s * scale_1 * pow(2, - (xi * xi + yi * yi) * BUMP_SHARP);

      add_to_v(x + xi, y+yi, a);
    }
  }

}

GLvoid sample_world() {
  u_int i;

  wavicle_ptr w;

  u_int x,y;

  float temp = 1 / (M_SQRT2 * M_SQRT2);

  wavicle_ptr n[8];

  if (sim_paused) {
    return;
  }

#ifdef WAVE_GEN
  phase_1 += 0.5;
  for (y = 0; y < WH; y++) {
    ws[WW-2][y].c = sin(phase_1) * 5;
    ws[WW-2][y].v = 0;
    ws[WW-1][y].c = 0;
  }
#endif

  if (frandom() > RAND_RATE) {
    place_bump();
  }

#ifdef WRAP
  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      w = & (ws[x][y]);

      n[0] = & (ws[x][(y-1 + WH) % WH]);
      n[1] = & (ws[(x-1 + WW) % WW][y]);
      n[2] = & (ws[x][(y+1) % WH]);
      n[3] = & (ws[(x+1) % WW][y]);
      n[4] = & (ws[(x-1 + WW) % WW][(y-1 + WH) % WH]);
      n[5] = & (ws[(x-1 + WW) % WW][(y+1) % WH]);
      n[6] = & (ws[(x+1) % WW][(y+1) % WH]);
      n[7] = & (ws[(x+1) % WW][(y-1 + WH) % WH]);
#else
  for (y = 1; y < WH-1; y++) {
    for (x = 1; x < WW-1; x++) {
      w = & (ws[x][y]);

      n[0] = & (ws[x][y-1]);
      n[1] = & (ws[x-1][y]);
      n[2] = & (ws[x][y+1]);
      n[3] = & (ws[x+1][y]);
      n[4] = & (ws[x-1][y-1]);
      n[5] = & (ws[x-1][y+1]);
      n[6] = & (ws[x+1][y+1]);
      n[7] = & (ws[x+1][y-1]);
#endif
      for (i = 0; i < 4; i++) {
	w->v += (n[i]->c - w->c) * rate_1 * rate_2;
      }
      for (i = 4; i < 8; i++) {
	w->v += (n[i]->c - w->c) * rate_1 * rate_2 * temp;
      }

    }
  }

#ifdef WRAP
  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
#else
  for (y = 1; y < WH-1; y++) {
    for (x = 1; x < WW-1; x++) {
#endif
      w = & (ws[x][y]);
      w->c += w->v * rate_2 * w->d;
      w->p->v[2] = w->c;
      w->v *= damp_1;
    }
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
	
  glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glShadeModel(GL_SMOOTH);
  glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
  glClearDepth(1.0f);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glEnable(GL_COLOR_MATERIAL);

  glDepthFunc(GL_LESS);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);
  glPolygonMode(GL_FRONT, GL_POLYGON);

  glHint(GL_LINE_SMOOTH_HINT |
	 GL_PERSPECTIVE_CORRECTION_HINT |
	 GL_POINT_SMOOTH_HINT |
	 GL_POLYGON_SMOOTH_HINT, 
	 GL_FASTEST);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Assign created components to GL_LIGHT0
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
  //glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

  sample_world();

  glPushMatrix(); glTranslatef(0.0f, 0.5f, -5.0f);

  glRotatef(45, -1, 0, 0);

  //glLineWidth(1.0f);
  //glColor3f(0.0f, 0.0f, 0.0f);

  draw_space();

  glPopMatrix();

  glPopAttrib();

}

static void ui_blip(AG_Event* event) {
  place_bump();
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
  AG_NumericalSetIncrement(num_rate, 0.05);

  num_rate = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "rate2: ");
  AG_WidgetBindFloat(num_rate, "value", &rate_2);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.05);

  num_rate = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "damp2: ");
  AG_WidgetBindFloat(num_rate, "value", &damp_1);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.0001);

  num_scale = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "scale: ");
  AG_WidgetBindFloat(num_scale, "value", &scale_1);
  AG_NumericalSetRangeFlt(num_scale, 0, 1000);
  AG_NumericalSetIncrement(num_scale, 5);

  pause_button = AG_ButtonNewFn(div1, 0, "pause", ui_toggle_pause, "%s", "help caption");
  AG_ButtonNewFn(div1, 0, "flatten", ui_flatten, "%s", "help caption");
  blip_button  = AG_ButtonNewFn(div1, 0, "blip", ui_blip, "%s", "help caption");

  glv = AG_GLViewNew(div2, AG_GLVIEW_EXPAND);

  AG_GLViewScaleFn(glv, gl_scale, NULL);
  AG_GLViewDrawFn(glv, gl_draw, NULL);

  AG_WindowShow(win);
  AG_WindowMaximize(win);

  dlist1 = glGenLists(1);
  printf("dlist1 = %d\n", dlist1);

  AG_EventLoop();
  AG_Destroy();

  return (0);
}
