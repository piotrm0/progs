#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include "mroom.h"
#include "util.h"
#include "serve.h"

int main(int argc, char* argv[]) {
  serve* s;
  mroom* r;

  r = mroom_new();

  void close_server(int sig) {
    if (! (s->flags & SERVE_FLAG_STARTED))
      exit(0);

    if (s->flags & SERVE_FLAG_CLOSING) {
      serve_close(s);
      exit(1);
    } else {
      serve_shutdown(s);
    }
  }

  signal(SIGINT, close_server);
  
  s = serve_new();
  serve_start(s);
  serve_loop(s);

  return 0;
}
