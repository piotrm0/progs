#ifndef __util_h__
#define __util_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/time.h>

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
