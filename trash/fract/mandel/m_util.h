#ifndef __m_util_h__
#define __m_util_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

typedef struct _box2d {
  double* c;
  double* s;
} box2d;

box2d* box2d_new(double x, double y, double width, double height);
void box2d_del(box2d* b);
void box2d_zoom_fixed(box2d* b, double x, double y, double zoom);

typedef struct _lmap2d {
  box2d* from, *to;
  double* r;
} lmap2d;

lmap2d* lmap2d_new(box2d* from, box2d* to);
void lmap2d_del(lmap2d* m);
void lmap2d_comp(lmap2d* m);
void lmap2d_map_intd_doubled(lmap2d* m, int* from, double* to);

typedef struct _v3 {
  float v[3];
} v3;

extern const v3 v3_ux, v3_uy, v3_uz;

v3* v3_new(float, float, float);
v3* v3_copy(v3*);
void v3_del(v3*);

void v3_mult(v3*, float);
void v3_mult_add(v3* va, v3* vb, float scalar);
void v3_mult_sub(v3* va, v3* vb, float scalar);
void v3_add(v3*, v3*);
void v3_sub(v3*, v3*);

void v3_unit(v3*);

float v3_mag(const v3*);
float v3_dot(v3*, v3*);

void v3_rot(v3*, float*);
void v3_rot_v3(v3*, const v3*, float);

float v3_dist(v3* va, v3* vb);

char* v3_str(v3* v);

#endif /* __m_util_h__ */
