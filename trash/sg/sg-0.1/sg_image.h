#ifndef __sg_image_h__
#define __sg_image_h__

#include "sg.h"

struct sg_image {
  recti* geo;
  SDL_Surface* image;
  GLuint tid;
};

sg_image* sg_image_load(sg_root* root, char* filename);
GLuint sg_image_texture(sg_image *image);
void sg_image_draw(sg_image *e, vi2 *o);

#endif /* __sg_image_h__ */
