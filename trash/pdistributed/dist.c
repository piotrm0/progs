#ifndef __dist_c__
#define __dist_c__

#include "dist.h"

void control_loop() {
  char* temp;

  list* args;

  temp = (char*) calloc(32, sizeof(char));
  assert(NULL != temp);

  while(NULL != fgets(temp, 32, stdin)) {
    chomp(temp);
    args = parse_tok_list(temp, " ");



    list_free(args);
  }

  free(temp);
}

#endif /* __dist_c__ */
