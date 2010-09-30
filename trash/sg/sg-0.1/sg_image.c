#ifndef __sg_image_c__
#define __sg_image_c__

#include "sg.h"

/* sg_image */

sg_image* sg_image_load(sg_root* root, char* filename) {
  SDL_Surface *image;
  sg_image* ret;

  //  image = SDL_LoadBMP(filename);
  image = IMG_Load(filename);
 
  if (! image) {
    fprintf(stderr, "Error: '%s' could not be opened: %s\n", filename, SDL_GetError());
    return NULL;
  }

  ret = (sg_image*) malloc(sizeof(sg_image));
  ret->image = image;
  ret->geo = recti_new(0,0, image->w, image->h);
  ret->tid = sg_image_texture(ret);

  return ret;
}

GLuint sg_image_texture(sg_image *image) {
  // http://gpwiki.org/index.php/SDL:Tutorials:Using_SDL_with_OpenGL

  SDL_Surface *surface;
  GLuint texture;

  GLenum texture_format;
  GLint  nOfColors;
 
  surface = image->image;

  if ( (surface->w & (surface->w - 1)) != 0 ) {
    printf("warning: image.bmp's width is not a power of 2\n");
  }

  if ( (surface->h & (surface->h - 1)) != 0 ) {
    printf("warning: image.bmp's height is not a power of 2\n");
  }
 
  nOfColors = surface->format->BytesPerPixel;
  if (nOfColors == 4) {
    if (surface->format->Rmask == 0x000000ff)
      texture_format = GL_RGBA;
    else
      texture_format = GL_BGRA;
  } else if (nOfColors == 3) {
    if (surface->format->Rmask == 0x000000ff)
      texture_format = GL_RGB;
    else
      texture_format = GL_BGR;
  } else {
    printf("warning: the image is not truecolor..  this will probably break\n");
    exit(1);
  }
  
  // Have OpenGL generate a texture object handle for us
  glGenTextures( 1, &texture );
 
  // Bind the texture object
  glBindTexture( GL_TEXTURE_2D, texture );
 
  // Set the texture's stretching properties
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
 
  // Edit the texture object's image data using the information SDL_Surface gives us
  glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
		texture_format, GL_UNSIGNED_BYTE, surface->pixels );
 
  return texture;
}

void sg_image_draw(sg_image *e, vi2 *o) {
  u_int top,right,bottom,left;

  assert(NULL != e);
  assert(NULL != o);

  left   = e->geo->v[0] + o->v[0];
  bottom = e->geo->v[1] + e->geo->v[3] + o->v[1];
  right  = e->geo->v[0] + e->geo->v[2] + o->v[0];
  top    = e->geo->v[1] + o->v[1];

  if (0 != e->tid) {
    glBindTexture(GL_TEXTURE_2D, e->tid);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,  1.0f); 
    glVertex2i(  left,  top);
    glTexCoord2f(1.0f,  1.0f); 
    glVertex2i(  right, top);
    glTexCoord2f(1.0f,  0.0f); 
    glVertex2i(  right, bottom);
    glTexCoord2f(0.0f,  0.0f); 
    glVertex2i(  left,  bottom);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glColor4f(1.0f, 0.0, 0.0, 1.0f);

  glBegin(GL_LINE_LOOP);
  glVertex2i(left,  top);
  glVertex2i(right+1, top-1); // line join bug hack
  glVertex2i(right, bottom);
  glVertex2i(left,  bottom);
  glEnd();

  return;
}

/* end of sg_image */

#endif /* __sg_image_c__ */

