#include "board.h"

//void* STICH_WALL = (void*) &taken_int_0;

u_int MARKS_OF_TYPE[] = {4,4,4,4,1};

u_int DIR_POS[] = {DIR_RIGHT, DIR_DOWN};
u_int DIR_NEG[] = {DIR_LEFT, DIR_UP};

u_int PRISM_TRANS[PRISMS][DIRS] = {{DIR_DOWN, DIR_RIGHT, DIR_UP, DIR_LEFT},
				   {DIR_UP, DIR_LEFT, DIR_DOWN, DIR_RIGHT}};

parse_section_f_p parse_section_handle[] = {&parse_section_root,
					    &parse_section_size,
					    &parse_section_wall,
					    &parse_section_mark,
					    &parse_section_prism};

//void* void_tile = (void*) malloc(1);
void* void_tile = (void*) 1;

/* misc */
char* coord_to_str(int* coord) {
  char* str = (char*) calloc(32, sizeof(char));

  if (NULL == coord)
    return "NULL";
  //    pexit(EFAULT, "coord_to_str: got null coord");

  sprintf(str, "%d,%d", coord[0], coord[1]);

  return str;
}

u_int dir_to(int* from, int* to) {
  u_int d;

  if ((NULL == from) ||
      (NULL == to))
    pexit(EFAULT, "dir_to: got null coord(s)");

  //  printf("dir from (%p)[%s] to (%p)[%s]\n", from, coord_to_str(from), to, coord_to_str(to));

  for (d = 0; d < DIMS; d++) {
    if (from[d] == to[d] - 1)
      return DIR_POS[d];
    else if (from[d] == to[d] + 1)
      return DIR_NEG[d];
  }

  pexit(0, "dir_to: non-adjacent coords provided");
  return 0;
}

inline u_int dir_opp(u_int d) {
  return ((d + DIRS/2) % DIRS);
}

u_int str_to_mark_type(char* str) {
  if (NULL == str)
    pexit(EFAULT, "str_to_mark_type: got null string pointer");

  if (0 == strcmp(str, "moon"))
    return MARK_MOON;
  else if (0 == strcmp(str, "star"))
    return MARK_STAR;
  else if (0 == strcmp(str, "saturn"))
    return MARK_SATURN;
  else if (0 == strcmp(str, "gear"))
    return MARK_GEAR;
  else if (0 == strcmp(str, "warp"))
    return MARK_WARP;

  pexit(0, "str_to_mark_type: invalid mark type [%s]", str);
  return 0;
}

u_int str_to_color(char* str) {
  if (NULL == str)
    pexit(EFAULT, "str_to_color: got null string pointer");

  if (0 == strcmp(str, "red"))
    return COLOR_RED;
  else if (0 == strcmp(str, "green"))
    return COLOR_GREEN;
  else if (0 == strcmp(str, "blue"))
    return COLOR_BLUE;
  else if (0 == strcmp(str, "yellow"))
    return COLOR_YELLOW;
  else if (0 == strcmp(str, "black"))
    return COLOR_BLACK;

  pexit(0, "str_to_color: invalid color [%s]", str);
  return 0;
}

u_int str_to_prism_dir(char* str) {
  if (NULL == str)
    pexit(EFAULT, "str_to_prism_dir: got null string pointer");

  if (0 == strcmp(str, "/"))
    return PRISM_DIR_RIGHT;
  else if (0 == strcmp(str, "\\"))
    return PRISM_DIR_LEFT;

  pexit(0, "str_to_prism_dir: invalid dir [%s]", str);
  return 0;
}

/* end misc */

/* robot */
tile** robots_new() {
  u_int i;
  tile** rs = (tile**) calloc(COLORS, sizeof(tile*));

  if (rs == NULL)
    pexit(errno, "robots_new: can't allocate robot array");

  for (i = 0; i < COLORS; i++)
    rs[i] = NULL;
  return rs;
}
/* end robot */

/* tile */
void tile_clear_flag(tile* t, u_int flag) {
  u_int i;

  if (t == NULL)
    return;

  if (! (t->flags & flag))
    return;

  t->flags -= flag;

  for (i = 0; i < DIRS; i++)
    tile_clear_flag(t->tile_p[i], flag);

  return;
}

tile* coord_to_tile(tile* o, int* offset) {
  u_int i;

  if (offset == NULL)
    pexit(EFAULT, "tile_coord_to_tile: null coord given");

  printf("coord_to_file: o[%p], c[%s]\n", o, coord_to_str(offset));

  if (o == NULL) {
    //    pexit(EFAULT, "coord_to_tile: got null origin");
    return NULL;
  }

  //  printf("tile [%s] offset[%d,%d]\n", tile_label(o), offset[0], offset[1]);

  for (i = 0; i < DIMS; i++) {
    if (offset[i] > 0) {
      offset[i]--;
      return coord_to_tile(o->tile_p[DIR_POS[i]], offset);
    } else if (offset[i] < 0) {
      offset[i]++;
      return coord_to_tile(o->tile_p[DIR_NEG[i]], offset);
    }
  }

  //  printf("finished coord_to_tile\n");

  return o;
}

void tile_list(tile* t) {
  if (t == NULL)
    return;
}

void tile_info(tile* t) {
  printf("tile: %s\n", tile_label(t));
  if (t == NULL) return;
  printf(" flags: %d\n", t->flags);
}

void print_robots(tile** r) {
  u_int i;
  for (i = 0; i < COLORS; i++) {
    printf("robot[%x]: %s\n", i, tile_label(r[i]));
  }
}

char* tile_label(tile* t) {
  char* temp = (char*) malloc(20 * sizeof(char));
  if (temp == NULL)
    pexit(errno, "tile_label: can't allocate temporary string");

  if (t == NULL)
    return "NULL";

  if (t->coord == NULL) {
    //    sprintf(temp, "%d[null]", (u_int) t);
    sprintf(temp, "[?,?]");
    return temp;
  }

  //  sprintf(temp, "%d[%d,%d]", (u_int) t, t->coord[0], t->coord[1]);
  sprintf(temp, "[%d,%d]", t->coord[0], t->coord[1]);

  return temp;
}

void tile_fill_coord(tile* tile, u_int* coord) {
  u_int i;

  if (coord == NULL) {
    printf("tile_fill_coord: null coord given\n");
    exit(1);
  }

  /*  printf("filling [%d] at coord [%d, %d]\n",
	 (u_int) tile,
	 coord[0], coord[1]);*/

  if (tile == NULL)
    return;

  if (tile->flags & TILE_CHECKED)
    return;

  tile->flags |= TILE_CHECKED;

  // !!! memcopy coord to tile->coord here
  tile->coord = (int*) calloc(DIMS, sizeof(int));
  if (tile->coord == NULL)
    pexit(errno, "tile_fill_coord: can't allocate coordinates");

  memcpy(tile->coord, coord, DIMS * sizeof(int));

  for (i = 0; i < DIMS; i++) {
    coord[i]++;
    tile_fill_coord(tile->tile_p[DIR_POS[i]], coord);
    coord[i]-=2;
    tile_fill_coord(tile->tile_p[DIR_NEG[i]], coord);
    coord[i]++;
  }
}

tile* tile_new(u_int flags) {
  tile* t = (tile*) malloc(sizeof(tile));
  if (t == NULL)
    pexit(errno, "tile_new: can't allocate tile");

  t->tile_p = (tile**) calloc(DIRS, sizeof(tile*));
  if (t->tile_p == NULL)
    pexit(errno, "tile_new: can't allocate adjacent tile pointers");

  t->flags = flags;
  t->prism_dir = 0;
  t->prism_color = 0;
  t->coord = NULL;
  return t;
}

void tile_connect(tile* t1, u_int d, tile* t2) {
  if ((NULL == t1) ||
      (NULL == t2)) {
    pexit(EFAULT, "tile_connect: got null tile(s)");
    exit(1);
  }

  //  printf("connecting [%p] to [%p] by [%u]\n", t1, t2, d);

  t1->tile_p[d] = t2;
  t2->tile_p[dir_opp(d)] = t1;
}

void tile_disconnect(tile* t1, u_int d, tile* t2) {
  if ((NULL == t1) ||
      (NULL == t2)) {
    pexit(EFAULT, "tile_disconnect: disconnecting null tile(s)");
  }
  t1->tile_p[d] = NULL;
  t2->tile_p[dir_opp(d)] = NULL;
}

tile** tiles_gen_2d(u_int width, u_int height) {
  u_int x,y;
  u_int* temp;
  tile** origin = (tile**) calloc(width * height, sizeof(tile));
  
  if (NULL == origin)
    pexit(errno, "tile_gen_2d: can't allocate tiles");

  for (x = 0; x < width * height; x++)
    origin[x] = tile_new(0);

  //  printf("allocated [%u] tiles (size %u) at [%p]\n", width * height, sizeof(tile), origin);

  for (x = 0; x < width; x++) {
    for (y = 0; y < height - 1; y++) {
      tile_connect(origin[y * width + x], DIR_DOWN, origin[(y+1) * width + x]);
    }
  }

  for (y = 0; y < height; y++) {
    for (x = 0; x < width - 1; x++) {
      tile_connect(origin[y * width + x], DIR_RIGHT, origin[y * width + x + 1]);
    }
  }

  tile_clear_flag(*origin, TILE_CHECKED);

  temp = (u_int*) calloc(DIMS, sizeof(u_int));
  temp[0] = 0;
  temp[1] = 0;
  tile_fill_coord(*origin, temp);

  return origin;
}
/* end tile */

/* board */
board* board_new(u_int width, u_int height) {
  board* b = (board*) malloc(sizeof(board));
  if (b == NULL)
    pexit(errno, "board_new: can't to allocate board");

  b->width  = width;
  b->height = height;

  b->tiles  = tiles_gen_2d(width, height);
  b->origin = *(b->tiles);

  return b;
}

void board_rotate_2d(board* b, u_int a) {
  tile** origin;
  tile* t;
  tile* first_t;
  u_int i;
  u_int temp;
  u_int x, y;
  u_int xp, yp;

  if (0 == a) return;

  origin = (tile**) calloc(b->width * b->height, sizeof(tile));

  for (x = 0; x < b->width; x++) {
    for (y = 0; y < b->height; y++) {
      xp = (b->height - y - 1);
      yp = x;
      t = get_tile_at_2d(b, x, y);
      t->prism_dir = (t->prism_dir + 1) % 2;
      first_t = t->tile_p[DIRS-1];
      for (i = DIRS-1; i > 0; i--) {
	t->tile_p[i] = t->tile_p[i-1];
      }
      t->tile_p[0] = first_t;
      origin[xp + yp * b->height] = t;
    }
  }
  
  temp = b->width;
  b->width = b->height;
  b->height = temp;

  b->origin = *origin;
  b->tiles = origin;

  board_rotate_2d(b, a-1);
}

board* read_2d(char* filename) {
  return parse_2d(filename);
}

void board_clean_stich(board* b) {
  u_int x, y, d;
  tile* t;

  for (x = 0; x < b->width; x++) {
    for (y = 0; y < b->height; y++) {
      t = get_tile_at_2d(b, x, y);
      for (d = 0; d < DIRS; d++) {
	if (void_tile == t->tile_p[d]) t->tile_p[d] = NULL;
      }
    }
  }
}

void boards_stich_2d(board* b1, u_int dir, board* b2) {
  int c1[2];
  int c2[2];
  
  tile* t1;
  tile* t2;

  u_int const_i;
  int const_v1, const_v2;
  u_int mod_i;
  int mod_vm;
  int mod_v;

  if ((NULL == b1) ||
      (NULL == b2))
    pexit(EFAULT, "boards_stich_2d: got null board(s)");

  if ((DIR_UP == dir) ||
      (DIR_DOWN == dir)) {
    if (b1->width != b2->width) pexit(0, "boards_stich_2d: stiching incompadible boards");
  } else {
    if (b1->height != b2->height) pexit(0, "boards_stich_2d: stiching incompadible boards");
  }

  switch (dir) {
  case DIR_DOWN:
    const_i = 1;
    mod_i = 0;
    const_v2 = 0;
    const_v1 = b2->height - 1;
    mod_vm = b1->width;
    break;
  case DIR_RIGHT:
    const_i = 0;
    mod_i = 1;
    const_v1 = b1->width - 1;
    const_v2 = 0;
    mod_vm = b1->height;
    break;
  case DIR_UP:
    const_i = 1;
    mod_i = 0;
    const_v2 = b1->height - 1;
    const_v1 = 0;
    mod_vm = b1->width;
    break;
  case DIR_LEFT:
    const_i = 0;
    mod_i = 1;
    const_v1 = 0;
    const_v2 = b2->width - 1;
    mod_vm = b1->height;
    break;
  }

  c1[const_i] = const_v1;
  c2[const_i] = const_v2;

  for (mod_v = 0; mod_v < mod_vm; mod_v++) {
    c1[mod_i] = mod_v;
    c2[mod_i] = mod_v;
    
    t1 = get_tile_at_2d_coord(b1, c1);
    t2 = get_tile_at_2d_coord(b2, c2);

    //    printf("stiching [%s]<->[%s]: ", coord_to_str(c1), coord_to_str(c2));

    if ((void_tile == t1->tile_p[dir]) ||
	(void_tile == t2->tile_p[dir_opp(dir)])) {
      tile_disconnect(t1, dir, t2);
      //      printf("disconnected\n");
    } else {
      tile_connect(t1, dir, t2);
      //      printf("connected\n");
    }

    //    printf("t1[%p], t2[%p]\n", t1, t2);
    //    printf("t1->t2[%p] t2->t1[%p]\n", t1->tile_p[dir], t2->tile_p[dir_opp(dir)]);
  }
}

tile* get_tile_at_2d_coord(board* b, int* coord) {
  return get_tile_at_2d(b, coord[0], coord[1]);
}

tile* get_tile_at_2d(board* b, int x, int y) {
  if ((x < 0) || (x >= (int) b->width) || (y < 0) || (y >= (int) b->height))
    return NULL;
  return b->tiles[b->width * y + x];
}
/* end board */

/* parse */
u_int parse_section_root(board** b, const char* line) {
  //  printf("b[%p], parse root [%s]\n", *b, line);

  if (0 == strcmp(line, "size"))
    return PARSE_SECTION_SIZE;
  else if (0 == strcmp(line, "walls"))
    return PARSE_SECTION_WALL;
  else if (0 == strcmp(line, "marks"))
    return PARSE_SECTION_MARK;
  else if (0 == strcmp(line, "prisms"))
    return PARSE_SECTION_PRISM;

  pexit(0, "parse_section_root: parse error");
  return PARSE_SECTION_ROOT;
}

u_int parse_section_size(board** b, const char* line) {
  u_int width, height;

  //  printf("b[%p], parse size [%s]\n", *b, line);

  if (2 != sscanf(line, "%u %u", &width, &height))
    return parse_section_root(b, line);

  //  printf("got size [%u, %u]\n", width, height);

  if (NULL != *b)
    pexit(0, "parse_section_size: board size already defined");

  *b = board_new(width, height);

  //  printf("got board at [%p]\n", *b);

  return PARSE_SECTION_SIZE;
}
u_int parse_section_wall(board** b, const char* line) {
  int x1, y1, x2, y2;
  int* c_from;
  int* c_to;
  int* c_from2;
  int* c_to2;
    //int c_from[2];
    //int c_to[2];
    //  int c_from2[2];
    //  int c_to2[2];
  
  tile* t_from;
  tile* t_to;
  u_int dir;
  //  printf("b[%p], parse wall [%s]\n", *b, line);

  if (4 != sscanf(line, "%d %d - %d %d", &x1, &y1, &x2, &y2))
    return parse_section_root(b, line);

  //  c_from = (int[]) {x1, y1};
  //  c_to   = (int[]) {x2, y2};
  c_from = (int*) calloc(DIMS, sizeof(int));
  c_to   = (int*) calloc(DIMS, sizeof(int));
  *c_from = x1;
  *(c_from+1) = y1;
  *c_to = x2;
  *(c_to+1) = y2;
  //  c_from = (int*) {x1, y1};
  //  c_to   = (int*) {x2, y2};

  c_from2 = (int*) calloc(DIMS, sizeof(int));
  c_to2 = (int*) calloc(DIMS, sizeof(int));

  memcpy(c_from2, c_from, DIMS * sizeof(int));
  memcpy(c_to2, c_to, DIMS * sizeof(int));
    
  //  printf("x1=%d; c_from[0]=%d; c_from=(%p)[%s]\n", x1, c_from[0], c_from, coord_to_str(c_from));
  //  printf("putting wall at [%s]-[%s]\n", coord_to_str(c_from), coord_to_str(c_to));

  //  t_from = coord_to_tile((*b)->origin, c_from2);
  //  t_to   = coord_to_tile((*b)->origin, c_to2);
  t_from = get_tile_at_2d(*b, x1, y1);
  t_to   = get_tile_at_2d(*b, x2, y2);

  //  printf("[%d,%d] <-> [%d,%d]", x1, y1, x2, y2);

  if (NULL == t_from)
    pexit(EFAULT, "parse_section_wall: first coord can't be outside of board");

  //  printf("[%s] <-> ", coord_to_str(t_from->coord));

  dir = dir_to(c_from, c_to);

  if (NULL == t_to) {
    //    printf(" meta-wall\n");

    t_from->tile_p[dir] = (_tile*) void_tile;
  } else {
    //    printf("[%s]\n", coord_to_str(t_to->coord));
    //    printf(" wall\n");

    tile_disconnect(t_from, dir, t_to);
  }

  return PARSE_SECTION_WALL;
}
u_int parse_section_mark(board** b, const char* line) {
  tile* t;
  int x, y;
  char color[LINE_LENGTH];
  char mark[LINE_LENGTH];
  //  printf("b[%p], parse mark [%s]\n", *b, line);

  if (4 != sscanf(line, "%d %d %s %s", &x, &y, color, mark))
    return parse_section_root(b, line);

  //  printf("putting mark[%s], color[%s] at [%d,%d]\n", mark, color, x, y);

  t = ((*b)->tiles[(*b)->width * y + x]);

  t->flags |= TILE_HAS_MARK;
  t->mark_color = str_to_color(color);
  t->mark_type = str_to_mark_type(mark);

  return PARSE_SECTION_MARK;
}
u_int parse_section_prism(board** b, const char* line) {
  int x, y;
  char dir[LINE_LENGTH];
  char color[LINE_LENGTH];
  tile* t;

  //  printf("b[%p], parse prism [%s]\n", *b, line);

  if (4 != sscanf(line, "%d %d %s %s", &x, &y, color, dir))
    return parse_section_root(b, line);

  //  printf("putting prism [%s] [%s] at [%d,%d]\n", color, dir, x, y);

  t = ((*b)->tiles[(*b)->width * y + x]);

  t->flags |= TILE_HAS_PRISM;
  t->prism_color = str_to_color(color);
  t->prism_dir   = str_to_prism_dir(dir);

  return PARSE_SECTION_PRISM;
}

board* parse_2d(char* filename) {
  u_int parse_mode = PARSE_SECTION_ROOT;
  board* b = NULL;
  char line[LINE_LENGTH];
  FILE* file;
  
  //  printf("loading board [%s]\n", filename);

  file = fopen(filename, "r");
  if (file == NULL)
    pexit(errno, "parse_2d: can't open file");

  while(NULL != fgets(line, LINE_LENGTH, file)) {
    chomp(line);
    parse_mode = parse_section_handle[parse_mode](&b, line);
  }

  if (ferror(file))
    pexit(errno, "parse_2d: file read error");

  fclose(file);

  //  printf("returning board [%p]\n", b);

  return b;
}
/* end parse */
