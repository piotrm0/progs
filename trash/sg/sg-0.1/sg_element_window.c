#ifndef __sg_element_window_c__
#define __sg_element_window_c__

#include "sg.h"

/* sg_window */

sg_window* sg_window_new(sg_root* root) {
  IF_DEBUG(assert(NULL != root););

  _SG_ELEMENT_INIT(ret, root, window, WINDOW, 2);

  sg_window_head* head;
  sg_pane* pane;

  head = sg_window_head_new(root);
  head->parent = (sg_element*) ret;

  pane = sg_pane_new(root);
  pane->parent = (sg_element*) ret;

  ret->subs[SG_WINDOW_SUB_HEAD] = (sg_element*) head;
  ret->subs[SG_WINDOW_SUB_PANE] = (sg_element*) pane;

  llist_push(ret->children, (void*) head);
  llist_push(ret->children, (void*) pane);

  return ret;
}

void sg_window_del(sg_window* e) {
  IF_DEBUG(assert(NULL != e););

  _SG_ELEMENT_DEL(e);

  return;
}

void sg_window_render(sg_window* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  _SG_SUBVIEW_DEFINE(e, view, view_sub);

  sg_window_draw(e, view);

  _SG_RENDER_CHILDREN(e, view_sub);

  //  sg_window_head_render((sg_window_head*) e->subs[SG_WINDOW_SUB_HEAD], view_sub);
  //  sg_pane_render(       (sg_pane*)        e->subs[SG_WINDOW_SUB_PANE], view_sub);

  recti_del(view_sub);

  return;
}

void sg_window_draw(sg_window* w, recti* view) {
  IF_DEBUG(assert(NULL != w);
	   assert(NULL != view););
    
  _SG_BOUNDS_DEFINE(w, view, left, top, right, bottom);

  glBegin(GL_QUADS);
    glColor4f(1.0f, 0.0, 0.0, 0.5f);
    _SG_BOUNDS_VERTICES(left, top, right, bottom)
  glEnd();

  if (w->flags & SG_FLAG_OVER) {
    glColor4f(1.0f, 1.0, 1.0, 1.0f);
  } else {
    glColor4f(1.0f, 0.0, 0.0, 1.0f);
  }

  glLineWidth(1.0f);
  glBegin(GL_LINE_LOOP);
    _SG_BOUNDS_VERTICES_LINEFIX(left, top, right, bottom);
  glEnd();

  return;
}

void sg_window_set_geo(sg_window *e, u_int x, u_int y, u_int w, u_int h) {
  IF_DEBUG(assert(NULL != e););

  sg_element_set_geo((sg_element*) e, x,y,w,h);
  sg_window_head_set_geo((sg_window_head*) e->subs[SG_WINDOW_SUB_HEAD], 0, 0, w, HEAD_HEIGHT);
  sg_pane_set_geo       ((sg_pane*)        e->subs[SG_WINDOW_SUB_PANE], 0, HEAD_HEIGHT, w, h - HEAD_HEIGHT);

  return;
}

sg_window_head* sg_window_head_new(sg_root* root) {
  IF_DEBUG(assert(NULL != root););

  _SG_ELEMENT_INIT(ret, root, window_head, WINDOW_HEAD, 4);

  sg_label* label;
  label = sg_label_new(ret->root);

  sg_button *button_min, *button_max, *button_close;
  button_min   = sg_button_new(root);
  button_max   = sg_button_new(root);
  button_close = sg_button_new(root);

  ret->subs[SG_WINDOW_HEAD_SUB_LABEL]    = (sg_element*) label;
  ret->subs[SG_WINDOW_HEAD_SUB_MINIMIZE] = (sg_element*) button_min;
  ret->subs[SG_WINDOW_HEAD_SUB_MAXIMIZE] = (sg_element*) button_max;
  ret->subs[SG_WINDOW_HEAD_SUB_CLOSE]    = (sg_element*) button_close;
  llist_push(ret->children, (void*) label);
  llist_push(ret->children, (void*) button_min);
  llist_push(ret->children, (void*) button_max);
  llist_push(ret->children, (void*) button_close);

  return ret;
}

void sg_window_head_render(sg_window_head* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  _SG_SUBVIEW_DEFINE(e, view, view_sub);

  sg_window_head_draw(e, view);

  _SG_RENDER_CHILDREN(e, view_sub);

  //  sg_label_render((sg_label*) e->subs[SG_WINDOW_HEAD_SUB_LABEL], view_sub);
  recti_del(view_sub);

  return;
}

void sg_window_head_draw(sg_window_head* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  _SG_BOUNDS_DEFINE(e, view, left, top, right, bottom);
  
  sg_element_scissor((sg_element*) e, view);
  glEnable(GL_SCISSOR_TEST);

  glBegin(GL_QUADS);
    glColor4f(1.0f, 0.0, 0.0, 0.5f);
    _SG_BOUNDS_VERTICES(left, top, right, bottom);
  glEnd();

  glLineWidth(1.0f);
  if (e->flags & SG_FLAG_OVER) {
    glColor4f(1.0f, 1.0, 1.0, 1.0f);
  } else {
    glColor4f(1.0f, 0.0, 0.0, 1.0f);
  }

  glBegin(GL_LINE_LOOP);
    _SG_BOUNDS_VERTICES_LINEFIX(left, top, right, bottom);
  glEnd();

  glDisable(GL_SCISSOR_TEST);

  return;
}

void sg_window_head_set_geo(sg_window_head* e, u_int x, u_int y, u_int w, u_int h) {
  IF_DEBUG(assert(NULL != e););

  sg_element_set_geo((sg_element*) e, x,y,w,h);
  sg_element_set_geo(e->subs[SG_WINDOW_HEAD_SUB_LABEL], 20, 0, w - 80, 20);
  sg_element_set_geo(e->subs[SG_WINDOW_HEAD_SUB_MINIMIZE], w-60, 0, 20, 20);
  sg_element_set_geo(e->subs[SG_WINDOW_HEAD_SUB_MAXIMIZE], w-40, 0, 20, 20);
  sg_element_set_geo(e->subs[SG_WINDOW_HEAD_SUB_CLOSE],    w-20, 0, 20, 20);

  return;
}

/* end of sg_window */

#endif /* __sg_element_window_c__ */
