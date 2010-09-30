#include <gmp.h>
#include <stdio.h>

int main() {
  printf("%d %d %d\n", __GNU_MP_VERSION, __GNU_MP_VERSION_MINOR, __GNU_MP_VERSION_PATCHLEVEL);
  return 0;
}
