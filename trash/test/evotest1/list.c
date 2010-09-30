#ifndef __list_c__
#define __list_c__

#include "list.h"

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
}

void list_print_info(list* l) {
  IF_DEBUG(assert(NULL != l););

  printf("list at [%x] of size [%d] and allocated [%d], elements at [%x]\n", (u_int) l, l->size, l->_asize, (u_int) l->e);
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

// llist (linked list)

lliste* lliste_new() {
  MALLOC(e, lliste);

  e->e = NULL;
  e->holder = NULL;
  e->prev = NULL;
  e->next = NULL;

  return e;
}

void   lliste_free(lliste* e) {
  IF_DEBUG(assert(NULL != e););

  free(e);
}

llist* llist_new() {
  MALLOC(l, llist);

  l->size = 0;
  l->first = NULL;
  l->last  = NULL;

  return l;
}

void llist_free(llist* l) {
  // free the entire list

  IF_DEBUG(assert(NULL != l););

  while (l->first != NULL) {
    llist_remove(l, l->first);
  }
}

void llist_print(llist* l) {
  IF_DEBUG(assert(NULL != l););

  printf("list(%p) first=%p last=%p size=%d\n", l, l->first, l->last, l->size);

  u_int i = 0;
  LLIST_FOREACH(e, l, {
    printf("  [%d]: (%p) holding %p prev=%p next=%p\n", i, e, e->e, e->prev, e->next);
    i++;
    });

}

/*
void llist_apply(llist* l, void f(lliste* e)) {
  IF_DEBUG(assert(NULL != l););

  LLIST_FOREACH(e, l, {
    f(e);
    });

  return;
}
*/

lliste* llist_push(llist* l, void* element) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != element););

  lliste* new = lliste_new();
  new->e = element;
  new->holder = l;
  new->prev = l->last;
  new->next = NULL;

  if (NULL != l->last)
    l->last->next = new;

  if (NULL == l->first)
    l->first = new;

  l->last = new;

  l->size ++;

  return new;
}

void* llist_remove(llist* l, lliste* e) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != e);
	   assert(l == e->holder));

  lliste* prev = e->prev;
  lliste* next = e->next;

  void* ret = e->e;

  if (l->first == e) {
    l->first = next;
  }

  if (l->last == e) {
    l->last = prev;
  }

  if (NULL != prev) {
    prev->next = next;
  }

  if (NULL != next) {
    next->prev = prev;
  }

  l->size --;

  lliste_free(e);

  return ret;
}

/*
void llist_print_info(list* l) {
  assert(NULL != l);
  printf("list at [%x] of size [%d] and allocated [%d], elements at [%x]\n", (u_int) l, l->size, l->_asize, (u_int) l->e);
}
*/

/*
void* llist_ref(llist* l, int eindex) {
  IF_DEBUG(assert(NULL != l););

  llist* p;

  assert(NULL != l);

  if (eindex > 0) {
    p = l;

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
    p = l;

    assert(NULL != l->last); // empty list    
  }

  return p->e;
}
*/

/*

llist* llist_find(llist* l, bool f(llist* l)) {
  IF_DEBUG(assert(NULL != l););

  llist* p;

  if (NULL == l->last)
    return NULL;

  p = l;

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
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != l2););

  if (NULL != l->last) {
    l->last->next = (llist*) l2;
  }

  l2->prev = l->last;

  void set_last(llist* l3) {
    l3->last = l2->last;
  }

  llist_apply(l, (fun_apply) set_last);

  return;
}
*/

#endif /* __list_c__ */
