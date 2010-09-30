#ifndef __sg_element_label_c__
#define __sg_element_label_c__

#include "sg.h"

/* sg_label */

sg_label* sg_label_new(sg_root* root) {
  IF_DEBUG(assert(NULL != root););

  _SG_ELEMENT_INIT(ret, root, label, LABEL, 0);

  ret->text   = NULL;
  ret->string = NULL;

  return ret;
}

void sg_label_render(sg_label* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  sg_label_draw(e, view);

  return;
}

void sg_label_draw(sg_label* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view););

  _SG_BOUNDS_DEFINE(e, view, left, top, right, bottom);
  _SG_SUBVIEW_DEFINE(e, view, view_sub);

  glBegin(GL_QUADS);
  glColor4f(1.0f, 0.0, 0.0, 0.5f);
  _SG_BOUNDS_VERTICES(left, top, right, bottom);
  glEnd();

  glEnable(GL_SCISSOR_TEST);  
  sg_element_scissor((sg_element*) e, view);
  sg_type_draw(e->text, view_sub);
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

void sg_label_set_geo(sg_label *e, u_int x, u_int y, u_int w, u_int h) {
  IF_DEBUG(assert(NULL != e););

  sg_element_set_geo((sg_element*) e, x,y,w,h);
}

void sg_label_set_string(sg_label *e, char* string) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != string));

  u_int len;
  
  if (NULL != e->string)
    free(e->string);

  len = strlen(string);

  e->string = (char*) calloc(len, sizeof(char));

  memcpy(e->string, string, len);

  e->text = sg_type_new_render(e->root->font, string);
}

/* end of sg_label */

#endif /* __sg_element_label_c__ */
