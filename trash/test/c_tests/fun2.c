#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  printf("%d\n", test(5));
}

int test(int a) {

  int inner_test(int b) {
    return a + b;
  }

  return inner_test(10);
}
