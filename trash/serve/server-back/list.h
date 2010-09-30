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

typedef bool fun_filter(void* a);
typedef bool fun_greater(void* a, void* b);
typedef bool fun_greater_by_index(list* l, u_int n1, u_int n2);
typedef int     fun_discrete(void* a);
typedef double  fun_value(void* a);
typedef double  fun_value_by_index(list* l, u_int n);

list* list_new();
void  list_free(list* l);
void* list_ref(list* l, int eindex);
void list_preset_size(list* l, u_int size);
void list_set_size(list* l, u_int size);
void list_push(list* l, void* element);
void list_append(list* l, list* l2);

list* list_filter(list* l, fun_filter filter);
void list_filter_to(list* l, list* tl, fun_filter filter);
void list_print_info(list* l);

typedef struct _lliste {
  void* e;
  struct _lliste* next;
  struct _lliste* prev;
} lliste;

typedef bool (*fun_find)  (lliste* l);
typedef void (*fun_apply) (lliste* l);

lliste* lliste_new();
void   lliste_del(lliste* l);
void   lliste_free(lliste* l); // del the list elements from given element onwards

typedef struct _llist {
  u_int len;
  lliste* first;
  lliste* last;
} llist;

llist* llist_new();
void   llist_del(llist* l);  // free a single element

void*   llist_ref(llist* l, int eindex);
lliste* llist_push(llist* l, void* element);
void    llist_join(llist* l, llist* l2);
void    llist_apply(llist* l, fun_apply f);
lliste* llist_find(llist* l, fun_find f);
void    llist_print(llist* l);
void    llist_remove(llist* l, lliste* lr);
bool    llist_isempty(llist* l);

#endif /* __list_h__ */
