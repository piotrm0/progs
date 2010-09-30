#include "m_util.h"

v3* v3_new(float a, float b, float c) {
  v3* ret;
  ret = (v3*) malloc(sizeof(v3));
  ret->v[0] = a;
  ret->v[1] = b;
  ret->v[2] = c;
  return ret;
}

v3* v3_copy(v3* v) {
  v3* ret = v3_new(0,0,0);
  memcpy(ret->v, v->v, 3 * sizeof(float));
  return ret;
}

void v3_del(v3* v) {
  free(v->v);
  free(v);
}

void v3_mult(v3* v, float scalar) {
  v->v[0] *= scalar;
  v->v[1] *= scalar;
  v->v[2] *= scalar;
}

void v3_mult_add(v3* va, v3* vb, float scalar) {
  va->v[0] += vb->v[0] * scalar;
  va->v[1] += vb->v[1] * scalar;
  va->v[2] += vb->v[2] * scalar;
}

void v3_mult_sub(v3* va, v3* vb, float scalar) {
  va->v[0] -= vb->v[0] * scalar;
  va->v[1] -= vb->v[1] * scalar;
  va->v[2] -= vb->v[2] * scalar;
}

void v3_add(v3* va, v3* vb) {
  va->v[0] += vb->v[0];
  va->v[1] += vb->v[1];
  va->v[2] += vb->v[2];
}

void v3_sub(v3* va, v3* vb) {
  va->v[0] -= vb->v[0];
  va->v[1] -= vb->v[1];
  va->v[2] -= vb->v[2];
}

void v3_unit(v3* v) {
  float mag = v3_mag(v);
  if (mag == 0) return;
  mag = 1 / mag;
  v->v[0] *= mag;
  v->v[1] *= mag;
  v->v[2] *= mag;
}

float v3_mag(v3* v) {
  return (sqrt(v->v[0] * v->v[0] +
	       v->v[1] * v->v[1] +
	       v->v[2] * v->v[2]));
}

float v3_dot(v3* va, v3* vb) {
  return (va->v[0] * vb->v[0] + va->v[1] * vb->v[1] + va->v[2] * vb->v[2]);
}

float v3_dist(v3* va, v3* vb) {
  return sqrt((va->v[0] - vb->v[0]) * (va->v[0] - vb->v[0]) +
	      (va->v[1] - vb->v[1]) * (va->v[1] - vb->v[1]) +
	      (va->v[2] - vb->v[2]) * (va->v[2] - vb->v[2]));
}

char* v3_str(v3* v) {
  char* ret = (char*) malloc(64 * sizeof(char));
  sprintf(ret, "<%0.3f,%0.3f,%0.3f>", v->v[0], v->v[1], v->v[2]);
  return ret;
}
