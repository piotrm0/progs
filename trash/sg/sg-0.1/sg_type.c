#ifndef __sg_type_c__
#define __sg_type_c__

#include "sg.h"

/* sg_font */

sg_font* sg_font_load(sg_root* root, char* filename, u_int ptsize) {
  sg_font* ret;
  TTF_Font* font;

  font = TTF_OpenFont(filename, ptsize);
  if (NULL == font) {
    fprintf(stderr, "Unable to load font: %s %s\n", filename, TTF_GetError());
    return NULL;
  }

  ret = (sg_font*) malloc(sizeof(sg_font));
  ret->ptsize = ptsize;
  ret->font   = font;
  ret->root   = root;

  printf("loaded [%s], ptsize=%d, fontheight=%d\n", filename, ptsize, TTF_FontHeight(font));

  return ret;
}

GLint sg_font_draw_texture(sg_font* font, char* string) {
  // partially from http://www.gamedev.net/community/forums/topic.asp?topic_id=284259

  SDL_Surface *initial;
  SDL_Surface *intermediary;
  //SDL_Rect rect;
  SDL_Color color;
  u_int w,h;
  GLuint texture;
  
  color.r = 255;
  color.g = 255;
  color.b = 255;

  /* Use SDL_TTF to render our text */
  initial = TTF_RenderText_Blended(font->font, string, color);
  
  /* Convert the rendered text to a known format */
  w = pow2_ceil(initial->w);
  h = pow2_ceil(initial->h);
  
  intermediary = 
    SDL_CreateRGBSurface(0, w, h, 32, 
			 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

  assert(NULL != initial);
  assert(NULL != intermediary);

  SDL_BlitSurface(initial, 0, intermediary, 0);
  
  /* Tell GL about our new texture */
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP);

  glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, 
	       //glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_BGR, 
	       GL_UNSIGNED_BYTE, intermediary->pixels);
  //GL_UNSIGNED_BYTE, initial->pixels);
    
  /* Bad things happen if we delete the texture before it finishes */
  // glFinish();
  
  /* return the deltas in the unused w,h part of the rect */
  // location->w = initial->w;
  // location->h = initial->h;
  
  /* Clean up */
  // SDL_FreeSurface(initial);
  // SDL_FreeSurface(intermediary);
  // glDeleteTextures(1, &texture);

  return texture;
}

/* end of sg_font */

/* sg_type */

sg_type* sg_type_new_render(sg_font* font, char* string) {
  sg_type* ret;

  u_int len;

  SDL_Surface *initial;
  SDL_Surface *intermediary;
  SDL_Color color;
  u_int w,h;
  GLuint texture;
  
  color.r = 255;
  color.g = 255;
  color.b = 255;

  //  initial = TTF_RenderText_Blended(font->font, string, color);
  initial = TTF_RenderText_Solid(font->font, string, color);
  
  w = pow2_ceil(initial->w);
  h = pow2_ceil(initial->h);
  
  intermediary = 
    SDL_CreateRGBSurface(0, w, h, 32, 
			 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

  assert(NULL != initial);
  assert(NULL != intermediary);

  SDL_BlitSurface(initial, 0, intermediary, 0);
  
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP);

  glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, 
	       GL_UNSIGNED_BYTE, intermediary->pixels);

  ret = (sg_type*) malloc(sizeof(sg_type));

  ret->font = font;
  ret->size = vi2_new(0,0);
  ret->rsize = v2_new(0,0);

  len = strlen(string);
  ret->string = (char*) calloc(len, sizeof(char));
  memcpy(ret->string, string, len);

  ret->size->v[0] = initial->w;
  ret->size->v[1] = initial->h;
  ret->rsize->v[0] = (float) initial->w / (float) w;
  ret->rsize->v[1] = (float) initial->h / (float) h;
  
  SDL_FreeSurface(initial);
  SDL_FreeSurface(intermediary);

  ret->tid = texture;

  return ret;;
}

void sg_type_draw(sg_type* e, recti* view) {
  u_int top,right,bottom,left;

  assert(NULL != e);
  assert(NULL != view);

  left   = view->v[0];
  bottom = view->v[1] + e->size->v[1];
  right  = view->v[0] + e->size->v[0];
  top    = view->v[1];

  if (0 != e->tid) {
    glColor4f(1.0f, 1.0, 1.0, 1.0f);

    glBindTexture(GL_TEXTURE_2D, e->tid);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,           0.0f); 
    glVertex2i(  left,           top);
    glTexCoord2f(e->rsize->v[0], 0); 
    glVertex2i(  right,          top);
    glTexCoord2f(e->rsize->v[0], e->rsize->v[1]); 
    glVertex2i(  right,          bottom);
    glTexCoord2f(0.0f,           e->rsize->v[1]); 
    glVertex2i(  left,           bottom);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  return;
}

/* end of sg_type */

#endif /* __sg_type_c__ */

