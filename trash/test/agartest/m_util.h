#ifndef __m_util_h__
#define __m_util_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

typedef struct ravg ravg; // running average
typedef struct recti recti; // rectangle bounded by ints
typedef struct vi2 vi2; // vector of 2 ints
typedef struct v2  v2;  // vector of 2 floats
typedef struct v3  v3;  // vector of 3 floats

/* ravg */

struct ravg {
  u_int size;
  u_int pos;
  float* vals;
  float sum;
  float avg;
};

ravg* ravg_new(u_int size);
void ravg_mark(ravg* r, float val);

/* end of ravg */

/* recti */

struct recti {
  int v[4];
};

recti* recti_new(int x, int y, int w, int h);
recti* recti_copy(recti* r);
recti* recti_intersect(recti* a, recti* b);
recti* recti_intersect_rel(recti* a, recti* b); // b is assumed to be relative to a
void   recti_del(recti* r);

/* end of recti */

// ### v1 ###

typedef struct _v1 {
  float v[1];
} v1;

extern const v1 v1_ux;

v1* v1_new(float);
v1* v1_copy(v1*);
void v1_del(v1*);

char* v1_str(v1* v);

/* vi2 */

struct vi2 { 
  int v[2];
};

extern const vi2 vi2_0, vi2_1, vi2_ux, vi2_uy; // zero vector, one vector, and unit vectors

vi2* vi2_new(int, int);
vi2* vi2_copy(vi2*);
void vi2_del(vi2*);

/* end of vi2 */

/* v2 */

struct v2 {
  float v[2];
};

extern const v2 v2_0, v2_1, v2_ux, v2_uy; // zero vector, one vector, and unit vectors

v2* v2_new(float, float);
v2* v2_copy(v2*);
void v2_del(v2*);

/* end of v2 */

/* v3 */

struct v3 {
  float v[3];
};

extern const v3 v3_0, v3_1, v3_ux, v3_uy, v3_uz; // zero vector, one vector, and unit vectors

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

/* end of v3 */

#endif /* __m_util_h__ */
