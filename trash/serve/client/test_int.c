#include <stdio.h>

#define TYPE int
#define TYPE_FORMAT "%d\n"

int main(int argc, char* argv[]) {
  TYPE sum = 0;
  TYPE i   = 0;
  
  for (i = 0; i < 10000000; i++) {
    sum += i;
  }
  
  printf(TYPE_FORMAT, sum);
}
