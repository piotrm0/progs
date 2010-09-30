#ifndef __sg_mouse_h__
#define __sg_mouse_h__

#include "sg.h"

struct _sg_mouse {
  vi2* pos_start;
  vi2* pos;
  sg_element* over;
  sg_element* drag;
};

_sg_mouse* _sg_mouse_new();
void       _sg_mouse_del(_sg_mouse* m);

#endif /* __sg_mouse_h__ */
