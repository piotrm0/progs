#ifndef __parse_c__
#define __prase_c__

#include "parse.h"

list* parse_tok_list(char* src, char* delim) {
  list* ret;
  char* temp;

  assert(NULL != src);
  assert(NULL != delim);

  ret = list_new();

  if (NULL == (temp = strtok(src, delim))) return ret;
  list_push(ret, temp);
  
  while(NULL != (temp = strtok(NULL, delim)))
    list_push(ret, temp);

  return ret;
}

#endif /* __parse_c__ */
