#ifndef __sg_element_h__
#define __sg_element_h__

#include "sg.h"
#include "sg_element.h"
#include "sg_event.h"

#define HEAD_HEIGHT 20
#define SCROLL_SIZE 20

#define SG_FLAG_NONE  0
#define SG_FLAG_OVER  1
#define SG_FLAG_FOCUS 2

#define SG_MAX_CALLBACKS 10

/* sg_element */

typedef void (*fun_render) (sg_element* e, recti* view);
typedef void (*fun_draw)   (sg_element* e, recti* view);
typedef void (*fun_over)   (sg_element* e, recti* view, vi2* point);
typedef void (*fun_set_geo)(sg_element* e, u_int, u_int, u_int, u_int);

#define _SG_ELEMENT_STRUCT \
  u_int type;\
  u_int flags;\
  struct sg_element *parent;\
  struct sg_element *focus;\
  struct sg_element *over;\
  sg_root *root;\
  recti *geo;\
  llist* children;\
  sg_element **subs;\
  fun_render  render;\
  fun_draw    draw;\
  fun_set_geo set_geo;\
  llist*   callbacks;\
  llist** vcallbacks;\

struct sg_element { 
  _SG_ELEMENT_STRUCT;
};

#define _SG_ELEMENT_FUNCS(typename) \
  sg_##typename* sg_##typename##_new(sg_root* root);\
  void sg_##typename##_del(sg_##typename* e);\
  void sg_##typename##_render(sg_##typename* e, recti* view);\
  void sg_##typename##_draw(sg_##typename* e, recti* view);\
  void sg_##typename##_clear_over(sg_##typename* e);\
  void sg_##typename##_set_geo(sg_##typename* e, u_int x, u_int y, u_int w, u_int h);\
  void sg_##typename##_over(sg_##typename* e, recti *view, vi2 *point);\
  int  sg_##typename##_add_callback(sg_##typename* e, u_int cover, fun_handler h);\
  void sg_##typename##_del_callback(sg_##typename* e, int cid);\
  void sg_##typename##_callback(sg_##typename* e, sg_event* event);\

_SG_ELEMENT_FUNCS(element);
void sg_element_scissor(sg_element* e, recti* view);

/* end of sg_element */

/* sg_root */

struct sg_root { // extend sg_element
  _SG_ELEMENT_STRUCT;

  _sg_mouse* mouse;
  sg_timing* timing;
  SDL_Surface* screen;
  sg_font*    font;
};

sg_root* _sg_root_new();

_SG_ELEMENT_FUNCS(root)
void sg_root_add_window(sg_root* root, sg_window* w);

/* end of sg_root */

/* sg_window */

#define SG_WINDOW_SUB_HEAD       0
#define SG_WINDOW_SUB_PANE       1

struct sg_window { // extend sg_element
  _SG_ELEMENT_STRUCT;
};

_SG_ELEMENT_FUNCS(window);

#define SG_WINDOW_HEAD_SUB_LABEL    0
#define SG_WINDOW_HEAD_SUB_ICON     1
#define SG_WINDOW_HEAD_SUB_MINIMIZE 2
#define SG_WINDOW_HEAD_SUB_MAXIMIZE 3
#define SG_WINDOW_HEAD_SUB_CLOSE    4

struct sg_window_head { // extend sg_element
  _SG_ELEMENT_STRUCT;
};

_SG_ELEMENT_FUNCS(window_head);


/* end of sg_window */

#define SG_PANE_OPTION_NONE    0
#define SG_PANE_OPTION_VSCROLL 1
#define SG_PANE_OPTION_HSCROLL 2

#define SG_PANE_SUB_VIEW    0
#define SG_PANE_SUB_VSCROLL 1
#define SG_PANE_SUB_HSCROLL 2

/* sg_pane */

struct sg_pane { // extend sg_element
  _SG_ELEMENT_STRUCT;

  u_int options;  // subset of SG_PANE_x
};

_SG_ELEMENT_FUNCS(pane);

struct sg_view { // extend sg_element
  _SG_ELEMENT_STRUCT;

  recti* vgeo;    // virtual geometry
};

_SG_ELEMENT_FUNCS(view);

/* end of sg_pane */

/* sg_label */

struct sg_label { // extend sg_element
  _SG_ELEMENT_STRUCT;

  char *string;
  sg_type *text;
};

_SG_ELEMENT_FUNCS(label);
void sg_label_set_string(sg_label *e, char* string);

/* end of sg_label */

/* sg_button */

#define SG_BUTTON_NONE  0
#define SG_BUTTON_TEXT  1
#define SG_BUTTON_IMAGE 2
#define SG_BUTTON_MISC  3

struct sg_button { // extend sg_element
  _SG_ELEMENT_STRUCT;

  u_int button_type;

  char *string;  // for SG_BUTTON_TEXT type
  sg_type *text; //
};

_SG_ELEMENT_FUNCS(button);
void sg_button_set_string(sg_button *e, char* string);

/* end of sg_label */

/* common element lines */

#define _SG_ELEMENT_INIT(ret, rootname, typename, typenamecap, num_subs) \
  sg_##typename* ret;\
  ret = (sg_##typename*) malloc(sizeof(sg_##typename));\
  ret->type = SG_TYPE_##typenamecap;\
  ret->parent = NULL;\
  ret->focus  = NULL;\
  ret->over   = NULL;\
  ret->root   = rootname;\
  ret->flags = SG_FLAG_NONE;\
  ret->geo  = recti_new(0,0,0,0);\
  ret->render  = (fun_render)  sg_##typename##_render;\
  ret->draw    = (fun_draw)    sg_##typename##_draw;\
  ret->set_geo = (fun_set_geo) sg_##typename##_set_geo;\
  ret->children = llist_new();\
  ret->subs = (sg_element**) calloc(num_subs, sizeof(sg_element*));

#define _SG_ELEMENT_DEL(e) \
  recti_del(e->geo);\
  llist_free(e->children);\
  free(e->subs);\
  free(e);\

#define _SG_RENDER_CHILDREN(ename, view) \
  void render(llist *l) {\
    sg_element* e2;\
    e2 = (sg_element*) l->e;\
    e2->render(e2, view);\
  }\
  llist_apply(ename->children, render);\

/* end of common element lines */

#endif /* __sg_element_h__ */
