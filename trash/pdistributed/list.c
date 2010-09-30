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
  assert(NULL != l);
  assert(NULL != l->e);

  free(l->e);
  free(l);
}

void list_print_info(list* l) {
  assert(NULL != l);

  printf("list at [%x] of size [%d] and allocated [%d], elements at [%x]\n", (u_int) l, l->size, l->_asize, (u_int) l->e);
}

void* list_ref(list* l, int eindex) {
  assert(NULL != l);

  while (eindex < 0) {
    eindex = l->size + eindex;
  }
  return l->e[eindex];
}

void list_push(list* l, void* element) {
  assert(NULL != l);
  assert(NULL != element);

  list_set_size(l, l->size + 1);
  l->e[l->size-1] = element;
}

void list_append(list* l, list* l2) {
  assert(NULL != l);
  assert(NULL != l2);

  u_int size = l->size;
  //  printf("appending %d bytes from %x to %x\n", l2->size * sizeof(void*), (void*) l2->e, &(l->e[size]));

  if (0 == l2->size)
    return;

  list_set_size(l, l->size + l2->size);
  memcpy(&(l->e[size]), (void*) l2->e, l2->size * sizeof(void*));
}

void list_preset_size(list* l, u_int size) {
  assert(NULL != l);

  if (size <= l->_asize) {
    return;
  }

  while (l->_asize < size) {
    l->_asize = l->_asize * GROWTH_RATE;
  }

  l->e = (void*) realloc(l->e, l->_asize * sizeof(void*));

  //printf("allocating %d elements at %x (needed %d)\n", l->_asize, l->e, size);
  if (NULL == l->e) {
    printf("BAD: could not allocate list of %d elements\n", l->_asize);
  }
}

void list_set_size(list* l, u_int size) {
  assert(NULL != l);

  if (size <= l->_asize) {
    l->size = size;
    return;
  }

  list_preset_size(l, size);
  l->size = size;
}

u_int list_filter_count(list* l, fun_filter filter) {
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
  list* ret;

  ret = list_new();

  list_filter_to(l, ret, filter);

  return ret;
}

void list_filter_to(list* l, list* tl, fun_filter filter) {
  u_int i;

  assert(NULL != l);
  assert(NULL != tl);

  for (i = 0; i < l->size; i++)
    if (filter(l->e[i]))
      list_push(tl,l->e[i]);
}

#endif /* __list_c__ */
