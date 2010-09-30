#ifndef __cell_c__
#define __cell_c__

#include "cell.h"

u_int cell_sim_paused = 0;

GLuint cell_gl_texture;

GLfloat cell_gl_light_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat cell_gl_light_diffuse[]  = { DIFFUSE_MAG, DIFFUSE_MAG, DIFFUSE_MAG, 1.0f };
GLfloat cell_gl_light_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat cell_gl_light_position[] = { 10.0f, 100.0f, 10.0f, 1.0f };
GLfloat cell_gl_light_phase = 0;

GLvoid init_scene(GLvoid) {
  u_int y,x;
  float yf,xf, ww, wh, wwm, whm;

  ww = WW;
  wh = WH;

  wwm = ww/2;
  whm = wh/2;

  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      ws[x][y].pos = v3_new((float) x - WW / 2, (float) y - WW / 2, 0);
      ws[x][y].color[0] = 0.0f;
      ws[x][y].color[1] = 0.0f;
      ws[x][y].color[2] = 0.0f;
      ws[x][y].color[3] = 1.0f;

      #ifdef USE_NORMALS
      ws[x][y].norm = v3_new(0,0,0);
      ws[x][y].norm1 = v3_new(0,0,0);
      ws[x][y].norm2 = v3_new(0,0,0);
      #endif

      ws[x][y].x = x;
      ws[x][y].y = y;

      yf = y;
      xf = x;

      ws[x][y].height = 0.0f;
    }
  }

  custom_init_cells();
}

GLvoid update_light() {
  cell_gl_light_phase += 0.05;
  cell_gl_light_position[0] = sin(cell_gl_light_phase) * WW / 1.5;
  cell_gl_light_position[1] = cos(cell_gl_light_phase) * WH / 1.5;
  cell_gl_light_position[2] = 10.0f;

  glLightfv(GL_LIGHT0, GL_POSITION, cell_gl_light_position);

  glPushMatrix();

  glTranslatef(cell_gl_light_position[0],
	       cell_gl_light_position[1],
	       cell_gl_light_position[2]);
  glutSolidSphere(1.0f, 10, 10);

  glPopMatrix();
}

#ifdef USE_NORMALS
GLvoid compute_normals() {
  u_int x,y;
  cell *w00, *w01, *w10, *w11;

  v3 *norm1, *norm2;
  norm1 = v3_new(0,0,0);
  norm2 = v3_new(0,0,0);

  for (x = 0; x < WW; x++) {
    for (y = 0; y < WH; y++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][(y+1) % WH]);
      w10 = & (ws[(x+1) % WW][y]);
      w11 = & (ws[(x+1) % WW][(y+1) % WH]);

      v3_copy_from(norm1, w10->pos);
      v3_sub(norm1, w11->pos);
      v3_copy_from(norm2, w11->pos);
      v3_sub(norm2, w00->pos);

      v3_cross(norm1, norm2);
      v3_unit(norm1);

      v3_copy_from(w00->norm1, norm1);

      v3_copy_from(norm1, w00->pos);
      v3_sub(norm1, w11->pos);
      v3_copy_from(norm2, w11->pos);
      v3_sub(norm2, w01->pos);

      v3_cross(norm1, norm2);
      v3_unit(norm1);

      v3_copy_from(w00->norm2, norm1);
    }
  }

  v3* n;

  for (x = 0; x < WW; x++) {
    for (y = 0; y < WH; y++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][(y-1+WH) % WH]);
      w10 = & (ws[(x-1+WW) % WW][y]);
      w11 = & (ws[(x-1+WW) % WW][(y-1+WH) % WH]);

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

  v3_free(norm1);
  v3_free(norm2);
}
#endif

timer* bench;

GLvoid draw_space() {
#ifndef USE_CANVAS
  int y,x;
  cell *w00, *w01, *w10, *w11;
#endif

#ifdef USE_NORMALS
  v3 *norm1, *norm2;
  compute_normals();

  norm1 = v3_new(0,0,0);
  norm2 = v3_new(0,0,0);
#endif

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  float s = 5.0f / (float) WW;

#ifdef TILE

  glNewList(dlist1, GL_COMPILE);

#else 

  glPushMatrix();

  glScalef(s,s,s);

  //  update_light();

#endif

#ifdef USE_CANVAS
  glPushAttrib(GL_TEXTURE_BIT);

  // build our texture mipmaps
  glBindTexture(GL_TEXTURE_2D, cell_gl_texture);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, WW, WH,
  		    GL_RGB, GL_UNSIGNED_BYTE, canvas);


  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); gl_vertex_v3(ws[0][0].pos);
  glTexCoord2d(1.0, 0.0); gl_vertex_v3(ws[WW-1][0].pos);
  glTexCoord2d(1.0, 1.0); gl_vertex_v3(ws[WW-1][WH-1].pos);
  glTexCoord2d(0.0, 1.0); gl_vertex_v3(ws[0][WH-1].pos);
  glEnd();

  glPopAttrib();
#else
  for (y = 0; y < WH-1; y++) {
    for (x = 0; x < WW-1; x++) {
      w00 = & (ws[x][y]);
      w01 = & (ws[x][y+1]);
      w10 = & (ws[x+1][y]);
      w11 = & (ws[x+1][y+1]);

      glBegin(GL_TRIANGLES);

#ifdef USE_NORMALS
      gl_normal_v3(w10->norm); COLOR_ME(w10); gl_vertex_v3(w10->pos);
      gl_normal_v3(w11->norm); COLOR_ME(w11); gl_vertex_v3(w11->pos);
      gl_normal_v3(w00->norm); COLOR_ME(w00); gl_vertex_v3(w00->pos);

      gl_normal_v3(w00->norm); COLOR_ME(w00); gl_vertex_v3(w00->pos);
      gl_normal_v3(w11->norm); COLOR_ME(w11); gl_vertex_v3(w11->pos);
      gl_normal_v3(w01->norm); COLOR_ME(w01); gl_vertex_v3(w01->pos);
#else

      COLOR_ME(w10); gl_vertex_v3(w10->pos);
      COLOR_ME(w11); gl_vertex_v3(w11->pos);
      COLOR_ME(w00); gl_vertex_v3(w00->pos);

      COLOR_ME(w00); gl_vertex_v3(w00->pos);
      COLOR_ME(w11); gl_vertex_v3(w11->pos);
      COLOR_ME(w01); gl_vertex_v3(w01->pos);

#endif

      glEnd();

#ifdef DRAW_NORMALS
      v3_copy_from(norm1, w10->norm);
      v3_add(norm1, w10->pos);

      glColor4f(1.0,1,1,1);

      glBegin(GL_LINES);
      gl_vertex_v3(w10->pos);
      gl_vertex_v3(norm1);
      glEnd();
#endif

    }
  }

#endif

#ifdef TILE

  glEndList();

#endif

#ifdef USE_NORMALS
  v3_free(norm1);
  v3_free(norm2);
#endif

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
#endif
  
  glPopMatrix();
}


GLvoid sample_world() {
  #ifdef USE_HEIGHT
  cell_ptr w;
  u_int x,y;
  #endif

  if (cell_sim_paused) {
    return;
  }

  custom_sample();
  custom_update();

  //  printf("mark: %d (ms)\n", timer_mark_ms(bench));

  timer_start(bench);

  #ifdef USE_HEIGHT
  for (y = 0; y < WH; y++) {
    for (x = 0; x < WW; x++) {
      w = & (ws[x][y]);
      w->pos->v[2] = w->height;
    }
  }
  #endif
}

static void gl_scale(AG_Event *event) {
  GLdouble xMin, xMax, yMin, yMax;
	
  glLoadIdentity();

  GLdouble aspect = (GLdouble) AGWIDGET(cell_ui_glv)->w / (GLdouble) AGWIDGET(cell_ui_glv)->h;

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
	
  glPushAttrib(GL_POLYGON_BIT |
	       GL_LIGHTING_BIT |
	       GL_COLOR_BUFFER_BIT |
	       GL_DEPTH_BUFFER_BIT |
	       GL_TEXTURE_BIT |
	       GL_ENABLE_BIT
	       );

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
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, cell_gl_light_ambient);
  //glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, cell_gl_light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, cell_gl_light_specular);

  // texturing tutorial: http://www.nullterminator.net/gltexture.html
  // select modulate to mix texture with color for shading
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  // when texture area is small, bilinear filter the closest mipmap
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
  		  GL_LINEAR_MIPMAP_NEAREST);
  // when texture area is large, bilinear filter the original
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // the texture wraps over at the edges (repeat)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  sample_world();

  glPushMatrix(); glTranslatef(0.0f, 0.5f, -5.0f);

  glRotatef(45, -1, 0, 0);

  //glLineWidth(1.0f);
  //glColor3f(0.0f, 0.0f, 0.0f);

  draw_space();

  glPopMatrix();

  glPopAttrib();
}

static void cell_ui_toggle_pause(AG_Event* event) {
  cell_sim_paused = ! cell_sim_paused;
  AG_ButtonText(cell_ui_pause_button, (cell_sim_paused ? "unpause" : "pause"));
}

static void cell_ui_toggle_reset(AG_Event* event) {   
  CELL_FOREACH(c, {
      custom_reset_cell(c);
    });
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

  win = AG_WindowNew(AG_WINDOW_PLAIN);

  pane = AG_PaneNew(win, AG_PANE_HORIZ, AG_PANE_EXPAND);
  cell_ui_div_controls = pane->div[0];
  cell_ui_div_view = pane->div[1];

  AG_LabelNew(cell_ui_div_controls, 0, "Controls");

  //AG_Numerical *num_rate;
  //AG_Numerical *num_scale;

  /*
  num_rate = AG_NumericalNew(cell_ui_div_controls, AG_NUMERICAL_HFILL, NULL, "rate: ");
  AG_WidgetBindFloat(num_rate, "value", &rate_1);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.05);

  num_rate = AG_NumericalNew(cell_ui_div_controls, AG_NUMERICAL_HFILL, NULL, "rate2: ");
  AG_WidgetBindFloat(num_rate, "value", &rate_2);
  AG_NumericalSetPrecision(num_rate, "f", 4);
  AG_NumericalSetRangeFlt(num_rate, 0.0, 1.0);
  AG_NumericalSetIncrement(num_rate, 0.05);
  */

  cell_ui_pause_button = AG_ButtonNewFn(cell_ui_div_controls, 0, "pause", cell_ui_toggle_pause, "%s", "");
  cell_ui_reset_button = AG_ButtonNewFn(cell_ui_div_controls, 0, "reset", cell_ui_toggle_reset, "%s", "");

  custom_init_gui();

  cell_ui_glv = AG_GLViewNew(cell_ui_div_view, AG_GLVIEW_EXPAND);

  AG_GLViewScaleFn(cell_ui_glv, gl_scale, NULL);
  AG_GLViewDrawFn(cell_ui_glv, gl_draw, NULL);

  AG_WindowShow(win);
  AG_WindowMaximize(win);

  dlist1 = glGenLists(1);
  glGenTextures(1, &cell_gl_texture);

  canvas = (char*) malloc(WW * WH * 3);
  assert(NULL != canvas);

  bench = timer_new();
  timer_start(bench);

  AG_EventLoop();
  AG_Destroy();

  return (0);
}

 void cell_add_button(char* label, cell_button_handler handler) {
   AG_ButtonNewFn(cell_ui_div_controls, 0, label, handler, "%s", "");
 }

 void cell_add_float_input(char* label, float min, float max, float inc, int prec, float* storer) {
   AG_Numerical *num_rate;
   num_rate = AG_NumericalNew(cell_ui_div_controls, AG_NUMERICAL_HFILL, NULL, label);
   AG_WidgetBindFloat(num_rate, "value", storer);
   AG_NumericalSetPrecision(num_rate, "f", prec);
   AG_NumericalSetRangeFlt(num_rate, min, max);
   AG_NumericalSetIncrement(num_rate, inc);
 }

#endif /* __cell_c__ */
