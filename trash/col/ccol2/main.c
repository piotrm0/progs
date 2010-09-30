#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "util.h"
#include "col.h"

int main(int argc, char* argv[]) {
  col* c = col_new();
  col* sol;

  col_random(c);
  
  //sol = col_g1search(300000, c);
  sol = col_gnsearch(300000, c);
  //sol = col_sq1search(300000, c);

  col_print_info(sol);
  col_print(sol);
  //col_print_fail(sol);
  
  return 0;
}
