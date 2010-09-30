#ifndef __sg_main_c__
#define __sg_main_c__

#include "sg.h"

/* misc */

void sg_render(sg_root* root) {
  //  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  root->render((sg_element*) root, root->geo);
}

void sg_loop(sg_root* root) {
  SDL_Event event;

  bool running = true;

  while(running) {
    glClear(GL_COLOR_BUFFER_BIT);

    sg_timing_delay(root->timing);
    sg_timing_mark(root->timing);
    sg_timing_mark_callback(root->timing);
    sg_root_over(root, root->geo, root->mouse->pos);
    sg_render(root);

    SDL_GL_SwapBuffers();

    while(SDL_PollEvent(&event)) {

      switch(event.type){
      case SDL_KEYDOWN:
	break;
      case SDL_KEYUP:
	break;
      case SDL_QUIT:
	running = false;
	break;
      case SDL_MOUSEMOTION:
	root->mouse->pos->v[0] = event.motion.x - 1;
	root->mouse->pos->v[1] = event.motion.y - 1;
	break;
      case SDL_MOUSEBUTTONUP:
	printf("up\n");
	break;
      case SDL_MOUSEBUTTONDOWN:
	printf("down\n");
	break;
      
      }
      
    }

  }
}

u_int _sg_gl_init(sg_root* root) {
  glEnable(GL_TEXTURE_2D);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.5f, 0.5f, 0.5f, 0.5f);

  glViewport(0, 0, 640, 480);

  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 640, 480, 0, -1.0f, 1.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return true;
}

sg_root* sg_init() {
  sg_root* root;

  if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    return NULL;
  }

  if (TTF_Init() == -1) {
    fprintf(stderr, "Unable to initialize SDL_ttf: %s\n", TTF_GetError());
    return NULL;
  }

  SDL_WM_SetCaption("sg test", 0);
  
  root = _sg_root_new();

  _sg_gl_init(root);

  temp = sg_image_load(root, "test.bmp");

  return root;
}

/* end of misc */

#endif /* __sg_main_c__ */

