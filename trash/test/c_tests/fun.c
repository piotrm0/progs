#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

}

typedef float (adder_func) (float);

adder_func make_adder(float amount) {
  float add_x(float f){
    return f + amount;
  }

  return add_x;
}
