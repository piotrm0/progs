#ifndef __sg_element_c__
#define __sg_element_c__

#include "sg.h"

/* sg_element */

int _sg_element_free_callback_id(sg_element* e) {
  // return of -1 denotes no more callbacks allowed

  int ret ;

  for (ret = 0; ret < SG_MAX_CALLBACKS; ret++) {
    if (NULL == e->vcallbacks[ret]) {
      return ret;
    }
  }

  return -1;
}

int sg_element_add_callback(sg_element* e, u_int cover, fun_handler h) {
  // return of -1 denotes no more callbacks allowed

  int id = _sg_element_free_callback_id(e);

  if (-1 == id)
    return -1;

  _sg_callback* cb = _sg_callback_new();
  cb->cover   = cover;
  cb->handler = h;

  llist* l = llist_push(e->callbacks, (void*) cb);

  e->vcallbacks[id] = l;

  return id;
}

void sg_element_del_callback(sg_element* e, int id) {
  IF_DEBUG(assert(NULL != e);
	   assert(id >= 0);
	   assert(id < SG_MAX_CALLBACKS););

  llist* l = e->vcallbacks[id];

  if (NULL == l)
    return;

  _sg_callback* c = (_sg_callback*) l->e;
  _sg_callback_del(c);

  llist_remove(e->callbacks, l);

  return;
}

void sg_element_scissor(sg_element* e, recti* view) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view);)

  u_int left, bottom, width, height;
  recti* view_self;

  //  sg_recti_draw(view);
  view_self = recti_intersect_rel(view, e->geo);
  //  sg_recti_draw(view_self);

  left = view_self->v[0];
  width = view_self->v[2];
  height = view_self->v[3] + 1;
  bottom = e->root->geo->v[3] - view_self->v[1] - view_self->v[3];

  glScissor(left, bottom, width, height);

  recti_del(view_self);

  return;
}

void sg_element_clear_over(sg_element* e) {
  IF_DEBUG(assert(NULL != e);)

  sg_element* p;

  p = e;

  if (p->flags & SG_FLAG_OVER)
    p->flags -= SG_FLAG_OVER;

  while (NULL != p->over) {
    p = p->over;
    if (p->flags & SG_FLAG_OVER) {
      p->flags -= SG_FLAG_OVER;
    } else {
      return;
    }
  }
}

void sg_element_set_geo(sg_element* e, u_int x, u_int y, u_int w, u_int h) {
  IF_DEBUG(assert(NULL != e);)

  e->geo->v[0] = x;
  e->geo->v[1] = y;
  e->geo->v[2] = w;
  e->geo->v[3] = h;
  return;
}

void sg_element_over(sg_element* e, recti* view, vi2* point) {
  IF_DEBUG(assert(NULL != e);
	   assert(NULL != view);
	   assert(NULL != point);)

  llist* found;
  sg_element* e2;
  recti* view_sub;
  recti* view_temp;

  //printf("%x,%x,%x\n", e, offset, point);

  //printf("over: [%d,%d] point [%d,%d]\n", offset->v[0], offset->v[1], point->v[0], point->v[1]);

  bool find(llist* l) {
    sg_element* e;
    e = (sg_element*) l->e;
    if ((point->v[0] >= e->geo->v[0] + view->v[0]) &&
	(point->v[1] >= e->geo->v[1] + view->v[1]) &&
	(point->v[0] <= e->geo->v[0] + view->v[0] + e->geo->v[2]) &&
	(point->v[1] <= e->geo->v[1] + view->v[1] + e->geo->v[3])) {
      return true;
    } else {
      return false;
    }
  }

  found = llist_find(e->children, find);

  if (NULL != found) {
    //printf("found over\n");

    e2 = (sg_element*) found->e;

    if ((e->over) && (e->over != e2)) {
      //printf("clearing old over\n");
      sg_element_clear_over(e->over);
    }
    
    view_temp = recti_copy(view);
    view_temp->v[0] += e2->geo->v[0];
    view_temp->v[1] += e2->geo->v[1];
    view_sub = recti_intersect(view, view_temp);

    e2->flags |= SG_FLAG_OVER;

    e->root->mouse->over = e2;
    sg_element_over(e2, view_sub, point);

    recti_del(view_sub);
    recti_del(view_temp);

    e->over = e2;
  } else {
    //printf("not found\n");
    if (NULL != e->over) {
      sg_element_clear_over(e->over);
      e->over = false;
      //printf("clearing over\n");
    }
  }

  return;
}

/* end of sg_element */

#endif /* __sg_element_c__ */

