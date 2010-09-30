#include "m_util.h"

/* ravg */

ravg* ravg_new(u_int size) {
  ravg* ret;
  u_int i;

  ret = (ravg*) malloc(sizeof(ravg));

  ret->size = size;
  ret->pos  = 0;
  ret->sum = 0;
  ret->avg = 0;
  ret->vals = (float*) calloc(size, sizeof(float));

  for (i = 0; i < size; i++)
    ret->vals[i] = 0;

  return ret;
}

void ravg_mark(ravg* r, float val) {
  r->sum -= r->vals[r->pos];
  r->sum += val;
  r->vals[r->pos] = val;
  r->pos++;
  r->pos%= r->size;

  r->avg = r->sum / (float) r->size;

  return;
}

/* end of ravg */

/* recti */

recti* recti_new(int x, int y, int w, int h) {
  recti* ret;

  ret = (recti*) malloc(sizeof(recti));

  ret->v[0] = x;
  ret->v[1] = y;
  ret->v[2] = w;
  ret->v[3] = h;

  return ret;
}

recti* recti_intersect(recti* a, recti* b) {
  recti* ret;

  int axend, bxend;
  int ayend, byend;

  ret = recti_new(0,0,0,0);

  ret->v[0] = (a->v[0] > b->v[0] ? a->v[0] : b->v[0]);
  ret->v[1] = (a->v[1] > b->v[1] ? a->v[1] : b->v[1]);

  axend = a->v[0] + a->v[2];
  ayend = a->v[1] + a->v[3];
  bxend = b->v[0] + b->v[2];
  byend = b->v[1] + b->v[3];

  ret->v[2] = (axend > bxend ? bxend - ret->v[0] : axend - ret->v[0]);
  ret->v[3] = (ayend > byend ? byend - ret->v[1] : ayend - ret->v[1]);

  return ret;
}

recti* recti_intersect_rel(recti* a, recti* b) {
  recti* ret;

  int axend, bxend;
  int ayend, byend;

  ret = recti_new(0,0,0,0);

  ret->v[0] = (a->v[0] > b->v[0] + a->v[0] ? a->v[0] : b->v[0] + a->v[0]);
  ret->v[1] = (a->v[1] > b->v[1] + a->v[0] ? a->v[1] : b->v[1] + a->v[1]);

  axend = a->v[0] + a->v[2];
  ayend = a->v[1] + a->v[3];
  bxend = b->v[0] + b->v[2] + a->v[0];
  byend = b->v[1] + b->v[3] + a->v[1];

  ret->v[2] = (axend > bxend ? bxend - ret->v[0] : axend - ret->v[0]);
  ret->v[3] = (ayend > byend ? byend - ret->v[1] : ayend - ret->v[1]);

  return ret;
}

recti* recti_copy(recti* v) {
  recti *ret = recti_new(0,0,0,0);
  memcpy(ret->v, v->v, 4 * sizeof(int));

  return ret;
}

void recti_free(recti* r) {
  free(r);

  return;
}

/* end of recti */

/* v1 */

const v1 v1_ux = {{1}};

v1* v1_new(float a) {
  v1* ret;
  ret = (v1*) malloc(sizeof(v1));
  ret->v[0] = a;
  return ret;
}

v1* v1_copy(v1* v) {
  v1* ret = v1_new(0);
  memcpy(ret->v, v->v, 1 * sizeof(float));
  return ret;
}

void v1_free(v1* v) {
  free(v);
}

char* v1_str(v1* v) {
  char* ret = (char*) malloc(16 * sizeof(char));
  sprintf(ret, "<%0.3f>", v->v[0]);
  return ret;
}

/* vi2 */

const vi2 vi2_0 = {{0,0}};
const vi2 vi2_1 = {{1,1}};
const vi2 vi2_ux = {{1,0}};
const vi2 vi2_uy = {{0,1}};

vi2* vi2_new(int a, int b) {
  vi2* ret;
  ret = (vi2*) malloc(sizeof(vi2));
  ret->v[0] = a;
  ret->v[1] = b;

  return ret;
}

vi2* vi2_copy(vi2* v) {
  vi2* ret = vi2_new(0,0);
  memcpy(ret->v, v->v, 2 * sizeof(int));

  return ret;
}

void vi2_free(vi2* v) {
  free(v);

  return;
}

/* end of vi2 */

/* v2 */

const v2 v2_0 = {{0,0}};
const v2 v2_1 = {{1,1}};
const v2 v2_ux = {{1,0}};
const v2 v2_uy = {{0,1}};

v2* v2_new(float a, float b) {
  v2* ret;
  ret = (v2*) malloc(sizeof(v2));
  ret->v[0] = a;
  ret->v[1] = b;

  return ret;
}

v2* v2_copy(v2* v) {
  v2* ret = v2_new(0,0);
  memcpy(ret->v, v->v, 2 * sizeof(float));

  return ret;
}

void v2_free(v2* v) {
  free(v);

  return;
}

/* end of v2 */

/* vi3 */

const v3 v3_0 = {{0,0,0}};
const v3 v3_1 = {{1,1,1}};
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

void v3_free(v3* v) {
  free(v);

  return;
}

void v3_mult(v3* v, float scalar) {
  v->v[0] *= scalar;
  v->v[1] *= scalar;
  v->v[2] *= scalar;

  return;
}

void v3_mult_add(v3* va, v3* vb, float scalar) {
  va->v[0] += vb->v[0] * scalar;
  va->v[1] += vb->v[1] * scalar;
  va->v[2] += vb->v[2] * scalar;

  return;
}

void v3_mult_sub(v3* va, v3* vb, float scalar) {
  va->v[0] -= vb->v[0] * scalar;
  va->v[1] -= vb->v[1] * scalar;
  va->v[2] -= vb->v[2] * scalar;

  return;
}

void v3_add(v3* va, v3* vb) {
  va->v[0] += vb->v[0];
  va->v[1] += vb->v[1];
  va->v[2] += vb->v[2];

  return;
}

void v3_sub(v3* va, v3* vb) {
  va->v[0] -= vb->v[0];
  va->v[1] -= vb->v[1];
  va->v[2] -= vb->v[2];

  return;
}

void v3_unit(v3* v) {
  float mag = v3_mag(v);
  if (mag == 0) return;
  mag = 1 / mag;
  v->v[0] *= mag;
  v->v[1] *= mag;
  v->v[2] *= mag;

  return;
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

  return;
}

void v3_rot(v3* v, float* fv) {
  if (fv[0] != 0)
    v3_rot_v3(v, &v3_ux, fv[0]);
  if (fv[1] != 0)
    v3_rot_v3(v, &v3_uy, fv[1]);
  if (fv[2] != 0)
    v3_rot_v3(v, &v3_uz, fv[2]);

  return;
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

#define v3_rand_single(vect,i,mag)			\
  vect->v[i] = frandom() * mag * 2 - mag;		\

void v3_rand(v3* v, float mag) {
  v3_rand_single(v,0,mag);
  v3_rand_single(v,1,mag);
  v3_rand_single(v,2,mag);
}

#define v3_bound_single(vect,i,bound)		\
  if (vect->v[i] < -bound)			\
    vect->v[i] = -bound;			\
  else if (vect->v[i] > bound)			\
    vect->v[i] = bound;				\

void v3_bound(v3* v, float bound) {
  v3_bound_single(v, 0, bound);
  v3_bound_single(v, 1, bound);
  v3_bound_single(v, 2, bound);
}

void v3_set(v3* v, float set) {
  v->v[0] = set;
  v->v[1] = set;
  v->v[2] = set;
}

#ifdef HAVE_OPENGL
void v3_vertex_gl_translate(v3* v) {
  glTranslatef(v->v[0], v->v[1], v->v[2]);
}

void v3_vertex_gl(v3* v) {
  glVertex3f(v->v[0], v->v[1], v->v[2]);
}

void v3_draw_gl(v3* v) {
  glBegin(GL_LINES);
  glVertex3f(v->v[0]-1, v->v[1], v->v[2]);
  glVertex3f(v->v[0]+1, v->v[1], v->v[2]);
  glVertex3f(v->v[0], v->v[1]-1, v->v[2]);
  glVertex3f(v->v[0], v->v[1]+1, v->v[2]);
  glVertex3f(v->v[0], v->v[1], v->v[2]-1);
  glVertex3f(v->v[0], v->v[1], v->v[2]+1);
  glEnd();
}
#endif

/* end of vi3 */
