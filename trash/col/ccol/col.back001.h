#ifndef __col_h__
#define __col_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include "util.h"

#define COL_WIDTH  15
#define COL_HEIGHT 15
#define COL_ELES   COL_WIDTH * COL_HEIGHT

#define COL_COLORS 4

struct col {
  u_int num_fails;
  u_int fitness;
  u_int color[COL_ELES];
  u_int fail[COL_ELES];
};

typedef struct col col;

#define PASS

#define COL_INDEX(x,y)				\
  COL_HEIGHT * y + x				\

#define COL_GET_FAIL(c,x,y)			\
  c->fail[COL_INDEX(x,y)]			\
  
#define COL_SET_FAIL(c,x,y,failval)		\
  c->fail[COL_INDEX(x,y)] = failval		\

#define COL_FITNESS_SELECT_MAX(cnt)		\
  ({						\
    u_int cj = 0;				\
    u_int temp;					\
    u_int ret;					\
    temp = 0;					\
    for (cj; cj < COL_COLORS; cj++) {		\
      if (cnt[cj] > cnt[temp]) {		\
	temp = cj;				\
      }						\
    }						\
    ret = cnt[temp];				\
    cnt[temp] = 0;				\
    ret;					\
  })						\

#define COL_FITNESS(c,x1,y1,x2,y2)					\
  ({									\
    u_int cnt[COL_COLORS];						\
    u_int ci  = 0;							\
    u_int ret = 0;							\
    									\
    for (; ci < COL_COLORS; ci++)					\
      cnt[ci] = 0;							\
									\
    cnt[c->color[COL_INDEX(x1,y1)]]++;					\
    cnt[c->color[COL_INDEX(x1,y2)]]++;					\
    cnt[c->color[COL_INDEX(x2,y1)]]++;					\
    cnt[c->color[COL_INDEX(x2,y2)]]++;					\
									\
    u_int big1 = COL_FITNESS_SELECT_MAX(cnt);				\
    u_int big2 = COL_FITNESS_SELECT_MAX(cnt);				\
									\
    if (big1 == 4) {							\
      ret = 32;								\
    } else if (big1 == 3) {						\
      ret = 2;								\
    } else if ((big1 == 2) & (big2 == 2)) {				\
      ret = 4;								\
    } else if (big1 == 2) {						\
      ret = 8;								\
    } else {								\
      ret = 16;								\
    }									\
    ret;								\
  })									\

#define COL_IS_FAIL(c,x1,y1,x2,y2)					\
  ((c->color[COL_INDEX(x1,y1)] ==					\
    c->color[COL_INDEX(x1,y2)]) &&					\
   (c->color[COL_INDEX(x1,y1)] ==					\
    c->color[COL_INDEX(x2,y1)]) &&					\
   (c->color[COL_INDEX(x1,y1)] ==					\
    c->color[COL_INDEX(x2,y2)]) ? 1 : 0)				\

#define COL_GET_COLOR(c,x,y)						\
  c->color[COL_INDEX(x,y)]						\

#define COL_SET_COLOR(c,x1,y1,colval)					\
  {									\
    u_int oldval = c->color[COL_INDEX(x1,y1)];				\
    COL_LOOP_COORD(x2,y2,						\
		   if (y1 == y2) goto outer,				\
		   if (x1 == x2) goto inner;				\
									\
		   u_int old_fitness = COL_FITNESS(c,x1,y1,x2,y2);	\
		   u_int old_fail    = COL_IS_FAIL(c,x1,y1,x2,y2);	\
		   c->color[COL_INDEX(x1,y1)] = colval;			\
		   u_int new_fitness = COL_FITNESS(c,x1,y1,x2,y2);	\
		   u_int new_fail    = COL_IS_FAIL(c,x1,y1,x2,y2);	\
									\
		   c->fitness   += new_fitness - old_fitness;		\
		   c->num_fails += new_fail    - old_fail;		\
									\
		   c->color[COL_INDEX(x1,y1)] = oldval;			\
		   ,							\
		   PASS);						\
    c->color[COL_INDEX(x1,y1)] = colval;				\
  }									\
		 
#define COL_LOOP_ELE(i,exp)			\
  u_int i;					\
  for (i = 0; i < COL_ELES; i++) {		\
    exp;					\
  }						\

#define COL_LOOP_COORD(x,y, exp_row, exp_coord, exp_row_end)	\
  u_int x;							\
  u_int y = 0;							\
  for (; y < COL_HEIGHT; y++) {					\
    __label__ outer, inner;					\
    exp_row;							\
    for (x = 0; x < COL_WIDTH; x++) {				\
      exp_coord;						\
    inner: continue;						\
      goto inner;						\
    }								\
    exp_row_end;						\
  outer: continue;						\
    goto outer;							\
  }								\

#define COL_LOOP_RECT(x1,y1,x2,y2, exp)					\
  u_int y1,x1,y2,x2;							\
  for (y1 = 0; y1 < COL_HEIGHT; y1++) {					\
    for (x1 = 0; x1 < COL_WIDTH; x1++) {				\
      for (y2 = y1 + 1; y2 < COL_HEIGHT; y2++) {			\
	for (x2 = x1 + 1; x2 < COL_WIDTH; x2++) {			\
	  exp;								\
	}								\
      }									\
    }									\
  }									\

#define STEP_SQ1_APPLY(c,s)			\
  COL_SET_COLOR(c, s[4], s[6], s[0]);		\
  COL_SET_COLOR(c, s[4], s[7], s[1]);		\
  COL_SET_COLOR(c, s[5], s[6], s[2]);		\
  COL_SET_COLOR(c, s[5], s[7], s[3]);		\

#define STEP_SQ1_START(s,sm)					\
  s  = (u_int*) malloc(8 * sizeof(u_int));			\
  sm = (u_int*) malloc(8 * sizeof(u_int));			\
  s[0] = 0;							\
  s[1] = 0;							\
  s[2] = 0;							\
  s[3] = 0;							\
  s[4] = 1;							\
  s[5] = 0;							\
  s[6] = 1;							\
  s[7] = 0;							\
  sm[0] = COL_COLORS;						\
  sm[1] = COL_COLORS;						\
  sm[2] = COL_COLORS;						\
  sm[3] = COL_COLORS;						\
  sm[4] = COL_WIDTH;						\
  sm[5] = COL_WIDTH-1;						\
  sm[6] = COL_HEIGHT;						\
  sm[7] = COL_HEIGHT-1;						\

#define STEP_SQ1_END(s,sm)				\
  free(s);						\
  free(sm);						\
  s = NULL;						\

#define STEP_SQ1_PRINT(s)					\
  {								\
    u_int i = 0;						\
    printf("\r");						\
    for (; i < 8; i++) {					\
      printf("%d\t", s[i]);					\
    }								\
  }								\
    

#define STEP_SQ1_NEXT(s,sm)			\
  {__label__ bail;				\
    u_int pos = 0;				\
    s[pos] ++;					\
						\
    while(s[pos] >= sm[pos]) {			\
      if ((pos == 4) || (pos == 6)) {		\
	if (s[pos+1] != sm[pos+1]-1) {		\
	  s[pos] = s[pos+1] + 2;		\
	} else {				\
	  s[pos] = 1;				\
	}					\
      } else {					\
	s[pos] = 0;				\
      }						\
      pos++;					\
      if (pos > 7) {				\
	STEP_SQ1_END(s,sm);			\
	goto bail;				\
      }						\
      s[pos] ++;				\
    }						\
  bail:;					\
  }						\

#define STEP_MAX_D 4

#define STEP_DN_APPLY(c,s,d)				\
  {							\
    u_int i = 0;					\
    for (i; i < d; i++) {				\
      COL_SET_COLOR(c, s[1+i*3], s[2+i*3], s[i*3]);	\
    }							\
  }							\

#define STEP_DN_START(s,sm,d)				\
  u_int i = 0;						\
  s  = (u_int*) malloc(3 * d * sizeof(u_int));		\
  sm = (u_int*) malloc(3 * d * sizeof(u_int));		\
  for (i; i < d; i++) {					\
    sm[  i*3] = COL_COLORS;				\
    sm[1+i*3] = COL_WIDTH;				\
    sm[2+i*3] = COL_HEIGHT;				\
    s[  i*3] = 0;					\
    s[1+i*3] = 0;					\
    s[2+i*3] = 0;					\
  }							\

#define STEP_DN_END(s,sm,d)				\
  free(s);						\
  free(sm);						\
  s = NULL;						\

#define STEP_DN_PRINT(s,d)					\
  {								\
    u_int i = 0;						\
    printf("\r");						\
    for (; i < d; i++) {					\
      printf("[%d %d \t %d] \t", s[3*i], s[1+3*i], s[2+3*i]);	\
    }								\
  }								\

#define STEP_DN_NEXT(s,sm,d)			\
  {__label__ bail;				\
    u_int pos = 0;				\
    s[pos] ++;					\
    while(s[pos] >= sm[pos]) {			\
      s[pos] = 0;				\
      pos++;					\
      s[pos] ++;				\
      if (pos >= 3 * d) {			\
	STEP_DN_END(s,sm,d);			\
	goto bail;				\
      }						\
    }						\
  bail:;					\
  }						\

#define STEP_D1_APPLY(c,s)			\
  COL_SET_COLOR(c, s[1],s[2],s[0]);		\

#define STEP_D1_START(s)			\
  s = (u_int*) malloc(3 * sizeof(u_int));	\
  s[0] = 0;					\
  s[1] = 0;					\
  s[2] = 0;					\

#define STEP_D1_END(s)				\
  free(s);					\
  s = NULL;					\

#define STEP_D1_NEXT(s)				\
  s[0] ++;					\
  if (s[0] >= COL_COLORS) {			\
    s[0] = 0;					\
    s[1] ++;					\
    if (s[1] >= COL_WIDTH) {			\
      s[1] = 0;					\
      s[2] ++;					\
      if (s[2] >= COL_HEIGHT) {			\
	free(s);				\
	s = NULL;				\
      }						\
    }						\
  }						\

col* col_new();
void col_free(col* c);
void col_print(col* c);
void col_print_info(col* c);
void col_print_fail(col* c);
void col_random(col* c);
void col_fill_fails(col* c);

col* col_g1search(u_int iters, col* start);
col* col_gnsearch(u_int iters, col* start);
col* col_sq1search(u_int iters, col* start);

#endif /* __col_h__ */
