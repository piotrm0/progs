#ifndef __list_h__
#define __list_h__

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "util.h"

#define GROWTH_RATE 2
#define INIT_SIZE 128

typedef struct _list {
  void** e;
  u_int size;
  u_int _asize;
} list;

typedef struct _llist {
  void* e;
  struct _llist* next;
  struct _llist* prev;
  struct _llist* last;
} llist;

list* list_new();
void  list_free(list* l);
void* list_ref(list* l, int eindex);
void list_preset_size(list* l, u_int size);
void list_set_size(list* l, u_int size);
void list_push(list* l, void* element);
void list_append(list* l, list* l2);

typedef bool (*fun_find)  (llist* l);
typedef void (*fun_apply) (llist* l);

llist* llist_new();
void  llist_del(llist* l);  // free a single element
void  llist_free(llist* l); // free the entire list
void* llist_ref(llist* l, int eindex);
llist* llist_push(llist* l, void* element);
void llist_join(llist* l, llist* l2);
void llist_apply(llist* l, fun_apply f);
llist* llist_find(llist* l, fun_find f);
void llist_print(llist* l);
llist* llist_remove(llist* l, llist* lr);

typedef bool fun_filter(void* a);
typedef bool fun_greater(void* a, void* b);
typedef bool fun_greater_by_index(list* l, u_int n1, u_int n2);
typedef int     fun_discrete(void* a);
typedef double  fun_value(void* a);
typedef double  fun_value_by_index(list* l, u_int n);

list* list_filter(list* l, fun_filter filter);
void list_filter_to(list* l, list* tl, fun_filter filter);

void list_print_info(list* l);

#endif /* __list_h__ */
