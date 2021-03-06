#include "m_util.h"

/* box2d */

box2d* box2d_new(double x, double y, double width, double height) {
  box2d* ret;
  ret = (box2d*) malloc(sizeof(box2d));
  ret->c = (double*) calloc(2, sizeof(double));
  ret->s = (double*) calloc(2, sizeof(double));
  ret->c[0] = x;
  ret->c[1] = y;
  ret->s[0] = width;
  ret->s[1] = height;
  return ret;
}

void box2d_del(box2d* b) {
  free(b->c);
  free(b->s);
  free(b);
}

void box2d_zoom_fixed(box2d* b, double x, double y, double zoom) {
  b->c[0] = x - (x - b->c[0]) / zoom;
  b->c[1] = y - (y - b->c[1]) / zoom;
  b->s[0] /= zoom;
  b->s[1] /= zoom;
}

/* end of box2d */

/* lmap2d */

lmap2d* lmap2d_new(box2d* from, box2d* to) {
  lmap2d* ret;
  ret = (lmap2d*) malloc(sizeof(lmap2d));
  ret->from = from;
  ret->to   = to;
  ret->r = (double*) calloc(2, sizeof(double));
  return ret;
}

void lmap2d_del(lmap2d* m) {
  free(m->r);
  free(m);
}

void lmap2d_comp(lmap2d* m) {
  m->r[0] = m->to->s[0] / m->from->s[0];
  m->r[1] = m->to->s[1] / m->from->s[1];
}

void lmap2d_map_intd_doubled(lmap2d* m, int* from, double* to) {
  u_int i;
  for (i = 0; i < 2; i++) {
    to[i] = ((double) from[i] - m->from->c[i]) * m->r[i] + m->to->c[i];
  }
}

/* end of lmap2d */

/* v3 */

const v3 v3_ux = {{1,0,0}};
const v3 v3_uy = {{0,1,0}};
const v3 v3_uz = {{0,0,1}};

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
  //  free(v->v);
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

float v3_mag(const v3* v) {
  return (sqrt(v->v[0] * v->v[0] +
	       v->v[1] * v->v[1] +
	       v->v[2] * v->v[2]));
}

float v3_dot(v3* va, v3* vb) {
  return (va->v[0] * vb->v[0] + va->v[1] * vb->v[1] + va->v[2] * vb->v[2]);
}

void v3_rot_v3(v3* va, const v3* vr, float angle) {
  float mag = v3_mag(vr);
  float mag2 = mag * mag;

  float x = va->v[0];
  float y = va->v[1];
  float z = va->v[2];

  float u = vr->v[0];
  float v = vr->v[1];
  float w = vr->v[2];
  float u2 = u * u;
  float v2 = v * v;
  float w2 = w * w;

  float c = cos(angle);
  float s = sin(angle);

  float m00 = (u2 + (v2 + w2) * c) / mag2;
  float m01 = (u * v * (1 - c) + w * mag * s) / mag2;
  float m02 = (v * w * (1 - c) - v * mag * s) / mag2;

  float m10 = (u * v * (1 - c) - w * mag * s) / mag2;
  float m11 = (v2 + (u2 + w2) * c) / mag2;
  float m12 = (v * w * (1 - c) + u * mag * s) / mag2;

  float m20 = (u * w * (1 - c) + v * mag * s) / mag2;
  float m21 = (v * w * (1 - c) - u * mag * s) / mag2;
  float m22 = (w2 + (u2 + v2) * c) / mag2;

  va->v[0] = m00 * x + m10 * y + m20 * z;
  va->v[1] = m01 * x + m11 * y + m21 * z;
  va->v[2] = m02 * x + m12 * y + m22 * z;
}

void v3_rot(v3* v, float* fv) {
  if (fv[0] != 0)
    v3_rot_v3(v, &v3_ux, fv[0]);
  if (fv[1] != 0)
    v3_rot_v3(v, &v3_uy, fv[1]);
  if (fv[2] != 0)
    v3_rot_v3(v, &v3_uz, fv[2]);
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

/* end of v3 */
