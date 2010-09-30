#ifndef __serve_h__
#define __serve_h__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "util.h"
#include "list.h"

/* client */

#define CLIENT_FLAG_NONE      0
#define CLIENT_FLAG_CONNECTED 1
#define CLIENT_FLAG_VERIFIED  2
#define CLIENT_FLAG_AUTHED    4
#define CLIENT_FLAG_CLOSING   8

#define CLIENT_BUFFER_SIZE 1024

typedef struct _client {
  u_int flags;
  int socket;
  int id;
  lliste* index;

  buffer* buffer_in;
  buffer* buffer_out;
  char* buffer_temp;

} client;

client* client_new();
void    client_del(client* c);

/* end client */

/* serve */

#define SERVE_PORT_DEFAULT  7000
#define SERVE_PROTO_DEFAULT "serve/0.1"

#define SERVE_FLAG_NONE    0
#define SERVE_FLAG_STARTED 1
#define SERVE_FLAG_CLOSING 2

#define SERVE_MAX_CONNS  128
#define SERVE_MAX_LISTEN 3

typedef struct _serve {
  u_int flags;

  int socket;
  int port;
  struct sockaddr_in address;
  u_int              address_len;
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_error;

  char* proto;

  llist* clients;

} serve;

serve* serve_new();
void   serve_set_port(serve* s, int port);
void   serve_set_proto(serve* s, char* string);
void   serve_del(serve* s);
void   serve_start(serve* s);
void   serve_shutdown(serve* s);
void   serve_close(serve* s);
void   serve_loop(serve* s);

void   serve_close_client(serve* s, client* c);
void   serve_shutdown_client(serve* s, client* c);
void   serve_read_client(serve* s, client* c);

/* end serve */

#endif /* __serve_h__ */
