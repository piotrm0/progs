#ifndef __list_h__
#define __list_h__

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define GROWTH_RATE 2
#define INIT_SIZE 128

typedef u_int boolean;

typedef struct _list {
  void** e;
  u_int size;
  u_int _asize;
} list;

list* list_new();
void list_free(list* l);
void* list_ref(list* l, int eindex);
void list_preset_size(list* l, u_int size);
void list_set_size(list* l, u_int size);
void list_push(list* l, void* element);
void list_append(list* l, list* l2);

typedef boolean fun_filter(void* a);
typedef boolean fun_greater(void* a, void* b);
typedef boolean fun_greater_by_index(list* l, u_int n1, u_int n2);
typedef int     fun_discrete(void* a);
typedef double  fun_value(void* a);
typedef double  fun_value_by_index(list* l, u_int n);

list* list_filter(list* l, fun_filter filter);
void list_filter_to(list* l, list* tl, fun_filter filter);

void list_print_info(list* l);

#endif /* __list_h__ */
