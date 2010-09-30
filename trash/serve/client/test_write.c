#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define PIPE  "/tmp/pipe.fifo"
#define WRITE "hello"

int main(int argc, char* argv[]) {
  int fd;

  int close_ret;
  int write_ret;

  int try = 0;

  while(1) {
    try ++;
    fd = open(PIPE, O_WRONLY, 0);
    printf("opened (try %d) (fd=%d)\n", try, fd);
    write_ret = write(fd, WRITE, strlen(WRITE));
    printf("wrote (ret=%d)\n", write_ret);
    close_ret = close(fd);
    printf("closed (ret=%d)\n", close_ret);
    //sleep(1);
  }
}
