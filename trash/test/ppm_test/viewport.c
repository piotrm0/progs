#include "viewport.h"
#include "common.h"

GLfloat default_bg[4] = {1.0f, 1.0f, 1.0f, 0.5f};
GLfloat default_fg[4] = {1.0f, 1.0f, 1.0f, 1.0f};

view* view_new(u_int subviews, u_int flags, u_int clear_flags, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
  view* v = (view*) malloc(sizeof(view));

  v->flags = flags;
  v->flags_gl_clear = clear_flags;

  v->draw_func = NULL;

  v->num_subviews = subviews;
  v->subviews = (view**) calloc(v->num_subviews, sizeof(view*));

  v->aspect = (y2 - y1) / (x2 - x1);

  v->alpha = 1.0f;

  v->x1 = x1;
  v->y1 = y1;
  v->x2 = x2;
  v->y2 = y2;

  v->color_bg = default_bg;
  v->color_fg = default_fg;

  return v;
}

void view_set_bounds(view* v, GLint x1, GLint y1, GLint x2, GLint y2) {
  if (NULL == v)
    pexit(EFAULT, "view_set_bounds: got null view");

  /*  v->x = x1;
  v->y = y1;
  v->width  = (x2-x1);
  v->height = (y2-y1);
  v->x1 = x1;
  v->x2 = x2;
  v->y1 = y1;
  v->y2 = y2;*/
}

void view_gl_setup(view* v, GLint x, GLint y, GLint width, GLint height) {
  if (NULL == v)
    pexit(EFAULT, "view_gl_setup: got null view");

  GLint xr, yr, widthr, heightr;

  xr = x;
  yr = y;
  widthr  = (GLfloat) width;
  heightr = (GLfloat) height;
  
  //  printf("view (got[%d,%d]+%d,%d]): [%d,%d]+[%d,%d]\n", x,y,width,height, xr, yr, widthr, heightr);

  glViewport(xr, yr, widthr, heightr);

  if (v->flags & VIEW_CROP) {
    glScissor (xr, yr, widthr, heightr);
    glEnable(GL_SCISSOR_TEST);
  } else {
    glDisable(GL_SCISSOR_TEST);
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);

  if (v->flags & VIEW_BG) {
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, v->color_bg);
    glRecti(x,y,x+width,y+height);
  }
  if (v->flags & VIEW_FG) {
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, v->color_fg);
    glMaterialfv(GL_FRONT, GL_EMISSION, v->color_fg);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x,      y);
    glVertex2i(x+width-1,y);
    glVertex2i(x+width-1,y+height-1);
    glVertex2i(x,      y+height-1);
    glEnd();
  }

  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f,	(GLfloat) width / (GLfloat) height,0.1f,100.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void view_draw(view* v) {
  if (NULL == v)
    pexit(EFAULT, "view_draw: got null view");

  view_draw_sub(v, 0, 0, w_width, w_height);
}

void view_draw_pre_sub(view* v, GLint x, GLint y, GLint width, GLint height) {
  if (NULL == v)
    pexit(EFAULT, "view_draw_pre_sub: got null view");

  GLint xr, yr, widthr, heightr;

  xr = x + (GLint) (v->x1 * (GLfloat) width);
  yr = y + (GLint) (v->y1 * (GLfloat) height);
  widthr = (GLint) ((GLfloat) width * (v->x2 - v->x1));

  if (v->flags & VIEW_ASPECT) {
    heightr = ((GLfloat) widthr) * v->aspect;
  } else {
    heightr = (GLint) ((GLfloat) height * (v->y2 - v->y1));
  }

  view_draw_sub(v, xr, yr, widthr, heightr);
}

void view_draw_sub(view* v, GLint x, GLint y, GLint width, GLint height) {
  u_int i;

  if (NULL == v)
    pexit(EFAULT, "view_draw_sub: got null view");

  //  printf("drawing in [%d, %d]+[%d, %d]\n", x,y, width, height);

  view_gl_setup(v, x, y, width, height);

  if (NULL == v->draw_func)
    pexit(EFAULT, "view_draw_sub: no draw function registered");

  if (v->alpha != 1.0f) {
    glDrawBuffer(GL_LEFT);
  } else {
    glDrawBuffer(GL_BACK);
  }

  glClear(v->flags_gl_clear);

  v->draw_func();

  if (v->alpha != 1.0f) {
    glPixelTransferf(GL_ALPHA_SCALE, v->alpha);
    glReadBuffer(GL_LEFT);
    glDrawBuffer(GL_BACK);
    glCopyPixels(x, y, width, height, GL_COLOR);
  }

  for (i = 0; i < v->num_subviews; i++)
    view_draw_pre_sub(v->subviews[i], x, y, width, height);

}
