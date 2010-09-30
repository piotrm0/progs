#ifndef __sg_element_button_c__
#define __sg_element_button_c__

#include "sg.h"

/* sg_button */

sg_button* sg_button_new(sg_root* root) {
  IF_DEBUG(assert(NULL != root););

  _SG_ELEMENT_INIT(ret, root, button, BUTTON, 0);

  return ret;
}

void sg_button_render(sg_button* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  _SG_SUBVIEW_DEFINE(e, view, view_sub);

  sg_button_draw(e, view);

  recti_del(view_sub);  

  return;
}

void sg_button_draw(sg_button* e, recti* view) {
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

void sg_button_set_geo(sg_button* e, u_int x, u_int y, u_int w, u_int h) {
  IF_DEBUG(assert(NULL != e););

  sg_element_set_geo((sg_element*) e, x,y,w,h);

  return;
}

/* end of sg_button */

#endif /* __sg_element_button_c__ */
