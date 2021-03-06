#include "common.h"

GLfloat rot_x = 0.0f;
GLfloat rot_y = 0.0f;
GLfloat rot_z = 0.0f;
GLfloat zoom = 0.15f;

GLint w_width = W_WIDTH;
GLint w_height = W_HEIGHT;
GLfloat w_aspect = W_WIDTH / W_HEIGHT;

GLfloat light_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat light_diffuse[]  = {0.6f, 0.6f, 0.6f, 1.0f};
//GLfloat light_position[] = {2.0f, 8.0f, -4.0f, 1.0f};
GLfloat light_position[] = {0.0f, 1.0f, 0.0f, 1.0f};

view* view_main = NULL;

GLvoid handle_special(int key, int x, int y) {
  switch(key) {
  default:
    custom_special(key, x, y);
  }
  glutPostRedisplay();
}

GLvoid handle_keyboard(unsigned char key, int x, int y) {
  switch(key) {
  case 'q':
    exit(0);
    break;
  case 'x':
    rot_x -= rot_int;
    break;
  case 'X':
    rot_x += rot_int;
    break;
  case 'y':
    rot_y -= rot_int;
    break;
  case 'Y':
    rot_y += rot_int;
    break;
  case 'z':
    rot_z -= rot_int;
    break;
  case 'Z':
    rot_z += rot_int;
    break;
  case '-':
    zoom /= 1.5;
    break;
  case '+':
    zoom *= 1.5;
    break;
  default:
    custom_keyboard(key, x, y);
  }
  glutPostRedisplay();
}

GLvoid draw_gl(GLvoid) {
  view_draw(view_main);
  glutSwapBuffers();
}

GLvoid init_gl(GLvoid) {
  glShadeModel(GL_SMOOTH);
  glClearColor(0.3f, 0.3f, 0.3f, 0.5f);
  glClearDepth(1.0f);

  glEnable(GL_TEXTURE_2D);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glDepthFunc(GL_LESS);

  glHint(GL_LINE_SMOOTH_HINT |
	 GL_PERSPECTIVE_CORRECTION_HINT |
	 GL_POINT_SMOOTH_HINT |
	 GL_POLYGON_SMOOTH_HINT, 
	 GL_FASTEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f,(GLfloat)w_width/(GLfloat)w_height,0.1f,10.0f);
  glMatrixMode(GL_MODELVIEW);
}

GLvoid resize_gl(int w, int h) {
  w_width = w;
  w_height = h;
  w_aspect = w/h;

  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

GLvoid idle_gl(GLvoid) {
  //  update_scene();
  //  glutPostRedisplay();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(w_width, w_height);
  glutInitWindowPosition (100, 100);

  glutCreateWindow(argv[0]);

  init_gl();
  init_scene();

  glutDisplayFunc(draw_gl);
  glutReshapeFunc(resize_gl);
  //  glutIdleFunc(idle_gl);
  glutKeyboardFunc(handle_keyboard);
  glutSpecialFunc(handle_special);

  glutMainLoop();

  return 0;
}
