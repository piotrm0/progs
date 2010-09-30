#ifndef __board_h__
#define __board_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include "common.h"
#include "util.h"

#define LINE_LENGTH 32

#define DIMS 2
enum {DIR_RIGHT, // / -> up    \ -> down
      DIR_DOWN,  // / -> left  \ -> left
      DIR_LEFT,  // / -> down  \ -> up
      DIR_UP,    // / -> right \ -> left
      DIRS};

extern u_int MARKS_OF_TYPE[];
extern u_int DIR_POS[];
extern u_int DIR_NEG[];

//int taken_int_0 = 0;
//extern void* STICH_WALL;
//#define STICH_WALL (void*) &taken_int_0

enum {TILE_BLANK     = 0 << 0,
      TILE_HAS_PRISM = 1 << 0,
      TILE_HAS_ROBOT = 1 << 1,
      TILE_HAS_MARK  = 1 << 2,
      TILE_DRAWN     = 1 << 3,
      TILE_CHECKED   = 1 << 4,
      TILE_META      = 1 << 5};

enum {PRISM_DIR_LEFT,   // \ top left to bottom right
      PRISM_DIR_RIGHT,  // / top right to bottom left
      PRISMS};

extern u_int PRISM_TRANS[PRISMS][DIRS];

enum {MARK_MOON,
      MARK_STAR,
      MARK_SATURN,
      MARK_GEAR,
      MARK_WARP,
      MARKS};

enum {COLOR_RED,
      COLOR_GREEN,
      COLOR_BLUE,
      COLOR_YELLOW,
      COLOR_BLACK,
      COLORS};

#define ROBOTS 5
#define ROBOT_MOD 1

enum {PARSE_SECTION_ROOT,
      PARSE_SECTION_SIZE,
      PARSE_SECTION_WALL,
      PARSE_SECTION_MARK,
      PARSE_SECTION_PRISM};

/* misc */
inline u_int dir_opp(u_int);
u_int dir_to(int* from, int* to);
u_int str_to_mark_type(char* str);
u_int str_to_color(char* str);
/* end misc */

/* tile */
typedef struct _tile {
  u_int flags;

  struct _tile** tile_p;

  u_int mark_color;
  u_int mark_type;

  u_int prism_dir;
  u_int prism_color;

  int* coord;
} tile;

extern void* void_tile;

tile*  tile_new(u_int);
void   tile_connect(tile*, u_int, tile*);
void   tile_disconnect(tile*, u_int, tile*);
void   tile_clear_flag(tile*, u_int);
void   tile_fill_coord(tile*, u_int*);
char*  tile_label(tile*);
void   tile_info(tile*);
tile*  coord_to_tile(tile*, int*);
char*  coord_to_str(int*);
tile** tiles_gen_2d(u_int width, u_int height);
/* end tile */

/* robot */
tile** robots_new();
void print_robots(tile** r);
/* end robot */

/* board */
typedef struct _board {
  u_int width;
  u_int height;
  struct _tile* origin;
  struct _tile** tiles;
} board;

board* board_new(u_int width, u_int height);
void   board_clean_stich(board* b);
tile*  get_tile_at_2d(board* b, int x, int y);
tile*  get_tile_at_2d_coord(board* b, int* coord);
board* read_2d(char* filename);
board* parse_2d(char* filename);
void   boards_stich_2d(board* b1, u_int dir, board* b2);
void   board_rotate_2d(board* b, u_int a);
/* end board */

/* parsing */
typedef u_int (*parse_section_f_p)(struct _board**, const char*);
extern parse_section_f_p parse_section_handle[];

u_int parse_section_root(board**, const char*);
u_int parse_section_size(board**, const char*);
u_int parse_section_wall(board**, const char*);
u_int parse_section_mark(board**, const char*);
u_int parse_section_prism(board**, const char*);
/* end parsing */

#endif /* __board_h__ */
