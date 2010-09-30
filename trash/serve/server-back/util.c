#ifndef __util_c__
#define __util_c__

#include "util.h"

/* timing */

timer* timer_new() {
  return ((timer*) malloc(sizeof(timer)));
}

void timer_start(timer* t) {
  t->last = (timeval_ptr) malloc(sizeof(struct timeval));
  t->new  = (timeval_ptr) malloc(sizeof(struct timeval));
  gettimeofday(t->last, NULL);
}

int timer_mark(timer* t) {
  int timer_ret;
  gettimeofday(t->new, NULL);
  timer_ret = 1000000 * (t->new->tv_sec - t->last->tv_sec) + (t->new->tv_usec - t->last->tv_usec);;
  memcpy(t->last, t->new, sizeof(struct timeval));
  return timer_ret;
}

int timer_mark_ms(timer* t) {
  return ((int) (timer_mark(t) / 1000));  
}

/* end timing */


/* math */

float frandom() {
  return (((float) random()) / (float) LONG_MAX);
}

u_int pow2_ceil(u_int x) {
  return (1 << (u_int) ceil(log(x) / M_LN2));
}

/* end math */


/* error handling */

int perrv(int error, const char* fmt, va_list ap) {
  // rpetty

  const char* errorstr;

  vfprintf(stderr, fmt, ap);
  if (error) {
    if (NULL == (errorstr = strerror(error)))
      errorstr = ("unknown error");

    fprintf(stderr, ": %s\n", errorstr);
  } else
    fprintf(stderr, "\n");

  fflush(stderr);       /* just in case */
  return error;
}

int perr(int error, const char* fmt, ...) {
  // rpetty
  // not to be confused with perror(3)

  va_list ap;

  va_start(ap, fmt);
  perrv(error, fmt, ap);
  va_end(ap);
  return error;
}

void pexit(int error, const char* fmt, ...) {
  // rpetty

  va_list ap;

  va_start(ap, fmt);
  perrv(error, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}

/* end error handling */


/* strings */

void chomp(char* str) {
  // rpetty, piotrm

  char* str_p;

  if (NULL == str)
    return;

  for (str_p = str + strlen(str); --str_p >= str; *str_p = '\0')
    if ('\r' != *str_p && '\n' != *str_p)
      break;

  return;
}

void print_raw (char* str, u_int len) {
  u_int i;

  for (i = 0; i < len; i++)
    printf("%x ", str[i]);

  printf("\n");

  return;
}

/* end strings */


/* buffer */

buffer* buffer_new() {
  MALLOC(b, buffer);

  b->data = (char*) calloc(BUFFER_SIZE, sizeof(char));
  b->head = b->data;
  b->tail = b->data;

  return b;
}

void buffer_del(buffer* b) {
  IF_DEBUG(assert(NULL != b););

  free(b->data);
  free(b);

  return;
}

bool buffer_isempty(buffer* b) {
  IF_DEBUG(assert(NULL != b));
  return (b->head == b->tail);
}

int buffer_send(buffer* b, int fh) {
  IF_DEBUG(assert(NULL != b);
	   assert(0    != fh););

  if (buffer_isempty(b))
    return 0;

  int send_return;

  send_return = send(fh, b->head, b->tail - b->head, 0);

  if (-1 == send_return) {
    perror("buffer_send");
    return -1;
  }

  b->head += send_return;

  if (b->head == b->tail) {
    b->head = b->tail = b->data;
  }

  return send_return;
}

int buffer_recv(buffer* b, int fh) {
  IF_DEBUG(assert(NULL != b);
	   assert(0    != fh););

  if (0 == BUFFER_SIZE - (b->tail - b->data)) {
    printf("buffer_recv: no more buffer room\n");
    return -1;
  }

  int added;

  added = recv(fh, b->tail, BUFFER_SIZE - (b->tail - b->data), 0);
  //printf("socket: recv on %d, returned %d\n", fh, added);

  if (0 > added) {
    perror("buffer_recv");
    return -1;
  }

  if (0 == added) {
    return -1;
  }

  b->tail += added;

  return added;
}

int buffer_add(buffer* b, char* add) {
  IF_DEBUG(assert(NULL != b);
	   assert(NULL != add););

  int add_len = strlen(add);

  if (b->tail + add_len + 1 > b->data + BUFFER_SIZE) {
    printf("buffer_add: no more room on buffer\n");
    return -1;
  }

  memcpy(b->tail, add, add_len+1);
  b->tail += add_len;
  
  return add_len;
}

int buffer_add_line(buffer* b, char delim, char* input) {
  IF_DEBUG(assert(NULL != b);
	   assert(NULL != input););

  int add_return;

  add_return = buffer_add(b, input);

  if (-1 == add_return)
    return -1;

  if (b->tail + 1 > b->data + BUFFER_SIZE) {
    printf("buffer_add_line: no more room on buffer\n");
    return -1;
  }

  *b->tail = delim;
  b->tail++;

  return (add_return + 1);
}

u_int buffer_get_line(buffer* b, char delim, char* output) {
  IF_DEBUG(assert(NULL != b);
	   assert(NULL != output););

  char* delim_pos;

  delim_pos = memchr(b->head, (int) delim, b->tail - b->head);

  if (NULL == delim_pos) {
    if (b->data != b->head) {
      memcpy(b->data, b->head, b->tail - b->head);
      b->tail -= (b->head - b->data);
      b->head  = b->data;             // b->head -= (b->head - b->data);
    }

    return 0;
  }

  *delim_pos = '\0';
  memcpy(output, b->head, delim_pos - b->head + 1);

  b->head = delim_pos + 1;
  
  return 1;
}

/* end buffer */

#endif /* __util_c__ */
