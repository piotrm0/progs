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

double scale_1 = 1.0f;
double rate_1  = 0.6f;
double rate_2  = 0.8f;
double damp_1  = 0.99f;

//#define DRAW_NORMALS = 1

#define RAND_RATE 1.0f

#define WW 99
#define WH 99

GLuint dlist1;

#define DIFFUSE_MAG 0.2f

GLfloat ambientLight[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat diffuseLight[]  = { DIFFUSE_MAG, DIFFUSE_MAG, DIFFUSE_MAG, 1.0f };
GLfloat specularLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat position[] = { 10.0f, 100.0f, 10.0f, 1.0f };

double phase_1 = 0;

u_int sim_paused = 0;

AG_Button* pause_button;
AG_Button* blip_button1;
AG_Button* blip_button2;
AG_GLView* glv;

void sample_world();
void reset_c();
void reset_v();

#define MAX_V 1.0f

typedef struct _wavicle {
  v3* p;

  //v3* norm;
  //v3* norm1;
  //v3* norm2;

  //double c;
  //double v;
  //double d;

  double m; // mass
  double tm; // temp mass
  double vx; //
  double vy; //
  double ax; //
  double ay; //
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

void reset_m();
void reset_v();

void ui_flatten() {
  reset_m();
  reset_v();
}

void reset_m() {
  WAVICLE_FOREACH(w, {
      w->m = 0;
      //w->p->v[2] = 0;
    });
}

void reset_v() {
  WAVICLE_FOREACH(w, {
      w->vx = 0;
      w->vy = 0;
    });
}

void color_me(double e, double d) {
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
  double yf,xf, ww, wh, wwm, whm;
  u_int i;

  ww = WW;
  wh = WH;

  wwm = ww/2;
  whm = wh/2;

  double tf;

  double rs = 5.0f;

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      ws[x][y].p = v3_new((double) x - WW / 2, (double) y - WW / 2, 0);

      //ws[x][y].norm  = v3_new(0,0,0);
      //ws[x][y].norm1 = v3_new(0,0,0);
      //ws[x][y].norm2 = v3_new(0,0,0);

      //yf = y;
      //xf = x;

      //ws[x][y].c = 0.0f;
      //ws[x][y].v = 0.0f;
      //ws[x][y].d = 1.0f;
      
      ws[x][y].m = frandom() * rs;

      // !!! initial

      //ws[x][y].m = 1.0f;

      ws[x][y].ax = 0.0f;
      ws[x][y].ay = 0.0f;
      ws[x][y].vx = 0.0f;
      ws[x][y].vy = 0.0f;
    }
  }
}

double lp = 0;

GLvoid update_light() {
  //lp += 0.05;
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

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  double s = 5.0f / (double) WW;

  glPushMatrix();

  glScalef(s,s,s);

  update_light();

  float h = 0;

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      
      w00 = & (ws[x][y]);

      /* w01 = & (ws[x][y+1]);
	 w10 = & (ws[x+1][y]);
	 w11 = & (ws[x+1][y+1]); */

      glPushMatrix();
      gl_translate_v3(w00->p);

      //glBegin(GL_LINE_LOOP);
      //glBegin(GL_POLYGON);
      //glBegin(GL_TRIANGLES);

      //glPushMatrix();
      
      h = log(w00->m + 1);

      glTranslatef(0,0,h);

      glBegin(GL_QUADS);

      color_me(w00->m, 0);

      glNormal3f(0,0,1.0f);

      glVertex3f(+0.5, +0.5, 0);
      glVertex3f(-0.5, +0.5, 0);
      glVertex3f(-0.5, -0.5, 0);
      glVertex3f(+0.5, -0.5, 0);

      glEnd();

      glTranslatef(0,0,-h);

      //glPopMatrix();

      //glClear(GL_DEPTH_BUFFER_BIT);

      glBegin(GL_LINES);

      glColor4f(0.0, 0.0, 0.0, 1.0);

      glVertex3f(0,0,h + 0.1f);
      glVertex3f(w00->ax, w00->ay, h + 0.1f);

      glEnd();

      glPopMatrix();


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

  //v3_free(norm1);
  //v3_free(norm2);
  
  glPopMatrix();
}

static void ui_blip1(AG_Event* event) {
  place_bump(1.0f);
}
static void ui_blip2(AG_Event* event) {
  place_bump(-1.0f);
}

#define BUMP_RADIUS 40
#define BUMP_SHARP  0.01

GLvoid place_bump(double s) {
  //int x = irandom(WW - 2) + 1;
  //int y = irandom(WH - 2) + 1;
  int x = WW / 2;
  int y = WH / 2;

  double xi, yi;

  double a;

  wavicle* w;

  double xm;
  double ym;
  double mag;

  for (xi = - BUMP_RADIUS; xi <= BUMP_RADIUS; xi++) {
    for (yi = - BUMP_RADIUS; yi <= BUMP_RADIUS; yi++) {

      a = s * scale_1 * pow(2, - (xi * xi + yi * yi) * BUMP_SHARP);

      //add_to_v(x + xi, y+yi, a);

      w = & (ws[(x + (int) xi + WW) % WW][(y + (int) yi + WH) % WH]);

      xm = xi;
      ym = yi;

      mag = sqrt(xi * xi + yi * yi);
      if (mag != 0) {
	xm /= mag;
	ym /= mag;
      }

      w->ax += xm * a;
      w->ay += ym * a;

      /*
      if (xi < 0.0f) {
	w->ax += a;
      } else if (xi > 0.0f) {
	w->ax -= a;
      }

      if (yi < 0.0f) {
	w->ay += a;
      } else if (yi > 0.0f) {
	w->ay -= a;
      }
      */

    }
  }

}

GLvoid add_to_v(int x, int y, double amount) {
  if (x >= 1 && y >= 1 && x < WW-1 && y < WH-1) {
    //ws[x][y].c += amount;
  }
}

GLvoid sample_world() {
  u_int i;

  wavicle_ptr w;

  u_int x,y;

  double temp = 1 / (M_SQRT2 * M_SQRT2);

  double part = 0.125;

  wavicle_ptr n[8];

  if (sim_paused) {
    return;
  }

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      w = & (ws[x][y]);

      w->tm = 0.0f;

      n[0] = & (ws[x][(y-1+WH) % WH]);
      n[2] = & (ws[x][(y+1) % WH]);
      n[1] = & (ws[(x-1+WW) % WW][y]);
      n[3] = & (ws[(x+1) % WW][y]);

      n[4] = & (ws[(x-1+WW) % WW][(y-1+WH) % WH]);
      n[5] = & (ws[(x-1+WW) % WW][(y+1) % WH]);
      n[6] = & (ws[(x+1) % WW][(y+1) % WH]);
      n[7] = & (ws[(x+1) % WW][(y-1+WH) % WH]);

      for (i = 0; i < 4; i++) {
	w->ax += (n[i]->vx - w->vx) * rate_1 * part;
	w->ay += (n[i]->vy - w->vy) * rate_1 * part;
      }
      /*
      for (i = 4; i < 8; i++) {
      	w->ax += (n[i]->vx - w->vx) * rate_1 * part * temp;
      	w->ay += (n[i]->vy - w->vy) * rate_1 * part * temp;
      }
      */
    }
  }

  double tmass = 0;

  double mag = 0;

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      w = & (ws[x][y]);

      //w->c += w->v * rate_2 * w->d;
      //w->p->v[2] = w->c;
      //w->ay -= 0.01;

      w->ax *= damp_1;
      w->ay *= damp_1;

      w->vx += w->ax * rate_2;
      w->vy += w->ay * rate_2;

      mag = w->vx * w->vx + w->vy * w->vy;

      if (mag > 1) {
	mag = sqrt(mag);
	w->vx /= mag;
	w->vy /= mag;
      }

      tmass += w->m;
    }
  }

  //printf("total mass=%f\n", tmass);

  double lostx = 0;
  double losty = 0;

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      w = & (ws[x][y]);

      //w->c += w->v * rate_2 * w->d;
      //w->p->v[2] = w->c;

      lostx = w->m * w->vx * w->vx;
      if (w->vx > 0.0f) {
	//lostx *= (1 - ws[(x+1) % WW][y].m);
	if (lostx < 0) { lostx = 0; };
	ws[(x+1) % WW][y].tm += lostx;
      } else { //if (w->vx < 0.0f) {
	//lostx *= (1 - ws[(x-1+WW) % WW][y].m);
	if (lostx < 0) { lostx = 0; };
	ws[(x-1+WW) % WW][y].tm += lostx;
      }

      losty = w->m * w->vy * w->vy;
      if (w->vy > 0.0f) {
	//losty *= (1 - ws[x][(y+1) % WH].m);
	if (lostx < 0) { lostx = 0; };
	ws[x][(y+1) % WH].tm += losty;
      } else { //if (w->vy < 0.0f) {
	//losty *= (1 - ws[x][(y-1+WH) % WH].m);
	if (lostx < 0) { lostx = 0; };
	ws[x][(y-1+WH) % WH].tm += losty;
      }

      w->tm -= lostx;
      w->tm -= losty;

    }
  }

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      w = & (ws[x][y]);
      w->m += w->tm;
    }
  }

}

static void gl_scale(AG_Event *event) {
  GLfloat xMin, xMax, yMin, yMax;
	
  glLoadIdentity();

  GLfloat aspect = (GLfloat) AGWIDGET(glv)->w / (GLfloat) AGWIDGET(glv)->h;

  yMax =  1.0f;
  yMin = -1.0f;
  xMin = yMin * aspect;
  xMax = yMax * aspect;
  glFrustum(xMin, xMax, yMin, yMax, 2.0, 10.0);

}

double ls = 0;
double rot = 0;

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

  glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);
  //glPolygonMode(GL_FRONT, GL_POLYGON);

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

static void ui_toggle_pause(AG_Event* event) {
  sim_paused = ! sim_paused;

  AG_ButtonText(pause_button, (sim_paused ? "unpause" : "pause"));
}

int main(int argc, char *argv[]) {
  printf("%d\n", (-3) % 10);

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
  AG_WidgetBindDouble(num_rate, "value", &rate_1);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.05);

  num_rate = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "rate2: ");
  AG_WidgetBindDouble(num_rate, "value", &rate_2);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.05);

  num_rate = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "damp2: ");
  AG_WidgetBindDouble(num_rate, "value", &damp_1);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.0001);

  num_scale = AG_NumericalNew(div1, AG_NUMERICAL_HFILL, NULL, "scale: ");
  AG_WidgetBindDouble(num_scale, "value", &scale_1);
  AG_NumericalSetRangeFlt(num_scale, 0, 1000);
  AG_NumericalSetIncrement(num_scale, 5);

  pause_button = AG_ButtonNewFn(div1, 0, "pause", ui_toggle_pause, "%s", "help caption");
  AG_ButtonNewFn(div1, 0, "flatten", ui_flatten, "%s", "help caption");
  blip_button1  = AG_ButtonNewFn(div1, 0, "blip (pos)", ui_blip1, "%s", "help caption");
  blip_button2  = AG_ButtonNewFn(div1, 0, "blip (neg)", ui_blip2, "%s", "help caption");

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
