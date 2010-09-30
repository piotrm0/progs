#ifndef __util_h__
#define __util_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>

#ifdef DEBUG
#define IF_DEBUG(statements) statements
#else
#define IF_DEBUG(statements) 
#endif

#define true 1
#define false 0

#define MIN(a,b) (a > b ? b : a)
#define MAX(a,b) (a < b ? b : a)

#define MALLOC(varname, typename) \
  typename* varname = (typename*) malloc(sizeof(typename));\
  assert(NULL != varname);\

typedef u_int bool;

/* timing */
typedef struct timeval* timeval_ptr;
typedef struct _timer {
  timeval_ptr last;
  timeval_ptr new;
} timer;
timer* timer_new();
void timer_start(timer*);
int timer_mark(timer*);
int timer_mark_ms(timer*);
/* end timing */

/* math */
float frandom();
u_int pow2_ceil(u_int x);
/* end math */

/* error handling */
int perrv(int error, const char* fmt, va_list ap);
int perr(int error, const char* fmt, ...);
void pexit(int error, const char* fmt, ...);
/* end error handling */

/* strings */
void chomp (char* str);
/* end strings */

#endif /* __util_h__ */
