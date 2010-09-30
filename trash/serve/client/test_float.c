#include <stdio.h>

#define TYPE float
#define TYPE_FORMAT "%f\n"

int main(int argc, char* argv[]) {
  TYPE sum = 0;
  TYPE i   = 0;
  
  for (i = 0; i < 10000000; i++) {
    sum += i;
  }
  
  printf(TYPE_FORMAT, sum);
}
