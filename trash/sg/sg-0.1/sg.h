#ifndef __sg_h__
#define __sg_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "list.h"
#include "util.h"
#include "m_util.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

#define SG_TARGET_FPS 40
#define SG_TICKS      30 // number of frame timings used in average for fps control

typedef struct sg_image sg_image;
typedef struct sg_font sg_font;
typedef struct sg_type sg_type;
typedef struct sg_timing sg_timing;

typedef struct _sg_mouse _sg_mouse;

#define SG_TYPE_NONE        0
#define SG_TYPE_ROOT        1
#define SG_TYPE_WINDOW      2
#define SG_TYPE_WINDOW_HEAD 3
#define SG_TYPE_PANE        4
#define SG_TYPE_LABEL       5
#define SG_TYPE_VIEW        6
#define SG_TYPE_BUTTON      7

typedef struct sg_element sg_element;
  typedef struct sg_root   sg_root;
  typedef struct sg_window sg_window;
    typedef struct sg_window_head sg_window_head; // does not extend
  typedef struct sg_pane sg_pane;
    typedef struct sg_scroll sg_scroll; // does not extend
    typedef struct sg_view   sg_view;   // does not extend
  typedef struct sg_label  sg_label;
  typedef struct sg_button sg_button;

typedef struct _sg_callback _sg_callback;

typedef struct sg_event sg_event;
  typedef struct sg_event_mouse sg_event_mouse;
  typedef struct sg_event_keyboard sg_event_keyboard;
  typedef struct sg_event_drag sg_event_drag;

#include "sg_image.h"
#include "sg_timing.h"
#include "sg_type.h"
#include "sg_mouse.h"
#include "sg_element.h"
#include "sg_event.h"

sg_root* sg_init();
void sg_loop(sg_root *root);
u_int _sg_gl_init(sg_root *root);

/* misc */

void sg_recti_draw(recti* view);

/* end of misc */

/* misc for development only */

sg_image *temp;

/* end of misc for development only */

/* macros */

#define _SG_SUBVIEW_DEFINE(e, view, new) \
  recti* new;\
  new = recti_intersect_rel(view, e->geo);\

#define _SG_BOUNDS_DEFINE(e, view, left, top, right, bottom) \
  u_int top,right,bottom,left;\
  left   = e->geo->v[0] + view->v[0];\
  bottom = e->geo->v[1] + e->geo->v[3] + view->v[1];\
  right  = e->geo->v[0] + e->geo->v[2] + view->v[0];\
  top    = e->geo->v[1] + view->v[1];\

#define _SG_BOUNDS_VERTICES(left, top, right, bottom) \
  glVertex2i(left,  top);\
  glVertex2i(right, top);\
  glVertex2i(right, bottom);\
  glVertex2i(left,  bottom);\

#define _SG_BOUNDS_VERTICES_LINEFIX(left, top, right, bottom) \
  glVertex2i(left,  top);\
  glVertex2i(right+1, top-1);\
  glVertex2i(right, bottom);\
  glVertex2i(left,  bottom);\

/* end of macro */

#endif /* __sg_h__ */
