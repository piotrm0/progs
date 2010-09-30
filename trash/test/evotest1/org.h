#ifndef __org_h__
#define __org_h__

#include "geo.h"

// org

#define ORG_GEN_SIZE 10
#define ORG_GEN_BODY_MASS  0
#define ORG_GEN_ENERGY_MAX 1
#define ORG_GEN_ACCEL_MAX  2
#define ORG_GEN_SENSE_MAX  3
#define ORG_GEN_B_EAT 5
#define ORG_GEN_B_REP 6

typedef struct _org {
  geo_obj* geo;

  float* gen;
  float energy;

  struct _org_world* world;

  void* orgs_key;

  u_int flags;
} org;

org* org_new();
org* org_copy(org* o);
void org_free(org* o);

void org_mark(org* o, float dt);

void org_rand(org* o, float mag);

// org_world

typedef struct _org_world {
  geo_space* space;

  llist* orgs;

  u_int pop;

  u_int flags;
} org_world;

org_world* org_world_new();
org_world* org_world_copy(org_world* w);
void org_world_free(org_world* w);

void org_world_add_org(org_world* w, org* o);
void org_world_remove_org(org_world* w, org* o);
void org_world_populate(org_world* w, u_int pop);
void org_world_mark(org_world* world, float dt);

#define ORG_WORLD_SIZE(w) (w)->pop		\

#define ORG_WORLD_FOREACH(o, w, do) {		\
    LLIST_FOREACH(e, (w)->orgs, {		\
	org* o = (org*) e->e;			\
	do;					\
      });					\
  }						\

#endif /* __org_h__ */
