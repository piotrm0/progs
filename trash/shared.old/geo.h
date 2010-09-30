#ifndef __geo_h__
#define __geo_h__

#include <assert.h>
#include "util.h"
#include "m_util.h"
#include "geo.h"
#include "list.h"

#ifdef HAVE_OPENGL
#include "igl.h"
#endif

// geo_obj

#define GEO_OBJ_IMMOVABLE 0x0001

typedef struct _geo_obj {
  v3* pos;
  v3* vel;
  v3* rot;
  v3* spi;

  float mass;

  u_int flags;
  void* space_key;

  void* ref;
} geo_obj;

geo_obj* geo_obj_new();
geo_obj* geo_obj_copy(geo_obj* o);
void geo_obj_free(geo_obj* o);
void geo_obj_mark(geo_obj* o, float dt);
void geo_obj_rand(geo_obj* o, float mag);
void geo_obj_bound(geo_obj* o, float bound);

// geo_space

#define GEO_SPACE_GRAV  0x0001
#define GEO_SPACE_OGRAV 0x0002
#define GEO_SPACE_DAMP  0x0004

typedef struct _geo_space {
  llist* objs;

  u_int num_objs;

  float bound;

  u_int flags;

  v3* grav;
  float damp;

} geo_space;

geo_space* geo_space_new();
geo_space* geo_space_copy(geo_space* s);
void geo_space_free(geo_space* s);

void geo_space_add_obj(geo_space* s, geo_obj* o);
void geo_space_remove_obj(geo_space* s, geo_obj* o);
void geo_space_mark(geo_space* s, float dt);

#ifdef HAVE_OPENGL
void geo_space_draw_gl(geo_space* s);
#endif

#define GEO_SPACE_SIZE(s) (s)->num_objs		\

#define GEO_SPACE_FOREACH(o, s, do) {		\
    LLIST_FOREACH(e, (s)->objs, {		\
	geo_obj* o = (geo_obj*) e->e;		\
	do;					\
      });					\
  }						\

#endif /* __geo_h__ */
