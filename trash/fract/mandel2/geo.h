#ifndef __geo_h__
#define __geo_h__

#include "m_util.h"

#define DIMS 3
#define AXIS DIMS * (DIMS - 1) / 2

enum OBJ_FLAGS {
  OBJ_POINT,
  OBJ_SPHERE,
  OBJ_PLANE,
  OBJ_BOX
};

enum PH_FLAGS {
  PH_STAT,
  PH_PART
};

typedef struct _ph {
  float c[DIMS]; // poistion
  float v[DIMS]; // velocity
  float r[AXIS]; // rotation
  float s[AXIS]; // spin
  float m; // mass
  u_int f; // flags
} ph;

typedef struct _obj {
  u_int f;
  ph physical;
} obj;

typedef struct _point {
} point;

typedef struct _sphere {
  float radius;
} sphere;

typedef struct _plane {
  float width;
  float height;
} plane;

typedef struct _box {
  float width;
  float height;
  float depth;
} box;

#endif /* __geo_h__ */
