#include "util.h"

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

