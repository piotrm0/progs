#include "col.h"

col* col_new() {
  MALLOC(ret, col);

  ret->num_fails = 0;

  COL_LOOP_ELE
    (i,
     ret->color[i] = 0;
     ret->fail[i]  = 0;
     );

  return ret;
}

void col_free(col* c) {
  free(c);
}

void col_print(col* c) {
  COL_LOOP_COORD
    (x,y,
     printf("C: "),
     printf("%d ",COL_GET_COLOR(c,x,y)),
     printf("\n")
     );
}

void col_print_info(col* c) {
  printf("%d x %d, colors: %d, fitness: %d, fails: %d\n", COL_HEIGHT, COL_WIDTH, COL_COLORS, c->fitness, c->num_fails);
}

void col_print_fail(col* c) {
  COL_LOOP_COORD
    (x,y,
     printf("F: "),
     printf("%d ",COL_GET_FAIL(c,x,y)),
     printf("\n")
     );
}

void col_random(col* c) {
  COL_LOOP_ELE
    (i,
     c->color[i] = rand() % COL_COLORS
     );
  col_fill_fitness(c);
}

void col_fill_fitness(col* c) {
  c->fitness = 0;

  COL_LOOP_RECT(x1,y1,x2,y2,
		c->fitness += COL_FITNESS(c,x1,y1,x2,y2);
		);
}

void col_fill_fails(col* c) {
  c->num_fails = 0;

  COL_LOOP_RECT(x1,y1,x2,y2,
		if (COL_IS_FAIL(c,x1,y1,x2,y2)) {
		  c->fail[COL_INDEX(x1,y1)] ++;
		  c->num_fails ++;
		}		
		);

  /* COL_LOOP_COORD
    (x1,y1,
     PASS,
     COL_SET_FAIL(c,x1,y1,0);
     COL_LOOP_COORD(x2,y2,
		    if (y1 == y2) goto outer,

		    if (x1 == x2) goto inner;
		    if (COL_IS_FAIL(c,x1,y1,x2,y2)) {
		      c->fail[COL_INDEX(x1,y1)] ++;
		      c->num_fails ++;
		    },
		    PASS
		    );
     ,
     PASS); */
}

col* col_copy(col* c) {
  MALLOC(ret, col);
  memcpy(ret, c, sizeof(col));
  return ret;
}

void col_shuffle_rows(col* c) {
  u_int copy_size = COL_WIDTH * sizeof(u_int);
  u_int* temp_row = (u_int*) malloc(copy_size);

  u_int* perm = (u_int*) malloc(COL_HEIGHT * sizeof(u_int));

  u_int i;
  u_int r;
  u_int t;

  u_int* r1;
  u_int* r2;

  for (i = 0; i < COL_HEIGHT; i++) {
    perm[i] = i;
  }

  for (i = 0; i < COL_HEIGHT - 1; i++) {
    r = rand() % (COL_HEIGHT - i) + i;
  
    /*
    t = perm[i];
    perm[i] = perm[r];
    perm[r] = t;
    }*/

    r1 = c->color + COL_WIDTH * i;
    r2 = c->color + COL_WIDTH * r;

    //printf("c=%d size=%d copy_size=%d swapping rows %d (%d) and %d (%d) diff=%d\n", c, sizeof(u_int), copy_size,i, r1, r, r2, r1-r2);
  
    memcpy(temp_row, r1, copy_size);
    memcpy(r1, r2, copy_size);
    memcpy(r2, temp_row, copy_size);
  }

  free(perm);
  free(temp_row);
}

col* col_g1search(u_int iters, col* start) {
  __label__ bail_search, bail_step;

  u_int iter = 0;
  u_int* step;
  col* current;
  col* best;
  col* temp;

  u_int last_fitness;
  u_int last_num_fails;

  current = col_copy(start);

  last_fitness   = -1;
  last_num_fails = -1;

  col_fill_fitness(current);
  col_fill_fails(current);
 
  for (; iter < iters && current->num_fails != 0; iter++) {
    printf("iteration %d\n", iter);
    col_print_info(current);
    col_print(current);

    STEP_D1_START(step);

    best = col_copy(current);

    while(step) {
      temp = col_copy(current);

      STEP_D1_APPLY(temp, step);

      if (temp->fitness < best->fitness) {
	col_free(best);
	best = temp;

	STEP_D1_END(step);
	goto bail_step;
      } else {
	col_free(temp);
      }

      STEP_D1_NEXT(step);
    }

  bail_step:

    col_free(current);
    current = best;

    if ((current->num_fails == last_num_fails) &&
	(current->fitness   == last_fitness)) {
      goto bail_search;
    }

    last_fitness   = current->fitness;
    last_num_fails = current->num_fails;
  }

 bail_search:

  return current;
}

#define COL_DEPTH 2

col* col_gnsearch(u_int iters, col* start) {
  //  __label__ bail_search, bail_step;
  __label__ bail_step;

  u_int iter = 0;
  u_int* step;
  u_int* step_max;
  col* current;
  col* best;
  col* temp;

  u_int last_fitness;
  u_int last_num_fails;

  current = col_copy(start);

  last_fitness   = -1;
  last_num_fails = -1;

  col_fill_fitness(current);
  col_fill_fails(current);

  u_int depth = 1;
 
  for (; iter < iters && current->num_fails != 0; iter++) {
    printf("iteration %d\n", iter);
    col_print_info(current);
    col_print(current);

    STEP_DN_START(step, step_max, depth);

    best = col_copy(current);

    while(step) {
      STEP_DN_PRINT(step, depth);

      temp = col_copy(current);

      STEP_DN_APPLY(temp, step, depth);

      if ((temp->num_fails < best->num_fails) ||
	  ((temp->num_fails == best->num_fails) && ((temp->fitness < best->fitness)))) {
	col_free(best);
	best = temp;

	STEP_DN_END(step, step_max, depth);

	depth = 1;

	goto bail_step;
      } else {
	col_free(temp);
      }

      STEP_DN_NEXT(step, step_max, depth);
    }

    printf("\n");

  bail_step:

    col_free(current);
    current = best;

    if ((current->num_fails == last_num_fails) &&
	(current->fitness   == last_fitness)) {
      //goto bail_search;
      depth ++;
    }

    last_fitness   = current->fitness;
    last_num_fails = current->num_fails;
  }

  //bail_search:;

  return current;
}

//#define PRINT_STEP 1

col* col_sq1search(u_int iters, col* start) {
  u_int iter = 0;
  u_int* step;
  u_int* step_max;
  col* current;
  col* best;
  col* temp;

  u_int last_fitness;
  u_int last_num_fails;

  current = col_copy(start);

  last_fitness   = -1;
  last_num_fails = -1;

  col_fill_fitness(current);
  col_fill_fails(current);

  for (; iter < iters && current->num_fails != 0; iter++) {
    printf("iteration %d\n", iter);
    col_print_info(current);
    col_print(current);

    STEP_SQ1_START(step, step_max);

    best = col_copy(current);

    while(step) {
      #ifdef PRINT_STEP
      STEP_SQ1_PRINT(step);
      #endif

      temp = col_copy(current);

      STEP_SQ1_APPLY(temp, step);

      if ((temp->num_fails < best->num_fails) ||
      	  ((temp->num_fails == best->num_fails) && ((temp->fitness < best->fitness)))) {
	//if (temp->fitness < best->fitness) {
	col_free(best);
	best = temp;

	STEP_SQ1_END(step, step_max);

	goto bail_step;
      } else {
	col_free(temp);
      }

      STEP_SQ1_NEXT(step, step_max);
    }

  bail_step:

    printf("\n");

    col_free(current);
    current = best;

    //col_print(current);
    col_shuffle_rows(current);
    //printf("---\n");
    //col_print(current);

    u_int check_fails = current->num_fails;
    col_fill_fails(current);
    
    assert(current->num_fails == check_fails);

    if ((current->num_fails == last_num_fails) &&
	(current->fitness   == last_fitness)) {
      goto bail_search;
    }

    last_fitness   = current->fitness;
    last_num_fails = current->num_fails;
  }

 bail_search:;

  return current;
}
