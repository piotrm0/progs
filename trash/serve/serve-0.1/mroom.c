#ifndef __mroom_c__
#define __mroom_c__

#include "mroom.h"

/* object */

object* object_new() {
  MALLOC(o, object);

  o->flags = OBJECT_FLAG_NONE;
  o->id    = (int) o;

  o->index = NULL;

  o->pos = v2_new(0,0);
  o->vel = v2_new(0,0);

  return o;
}

void object_del(object* o) {
  IF_DEBUG(assert(NULL != o););

  v2_del(o->pos);
  v2_del(o->vel);

  free(o);

  return;
}

void object_tick(object* o, float dt) {
  IF_DEBUG(assert(NULL != o););

  v2_mult_add(o->pos, o->vel, dt);

  return;
}

/* end object */

/* room */

room* room_new() {
  MALLOC(r, room);

  r->flags = ROOM_FLAG_NONE;

  r->objects = llist_new();

  return r;
}

void room_del(room* r) {
  IF_DEBUG(assert(NULL != r););

  llist_del(r->objects);

  free(r);

  return;
}

void room_add_object(room* r, object* o) {
  IF_DEBUG(assert(NULL != r);
	   assert(NULL != o));

  o->index = llist_push(r->objects, (void*) o);

  return;
}

void room_tick(room* r, float dt) {
  IF_DEBUG(assert(NULL != r););

  LLIST_FOREACH_E(object, o, r->objects);
  object_tick(o,dt);
  LLIST_FOREACH_DONE();

  return;
}

/* end room */

/* mroom */

mroom* mroom_new() {
  MALLOC(r, mroom);

  r->flags = MROOM_FLAG_NONE;

  r->r = room_new();

  return r;
}

void mroom_del(mroom* r) {
  IF_DEBUG(assert(NULL != r););

  free(r);

  return;
}

/* end mroom */

#endif /* __mroom_c__ */
