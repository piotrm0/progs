#ifndef __serve_c__
#define __serve_c__

#include "serve.h"

/* client */

client* client_new() {
  MALLOC(c, client);

  c->socket = (int) NULL;
  c->id     = (int) NULL;

  c->buffer_in   = buffer_new();
  c->buffer_out  = buffer_new();
  c->buffer_temp = (char*) calloc(CLIENT_BUFFER_SIZE, sizeof(char));

  c->flags = CLIENT_FLAG_NONE;

  c->index = NULL;

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

/* serve */

serve* serve_new() {
  MALLOC(s, serve);

  s->flags = SERVE_FLAG_NONE;

  s->clients = llist_new();

  s->socket  = (int) NULL;
  s->address.sin_family = AF_INET;
  s->address.sin_addr.s_addr = INADDR_ANY;
  s->address.sin_port        = htons(SERVE_PORT_DEFAULT);
  s->address_len   = sizeof(s->address);
  FD_ZERO(& s->fds_read);
  FD_ZERO(& s->fds_write);
  FD_ZERO(& s->fds_error);

  s->proto = NULL;
  serve_set_proto(s, (char*) & SERVE_PROTO_DEFAULT);

  return s;
}

void serve_set_port(serve* s, int port) {
  IF_DEBUG(assert(NULL != s););

  assert(! (s->flags & SERVE_FLAG_STARTED));

  s->port = port;
  s->address.sin_port = htons(port);

  return;
}

void serve_set_proto(serve* s, char* string) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != string));
  
  if (NULL != s->proto)
    free(s->proto);

  s->proto = (char*) calloc(strlen(string), sizeof(char));

  strcpy(s->proto, string);

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

  assert(   SERVE_FLAG_STARTED & s->flags);
  assert(!( SERVE_FLAG_CLOSING & s->flags));

  printf("shutting down\n");

  s->flags |= SERVE_FLAG_CLOSING;

  LLIST_FOREACH_E(client, c, s->clients);
  if (CLIENT_FLAG_CONNECTED | c->flags)
    serve_shutdown_client(s, c);
  LLIST_FOREACH_DONE();

  if (llist_isempty(s->clients)) {
    serve_close(s);
  }

  return;
}

void serve_close(serve* s) {
  IF_DEBUG(assert(NULL != s););

  assert(SERVE_FLAG_STARTED & s->flags);
  assert(SERVE_FLAG_CLOSING & s->flags);

  assert(llist_isempty(s->clients));

  int close_return;
  
  printf("closing server socket\n");

  close_return = close(s->socket);
  //printf("socket: close on %d, returned %d\n", s->socket, close_return);

  if (0 != close_return) {
    perror("serve_close");
  }

  return;
}

void serve_start(serve* s) {
  IF_DEBUG(assert(NULL != s););

  assert(! (SERVE_FLAG_STARTED & s->flags));

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

  s->flags |= SERVE_FLAG_STARTED;

  return;
}

void serve_shutdown_client(serve* s, client* c) {
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
      perror("serve_close_client");

    return;
}

void serve_close_client(serve* s, client* c) {
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

void serve_write_client(serve* s, client* c) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != c););

  int write_return;

  write_return = buffer_send(c->buffer_out, c->socket);

  if (-1 == write_return) {
    printf("serve_write_client: buffer error, shutting down client\n");
    serve_shutdown_client(s, c);
  }

  if (buffer_isempty(c->buffer_out))
    FD_CLR(c->socket, & s->fds_write);

  return;
}

void serve_read_client(serve* s, client* c) {
  IF_DEBUG(assert(NULL != s);
	   assert(NULL != c););

  int recv_return;
  
  //printf("reading from client at socket %d\n", c->socket);

  recv_return = buffer_recv(c->buffer_in, c->socket);

  if (-1 == recv_return) {
    serve_close_client(s, c);
    return;
  }

  while(buffer_get_line(c->buffer_in, '\n', c->buffer_temp)) {
    printf("read from[%d]: [%s]\n", c->socket, c->buffer_temp);
    //print_raw(c->buffer_temp, 32);

    if (! (c->flags & CLIENT_FLAG_VERIFIED)) {
      // need to verify

      if (0 != strcmp(s->proto, c->buffer_temp)) {
	printf("received bad proto string\n");
	serve_shutdown_client(s,c);
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

void serve_loop(serve* s) {
  IF_DEBUG(assert(NULL != s););

  assert(SERVE_FLAG_STARTED & s->flags);

  client* c;
  int socket_new;
  int new_conns;

  while(1) {
    FD_SET(s->socket, & s->fds_read);

    LLIST_FOREACH_E(client, c, s->clients);
    if (CLIENT_FLAG_CONNECTED | c->flags)
      FD_SET(c->socket, & s->fds_read);
    if (! buffer_isempty(c->buffer_out))
      FD_SET(c->socket, & s->fds_write);
    LLIST_FOREACH_DONE();

    //llist_print(s->clients);

    new_conns = select(SERVE_MAX_CONNS, & s->fds_read, & s->fds_write, NULL, NULL);

    //printf("socket: select returned %d\n", new_conns);

    if (0 >= new_conns) {
      if (EINTR != errno) {
	perror("serve_loop");
      }
      continue;
    }

    LLIST_FOREACH_E(client, c, s->clients);
    if (FD_ISSET(c->socket, & s->fds_read))
      serve_read_client(s, c);
    if (FD_ISSET(c->socket, & s->fds_write))
      serve_write_client(s, c);
    LLIST_FOREACH_DONE();

    if ((FD_ISSET(s->socket, & s->fds_read)) &&
	(! (s->flags & SERVE_FLAG_CLOSING))) {
      socket_new = accept(s->socket, (struct sockaddr*) (& s->address), & s->address_len);

      printf("connected client at %d\n", socket_new);

      c = client_new();
      c->flags |= CLIENT_FLAG_CONNECTED;
      c->socket = socket_new;
      c->index  = llist_push(s->clients, (void*) c);
    }

    if ((s->flags & SERVE_FLAG_CLOSING) &&
	(llist_isempty(s->clients))) {
      serve_close(s);
      break;
    }
  }

  return;
}

/* end serve */

#endif /* __serve_c__ */
