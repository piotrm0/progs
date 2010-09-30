#ifndef __sg_event_h__
#define __sg_event_h__

#include "sg.h"

/* _sg_callback */

typedef void (*fun_handler) (sg_event* e);

struct _sg_callback {
  u_int cover;         // specify event types covered (SG_EVENT_x)
  u_int id;
  fun_handler handler;
};

_sg_callback* _sg_callback_new();
void _sg_callback_del(_sg_callback* c);

/* end of _sg_callback */

/* sg_event */

#define SG_EVENT_NONE     0
#define SG_EVENT_MOUSE    1
#define SG_EVENT_KEYBOARD 2
#define SG_EVENT_DRAG     4

#define SG_EVENT_FLAG_NONE 0

#define _SG_EVENT_STRUCT \
  u_int type;\
  u_int flags;\
  sg_element* target;\

struct sg_event {
  _SG_EVENT_STRUCT;
};

#define _SG_EVENT_FUNCS(typename) \
  sg_##typename* sg_##typename##_new();\
  void sg_##typename##_del(sg_##typename* e);\

_SG_EVENT_FUNCS(event);

/* end of sg_event */

/* sg_event_mouse */

#define SG_MOUSE_NONE    0
#define SG_MOUSE_LEFT    1
#define SG_MOUSE_RIGHT   2
#define SG_MOUSE_MIDDLE  4

#define SG_MOUSE_DOWN 8
#define SG_MOUSE_UP   16

struct sg_event_mouse {
  _SG_EVENT_STRUCT;
  u_int action;
  vi2* pos;
};

_SG_EVENT_FUNCS(event_mouse);
sg_event_mouse* sg_event_mouse_new_sdl(SDL_MouseButtonEvent* e);

/* end of sg_event_mouse */

/* sg_event_keyboard */

struct sg_event_keyboard {
  _SG_EVENT_STRUCT;
};

/* end of sg_event_keyboard */

/* sg_event_drag */

struct sg_event_drag {
  _SG_EVENT_STRUCT;
};

/* end of sg_event_drag */

/* common event lines */

#define _SG_EVENT_INIT(ret, typename, typenamecap) \
  sg_##typename* ret;\
  ret = (sg_##typename*) malloc(sizeof(sg_##typename));\
  ret->type  = SG_EVENT_NONE;\
  ret->flags = SG_EVENT_FLAG_NONE;\
  ret->target = NULL;\

#define _SG_EVENT_DEL(e) \
  free(e);\

/* end of common event lines */

#endif /* __sg_event_h__ */
