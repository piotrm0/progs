#ifndef __sg_event_c__
#define __sg_event_c__

#include "sg.h"

/* _sg_callback */

_sg_callback* _sg_callback_new() {
  _sg_callback* ret;

  ret = (_sg_callback*) malloc(sizeof(_sg_callback));

  ret->cover   = SG_EVENT_NONE;
  ret->id      = 0;
  ret->handler = NULL;

  return ret;
}

void _sg_callback_del(_sg_callback* c) {
  IF_DEBUG(assert(NULL != c););
  
  return;
}

/* end of _sg_callback */

/* sg_event_mouse */

sg_event_mouse* sg_event_mouse_new() {
  _SG_EVENT_INIT(ret, event_mouse, EVENT_MOUSE);

  ret->action = SG_MOUSE_NONE;
  ret->pos    = vi2_new(0,0);

  return ret;
}

sg_event_mouse* sg_event_mouse_new_sdl(SDL_MouseButtonEvent* e) {
  IF_DEBUG(assert(NULL != e););

  sg_event_mouse* ret;

  ret = sg_event_mouse_new();

  if (SDL_MOUSEBUTTONDOWN == e->type) {
    ret->action |= SG_MOUSE_DOWN;
  }

  if (SDL_MOUSEBUTTONUP == e->type) {
    ret->action |= SG_MOUSE_UP;
  }

  if (SDL_BUTTON_LEFT == e->button) {
    ret->action |= SG_MOUSE_LEFT;
  }

  if (SDL_BUTTON_MIDDLE == e->button) {
    ret->action |= SG_MOUSE_MIDDLE;
  }

  if (SDL_BUTTON_RIGHT == e->button) {
    ret->action |= SG_MOUSE_RIGHT;
  }

  ret->pos->v[0] = e->x;
  ret->pos->v[1] = e->y;

  return ret;
}

void sg_event_mouse_del(sg_event_mouse* e) {
  IF_DEBUG(assert(NULL != e););

  vi2_del(e->pos);

  _SG_EVENT_DEL(e);

  return;
}

/* end of sg_event_mouse */

/* sg_event_keyboard */

sg_event_keyboard* sg_event_keyboard_new() {
  _SG_EVENT_INIT(ret, event_keyboard, EVENT_KEYBOARD);

  return ret;
}

/* end of sg_event_keyboard */

/* sg_event_drag */

sg_event_drag* sg_event_drag_new() {
  _SG_EVENT_INIT(ret, event_drag, EVENT_DRAG);

  return ret;
}

/* end of sg_event_drag */

#endif /* __sg_event_c__ */
