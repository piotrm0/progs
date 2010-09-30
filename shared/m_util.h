#ifndef __m_util_h__
#define __m_util_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

#include "util.h"

/* box2d */

typedef struct _box2d {
  double* c;
  double* s;
} box2d;

box2d* box2d_new(double x, double y, double width, double height);
void box2d_del(box2d* b);
void box2d_zoom_fixed(box2d* b, double x, double y, double zoom);

/* end box2d */

/* lmap2d */

typedef struct _lmap2d {
  box2d* from, *to;
  double* r;
} lmap2d;

lmap2d* lmap2d_new(box2d* from, box2d* to);
void lmap2d_del(lmap2d* m);
void lmap2d_comp(lmap2d* m);
void lmap2d_map_intd_doubled(lmap2d* m, int* from, double* to);

/* end lmap2d */

/* ravg */

typedef struct ravg ravg;

struct ravg {
  u_int size;
  u_int pos;
  float* vals;
  float sum;
  float avg;
};

ravg* ravg_new(u_int size);
void ravg_mark(ravg* r, float val);

/* end ravg */

/* recti */

typedef struct recti recti; // rectangle bounded by ints

struct recti {
  int v[4];
};

recti* recti_new(int x, int y, int w, int h);
recti* recti_copy(recti* r);
recti* recti_intersect(recti* a, recti* b);
recti* recti_intersect_rel(recti* a, recti* b); // b is assumed to be relative to a
void   recti_free(recti* r);

/* end of recti */

/* v1 */

typedef struct _v1 {
  float v[1];
} v1;

extern const v1 v1_ux;

v1* v1_new(float);
v1* v1_copy(v1*);
void v1_free(v1*);

char* v1_str(v1* v);

/* end v1 */

/* vi2 */

typedef struct vi2 vi2; // vector of 2 ints

struct vi2 { 
  int v[2];
};

extern const vi2 vi2_0, vi2_1, vi2_ux, vi2_uy; // zero vector, one vector, and unit vectors

vi2* vi2_new(int, int);
vi2* vi2_copy(vi2*);
void vi2_free(vi2*);

/* end vi2 */

/* v2 */

typedef struct v2  v2;  // vector of 2 floats

struct v2 {
  float v[2];
};

extern const v2 v2_0, v2_1, v2_ux, v2_uy; // zero vector, one vector, and unit vectors

v2* v2_new(float, float);
v2* v2_copy(v2*);
void v2_free(v2*);

/* end v2 */

/* v3 */

typedef struct v3  v3;  // vector of 3 floats

struct v3 {
  float v[3];
};

extern const v3 v3_0, v3_1, v3_ux, v3_uy, v3_uz; // zero vector, one vector, and unit vectors

v3* v3_new(float, float, float);
v3* v3_copy(v3*);
void v3_copy_from(v3* v, v3* source);
void v3_free(v3*);

void v3_mult(v3*, float);
void v3_mult_add(v3* va, v3* vb, float scalar);
void v3_mult_sub(v3* va, v3* vb, float scalar);
void v3_add(v3*, v3*);
void v3_sub(v3*, v3*);

void v3_unit(v3*);

float v3_mag(const v3*);
float v3_dot(v3*, v3*);
void v3_cross(v3* va, v3* vb);

void v3_rot(v3*, float*);
void v3_rot_v3(v3*, const v3*, float);

float v3_dist(v3* va, v3* vb);

char* v3_str(v3* v);

void v3_rand(v3* v, float mag);
void v3_set(v3* v, float set);
void v3_set_all(v3* v, float set1, float set2, float set3);
void v3_bound(v3* v, float bound);

/* end v3 */

#endif /* __m_util_h__ */
