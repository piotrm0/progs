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
#include <pthread.h>
#include "util.h"
#include "list.h"

typedef struct client client;
typedef struct server server;
typedef struct servet servet; // (extend server)
typedef struct serve  serve;  // (extend server)

/* begin client */

#define CLIENT_FLAG_NONE      0
#define CLIENT_FLAG_CONNECTED 1
#define CLIENT_FLAG_VERIFIED  2
#define CLIENT_FLAG_AUTHED    4
#define CLIENT_FLAG_CLOSING   8

#define CLIENT_BUFFER_SIZE 1024

struct client {
  u_int flags;
  int socket;
  int id;
  lliste* indext;
  lliste* index;

  buffer* buffer_in;
  buffer* buffer_out;
  char* buffer_temp;
};

client* client_new();
void    client_del(client* c);

/* end client */


/* begin server */

#define SERVER_FLAG_NONE    0
#define SERVER_FLAG_STARTED 1
#define SERVER_FLAG_CLOSING 2

#define SERVER_STRUCT \
  u_int flags;\
  fd_set fds_read;\
  fd_set fds_write;\
  fd_set fds_error;\
  llist* clients;\
  char* proto;\

struct server {
  SERVER_STRUCT;
};

void   server_read_client(    server* s, client* c);
void   server_write_client(   server* s, client* c);
void   server_close_client(   server* s, client* c);
void   server_shutdown_client(server* s, client* c);
void   server_set_proto(      server* s, char* string);

/* end server */

/* begin servet */

struct servet {
  SERVER_STRUCT;

  pthread_t thread;
  u_int index;

  serve* parent;
};

servet* servet_new();
void    servet_del(servet* st);
void    servet_fork(servet* st);
void*   servet_create(void* arg);
void    servet_loop(servet* st);
void    servet_shutdown(servet* st);
void    servet_close(servet* st);

/* end servet */


/* begin serve */

#define SERVE_THREADED 1

#define SERVE_THREADS 4

#define SERVE_PORT_DEFAULT  7000
#define SERVE_PROTO_DEFAULT "serve/0.1"

#define SERVE_MAX_CONNS  128
#define SERVE_MAX_LISTEN 3

struct serve {
  SERVER_STRUCT;

  int socket;
  int port;
  struct sockaddr_in address;
  u_int              address_len;

#ifdef SERVE_THREADED
  servet** threads;
#endif

};

serve* serve_new();
void   serve_set_port(serve* s, int port);
void   serve_del(serve* s);
void   serve_start(serve* s);
void   serve_start_listen(serve* s);

void   serve_shutdown(serve* s);
void   serve_close(serve* s);
void   serve_loop(serve* s);

#ifdef SERVE_THREADED
void   serve_start_threads(serve* s);
servet* serve_least_thread(serve* s);
#endif

/* end serve */

#endif /* __serve_h__ */
