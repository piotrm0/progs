#ifndef __sg_element_root_c__
#define __sg_element_root_c__

#include "sg.h"

/* sg_root */

sg_root* _sg_root_new() {
  u_int flags;
  SDL_Surface* screen;

  flags = SDL_OPENGL;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return NULL;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
 
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, flags);
  if (!screen) {
    fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
    return NULL;
  }

  _SG_ELEMENT_INIT(ret, ret, root, ROOT, 0);

  ret->geo = recti_new(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);

  ret->screen = screen;
  ret->mouse  = _sg_mouse_new();
  ret->timing = sg_timing_new();
  ret->font   = sg_font_load(ret->root, "fonts/zeimusu0.ttf", 20);

  return ret;
}

void sg_root_add_window(sg_root* root, sg_window* w) {
  assert(NULL != root);
  assert(NULL != w);

  llist_push(root->children, w);

  w->parent = (sg_element*) root;

  return;
}

void sg_root_draw(sg_root* r, recti* view) {
  assert(NULL != r);
  assert(NULL != view);

  _SG_BOUNDS_DEFINE(r, view, left, top, right, bottom);

  glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 1.0f, 0.25f);
    _SG_BOUNDS_VERTICES(left, top, right, bottom);
  glEnd();
}

void sg_root_draw_mouse(sg_root* r) {
  assert(NULL != r);

  glBegin(GL_LINES);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glVertex2i(r->mouse->pos->v[0], r->mouse->pos->v[1]-10);
  glVertex2i(r->mouse->pos->v[0], r->mouse->pos->v[1]+10);
  glVertex2i(r->mouse->pos->v[0]-10, r->mouse->pos->v[1]);
  glVertex2i(r->mouse->pos->v[0]+10, r->mouse->pos->v[1]);

  glEnd();

  return;
}

void sg_root_render(sg_root* root, recti* view) {
  //  sg_image_draw(temp, (vi2*) &vi2_0);

  sg_root_draw(root, view);

  _SG_RENDER_CHILDREN(root, view);

  sg_root_draw_mouse(root);

  return;
}

void sg_root_over(sg_root* root, recti* view, vi2* point) {
  root->mouse->over = NULL;
  sg_element_over((sg_element*) root, view, point);

  return;
}

void sg_root_set_geo(sg_root* e, u_int x, u_int y, u_int w, u_int h) {
  sg_element_set_geo((sg_element*) e, x,y,w,h);
}

/* end of sg_root */

#endif /* __sg_element_root_c__ */

