#include "Python.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

int main(int argc, char** argv) {
  Py_Initialize();

  return 0;
}
