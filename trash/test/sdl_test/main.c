#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sg.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

int test();
GLuint load_texture(char* filename);

int main(int argc, char *argv[]) {
  if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }
  
  printf("hello?\n");
 
  test();
 
  atexit(SDL_Quit);

  return 0;
}

int test() {
  //To use OpenGL, you need to get some information first,
  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if(!info) {
    /* This should never happen, if it does PANIC! */
    fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
    return 0;
  }

  int bpp = info->vfmt->BitsPerPixel;

  printf("bpp=%d\n",bpp);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_Surface *screen;
  screen = SDL_SetVideoMode(640, 480, bpp, SDL_OPENGL);
  assert(NULL != screen);

  glClearColor(0.3f, 0.3f, 0.3f, 0.5f);
  glClear(GL_COLOR_BUFFER_BIT);
  glClearDepth(1.0f);

  glViewport( 0, 0, 640, 480 );
 
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
 
  glOrtho(0.0f, 640, 480, 0.0f, -1.0f, 1.0f);
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  GLuint texture;
  texture = load_texture("test.bmp"); 
  //  glBindTexture( GL_TEXTURE_2D, texture );

  glEnable(GL_TEXTURE_2D);

  glBegin( GL_QUADS );
  glTexCoord2i( 0, 0 );
  glVertex3f( 100, 100, 0 );
  
  glTexCoord2i( 1, 0 );
  glVertex3f( 228, 100, 0 );
  
  glTexCoord2i( 1, 1 );
  glVertex3f( 228, 228, 0 );
  
  glTexCoord2i( 0, 1 );
  glVertex3f( 100, 228, 0 );
  glEnd();

  SDL_GL_SwapBuffers();

  SDL_Delay(3000);

  // glDeleteTextures( 1, &texture );

  return 0;
}

GLuint load_texture(char* filename) {
  GLuint texture;// This is a handle to our texture object
  SDL_Surface *surface;// This surface will tell us the details of the image
  GLenum texture_format;
  GLint  nOfColors;
 
  if ( (surface = SDL_LoadBMP(filename)) ) { 
 
    // Check that the image's width is a power of 2
    if ( (surface->w & (surface->w - 1)) != 0 ) {
      printf("warning: %s's width is not a power of 2\n", filename);
    }
  
    // Also check if the height is a power of 2
    if ( (surface->h & (surface->h - 1)) != 0 ) {
      printf("warning: %s's height is not a power of 2\n", filename);
    }
 
    // get the number of channels in the SDL surface
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
      // this error should not go unhandled
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
  } 
  else {
    printf("SDL could not load %s: %s\n", filename, SDL_GetError());
    SDL_Quit();
    return 1;
  } 

  // Free the SDL_Surface only if it was successfully created
  //  if ( surface ) { 
  //    SDL_FreeSurface( surface );
  //  }

  return texture;
} 
