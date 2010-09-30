#ifndef __cell_h__
#define __cell_h__

#include <assert.h>

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

#define WW 512
#define WH 512

//#define TILE
#define TILES_X 3
#define TILES_Y 3

#define USE_CANVAS
//#define USE_HEIGHT

//#define USE_NORMALS
//#define DRAW_NORMALS

#ifdef USE_CANVAS
char* canvas;
#endif

inline extern void custom_sample();
inline extern void custom_update();
inline extern void custom_init_cells();
inline extern void custom_init_gui();

GLuint dlist1;

typedef struct _cell {
  v3* pos;

  float color[4];

#ifdef USE_NORMALS
  v3* norm;
  v3* norm1;
  v3* norm2;
#endif

  u_int x, y;

  float height;

  void* cd;
} cell;

typedef cell* cell_ptr;

//extern inline void custom_init_cell(cell* c);
//extern inline void custom_update_cell(cell* c);
//extern inline void custom_color(cell* c);

cell ws[WW][WH];

#define COLOR_ME(c)				\
  glColor4fv(c->color)				\

#define CELL_AT_OFFSET(x, y, xo, yo)			\
  (ws[(x+xo) % WW][(y+yo) % WH])			\

#define CELL_FOREACH(w,do) {		\
    u_int x,y;				\
    cell* w;				\
    for (x = 0; x < WW; x++) {		\
      for (y = 0; y < WH; y++) {	\
	w = & ws[x][y];			\
	do;				\
      }					\
    }					\
  }

#define DIFFUSE_MAG 0.5f

extern GLfloat cell_gl_light_ambient[];
extern GLfloat cell_gl_light_diffuse[];
extern GLfloat cell_gl_light_specular[];
extern GLfloat cell_gl_light_position[];

u_int cell_sim_paused;

AG_Button* cell_ui_pause_button;
AG_Button* cell_ui_reset_button;
AG_GLView* cell_ui_glv;

AG_Box *cell_ui_div_controls, *cell_ui_div_view;

typedef void (*cell_button_handler) (AG_Event* event);

void cell_add_button(char* label, cell_button_handler handler);
void cell_add_float_input(char* label, float min, float max, float inc, int prec, float* storer);

#endif /* __cell_h__ */
