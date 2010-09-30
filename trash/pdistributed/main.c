#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "dist.h"

typedef struct _mtask_delim {
} mtask_delim;

typedef struct _mtask_result {
  u_int total;
} mtask_result;

typedef struct _task_delim {
  u_int start;
  u_int end;
} task_delim;

//typedef struct _task_result {  
//  u_int one;
//} task_result;

void compute();
void merge();
void resume();
void finalize();

int main(int argc, char** argv) {
  control_loop();
  return 0;
}

void compute() {
  
}

void merge() {

}

void resume() {

}

void finalize() {

}
