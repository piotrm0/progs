#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <pthread.h>

//#define THREADED
#define THREADS 8

typedef struct _custom_cell_data {
  u_int state;
  u_int state_update;
  float activity;
  struct _custom_cell_data* neighbors[8];
} custom_cell_data;

custom_cell_data* cd;

#define GET_CD(w) cd = (custom_cell_data*) w->cd
#define CD(w) ((custom_cell_data*) w->cd)

#include "cell.h"

void button_handle(AG_Event* e) {
  printf("pressed\n");
}

inline void custom_color(cell* c) {
  if (((custom_cell_data*) c->cd)->state) {
    glColor3f(1.0, 1.0, 1.0);
  } else {
    glColor3f(0.0, 0.0, 0.0);
  }
}

inline void custom_reset_cell(cell* c) {
  CD(c)->state = rand() % 2;
  CD(c)->activity = 0.0;
}

inline void custom_init_cell(cell* c) {
  cd = malloc(sizeof(custom_cell_data));
  assert(NULL != cd);
  c->cd = cd;
  custom_reset_cell(c);
}

inline void custom_init_cells() {
  CELL_FOREACH(c, {
      custom_init_cell(c);
    });
  CELL_FOREACH(c, {
      cd = (custom_cell_data*) c->cd;
      cd->neighbors[0] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, 0, 1).cd);
      cd->neighbors[1] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, 0, -1).cd);
      cd->neighbors[2] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, 1, 0).cd);
      cd->neighbors[3] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, -1, 0).cd);
      cd->neighbors[4] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, 1, 1).cd);
      cd->neighbors[5] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, 1, -1).cd);
      cd->neighbors[6] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, -1, -1).cd);
      cd->neighbors[7] = (custom_cell_data*) (CELL_AT_OFFSET(c->x, c->y, -1, 1).cd);
    });
}

inline void custom_update_cell(u_int t, cell* c) {
  custom_cell_data* cd = (custom_cell_data*) c->cd;

  cd->state = cd->state_update;

  char* red = canvas + 3 * (c->x + c->y * WW);
  char* green = canvas + 3 * (c->x + c->y * WW) + 1;

  if (cd->state) {
    //c->color[0] = 1.0f;
    *red = (char) 50;
  } else {
    //c->color[0] = 0.0f;
    *red = (char) 0;
  }

  u_int a = cd->activity;
  if (a > 255) a = 255;

  *green = (char) a;
}

inline void custom_sample_cell(u_int t, cell* c) {
  u_int live = 0;
  u_int i;

  custom_cell_data* cd = (custom_cell_data*) c->cd;

  for (i = 0; i < 8; i++) {
    live += cd->neighbors[i]->state;
  }

  /*
  if (cd->state == 1) {
    if (live < 2) {
      cd->state_update = 0;
      cd->activity ++;
    } else if (live > 3) {
      cd->state_update = 0;
      cd->activity += 5;
    } else {
      cd->state_update = 1;
    }
  } else {
    if (live == 3) {
      cd->state_update = 1;
      cd->activity += 10;
    } else {
      cd->state_update = 0;
    }
    }*/

  if (live > 4) {
    cd->state_update = 1;
  } else if (live < 4) {
    cd->state_update = 0;
  } else {
    cd->state_update = cd->state;
  }

  if (random() % 10 == 0) {
    cd->state_update = (1 - cd->state_update);
  } else {
    if (cd->state_update != cd->state) {
      cd->activity += 10;
    }
  }

  //  if (random() % 10 == 0) {
  //    cd->state_update = (1 - cd->state_update);
  //  }

  cd->activity *= 0.99;
}

float temp = 2.0f;

void custom_init_gui() {
  printf("initing gui\n");

  cell_add_button("bam!", &button_handle);
  cell_add_float_input("float?", -10, 10, 1.5, 2, &temp);
}

inline void sample_range(u_int t, u_int xmin, u_int ymin, u_int xmax, u_int ymax) {
  u_int x,y;
//  printf("sampling range %d %d %d %d\n", xmin, ymin, xmax, ymax);
  for (x = xmin; x < xmax; x++) {
    for (y = ymin; y < ymax; y++) {
      custom_sample_cell(t, &(ws[x][y]));
    }
  }
}

inline void update_range(u_int t, u_int xmin, u_int ymin, u_int xmax, u_int ymax) {
  u_int x,y;
  for (x = xmin; x < xmax; x++) {
    for (y = ymin; y < ymax; y++) {
      custom_update_cell(t, &(ws[x][y]));
    }
  }
}

void* sample_thread (void* arg) {
  u_int bar = WH / THREADS;  
  u_int i = ((u_int) arg);
  sample_range(i, 0, i * bar, WW, (i+1) * bar);  

  return NULL;
}

void* update_thread (void* arg) {
  u_int bar = WH / THREADS;  
  u_int i = ((u_int) arg);
  update_range(i, 0, i * bar, WW, (i+1) * bar);  

  return NULL;
}

void custom_sample() {
#ifdef THREADED
  u_int i;
  pthread_t pth[THREADS];

  for (i = 0; i < THREADS; i++) {
    pthread_create(&(pth[i]), NULL, sample_thread, (void*) i);
  }
  for (i = 0; i < THREADS; i++) {
    pthread_join(pth[i], NULL);
  }

#else
  CELL_FOREACH(c, {
      custom_sample_cell(0, c);
    });
#endif
}

void custom_update() {
#ifdef THREADED
  u_int i;
  pthread_t pth[THREADS];

  for (i = 0; i < THREADS; i++) {
    pthread_create(&(pth[i]), NULL, update_thread, (void*) i);
  }
  for (i = 0; i < THREADS; i++) {
    pthread_join(pth[i], NULL);
  }
  printf("all done\n");
#else
  CELL_FOREACH(c, {
      custom_update_cell(0, c);
    });
#endif
}
