#ifndef __dist_h__
#define __dist_h__

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "util.h"
#include "parse.h"
#include "list.h"

typedef struct _mtask {
  u_int last_seq_num;
  void* delim;
  void* result;
} mtask;

typedef struct _task {
  u_int seq_num;
  void* delim;
  void* result;
} task;

void control_loop();
extern void compute();
extern void merge();
extern void resume();
extern void finalize();

#endif /* __dist_h__ */
