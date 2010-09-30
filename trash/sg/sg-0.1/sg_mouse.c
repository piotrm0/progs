#ifndef __sg_mouse_c__
#define __sg_mouse_c__

#include "sg.h"
#include "sg_mouse.h"

_sg_mouse* _sg_mouse_new() {
  MALLOC(ret, _sg_mouse);

  ret->over = NULL;
  ret->drag = NULL;

  ret->pos_start = vi2_new(0,0);
  ret->pos       = vi2_new(0,0);

  return ret;
}

void _sg_mouse_del(_sg_mouse* m) {
  IF_DEBUG(assert(NULL != m););

  vi2_del(m->pos_start);
  vi2_del(m->pos);

  return;
}

#endif /* __sg_mouse_c__ */

