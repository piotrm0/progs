#include "geo.h";

// geo_obj

geo_obj* geo_obj_new() {
  MALLOC(o, geo_obj);

  o->pos = v3_new(0,0,0);
  o->vel = v3_new(0,0,0);
  o->rot = v3_new(0,0,0);
  o->spi = v3_new(0,0,0);

  o->flags = 0;
  o->space_key = 0;
  o->ref = NULL;

  return o;
}

geo_obj* geo_obj_copy(geo_obj* o) {
  IF_DEBUG(assert(NULL != o));

  MALLOC(ret, geo_obj);

  ret->pos = v3_copy(o->pos);
  ret->vel = v3_copy(o->vel);
  ret->rot = v3_copy(o->rot);
  ret->spi = v3_copy(o->spi);

  ret->space_key = o->space_key;
  ret->flags = o->flags;
  ret->ref   = o->ref;

  return ret;
}

void geo_obj_free(geo_obj* o) {
  IF_DEBUG(assert(NULL != o));

  v3_free(o->pos);
  v3_free(o->vel);
  v3_free(o->rot);
  v3_free(o->spi);

  free(o);
}

void geo_obj_mark(geo_obj* o, float dt) {
  IF_DEBUG(assert(NULL != o););

  if (o->flags & GEO_OBJ_IMMOVABLE)
    return;

  v3_mult_add(o->pos, o->vel, dt);
  v3_mult_add(o->rot, o->spi, dt);
}

#define geo_obj_bound_single(o, i, bound) \
  if (o->pos->v[i] < - bound) {		  \
    o->pos->v[i] = -bound;		  \
    o->vel->v[i] *= -1;			  \
  } else if (o->pos->v[i] > bound) {	  \
    o->pos->v[i] = bound;		  \
    o->vel->v[i] *= -1;			  \
  }					  \

void geo_obj_bound(geo_obj* o, float bound) {
  IF_DEBUG(assert(NULL != o););
  //  v3_bound(o->pos, bound);
  geo_obj_bound_single(o, 0, bound);
  geo_obj_bound_single(o, 1, bound);
  geo_obj_bound_single(o, 2, bound);
}

void geo_obj_rand(geo_obj* o, float mag) {
  IF_DEBUG(assert(NULL != o););

  v3_rand(o->pos, mag);
  v3_rand(o->vel, mag);
  v3_rand(o->rot, mag);
  v3_rand(o->spi, mag);
}

// geo_space

geo_space* geo_space_new() {
  MALLOC(s, geo_space);

  s->objs = llist_new();
  s->num_objs = 0;
  s->flags    = 0;
  s->bound    = 2;

  s->grav = NULL;

  return s;
}

geo_space* geo_space_copy(geo_space* s) {
  IF_DEBUG(assert(NULL != s));

  MALLOC(ret, geo_space);

  assert(0); // not implemented

  ret->flags    = s->flags;
  ret->num_objs = s->num_objs;
  ret->bound    = s->bound;

  return ret;
}

void geo_space_free(geo_space* s) {
  IF_DEBUG(assert(NULL != s));

  llist_free(s->objs);

  free(s);
}

void geo_space_add_obj(geo_space* s, geo_obj* o) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != o));

  o->space_key = (void*) llist_push(s->objs, (void*) o);
  s->num_objs ++;
}

void geo_space_remove_obj(geo_space* s, geo_obj* o) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != o));

  llist_remove(s->objs, (lliste*) o->space_key);
  s->num_objs --;
}

void geo_space_mark(geo_space* s, float dt) {
  float d;
  v3* temp;

  if (s->flags & GEO_SPACE_GRAV) {
    assert(NULL != s->grav);

    LLIST_FOREACH(e1, s->objs, {
	geo_obj* o1 = (geo_obj*) e1->e;
	v3_mult_add(o1->vel, s->grav, dt);
      });
  }

  if (s->flags & GEO_SPACE_OGRAV) {
    LLIST_FOREACH(e1, s->objs, {
	LLIST_FOREACH(e2, s->objs, {
	    if (e1 != e2) {
	      geo_obj* o1 = (geo_obj*) e1->e;
	      geo_obj* o2 = (geo_obj*) e2->e;
	      d = v3_dist(o1->pos, o2->pos);

	      if (d != 0) {
		temp = v3_copy(o2->pos);
		v3_sub(temp, o1->pos);
		v3_mult_add(o1->vel, temp, o2->mass * dt / (d));
		v3_free(temp);
	      }
	    }
	  });
      });
  }

  if (s->flags & GEO_SPACE_DAMP) {
    float dr = pow(s->damp, dt); 

    LLIST_FOREACH(e, s->objs, {
	v3_mult(((geo_obj*) (e->e))->vel, dr);
      });

  }

  LLIST_FOREACH(e, s->objs, {
      geo_obj_mark((geo_obj*) e->e, dt);
      geo_obj_bound((geo_obj*) e->e, s->bound);
    });
}

#ifdef HAVE_OPENGL
void geo_space_draw_gl(geo_space* s) {
  glLineWidth(2.0f);
  glColor3f(0.3f, 0.3f, 0.3f);

  glBegin(GL_LINE_LOOP);
  glVertex3f(- s->bound, - s->bound, - s->bound);
  glVertex3f(- s->bound,   s->bound, - s->bound);
  glVertex3f(  s->bound,   s->bound, - s->bound);
  glVertex3f(  s->bound, - s->bound, - s->bound);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(- s->bound, - s->bound, s->bound);
  glVertex3f(- s->bound,   s->bound, s->bound);
  glVertex3f(  s->bound,   s->bound, s->bound);
  glVertex3f(  s->bound, - s->bound, s->bound);
  glEnd();

  glBegin(GL_LINES);
  glVertex3f(- s->bound, - s->bound, - s->bound);
  glVertex3f(- s->bound, - s->bound,   s->bound);
  glVertex3f(- s->bound,   s->bound, - s->bound);
  glVertex3f(- s->bound,   s->bound,   s->bound);
  glVertex3f(  s->bound, - s->bound, - s->bound);
  glVertex3f(  s->bound, - s->bound,   s->bound);
  glVertex3f(  s->bound,   s->bound, - s->bound);
  glVertex3f(  s->bound,   s->bound,   s->bound);
  glEnd();

  glLineWidth(1.0f);
  glColor3f(0.1f, 0.1f, 0.1f);

  GEO_SPACE_FOREACH(o, s, {
      //printf("drawing at %f,%f,%f\n", o->pos->v[0], o->pos->v[1], o->pos->v[2]);
      glPushMatrix();
      gl_translate_v3(o->pos);
      
      glScalef(0.1,0.1,0.1);
      glBegin(GL_LINES);
      glVertex3f(-1, 0, 0);
      glVertex3f( 1, 0, 0);
      glVertex3f( 0,-1, 0);
      glVertex3f( 0, 1, 0);
      glVertex3f( 0, 0,-1);
      glVertex3f( 0, 0, 1);
      glEnd();
      //glutSolidSphere(0.1, 20, 20);
      glPopMatrix();
    });


}
#endif
