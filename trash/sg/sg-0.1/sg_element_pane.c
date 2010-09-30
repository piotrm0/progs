#ifndef __sg_element_pane_c__
#define __sg_element_pane_c__

#include "sg.h"

/* sg_pane */

sg_pane* sg_pane_new(sg_root* root) {
  IF_DEBUG(assert(NULL != root););

  _SG_ELEMENT_INIT(ret, root, pane, PANE, 3);
  ret->options = SG_PANE_OPTION_NONE;

  sg_view* view;

  view = sg_view_new(root);

  ret->subs[SG_PANE_SUB_VIEW] = (sg_element*) view;

  llist_push(ret->children, (void*) view);

  return ret;
}

void sg_pane_render(sg_pane* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  recti* view_sub;

  sg_pane_draw(e, view);

  view_sub = recti_intersect_rel(view, e->geo);
  sg_view_render((sg_view*) e->subs[SG_PANE_SUB_VIEW], view_sub);
  recti_del(view_sub);  

  return;
}

void sg_pane_draw(sg_pane* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  _SG_BOUNDS_DEFINE(e, view, left, top, right, bottom);
  _SG_SUBVIEW_DEFINE(e, view, view_sub);

  glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0, 1.0, 0.5f);
    _SG_BOUNDS_VERTICES(left, top, right, bottom);
  glEnd();

  glEnable(GL_SCISSOR_TEST);  
  sg_element_scissor((sg_element*) e, view);

  // draw here

  glDisable(GL_SCISSOR_TEST);

  recti_del(view_sub);

  glLineWidth(1.0f);
  if (e->flags & SG_FLAG_OVER) {
    glColor4f(1.0f, 1.0, 1.0, 1.0f);
  } else {
    glColor4f(1.0f, 0.0, 0.0, 1.0f);
  }

  glBegin(GL_LINE_LOOP);
    _SG_BOUNDS_VERTICES_LINEFIX(left, top, right, bottom);
  glEnd();

  return;
}

void sg_pane_set_geo(sg_pane *e, u_int x, u_int y, u_int w, u_int h) {
  IF_DEBUG(assert(NULL != e););

  sg_element_set_geo((sg_element*) e, x,y,w,h);

  sg_view_set_geo ((sg_view*) e->subs[SG_PANE_SUB_VIEW], 0, 0, w - SCROLL_SIZE, h - SCROLL_SIZE);

  return;
}

/* end of sg_pane */

/* sg_view */

sg_view* sg_view_new(sg_root* root) {
  IF_DEBUG(assert(NULL != root););

  _SG_ELEMENT_INIT(ret, root, view, VIEW, 0);

  ret->vgeo = recti_new(0,0,0,0);

  return ret;
}

void sg_view_del(sg_view* e) {
  IF_DEBUG(assert(NULL != e););

  _SG_ELEMENT_DEL(e);

  recti_del(e->vgeo);

  return;
}

void sg_view_render(sg_view* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  recti* view_sub;

  sg_view_draw(e, view);

  view_sub = recti_intersect_rel(view, e->geo);
  _SG_RENDER_CHILDREN(e, view_sub);
  recti_del(view_sub);

  return;
}

void sg_view_draw(sg_view* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););
    
  _SG_BOUNDS_DEFINE(e, view, left, top, right, bottom);

  glBegin(GL_QUADS);
    glColor4f(0.0f, 1.0, 0.0, 0.5f);
    _SG_BOUNDS_VERTICES(left, top, right, bottom)
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

  return;
}

void sg_view_set_geo(sg_view* e, u_int x, u_int y, u_int w, u_int h) {
  IF_DEBUG(assert(NULL != e););

  sg_element_set_geo((sg_element*) e, x,y,w,h);

  return;
}

/* end of sg_view */

#endif /* __sg_element_pane_c__ */
