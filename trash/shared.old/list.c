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

llist* llist_new() {
  llist* ret;

  ret = (llist*) malloc(sizeof(llist));
  assert(NULL != ret);

  ret->e = NULL;
  ret->next = NULL;
  ret->prev = NULL;
  ret->last = NULL; // stay null until element assigned to e, denote empty list

  return ret;
}

void  llist_del(llist* l) {
  // free a single element

  IF_DEBUG(assert(NULL != l););

  free(l);

  return;

}

void llist_free(llist* l) {
  // free the entire list

  IF_DEBUG(assert(NULL != l););

  llist* p;
  llist* prev;

  assert(NULL != l);

  p = l->last;
  prev = p->prev;

  if (NULL == p) {
    free(p);
    return;
  }

  while(NULL != prev) {
    p = prev;
    prev = p->prev;
    free(p);
  }

  return;
}

/*
void llist_print_info(list* l) {
  assert(NULL != l);
  printf("list at [%x] of size [%d] and allocated [%d], elements at [%x]\n", (u_int) l, l->size, l->_asize, (u_int) l->e);
}
*/

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

void llist_print(llist* l) {
  IF_DEBUG(assert(NULL != l););

  u_int i = 0;

  printf("list(%p) last=%p next=%p prev=%p\n", l, l->last, l->next, l->prev);

  void print_element(llist* l) {
    printf("  [%d]: (%p) holding %p next=%p\n", i, l, l->e, l->next);
    i++;
  }

  llist_apply(l, (fun_apply) print_element);

}

llist* llist_push(llist* l, void* element) {
  IF_DEBUG(assert(NULL != l);
	   assert(NULL != element););

  llist* new;

  if (NULL == l->last) { // list was empty
    l->e = element;
    l->last = l;
    return new;
  }

  new = llist_new();
  new->e = element;
  new->prev = l->last;
  new->last = new;

  l->last->next = new;

  void set_last(llist* l) {
    l->last = new;
  }
  
  llist_apply(l, (fun_apply) set_last);

  return new;
}

void llist_apply(llist* l, void f(llist* l)) {
  IF_DEBUG(assert(NULL != l););

  if (NULL == l->e) return;

  llist* p;

  p = l;

  f(p);

  while (NULL != p->next) {
    p = p->next;
    f(p);
  }

  return;
}

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

llist* llist_remove(llist* l, llist* lr) {
  // returns the new first element of the list

  IF_DEBUG(assert(NULL != l);
	   assert(NULL != lr););

  llist* ret;
  llist* new_last;

  if (l == lr) {
    // first and one to be removed are the same

    if (l->last == l) {
      // first and to remove is the only element in the list

      l->e    = NULL;
      l->next = NULL;
      l->prev = NULL;
      l->last = NULL;
      return l;

    } else {
      // first and to remove is not the only element

      ret = l->next;
      ret->prev = NULL;

      llist_del(lr);

      return ret;
    }
  } else {
    // list and element to remove are not the same

    if (lr == l->last) {
      // element to remove is the last in the list

      new_last = lr->prev;
      new_last->next = NULL;

      void set_last(llist* l2) {
	l2->last = new_last;
	return;
      }

      llist_apply(l, (fun_apply) set_last);

      llist_del(lr);

      return l;

    } else {
      // element to remove is not the last in the list

      lr->prev->next = lr->next;
      lr->next->prev = lr->prev;

      llist_del(lr);
      
      return l;
    }
  }
}

#endif /* __list_c__ */
