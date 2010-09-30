#ifndef __sg_timing_c__
#define __sg__c__

#include "sg.h"

/* sg_timing */

sg_timing* sg_timing_new() {
  sg_timing* ret;

  ret = (sg_timing*) malloc(sizeof(sg_timing));

  ret->target_tpf = 0;

  ret->tpf = ravg_new(SG_TICKS);
  ret->fps = ravg_new(SG_TICKS);

  ret->started_ticks   = 0;
  ret->abs_started_ticks = 0;

  ret->delay = 0;

  ret->target_tpf = 1000 / SG_TARGET_FPS;

  return ret;
}

void sg_timing_delay(sg_timing* t) {
  SDL_Delay(t->delay);
}

void sg_timing_mark_callback(sg_timing* t) {
  if (NULL != t->callback_frame)
    t->callback_frame(t->abs_elapsed_ticks, t->elapsed_ticks);
}

void sg_timing_mark(sg_timing* t) {
  int elapsed;
  int ended;

  assert(NULL != t);

  ended = SDL_GetTicks();
  elapsed = ended - t->started_ticks;
  t->started_ticks = ended;

  if (0 == t->abs_started_ticks) {
    t->abs_started_ticks = ended;
  }
  t->abs_elapsed_ticks = ended - t->abs_started_ticks;

  if (elapsed < 0) return;
  t->elapsed_ticks = (u_int) elapsed;

  ravg_mark(t->tpf, elapsed - t->delay);
  ravg_mark(t->fps, (float) 1000 / (float) elapsed);

  //  printf("sum: %f, avg: %3.3f + %d delay (target: %3.3f), fps: %3.3f\n", t->tpf->sum, t->tpf->avg, t->delay, t->target_tpf, t->fps->avg);

  t->delay = t->target_tpf - (u_int) t->tpf->avg;

  if (t->delay < 0)
    t->delay = 0;

  return;
}

/* end of sg_timing */

#endif /* __sg_timing_c__ */

