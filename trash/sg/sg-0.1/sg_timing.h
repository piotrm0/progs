#ifndef __sg_timing_h__
#define __sg_timing_h__

#include "sg.h"

typedef void (*fun_timing_mark)(u_int from_start, u_int from_last); // timing from start, time

struct sg_timing {
  float target_tpf; // target ticks per frame

  u_int elapsed_ticks;
  u_int abs_elapsed_ticks;

  u_int started_ticks;     // number of ticks at last mark
  u_int abs_started_ticks; // number of ticks at the start of timing

  ravg* tpf;
  ravg* fps;

  int delay; // delay in ticks used to keep tpf at target or near it

  fun_timing_mark callback_frame;
};

sg_timing* sg_timing_new();
void sg_timing_delay(sg_timing *t);
void sg_timing_mark(sg_timing *t);
void sg_timing_mark_callback(sg_timing *t);

#endif /* __sg_timing_h__ */
