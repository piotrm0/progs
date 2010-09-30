#ifndef __list_c__
#define __list_c__

#include "list.h"

/* begin list */

list* list_new() {
  list* ret;

  ret = (list*) malloc(sizeof(list));
  assert(NULL != ret);

  ret->size   = 0;
  ret->_asize = INIT_SIZE;

  ret->e = (void**) calloc(ret->_asize, sizeof(void*));
  assert(NULL != ret->e);

  return ret;
}

void list_free(list* l) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != l->e););

  free(l->e);
  free(l);

  return;
}

void list_print_info(list* l) {
  IF_DEBUG(assert(NULL != l););

  printf("list at [%x] of size [%d] and allocated [%d], elements at [%x]\n", (u_int) l, l->size, l->_asize, (u_int) l->e);

  return;
}

void* list_ref(list* l, int eindex) {
  IF_DEBUG(assert(NULL != l););

  while (eindex < 0) {
    eindex = l->size + eindex;
  }

  return l->e[eindex];
}

void list_push(list* l, void* element) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != element););

  list_set_size(l, l->size + 1);
  l->e[l->size-1] = element;

  return;
}

void list_append(list* l, list* l2) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != l2););

  u_int size = l->size;
  //  printf("appending %d bytes from %x to %x\n", l2->size * sizeof(void*), (void*) l2->e, &(l->e[size]));

  if (0 == l2->size)
    return;

  list_set_size(l, l->size + l2->size);
  memcpy(&(l->e[size]), (void*) l2->e, l2->size * sizeof(void*));

  return;
}

void list_preset_size(list* l, u_int size) {
  IF_DEBUG(assert(NULL != l););

  if (size <= l->_asize) {
    return;
  }

  while (l->_asize < size) {
    l->_asize = l->_asize * GROWTH_RATE;
  }

  l->e = (void*) realloc(l->e, l->_asize * sizeof(void*));
  assert(NULL != l->e);

  return;
}

void list_set_size(list* l, u_int size) {
  IF_DEBUG(assert(NULL != l););

  if (size <= l->_asize) {
    l->size = size;
    return;
  }

  list_preset_size(l, size);
  l->size = size;

  return;
}

u_int list_filter_count(list* l, fun_filter filter) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != filter););

  u_int i;
  u_int cnt;

  assert(NULL != l);

  cnt = 0;

  for (i = 0; i < l->size; i++)
    if (filter(l->e[i]))
      cnt++;

  return cnt;
}

list* list_filter(list* l, fun_filter filter) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != filter););

  list* ret;

  ret = list_new();

  list_filter_to(l, ret, filter);

  return ret;
}

void list_filter_to(list* l, list* tl, fun_filter filter) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != tl);
	   assert(NULL != filter););

  u_int i;

  for (i = 0; i < l->size; i++)
    if (filter(l->e[i]))
      list_push(tl,l->e[i]);

  return;
}

/* end list */

/* begin llist */

lliste* lliste_new() {
  MALLOC(ret, lliste);

  ret->e    = NULL;
  ret->next = NULL;
  ret->prev = NULL;

  return ret;
}

void lliste_del(lliste* l) {
  IF_DEBUG(assert(NULL != l););

  free(l);

  return;
}

void lliste_free(lliste* l) {
  // !!! new version not tested

  // free the entire list given the first element

  IF_DEBUG(assert(NULL != l););

  lliste* p;
  lliste* p_next;

  p      = l;
  p_next = p->next;

  lliste_del(p);

  while(NULL != p_next) {
    p = p_next;
    p_next = p->next;
    lliste_del(p);
  }

  return;
}

llist* llist_new() {
  MALLOC(ret, llist);

  ret->first = NULL;
  ret->last  = NULL; 

  return ret;
}

void llist_del(llist* l) {
  IF_DEBUG(assert(NULL != l););

  if (NULL != l->first)
    lliste_free(l->first);

  free(l);

  return;
}

void* llist_ref(llist* l, int eindex) {
  // !!! new version not tested

  IF_DEBUG(assert(NULL != l););

  lliste* p;

  assert(NULL != l->first); // index out of bounds (empty list)

  if (eindex > 0) {
    p = l->first;

    while (eindex > 0) {
      assert(NULL != p->next); // index out of bounds
      p = p->next;
      eindex--;
    }

  } else if(eindex < 0) {
    p = l->last;

    while (eindex < 0) {
      assert(NULL != p->prev); // index out of bounds
      p = p->prev;
      eindex++;
    }

  } else { // eindex == 0
    p = l->first;
  }

  return p->e;
}

/* // replaced by a macro in .h
bool llist_isempty(llist* l) {
  return (NULL == l->first);
}
*/

void llist_print(llist* l) {
  // !!! new version not tested

  IF_DEBUG(assert(NULL != l););

  u_int i = 0;

  printf("llist(%p) first=%p last=%p\n", l, l->first, l->last);

  void print_element(lliste* l) {
    printf("  [%d]: (%p) holding %p next=%p\n", i, l, l->e, l->next);
    i++;
  }

  llist_apply(l, (fun_apply) print_element);
}

lliste* llist_push(llist* l, void* element) {
  // !!! new version not tested

  IF_DEBUG(assert(NULL != l);
	   assert(NULL != element););

  lliste* new = lliste_new();
  new->e = element;

  if (NULL == l->first) { // list was empty
    l->first = new;

  } else {
    new->prev = l->last;
    l->last->next = new;

  }

  l->last = new;

  return new;
}

void llist_apply(llist* l, void f(lliste* l)) {
  // !!! new version not tested

  IF_DEBUG(assert(NULL != l););

  LLIST_FOREACH(p, l);
  f(p);
  LLIST_FOREACH_DONE();

  /*
  lliste* p;
  lliste* p_next;

  p = l->first;

  if (NULL == p) return; // empty list

  p_next = p->next;

  f(p);

  while (NULL != p_next) {
    // change (or check) so that f is allowed to delete the current element

    p = p_next;

    p_next = p->next;

    f(p);
  }
  */

  return;
}

lliste* llist_find(llist* l, bool f(lliste* l)) {
  // !!! new version not tested

  IF_DEBUG(assert(NULL != l););

  lliste* p;

  if (NULL == l->first)
    return NULL;

  p = l->first;

  if (f(p))
    return p;

  while (NULL != p->next) {
    p = p->next;
    if (f(p))
      return p;
  }

  return NULL;
}

void llist_join(llist* l, llist* l2) {
  // !!! new version not tested

  IF_DEBUG(assert(NULL != l);
	   assert(NULL != l2););

  if (NULL == l->first) {
    // first one is empty

    l->first = l2->first;
    l->last  = l2->last;

  } else if (NULL == l2->first) {
    // second one is empty

  } else {
    // neither is empty

    l->last->next   = l2->first;
    l2->first->prev = l->last;

    l->last = l2->last;
  }

  return;
}

void llist_remove(llist* l, lliste* lr) {
  // !!! new version not tested

  IF_DEBUG(assert(NULL != l);
	   assert(NULL != lr););

  assert(NULL != l->first);

  if (l->first == lr) {
    // first and one to be removed are the same

    if (l->last == l->first) {
      // first and to remove is the only element in the list

      l->first = l->last = NULL;

    } else {
      // first and to remove is not the only element

      l->first = lr->next;
      l->first->prev = NULL;

    }
  } else {
    // list and element to remove are not the same

    if (lr == l->last) {
      // element to remove is the last in the list

      l->last = lr->prev;
      l->last->next = NULL;

    } else {
      // element to remove is not the last in the list

      lr->prev->next = lr->next;
      lr->next->prev = lr->prev;
    }
  }

  lliste_del(lr);
}

/* end llist */

#endif /* __list_c__ */
