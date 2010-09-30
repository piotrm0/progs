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
/* end math */

/* error handling */
// rpetty
int perrv(int error, const char* fmt, va_list ap) {
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

// rpetty
// not to be confused with perror(3)
int perr(int error, const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  perrv(error, fmt, ap);
  va_end(ap);
  return error;
}

// rpetty
void pexit(int error, const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  perrv(error, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}
/* end error handling */

/* strings */
// rpetty, piotrm
void chomp(char* str) {
  char* str_p;
  if (NULL == str)
    return;
  for (str_p = str + strlen(str); --str_p >= str; *str_p = '\0')
    if ('\r' != *str_p && '\n' != *str_p)
      break;
}
/* end strings */

