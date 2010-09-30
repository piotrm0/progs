#include "org.h";

org* org_new() {
  MALLOC(ret, org);

  ret->geo = geo_obj_new();
  ret->geo->mass = 1;

  ret->gen = (float*) calloc(ORG_GEN_SIZE, sizeof(float));

  ret->energy = 0;
  ret->flags  = 0;
  ret->world    = NULL;
  ret->orgs_key = NULL;

  u_int i;

  for (i = 0; i < ORG_GEN_SIZE; i++) {
    ret->gen[i] = 0;
  }

  return ret;
}

org* org_copy(org* o) {
  MALLOC(ret, org);

  ret->geo = geo_obj_copy(o->geo);

  ret->gen = (float*) calloc(ORG_GEN_SIZE, sizeof(float));

  ret->energy = o->energy;
  ret->flags  = o->flags;

  ret->world    = NULL;
  ret->orgs_key = NULL;

  memcpy(o->gen, ret->gen, sizeof(float) * ORG_GEN_SIZE);
  
  return ret;
}

void org_free(org* o) {
  IF_DEBUG(assert(NULL != o);
	   assert(NULL != o->gen));

  geo_obj_free(o->geo);
  free(o->gen);
  free(o);
}

void org_mark(org* o, float dt) {
  //geo_obj_mark(o->geo, dt);
}

void org_rand(org* o, float mag) {
  IF_DEBUG(assert(NULL != o););

  geo_obj_rand(o->geo, mag);
}

// org_world

org_world* org_world_new() {
  MALLOC(ret, org_world);

  ret->flags = 0;
  ret->pop = 0;

  ret->orgs  = llist_new();

  ret->space = geo_space_new();
  ret->space->flags =
    GEO_SPACE_DAMP | 
    //GEO_SPACE_GRAV |
    GEO_SPACE_OGRAV |
    0
    ;
  ret->space->damp = 0.1;
  ret->space->grav = v3_new(0,-10,0);

  geo_obj* temp = geo_obj_new();
  //temp->flags = GEO_OBJ_IMMOVABLE;
  temp->mass = 1000;
  temp->pos->v[0] = 0;
  temp->pos->v[1] = 0;
  temp->pos->v[2] = 0;

  geo_space_add_obj(ret->space, temp);

  return ret;
}

void org_world_free(org_world* o) {
  IF_DEBUG(assert(NULL != o));

  geo_space_free(o->space);
  llist_free(o->orgs);

  free(o);
}

void org_world_add_org(org_world* w, org* o) {
  IF_DEBUG(assert(NULL != w);
	   assert(NULL != o);
	   assert(NULL == o->world););

  o->orgs_key = (void*) llist_push(w->orgs, (void*) o);
  o->world    = w;
  geo_space_add_obj(w->space, o->geo);

  w->pop ++;

  printf("space size = %d, orgs size = %d, pop = %d\n", GEO_SPACE_SIZE(w->space), LLIST_SIZE(w->orgs), ORG_WORLD_SIZE(w));
}

void org_world_remove_org(org_world* w, org* o) {
  IF_DEBUG(assert(NULL != w);
	   assert(NULL != o);
	   assert(w == o->world););

  llist_remove(w->orgs, (lliste*) o->orgs_key);
  geo_space_remove_obj(w->space, o->geo);

  o->orgs_key = NULL;
  o->world = NULL;

  w->pop --;
}

void org_world_populate(org_world* w, u_int pop) {
  u_int i;

  for (i = 0; i < pop; i++) {
    org* temp = org_new();
    temp->geo->mass = (float) (i*i);
    org_rand(temp, w->space->bound);
    org_world_add_org(w, temp);
  }
}

void org_world_mark(org_world* w, float dt) {
  geo_space_mark(w->space, dt);

  ORG_WORLD_FOREACH(o, w, {
      org_mark(o, dt);
    });
}
