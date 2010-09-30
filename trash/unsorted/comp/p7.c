#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OP_NONE -1
#define OP_UP    0
#define OP_DOWN  1
#define OP_TYPE  2
#define OP_START 3
#define OP_WAIT  4

#define MIN 1
#define MAX 999

int* opt;
int* cost;
int start;

int can_type[10];

int type_cost(int);
int fill_opt(int);

void trace (int);
int up_point(int);
int down_point(int);

char* seq;
int pos;

void reverse(char* in) {
  int last = strlen(in)-1;

  int i = 0;
  int j = last;

  char temp;

  while (i < j) {
    temp = in[i];
    in[i] = in[j];
    in[j] = temp;
    i++;
    j--;
  }
}

int main(int argc, char* argv[]) {
  int i;

  can_type[0] = 0;
  can_type[1] = 1;
  can_type[2] = 0;
  can_type[3] = 1;
  can_type[4] = 1;
  can_type[5] = 1;
  can_type[6] = 1;
  can_type[7] = 1;
  can_type[8] = 1;
  can_type[9] = 0;

  seq = (char*) calloc(1000, sizeof(char));
  pos = 0;

  opt  = calloc(MAX+1, sizeof(int));
  cost = calloc(MAX+1, sizeof(int));

  for (i = MIN; i <= MAX; i++) {
    opt[i] = OP_NONE;
  }

  start = 6;

  int end = 200;

  fill_opt(end);
  trace(end);
  reverse(seq);
  printf("%s\n", seq);
}

int type_cost(int num) {
  if (10 > num) {
    if (can_type[num]) {
      return 1;
    } else {
      return 9999;
    }
  }

  int parta = num / 10;
  int partb = num - 10 * (int)(num / 10);

  return (type_cost(parta) + type_cost(partb));
}

int up_point(int end) {
  if (end+1 > MAX) { return MIN; }
  return (end+1);
}
int down_point(int end) {
  if (end-1 < MIN) { return MAX; }
  return (end-1);
}

int fill_opt(int end) {
  int type, up, down;
  if (OP_WAIT == opt[end])
    return 9999;
  if (OP_NONE != opt[end])
    return cost[end];
  if (end == start) {
    opt[end]  = OP_START;
    cost[end] = 0;
    return 0;
  }

  opt[end] = OP_WAIT;

  type = type_cost(end) + 1;
  up   = 1 + fill_opt(up_point(end));
  down = 1 + fill_opt(down_point(end));

  if ((type <= up) && (type <= down)) {
    opt[end]  = OP_TYPE;
    cost[end] = type;
    return type;
  }

  if ((up <= down)) {
    opt[end]  = OP_DOWN;
    cost[end] = up;
    return up;
  }

  opt[end]  = OP_UP;
  cost[end] = down;
  return down;
}

void trace(int end) {
  int op = opt[end];

  char temp[10];

  if (OP_TYPE == op) {
    seq[pos] = 'E';
    pos++;
    sprintf(temp, "%d", end);
    reverse(temp);
    sprintf(seq+pos, "%s", temp);
    pos += strlen(seq+pos);
    return;
  }

  if (OP_UP == op) {
    seq[pos] = '+';
    pos++;
    trace(down_point(end));
  }

  if (OP_DOWN == op) {
    seq[pos] = '-';
    pos++;
    trace(up_point(end));
  }

  if (OP_START == op) {
    seq[pos] = 0;
  }
}
