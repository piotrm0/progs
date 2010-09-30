#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PIPE  "/tmp/pipe.fifo"
#define WRITE "hello"

int main(int argc, char* argv[]) {
  int fd;
  int read_ret;

  char* buff = (char*) calloc(33, sizeof(char));
  buff[32] = 0;

  fd = open(PIPE, O_RDONLY | O_NONBLOCK, 0);
  printf("opened\n");

  while(read_ret = read(fd, buff, 32)) {
    printf("read (ret=%d): %s\n", read_ret, buff);
  }

  printf("closed\n");

  return;
}
