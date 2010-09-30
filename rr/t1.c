#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>

#include "viewport.h"
#include "common.h"
#include "board.h"

#include "dfs.h"
#include "dfs.cpp"

#include "util.h"
#include "gl_util.h"

#define ROTV 0.037f

enum {DRAW_BOARD_FULL=1,
      DRAW_BOARD_SIMPLE};

enum {DRAW_LIST_TABLE=1,
      DRAW_LIST_TILE,
      DRAW_LIST_TILE_TRANS,
      DRAW_LIST_WALL,
      DRAW_LIST_PRISM,
      DRAW_LIST_MARK,
      DRAW_LIST_MARK_TRANS,
      DRAW_LIST_ROBOT,
      DRAW_LIST_INFO};

enum {DRAW_TILE  = 1 << 0,
      DRAW_WALL  = 1 << 1,
      DRAW_VOID  = 1 << 2,
      DRAW_PRISM = 1 << 3,
      DRAW_MARK  = 1 << 4,
      DRAW_ROBOT = 1 << 5,
      DRAW_INFO  = 1 << 6,
      DRAW_ALL   = (1 << 7) - 1};

//#define BOARD_WIDTH  40
//#define BOARD_HEIGHT 40
//#define BOARD_WIDTH  16
//#define BOARD_HEIGHT 16
u_int BOARD_WIDTH  = 24;
u_int BOARD_HEIGHT = 24;

enum {TEXTURE_TILE=1,
      TEXTURE_MARK_MOON_RED,
      TEXTURE_MARK_MOON_GREEN,
      TEXTURE_MARK_MOON_BLUE,
      TEXTURE_MARK_MOON_YELLOW,
      TEXTURE_MARK_STAR_RED,
      TEXTURE_MARK_STAR_GREEN,
      TEXTURE_MARK_STAR_BLUE,
      TEXTURE_MARK_STAR_YELLOW,
      TEXTURE_MARK_SATURN_RED,
      TEXTURE_MARK_SATURN_GREEN,
      TEXTURE_MARK_SATURN_BLUE,
      TEXTURE_MARK_SATURN_YELLOW,
      TEXTURE_MARK_GEAR_RED,
      TEXTURE_MARK_GEAR_GREEN,
      TEXTURE_MARK_GEAR_BLUE,
      TEXTURE_MARK_GEAR_YELLOW,
      TEXTURE_MARK_WARP,
      TEXTURE_LAST};
#define TEXTURES TEXTURE_LAST - 1

#define ROBOT_HOVER_RATE 0.1f

GLfloat robot_hover_t = 0.0f;

u_int show_coord = 0;

GLuint mark_texture[MARKS][COLORS] = {{TEXTURE_MARK_MOON_RED,
				       TEXTURE_MARK_MOON_GREEN,
				       TEXTURE_MARK_MOON_BLUE,
				       TEXTURE_MARK_MOON_YELLOW},
				      {TEXTURE_MARK_STAR_RED,
				       TEXTURE_MARK_STAR_GREEN,
				       TEXTURE_MARK_STAR_BLUE,
				       TEXTURE_MARK_STAR_YELLOW},
				      {TEXTURE_MARK_SATURN_RED,
				       TEXTURE_MARK_SATURN_GREEN,
				       TEXTURE_MARK_SATURN_BLUE,
				       TEXTURE_MARK_SATURN_YELLOW},
				      {TEXTURE_MARK_GEAR_RED,
				       TEXTURE_MARK_GEAR_GREEN,
				       TEXTURE_MARK_GEAR_BLUE,
				       TEXTURE_MARK_GEAR_YELLOW},
				      {0, 0, 0, 0, TEXTURE_MARK_WARP}};

enum {ROBOT_MODE_PLACE,
      ROBOT_MODE_TRACE};

u_int robot_mode = ROBOT_MODE_TRACE;

enum {CURSOR_LOCK_NONE,
      CURSOR_LOCK_ROBOT};

#define ROBOT_NONE COLORS

u_int cursor_lock = CURSOR_LOCK_NONE;
u_int cursor_lock_robot = ROBOT_NONE;

tile* cursor;
tile* origin;

tile** robots;
tile** robots_v;

GLuint* textures;

GLfloat rot = 0.0f;

GLfloat fvzero[]   = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat fvopaque[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat fvone[]    = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat cursor_bg[] = {1.0f, 1.0f, 1.0f, 0.5f};
GLfloat cursor_fg[] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat gray_opaque[] = {0.2f, 0.2f, 0.2f, 1.0f};

GLfloat tile_shine[]   = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat tile_emit[]    = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat tile_diffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};
GLfloat wall_shine[]   = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat wall_diffuse[] = {0.02f, 0.02f, 0.02f, 1.0f};

GLfloat mark_emit[] = {0.3f, 0.3f, 0.3f, 1.0f};

GLfloat prism_diffuse[COLORS][4] = {{1.0f, 0.0f, 0.0f, 0.5f},
				    {0.0f, 1.0f, 0.0f, 0.5f},
				    {0.0f, 0.0f, 1.0f, 0.5f},
				    {1.0f, 1.0f, 0.0f, 0.5f}};

GLfloat robot_diffuse[COLORS][4] = {{0.8f, 0.0f, 0.0f, 1.0f},
				    {0.0f, 0.8f, 0.0f, 1.0f},
				    {0.0f, 0.0f, 0.8f, 1.0f},
				    {0.8f, 0.8f, 0.0f, 1.0f},
				    {0.1f, 0.1f, 0.1f, 1.0f}};

GLvoid draw_cursor(tile** c);
GLvoid move_cursor(tile** c, u_int d);
GLvoid draw_static_board(GLvoid);
GLvoid draw_table(GLvoid);
GLvoid draw_board(tile*, u_int);
GLvoid draw_tile(tile*, u_int, u_int, GLfloat);
GLvoid draw_tile_wall(tile* t);
GLvoid draw_tile_prism(u_int, u_int);
GLvoid draw_tile_mark(u_int, u_int, GLfloat);
GLvoid draw_robots();
GLvoid draw_robot(u_int c, GLfloat alpha);
GLvoid draw_bot(GLvoid);
GLvoid compile_robots();
GLvoid tile_center(tile* t);
GLvoid place_robot(u_int c, tile* t);
u_int  get_robot_at(tile* t);
tile* robot_trace(tile* t, u_int c, u_int d);
u_int robot_locate(tile* t, u_int d);

GLvoid init_scene(GLvoid) {
  u_int* temp;

  temp = (u_int*) calloc(DIMS, sizeof(u_int));
  //  printf("allocated initial [%x]\n", temp);
  temp[0] = 0;
  temp[1] = 0;

  view_main = view_new(0,
		       VIEW_NONE,
		       GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		       0, 0,
		       1, 1);
  view_main->draw_func = draw_scene;

  /*
    view_main->subviews[0] = view_new(0,
				    VIEW_FG | VIEW_BG | VIEW_ASPECT,
				    GL_DEPTH_BUFFER_BIT,
				    0.0, 0.0,
				    0.3, 0.3);
  view_main->subviews[0]->alpha = 0.5f;
  view_main->subviews[0]->draw_func = draw_static_board;
  */
  

  board* b11;
  board* b12;
  board* b13;
  board* b21;
  board* b22;
  board* b23;
  board* b31;
  board* b32;
  board* b33;

  b11 = read_2d("boards/red-star-p.board");
  b21 = read_2d("boards/red-saturn-p.board");
  //b31 = read_2d("boards/red-gear-p.board");
  b12 = read_2d("boards/red-gear-p.board");

  b22 = read_2d("boards/red-moon-p.board");

  //b32 = read_2d("boards/red-saturn-np.board");
  //b13 = read_2d("boards/red-star-p.board");
  //b23 = read_2d("boards/red-gear-np.board");
  //b33 = read_2d("boards/red-moon-p.board");

  //  board_rotate_2d(b2, 1);
  //  board_rotate_2d(b3, 3);
  //  board_rotate_2d(b4, 2);

  boards_stich_2d(b11, DIR_RIGHT, b12);
  // boards_stich_2d(b12, DIR_RIGHT, b13);
  boards_stich_2d(b21, DIR_RIGHT, b22);
  //boards_stich_2d(b22, DIR_RIGHT, b23);
  //boards_stich_2d(b31, DIR_RIGHT, b32);
  //boards_stich_2d(b32, DIR_RIGHT, b33);

  boards_stich_2d(b11, DIR_DOWN, b21);
  //boards_stich_2d(b21, DIR_DOWN, b31);
  boards_stich_2d(b12, DIR_DOWN, b22);
  //boards_stich_2d(b22, DIR_DOWN, b32);
  //boards_stich_2d(b13, DIR_DOWN, b23);
  //boards_stich_2d(b23, DIR_DOWN, b33);

  board_clean_stich(b11);
  board_clean_stich(b12);
  //board_clean_stich(b13);
  board_clean_stich(b21);
  board_clean_stich(b22);
  //board_clean_stich(b23);
  //board_clean_stich(b31);
  //board_clean_stich(b32);
  //board_clean_stich(b33);

  tile_clear_flag(b11->origin, TILE_CHECKED);
  tile_fill_coord(b11->origin, temp);

  //  exit(0);

  robots   = robots_new();
  robots_v = robots_new();

  origin = b11->origin;
  cursor = origin;

  place_robot(COLOR_RED, origin);
  place_robot(COLOR_GREEN, origin->tile_p[DIR_RIGHT]);
  place_robot(COLOR_BLUE, origin->tile_p[DIR_DOWN]);
  place_robot(COLOR_YELLOW, origin->tile_p[DIR_RIGHT]->tile_p[DIR_RIGHT]);
  place_robot(COLOR_BLACK, origin->tile_p[DIR_DOWN]->tile_p[DIR_DOWN]);

  textures = (GLuint*) calloc(TEXTURES, sizeof(GLuint));
  if (textures == NULL)
    pexit(errno, "init_scene: can't allocate texture array");

  textures[TEXTURE_TILE] = gl_load_texture_png("gfx/tile.png", 0);
  textures[TEXTURE_MARK_MOON_RED]    = gl_load_texture_png("gfx/mark_moon_red.png", 1);
  textures[TEXTURE_MARK_MOON_GREEN]  = gl_load_texture_png("gfx/mark_moon_green.png", 1);
  textures[TEXTURE_MARK_MOON_BLUE]   = gl_load_texture_png("gfx/mark_moon_blue.png", 1);
  textures[TEXTURE_MARK_MOON_YELLOW] = gl_load_texture_png("gfx/mark_moon_yellow.png", 1);
  textures[TEXTURE_MARK_STAR_RED]    = gl_load_texture_png("gfx/mark_star_red.png", 1);
  textures[TEXTURE_MARK_STAR_GREEN]  = gl_load_texture_png("gfx/mark_star_green.png", 1);
  textures[TEXTURE_MARK_STAR_BLUE]   = gl_load_texture_png("gfx/mark_star_blue.png", 1);
  textures[TEXTURE_MARK_STAR_YELLOW] = gl_load_texture_png("gfx/mark_star_yellow.png", 1);
  textures[TEXTURE_MARK_SATURN_RED]    = gl_load_texture_png("gfx/mark_saturn_red.png", 1);
  textures[TEXTURE_MARK_SATURN_GREEN]  = gl_load_texture_png("gfx/mark_saturn_green.png", 1);
  textures[TEXTURE_MARK_SATURN_BLUE]   = gl_load_texture_png("gfx/mark_saturn_blue.png", 1);
  textures[TEXTURE_MARK_SATURN_YELLOW] = gl_load_texture_png("gfx/mark_saturn_yellow.png", 1);
  textures[TEXTURE_MARK_GEAR_RED]    = gl_load_texture_png("gfx/mark_gear_red.png", 1);
  textures[TEXTURE_MARK_GEAR_GREEN]  = gl_load_texture_png("gfx/mark_gear_green.png", 1);
  textures[TEXTURE_MARK_GEAR_BLUE]   = gl_load_texture_png("gfx/mark_gear_blue.png", 1);
  textures[TEXTURE_MARK_GEAR_YELLOW] = gl_load_texture_png("gfx/mark_gear_yellow.png", 1);
  textures[TEXTURE_MARK_WARP]        = gl_load_texture_png("gfx/mark_warp.png", 1);

  glNewList(DRAW_LIST_TABLE, GL_COMPILE);
  draw_table();
  glEndList();

  tile_clear_flag(origin, TILE_DRAWN);
  glNewList(DRAW_LIST_TILE, GL_COMPILE);
  draw_tile(origin, DIR_LEFT, DRAW_TILE, 1.0f);
  glEndList();

  tile_clear_flag(origin, TILE_DRAWN);
  glNewList(DRAW_LIST_TILE_TRANS, GL_COMPILE);
  draw_tile(origin, DIR_LEFT, DRAW_TILE, 0.75f);
  glEndList();

  tile_clear_flag(origin, TILE_DRAWN);
  glNewList(DRAW_LIST_MARK, GL_COMPILE);
  glDisable(GL_DEPTH_TEST);
  draw_tile(origin, DIR_LEFT, DRAW_MARK, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEndList();

  tile_clear_flag(origin, TILE_DRAWN);
  glNewList(DRAW_LIST_MARK_TRANS, GL_COMPILE);
  glDisable(GL_DEPTH_TEST);
  draw_tile(origin, DIR_LEFT, DRAW_MARK, 0.75f);
  glEnable(GL_DEPTH_TEST);
  glEndList();

  tile_clear_flag(origin, TILE_DRAWN);
  glNewList(DRAW_LIST_WALL, GL_COMPILE);
  draw_tile(origin, DIR_LEFT, DRAW_WALL, 1.0f);
  glEndList();

  tile_clear_flag(origin, TILE_DRAWN);
  glNewList(DRAW_LIST_PRISM, GL_COMPILE);
  draw_tile(origin, DIR_LEFT, DRAW_PRISM, 1.0f);
  glEndList();

  compile_robots();
}

void toggle_lock_cursor(tile* t) {
  u_int r;
  u_int new_lock = cursor_lock;

  if (NULL == t)
    pexit(EFAULT, "lock_cursor: got NULL tile");

  r = get_robot_at(t);

  switch(cursor_lock) {
  case CURSOR_LOCK_NONE:
    if (ROBOT_NONE != r) {
      new_lock = CURSOR_LOCK_ROBOT;
      cursor_lock_robot = r;
    } else {
      printf("nothing to lock onto\n");
    }
    break;
  case CURSOR_LOCK_ROBOT:
    if ((ROBOT_NONE == r) ||
	(cursor_lock_robot == r)) {
      //      place_robot(cursor_lock_robot, t);
      new_lock = CURSOR_LOCK_NONE;
      cursor_lock_robot = ROBOT_NONE;
      compile_robots();
    } else {
      printf("can't place a robot over another robot\n");
    }
    break;
  }
  cursor_lock = new_lock;
}

u_int get_robot_at(tile* t) {
  u_int i;

  if (NULL == t)
    return ROBOT_NONE;

  for (i = 0; i < COLORS; i++)
    if (robots[i] == t) return i;

  return ROBOT_NONE;
}


GLvoid place_robot(u_int c, tile* t) {
  if (NULL == t)
    pexit(EFAULT, "place_robot: got NULL tile");

  robots[c]   = t;
  robots_v[c] = t;
}

GLvoid compile_robots() {
  tile_clear_flag(origin, TILE_DRAWN);
  glNewList(DRAW_LIST_ROBOT, GL_COMPILE);
  draw_robots();
  glEndList();
}

GLvoid tile_center(tile* t) {
  if (NULL == t)
    pexit(EFAULT, "tile_center: null tile given");
  glTranslatef((GLfloat) t->coord[0], 0, (GLfloat) t->coord[1]);
}

GLvoid draw_robots() {
  u_int i;
  tile* t;

  for (i = 0; i < COLORS; i++) {
    t = robots[i];
    if (NULL != t) {
      glPushMatrix();
      tile_center(t);
      draw_robot(i, 1.0f);
      glPopMatrix();
    }
    if (robots[i] != robots_v[i]) {
      t = robots_v[i];
      glPushMatrix();
      tile_center(t);
      draw_robot(i, 0.3f);
      glPopMatrix();
    }
  }
}

GLvoid update_scene(GLvoid) {
  rot += ROTV;
}

GLvoid draw_scene(GLvoid) {
  //  printf("drawing scene\n");

  glTranslatef(0.0f, 0.0f, -20.0f);

  //  glRotatef(rot_x+30, 1.0f, 0.0f, 0.0f);
  glRotatef(rot_x+90, 1.0f, 0.0f, 0.0f);
  glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
  glRotatef(rot_z, 0.0f, 0.0f, 1.0f);

  glScalef(zoom, zoom, zoom);

  glRotatef(rot, 0.0f, 1.0f, 0.0f);

  //  draw_table();
  //  glCallList(DRAW_LIST_TABLE);

  draw_board(origin, DRAW_BOARD_FULL);
  draw_board(origin, 0);
}

GLvoid draw_static_board(GLvoid) {
  //  printf("drawing static board\n");
  glTranslatef(0.0f, 0.0f, -21.0f);
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  //  glScalef(0.1f, 0.1f, 0.1f);
  //  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  draw_board(origin, 0);
}

GLvoid draw_table() {
  GLfloat i;

  //  glColor4f(0.0f, 1.0f, 0.0f, 0.1f);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat[]) {0.0f, 1.0f, 0.0f, 0.5f});
  glMaterialfv(GL_FRONT, GL_EMISSION, fvzero);
  
  glLineWidth(1.0f);

  glBegin(GL_LINES);
  for (i = -10.0f; i <= 10.0f; i += 1.0f) {
    glVertex3f(i, 0.0f, -10.0f);
    glVertex3f(i, 0.0f, 10.0f);
  }
  for (i = -10.0f; i <= 10.0f; i += 1.0f) {
    glVertex3f(-10.0f, 0.0f, i);
    glVertex3f(10.0f, 0.0f, i);
  }
  glEnd();

}

GLvoid draw_board(tile* o, u_int op) {
  glPushMatrix();

  glTranslatef((GLfloat) BOARD_WIDTH / -2.0f + 0.5f,
	       0,
	       (GLfloat) BOARD_HEIGHT / -2.0f + 0.5f);

  if (op & DRAW_BOARD_FULL) {
    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glColorMask(0,0,0,0);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    //    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glDisable(GL_DEPTH_TEST);
    glCallList(DRAW_LIST_TILE);
    glCallList(DRAW_LIST_MARK);
    glEnable(GL_DEPTH_TEST);

     //glColorMask(1,1,1,1);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    //    glEnable(GL_CLIP_PLANE0);
    //    glClipPlane(GL_CLIP_PLANE0, (double[]) {0.0f, -1.0f, 0.0f, 0.0f});
    //    glEnable(GL_STENCIL_TEST);

    glPushMatrix();
    glScalef(1.0f, -1.0f, 1.0f);
    glCallList(DRAW_LIST_ROBOT);
    glCallList(DRAW_LIST_WALL);
    glCallList(DRAW_LIST_PRISM);
    glPopMatrix();

    glDisable(GL_STENCIL_TEST);
    //    glDisable(GL_CLIP_PLANE0);

    //    glDisable(GL_CLIP_PLANE0);
    //    glDisable(GL_STENCIL_TEST);

    //    glDrawBuffer(GL_LEFT);
    //    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //    glDisable(GL_DEPTH_TEST);
    //    glDisable(GL_STENCIL_TEST);

    //glClear(GL_DEPTH_BUFFER_BIT);
    glCallList(DRAW_LIST_TILE_TRANS);
    glCallList(DRAW_LIST_MARK_TRANS);
    //    glCallList(DRAW_LIST_MARK);
    //    glReadBuffer(GL_LEFT);
    //    glDrawBuffer(GL_BACK);
    //    glPixelTransferf(GL_ALPHA_SCALE, 0.5f);
    //    glCopyPixels(0,0,w_width,w_height, GL_COLOR);
    //    glEnable(GL_DEPTH_TEST);
    //    glClear(GL_DEPTH_BUFFER_BIT);

    draw_cursor(&cursor);

    glCallList(DRAW_LIST_WALL);
    glCallList(DRAW_LIST_PRISM);
    glCallList(DRAW_LIST_ROBOT);
  } else {
    glCallList(DRAW_LIST_TILE);
    glCallList(DRAW_LIST_MARK);
    draw_cursor(&cursor);
    glCallList(DRAW_LIST_WALL);
    glCallList(DRAW_LIST_PRISM);
    glCallList(DRAW_LIST_ROBOT);
  }

  /*  if (show_coord) {
    glDisable(GL_DEPTH_TEST);
    tile_clear_flag(o, TILE_DRAWN);
    draw_tile(o, DIR_LEFT, DRAW_INFO);
    glEnable(GL_DEPTH_TEST);
    } */

  glPopMatrix();
}

GLvoid move_cursor(tile** c, u_int d) {
  u_int r;

  if ((NULL == c) ||
      (NULL == *c))
    pexit(EFAULT, "draw_cursor: cursor at NULL");

  if ((ROBOT_MODE_PLACE == robot_mode) &&
      (NULL == (*c)->tile_p[d]))
    return;

  switch(cursor_lock) {
  case CURSOR_LOCK_ROBOT:
    if (ROBOT_MODE_PLACE == robot_mode) {
      *c = (*c)->tile_p[d];
    } else {
      *c = robot_trace(*c, cursor_lock_robot, d);
    }

    robots[cursor_lock_robot] = *c;
    compile_robots();

    break;
  default:
    if (ROBOT_MODE_TRACE == robot_mode) {
      r = robot_locate(*c, d);
      if (r != ROBOT_NONE) {
	*c = robots[r];
      }
    } else {
      *c = (*c)->tile_p[d];
    }
  }
}

tile* robot_trace(tile* t, u_int c, u_int d_original) {
  u_int d = d_original;
  u_int r;

  if (NULL == t)
    pexit(EFAULT, "robot_trace: got NULL start tile");

  //  printf("tracing robot[%u] from [%s] moving [%u]\n", c, coord_to_str(t->coord), d);

  r = get_robot_at(t->tile_p[d]);

  while ((t->tile_p[d] != NULL) &&
	 ((r == ROBOT_NONE) ||
	  (r == c))) {
    t = t->tile_p[d];
    if (t != NULL) {
      //      printf("tracing robot[%u] at [%s] moving [%u]\n", c, coord_to_str(t->coord), d);
    } else {
      //      printf("at null\n");
    }
    if ((t->flags & TILE_HAS_PRISM) &&
	(t->prism_color != c)) {
      //      printf("bouncing prism c[%u]d[%u] at tile [%s]f[%u]\n", t->prism_color, t->prism_dir, coord_to_str(t->coord), t->flags);
      d = PRISM_TRANS[t->prism_dir][d];
    }

    r = get_robot_at(t->tile_p[d]);
  }

  return t;
}

u_int robot_locate(tile* t, u_int d) {
  u_int i;
  u_int sign;
  tile* rt;
  int* c;
  int* cc;
  int* best_c = NULL;
  u_int best_i = ROBOT_NONE;

  u_int prime_d;
  u_int second_d;

  if (NULL == t)
    return ROBOT_NONE;

  cc = t->coord;

  switch(d) {
  case DIR_RIGHT:
    prime_d = 0;
    second_d = 1;
    sign = 1;
    break;
  case DIR_DOWN:
    prime_d = 1;
    second_d = 0;
    sign = 1;
    break;
  case DIR_LEFT:
    prime_d = 0;
    second_d = 1;
    sign = 0;
    break;
  case DIR_UP:
    prime_d = 1;
    second_d = 0;
    sign = 0;
    break;
  }

  for (i = 0; i < COLORS; i++) {
    rt = robots[i];
    if (NULL == rt) {
      continue;
    } else {
      c = rt->coord;
    }
    //    printf("cc[%s]\n", coord_to_str(cc));
    //    printf("checking r[%u] at [%s] against [%s]\n", i, coord_to_str(c), coord_to_str(best_c));

    if (((sign == 1) && (c[prime_d] > cc[prime_d])) ||
	((sign == 0) && (c[prime_d] < cc[prime_d]))) {
      //      printf("passed side");
      if (best_c == NULL) {
	//	printf("passed initial\n");
	best_c = c;
	best_i = i;
      } else if ((abs((int) cc[second_d] - (int) best_c[second_d])) * 2 +
		 (abs((int) cc[prime_d] - (int) best_c[prime_d])) >
		 (abs((int) cc[second_d] - (int) c[second_d])) * 2 +
		 (abs((int) cc[prime_d] - (int) c[prime_d]))) {
	//	printf("passed manhattan\n");
	best_c = c;
	best_i = i;
      } else {
	//	printf("failed manhattan\n");
      }
    } else {
      //      printf("failed side\n");
    }
  }

  return best_i;
}

GLvoid draw_cursor(tile** c) {
  if ((NULL == c) ||
      (NULL == *c))
    pexit(EFAULT, "draw_cursor: cursor at NULL");

  //  printf("drawing cursor at [%s]\n", coord_to_str((*c)->coord));

  glBindTexture(GL_TEXTURE_2D, 0);

  if (cursor_lock_robot != ROBOT_NONE) {
    robot_diffuse[cursor_lock_robot][3] = 0.5f;
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, robot_diffuse[cursor_lock_robot]);
    glMaterialfv(GL_FRONT,GL_EMISSION, robot_diffuse[cursor_lock_robot]);
    robot_diffuse[cursor_lock_robot][3] = 1.0f;
  } else {
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, cursor_bg);
    glMaterialfv(GL_FRONT,GL_EMISSION, cursor_bg);
  }

  glPushMatrix();
  
  glTranslatef((GLfloat) (*c)->coord[0], 0, (GLfloat) (*c)->coord[1]);

  glDisable(GL_DEPTH_TEST);

  glBegin(GL_QUADS);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
  glEnd();

  if (cursor_lock_robot != ROBOT_NONE) {
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, robot_diffuse[cursor_lock_robot]);
    glMaterialfv(GL_FRONT,GL_EMISSION, robot_diffuse[cursor_lock_robot]);
  } else {
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, cursor_fg);
    glMaterialfv(GL_FRONT,GL_EMISSION, cursor_fg);
  }

  glBegin(GL_LINE_LOOP);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
  glEnd();

  glEnable(GL_DEPTH_TEST);

  glPopMatrix();
}

GLvoid draw_tile_prism(u_int d, u_int c) {
  u_int j = 0;
  GLfloat t_w, t_h, t_l, x_p, y_p;

  //printf("drawing prism\n");

  //glColor4fv(prism_diffuse[c]);
  //glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
  //  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, prism_diffuse[c]);

  glBindTexture(GL_TEXTURE_2D, 0);
  glLineWidth(1.0f);
  //  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, prism_diffuse[c]);
  //  glMaterialfv(GL_FRONT, GL_EMISSION, prism_diffuse[c]);

  glPushMatrix();
    glRotatef(-45, 0.0f, 1.0f, 0.0f);

    if (d == PRISM_DIR_RIGHT)
      glRotatef(90, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
      glScalef(0.8f, 0.1f, 0.05f);
      glTranslatef(0.0f, 0.75f, 0.0f);

      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, prism_diffuse[c]);
      glMaterialfv(GL_FRONT, GL_EMISSION, prism_diffuse[c]);
      glutSolidCube(1.0f);

      //      glDisable(GL_DEPTH_TEST);

      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, robot_diffuse[c]);
      glMaterialfv(GL_FRONT, GL_EMISSION, robot_diffuse[c]);
      glutWireCube(1.0f);

      //      glEnable(GL_DEPTH_TEST);

    glPopMatrix();


    //    glMaterialfv(GL_FRONT, GL_EMISSION, prism_diffuse[c]);

    for (j = 0; j < 0; j++) {
      glPushMatrix();

      t_w = frandom() * 0.1f + 0.05;
      t_h = frandom() * 0.05f + 0.01;
      t_l = frandom() * 0.5f + 0.05;
      
      x_p = frandom() * 0.1 - 0.05;
      y_p = frandom() * (1 - t_l / 2) - (1 - t_l / 2) / 2;

      glScalef(t_l, t_w, t_h);

      glTranslatef(y_p,
		   1.0f,
		   x_p);

      glLineWidth(1.0f);

      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, prism_diffuse[c]);
      glMaterialfv(GL_FRONT, GL_EMISSION, prism_diffuse[c]);
      glutSolidCube(1.0f);

      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, robot_diffuse[c]);
      glMaterialfv(GL_FRONT, GL_EMISSION, robot_diffuse[c]);
      glutWireCube(1.0f);
      glPopMatrix();
    }

  glPopMatrix();
}

GLvoid draw_tile_mark(u_int t, u_int c, GLfloat alpha) {
  //  printf("drawing mark of color %d\n", c);

  mark_emit[3] = alpha;
  fvone[3] = alpha;

  glMaterialfv(GL_FRONT, GL_EMISSION, mark_emit);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fvone);

  fvone[3] = 1.0f;

  glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, textures[mark_texture[t][c]]);
    //    glBegin(GL_POLYGON);
    //      gl_circle_vertices_texture(0.26f);
    //    glEnd();
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3f(-0.4, 0.0, -0.4);
    glTexCoord2d(0.0, 1.0); glVertex3f(-0.4, 0.0,  0.4);
    glTexCoord2d(1.0, 1.0); glVertex3f( 0.4, 0.0,  0.4);
    glTexCoord2d(1.0, 0.0); glVertex3f( 0.4, 0.0, -0.4);
    glEnd();
  glPopMatrix();

  /*
  glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray_opaque);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, fvopaque);
    glBegin(GL_QUADS);
      gl_cylinder_vertices(0.3f, 0.025f);
    glEnd();
  glPopMatrix();
  */
}

GLvoid draw_robot(u_int c, GLfloat alpha) {
  robot_diffuse[c][3] = alpha;
  glBindTexture(GL_TEXTURE_2D, 0);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, robot_diffuse[c]);
  glMaterialfv(GL_FRONT, GL_EMISSION, fvzero);
  robot_diffuse[c][3] = 1.0f;

  glPushMatrix();

  glRotatef(-90.0f, 1.0, 0.0, 0.0f);

  glutSolidCone(0.2f, 0.7f, 20, 1); 
  glTranslatef(0.0f, 0.0f, 0.3f);
  glScalef(0.8f, 0.8f, 1.1f);
  glutSolidSphere(0.3f, 10, 10);
  glTranslatef(0.0f, 0.0f, 0.2f);
  glutSolidSphere(0.2f, 10, 10);

  glPopMatrix();
}

GLvoid draw_tile_wall(tile* t) {
  u_int i = 0;
  u_int j = 0;

  GLfloat t_w, t_h, t_l, x_p, y_p;

  //printf("draw_tile_wall(%d)\n", (u_int) t);

  if (t == NULL) pexit(EFAULT, "draw_tile_wall: got null tile");

  glBindTexture(GL_TEXTURE_2D, 0);

  glMaterialfv(GL_FRONT, GL_EMISSION, fvzero);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wall_diffuse);

  glPushMatrix();

  glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

  for (i = 0; i < DIRS; i++) {
    if (t->tile_p[i] == NULL) {
      glPushMatrix();
      glTranslatef(0.0f, 0.1f, -0.50f);
      glScalef(1.1f, 0.2f, 0.1f);
      glutSolidCube(1.0f);
      glPopMatrix();

      /*   for (j = 0; j < 10; j++) {
	glPushMatrix();

	t_w = frandom() * 0.1f + 0.04;
	t_h = frandom() * 0.1f + 0.04;
	t_l = frandom() * 0.5f + 0.04;

	x_p = frandom() * 0.1 - 0.05;
	y_p = frandom() * (1 - t_l / 2) - (1 - t_l / 2) / 2;

	glTranslatef(y_p,
		     0.0f,
		     -0.50+x_p);

	glScalef(t_l, t_w, t_h);

	glutSolidCube(1.0f);
	glPopMatrix();
	}*/
    }
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
  }

  glPopMatrix();

  return;
}

GLvoid draw_tile(tile* t, u_int d, u_int draw, GLfloat alpha) {
  u_int i;
  //printf("draw_tile(%d)\n", (u_int) t);

  if (t == NULL)
    return;

  if (t->flags & TILE_DRAWN)
    return;

  t->flags |= TILE_DRAWN;

  glPushMatrix();

  glTranslatef((GLfloat) t->coord[0],
	       0,
	       (GLfloat) t->coord[1]);

  if (draw & DRAW_WALL)
    draw_tile_wall(t);

  if ((t->flags & TILE_HAS_PRISM) &&
      (draw & DRAW_PRISM))
    draw_tile_prism(t->prism_dir, t->prism_color);
    
  if (draw & DRAW_INFO) {
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fvone);
    glMaterialfv(GL_FRONT, GL_EMISSION, fvone);
    gl_print(0.0f, 0.0f, 0.0f, tile_label(t));
  }

  if ((t->flags & TILE_HAS_MARK) &&
      (draw & DRAW_MARK))
    draw_tile_mark(t->mark_type, t->mark_color, alpha);

  if (draw & DRAW_TILE) {

    tile_diffuse[3] = alpha;
    tile_emit[3] = alpha;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tile_diffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, tile_emit);

    glPushMatrix();
    //glTranslatef(0.0f, 0.025f, 0.0f);
    //  glScalef(1.0f, 0.05f, 1.0f);
    //glutSolidCube(1.0f);
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_TILE]);

    //    glActiveTextureARB( GL_TEXTURE1_ARB );
    //    glClientActiveTextureARB( GL_TEXTURE1_ARB );
    //    glEnable( GL_TEXTURE_2D );

    //Set up the blending of the 2 textures
    /*glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_EXT);
    glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_INTERPOLATE_EXT);
    glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
    glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_EXT,GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB_EXT,GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND2_RGB_EXT,GL_SRC_ALPHA);*/

    //Texture 0 on TU 1
    //glBindTexture( GL_TEXTURE_2D, textures[TEXTURE_MARK_MOON_RED]);

    //Switch off the TU 1
    //    glDisable( GL_TEXTURE_2D );
    //    glActiveTextureARB( GL_TEXTURE0_ARB );
    //    glClientActiveTextureARB( GL_TEXTURE0_ARB );

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex3f(-0.5, 0.0, -0.5);
    glTexCoord2d(0.0, 1.0); glVertex3f(-0.5, 0.0,  0.5);
    glTexCoord2d(1.0, 1.0); glVertex3f( 0.5, 0.0,  0.5);
    glTexCoord2d(1.0, 0.0); glVertex3f( 0.5, 0.0, -0.5);
    glEnd();
    glPopMatrix();

    /*
    glLineWidth(2.0f);
    glPushMatrix();
      glTranslatef(0, 0.06, 0);
      glScalef(0.5f, 1, 0.5f);
      glBegin(GL_LINE_LOOP);
        glVertex3f(-1, 0, -1);
	glVertex3f(1, 0, -1);
	glVertex3f(1, 0, 1);
	glVertex3f(-1, 0, 1);
      glEnd();
    glPopMatrix();
    */

  }

  glPopMatrix();

  for (i = 0; i < DIRS; i++)
    draw_tile(t->tile_p[i], i, draw, alpha);
}

GLvoid custom_keyboard(unsigned char key, int x, int y) {
  uint i;
  tile** new_robots;

  switch(key) {
  case 'i':
    show_coord = ! show_coord;
    break;
  case ' ':
    toggle_lock_cursor(cursor);
    break;
  case '\t':
    if (robot_mode == ROBOT_MODE_PLACE)
      robot_mode = ROBOT_MODE_TRACE;
    else
      robot_mode = ROBOT_MODE_PLACE;
    break;
  case 13: // carriage return
    for (i = 0; i < COLORS; i++) {
      place_robot(i, robots[i]);
    }
    compile_robots();
    break;
  case 27: // escape
    if (cursor_lock == CURSOR_LOCK_ROBOT) {
      cursor_lock = CURSOR_LOCK_NONE;
      cursor = robots_v[cursor_lock_robot];
      cursor_lock_robot = ROBOT_NONE;
    } else {
      for (i = 0; i < COLORS; i++) {
	robots[i] = robots_v[i]; //robots_v = backup
      }
    }
    compile_robots();
    break;
  case 's': // solve next move
    printf("s pressed\n");
    for (i = 0; i < COLORS; i++) {
      printf("robot[%x]: active[%s] back[%s]\n", i, tile_label(robots[i]), tile_label(robots_v[i]));
    }
    if (cursor_lock == CURSOR_LOCK_ROBOT) {
      printf("solving for color[%x], tile[%s]\n", cursor_lock_robot, tile_label(cursor));
      new_robots = go_move(robots_v, cursor_lock_robot, cursor);
      if (NULL == new_robots) {
	printf("was over\n");
	return;
      }
      for (i = 0; i < COLORS; i++) {
	printf("robot[%x] old[%s] new[%s]\n", i, tile_label(robots_v[i]), tile_label(new_robots[i]));
	robots[i] = new_robots[i];
      }
      compile_robots();
    } else {
      printf("lock onto a robot first\n");
    }
   break;
  case '>':
    abort_secs *= 2;
    printf("timeout [%d]\n", abort_secs);
    break;
  case '<':
    abort_secs /= 2;
    printf("timeout [%d]\n", abort_secs);
    break;
  }
   
}

GLvoid custom_special(int key, int x, int y) {
  switch(key) {
  case GLUT_KEY_LEFT:
    move_cursor(&cursor, DIR_LEFT);
    break;
  case GLUT_KEY_UP:
    move_cursor(&cursor, DIR_UP);
    break;
  case GLUT_KEY_RIGHT:
    move_cursor(&cursor, DIR_RIGHT);
    break;
  case GLUT_KEY_DOWN:
    move_cursor(&cursor, DIR_DOWN);
    break;
  }
}
