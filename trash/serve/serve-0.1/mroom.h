#ifndef __mroom_h__
#define __mroom_h__

#include <sys/types.h>
#include <stdlib.h>
#include "list.h"
#include "util.h"
#include "m_util.h"

typedef struct object object;
typedef struct room room;
typedef struct mroom mroom;

/* object */

#define OBJECT_TYPE_NONE   0
#define OBJECT_TYPE_USER   1
#define OBJECT_TYPE_BOX    2
#define OBJECT_TYPE_BULLET 3

#define OBJECT_FLAG_NONE 0

struct object {
  u_int type;
  u_int flags;
  int id;
  lliste* index;

  v2* pos;
  v2* vel;
};

/* end object */

/* room */

#define ROOM_FLAG_NONE 0

struct room {
  llist* objects;
  u_int flags;
};

room* room_new();
void  room_del(room* r);
void  room_tick(room* r, float dt);

/* end room */

/* mroom */

#define MROOM_FLAG_NONE 0

struct mroom {
  u_int flags;
  room* r;
};

mroom* mroom_new();
void   mroom_del(mroom* r);

/* end mroom */

#endif /* __mroom_h__ */
