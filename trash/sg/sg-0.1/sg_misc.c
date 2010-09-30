#ifndef __sg_misc_c__
#define __sg_misc_c__

#include "sg.h"

void sg_recti_draw(recti* view) {
  u_int x1,x2,y1,y2;

  x1 = view->v[0];
  x2 = x1 + view->v[2];
  y1 = view->v[1];
  y2 = y1 + view->v[3];

  glLineWidth(3.0f);
  glColor4f(0,1,0,1);
  glBegin(GL_LINE_LOOP);
  glVertex2i(x1, y1);
  glVertex2i(x2, y1);
  glVertex2i(x2, y2);
  glVertex2i(x1, y2);
  glEnd();
  glLineWidth(1.0f);

}

#endif /* __sg_misc_c__ */

