#ifndef __serve_c__
#define __serve_c__

#include "serve.h"

/* begin client */

client* client_new() {
  MALLOC(c, client);

  c->socket = (int) NULL;
  c->id     = (int) NULL;

  c->buffer_in   = buffer_new();
  c->buffer_out  = buffer_new();
  c->buffer_temp = (char*) calloc(CLIENT_BUFFER_SIZE, sizeof(char));

  c->flags = CLIENT_FLAG_NONE;

  c->index  = NULL;
  c->indext = NULL;

  return c;
}

void client_del(client* c) {
  IF_DEBUG(assert(NULL != c););

  buffer_del(c->buffer_in);
  buffer_del(c->buffer_out);
  free(c->buffer_temp);
  free(c);

  return;
}

/* end client */


/* begin server */

#define SERVER_NEW(name, type) \
  MALLOC(name, type);\
  name->clients = llist_new();\
  FD_ZERO(& name->fds_read);\
  FD_ZERO(& name->fds_write);\
  FD_ZERO(& name->fds_error);\
  name->flags = SERVER_FLAG_NONE;\
  name->proto = NULL;\

void server_start(server* s) {
  IF_DEBUG(assert(NULL != s););

  assert(! (SERVER_FLAG_STARTED & s->flags));

  s->flags |= SERVER_FLAG_STARTED;

  return;
}

void server_close(server* s) {
  IF_DEBUG(assert(NULL != s););

  assert(SERVER_FLAG_STARTED & s->flags);
  assert(SERVER_FLAG_CLOSING & s->flags);

  assert(llist_isempty(s->clients));

  return;
}

void server_shutdown(server* s) {
  IF_DEBUG(assert(NULL != s););

  assert(   SERVER_FLAG_STARTED & s->flags);
  assert(!( SERVER_FLAG_CLOSING & s->flags));

  printf("shutting down server\n");

  s->flags |= SERVER_FLAG_CLOSING;

  return;
}

void server_set_proto(server* s, char* string) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != string));
  
  if (NULL != s->proto)
    free(s->proto);

  s->proto = (char*) calloc(strlen(string), sizeof(char));

  strcpy(s->proto, string);

  return;
}

void server_write_client(server* s, client* c) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != c););

  int write_return;

  write_return = buffer_send(c->buffer_out, c->socket);

  if (-1 == write_return) {
    printf("server_write_client: buffer error, shutting down client\n");
    server_shutdown_client(s, c);
  }

  if (buffer_isempty(c->buffer_out))
    FD_CLR(c->socket, & s->fds_write);

  return;
}

void server_read_client(server* s, client* c) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != c););

  int recv_return;
  
  //printf("reading from client at socket %d\n", c->socket);

  recv_return = buffer_recv(c->buffer_in, c->socket);

  if (-1 == recv_return) {
    server_close_client(s, c);
    return;
  }

  while(buffer_get_line(c->buffer_in, '\n', c->buffer_temp)) {
    printf("read from[%d]: [%s]\n", c->socket, c->buffer_temp);
    //print_raw(c->buffer_temp, 32);

    printf("proto is %s\n", s->proto);

    if (! (c->flags & CLIENT_FLAG_VERIFIED)) {
      // need to verify

      if (0 != strcmp(s->proto, c->buffer_temp)) {
	printf("received bad proto string\n");
	server_shutdown_client(s,c);
	return;
      } 
      
      printf("client verified\n");
      c->flags |= CLIENT_FLAG_VERIFIED;

    } else {
      // already verified, pass line up the chain

      buffer_add_line(c->buffer_out, '\n', c->buffer_temp);

    }
  }

  return;
}

void server_shutdown_client(server* s, client* c) {
    IF_DEBUG(assert(NULL != s);
	     assert(NULL != c););

    assert(   c->flags & CLIENT_FLAG_CONNECTED);
    assert(! (c->flags & CLIENT_FLAG_CLOSING));

    int shutdown_return;

    c->flags |= CLIENT_FLAG_CLOSING;

    printf("shutting down client socket %d\n", c->socket);

    shutdown_return = shutdown(c->socket, SHUT_RDWR);
    //printf("socket: shutdown on %d, returned %d\n", c->socket, shutdown_return);

    if (0 != shutdown_return)
      perror("server_close_client");

    return;
}

void server_close_client(server* s, client* c) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != c););

  int close_return;

  FD_CLR(c->socket, & s->fds_read);
  FD_CLR(c->socket, & s->fds_write);

  printf("closing client socket %d\n", c->socket);
  close_return = close(c->socket);

  //printf("socket: close on %d, returned %d\n", c->socket, close_return);

  if (0 != close_return)
    perror("serve_close_client");

  llist_remove(s->clients, c->index);

  client_del(c);

  return;
}

/* end server */


/* begin servet */

servet* servet_new() {
  SERVER_NEW(st, servet);

  return st;
}

void servet_del(servet* st) {
  IF_DEBUG(assert(NULL != st););

  llist_del(st->clients);

  free(st);

  return;
}

void servet_fork(servet* st) {
  IF_DEBUG(assert(NULL != st););

  int create_return;

  create_return = pthread_create(& st->thread, NULL, servet_create, (void*) st);

  if (0 != create_return) {
    perror("servet_fork");
    return;
  }

  return;
}

void* servet_create(void* arg) {
  IF_DEBUG(assert(NULL != arg););

  servet* st = (servet*) arg;

  //void signal_int(int sig) {
  //    return;
  //  }
  //signal(SIGINT, signal_int);

  servet_loop(st);
  
  return NULL;
}

void servet_add_client(servet* st, client* c) {
  IF_DEBUG(assert(NULL != st);
	   assert(NULL != c));

  printf("adding client %d to thread %d\n", c->socket, st->index);

  c->index = llist_push(st->clients, (void*) c);

  return;
}

void servet_loop(servet* st) {
  IF_DEBUG(assert(NULL != st););

  int select_return;

  st->flags |= SERVER_FLAG_STARTED;

  void set_socks(lliste* l) {
    client* c = (client*) l->e;
    if (CLIENT_FLAG_CONNECTED | c->flags)
      FD_SET(c->socket, & st->fds_read);
    if (! buffer_isempty(c->buffer_out))
      FD_SET(c->socket, & st->fds_write);
  }

  void check_reads(lliste* l) {
    client* c = (client*) l->e;
    if (FD_ISSET(c->socket, & st->fds_read))
      server_read_client((server*) st, c);
  }

  void check_writes(lliste* l) {
    client* c = (client*) l->e;
    if (FD_ISSET(c->socket, & st->fds_write))
      server_write_client((server*) st, c);
  }

  while(true) {
    llist_apply(st->clients, set_socks);

    select_return = select(SERVE_MAX_CONNS, & st->fds_read, & st->fds_write, NULL, NULL);

    if (0 >= select_return) {
      if (EINTR != errno) {
	perror("servet_loop");
      }
      continue;
    }

    llist_apply(st->clients, check_reads);
    llist_apply(st->clients, check_writes);

    if ((st->flags & SERVER_FLAG_CLOSING) &&
	(llist_isempty(st->clients))) {
      //serve_close(s);
      break;
    }
    //printf("thread %d, ping\n", st->index);
  }

  return;
}

void servet_shutdown(servet* st) {
  IF_DEBUG(assert(NULL != st););

  server_shutdown((server*) st);

  printf("shutting down at thread %d\n", st->index);
  
  void shutdown_client(lliste* l) {
    client* c = (client*) l->e;
    if (CLIENT_FLAG_CONNECTED | c->flags)
      server_shutdown_client((server*) st, c);
  }

  llist_apply(st->clients, shutdown_client);

  return;
}

void servet_close(servet* st) {
  IF_DEBUG(assert(NULL != st););

  server_close((server*) st);

  printf("closing servet at thread %d\n", st->index);

  return;
}

/* end servet */


/* begin serve */

serve* serve_new() {
  SERVER_NEW(s, serve);

#ifdef SERVE_THREADED
  s->threads = (servet**) calloc(SERVE_THREADS, sizeof(servet*));
#endif

  s->socket  = (int) NULL;
  s->address.sin_family = AF_INET;
  s->address.sin_addr.s_addr = INADDR_ANY;
  s->address.sin_port        = htons(SERVE_PORT_DEFAULT);
  s->address_len   = sizeof(s->address);

  server_set_proto((server*) s, (char*) & SERVE_PROTO_DEFAULT);

  return s;
}

void serve_set_port(serve* s, int port) {
  IF_DEBUG(assert(NULL != s););

  assert(! (s->flags & SERVER_FLAG_STARTED));

  s->port = port;
  s->address.sin_port = htons(port);

  return;
}

void serve_del(serve* s) {
  IF_DEBUG(assert(NULL != s););

  llist_del(s->clients);

  free(s);

  return;
}

void serve_shutdown(serve* s) {
  IF_DEBUG(assert(NULL != s););

  server_shutdown((server*) s);

  printf("shutting down serve\n");

#ifdef SERVE_THREADED
  int i;
  for (i = 0; i < SERVE_THREADS; i++) {
    servet_shutdown(s->threads[i]);
  }
#else
  void shutdown_client(lliste* l) {
    client* c = (client*) l->e;
    if (CLIENT_FLAG_CONNECTED | c->flags)
      server_shutdown_client((server*) s, c);
  }
  llist_apply(s->clients, shutdown_client);
#endif

  return;
}

void serve_close(serve* s) {
  IF_DEBUG(assert(NULL != s););

  server_close((server*) s);

  printf("closing server socket\n");

  int close_return = close(s->socket);

  if (0 != close_return) {
    perror("serve_close");
  }

  return;
}

#ifdef SERVE_THREADED
servet* serve_least_thread(serve* s) {
  IF_DEBUG(assert(NULL != s););

  int i;
  int li = 0;
  int lv = -1;

  for (i = 0; i < SERVE_THREADS; i++) {
    if (s->threads[i]->clients->len < lv) {
      lv = s->threads[i]->clients->len;
      li = i;
    }
  }

  return s->threads[li];
}

void serve_start_threads(serve* s) {
  IF_DEBUG(assert(NULL != s););

  int i;

  //pthread_init();

  for (i = 0; i < SERVE_THREADS; i++) {
    s->threads[i] = servet_new();
    s->threads[i]->index  = i;
    s->threads[i]->parent = s;
    server_set_proto((server*) s->threads[i], s->proto);
    servet_fork(s->threads[i]);
  }

  return;
}
#endif

void serve_start_listen(serve* s) {
  IF_DEBUG(assert(NULL != s););

  int bind_return;
  int opt_return;
  int true_val = 1;

  s->socket = socket(AF_INET, SOCK_STREAM, 0);
  assert(-1 != s->socket);

  opt_return = setsockopt(s->socket, SOL_SOCKET, SO_REUSEADDR, (void*) &true_val, sizeof(void*));

  //printf("socket: setsockopt returned %d\n", opt_return);
  if (0 != opt_return)
    perror("serve_start");

  bind_return = bind(s->socket, (struct sockaddr*) (& s->address), s->address_len);

  while(-1 == bind_return) {
    perror("serve_start");
    sleep(3);
    bind_return = bind(s->socket, (struct sockaddr*) (& s->address), s->address_len);
  }

  printf("started\n");

  listen(s->socket, SERVE_MAX_LISTEN);

  return;
}

void serve_start(serve* s) {
  IF_DEBUG(assert(NULL != s););

  server_start((server*) s);

#ifdef SERVE_THREADED
  serve_start_threads(s);
#endif

  serve_start_listen(s);

  return;
}

void serve_loop(serve* s) {
  IF_DEBUG(assert(NULL != s););

  assert(SERVER_FLAG_STARTED & s->flags);

#ifndef SERVE_THREADED
  void set_socks(lliste* l) {
    client* c = (client*) l->e;
    if (CLIENT_FLAG_CONNECTED | c->flags)
      FD_SET(c->socket, & s->fds_read);
    if (! buffer_isempty(c->buffer_out))
      FD_SET(c->socket, & s->fds_write);
  }

  void check_reads(lliste* l) {
    client* c = (client*) l->e;
    if (FD_ISSET(c->socket, & s->fds_read))
      server_read_client((server*) s, c);
  }

  void check_writes(lliste* l) {
    client* c = (client*) l->e;
    if (FD_ISSET(c->socket, & s->fds_write))
      server_write_client((server*) s, c);
  }
#else
  servet* st;
#endif

  client* c;

  int socket_new;
  int new_conns;

  while(true) {
    FD_SET(s->socket, & s->fds_read);

#ifndef SERVE_THREADED
    llist_apply(s->clients, set_socks);
#endif

    new_conns = select(SERVE_MAX_CONNS, & s->fds_read, & s->fds_write, NULL, NULL);

    if (0 >= new_conns) {
      if (EINTR != errno) {
	perror("serve_loop");
      }
      continue;
    }

#ifndef SERVE_THREADED
    llist_apply(s->clients, check_reads);
    llist_apply(s->clients, check_writes);
#endif

    if ((FD_ISSET(s->socket, & s->fds_read)) &&
	(! (s->flags & SERVER_FLAG_CLOSING))) {
      socket_new = accept(s->socket, (struct sockaddr*) (& s->address), & s->address_len);

      printf("connected client at %d\n", socket_new);

      c = client_new();
      c->flags |= CLIENT_FLAG_CONNECTED;
      c->socket = socket_new;

#ifndef SERVE_THREADED
      c->index = llist_push(s->clients, (void*) c);
#else
      st = serve_least_thread(s);
      servet_add_client(st, c);
      pthread_kill(st->thread, SIGINT);
#endif
    }

    if ((s->flags & SERVER_FLAG_CLOSING) &&
	(llist_isempty(s->clients))) {
      serve_close(s);
      break;
    }
  }

  return;
}

/* end serve */

#endif /* __serve_c__ */
