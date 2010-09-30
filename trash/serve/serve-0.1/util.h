#ifndef __util_h__
#define __util_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifdef DEBUG
#define IF_DEBUG(statements) statements
#else
#define IF_DEBUG(statements) 
#endif

#define true 1
#define false 0

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

void print_raw (char* str, u_int len);
void chomp (char* str);

/* end strings */


/* buffer */

#define BUFFER_SIZE 2048

typedef struct _buffer {
  char* data;
  char* head;
  char* tail;
} buffer;

buffer* buffer_new();
void    buffer_del(buffer* b);
int     buffer_add(buffer* b, char* add);
int     buffer_recv(buffer* b, int fh);
int     buffer_send(buffer* b, int fh);
int     buffer_add_line(buffer* b, char delim, char* input);
u_int   buffer_get_line(buffer* b, char delim, char* output);

//bool    buffer_isempty(buffer* b);
#define buffer_isempty(b) (b->head == b->tail)

/* end buffer */

#endif /* __util_h__ */
