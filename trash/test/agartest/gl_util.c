#include "gl_util.h"

GLvoid gl_print(GLfloat x, GLfloat y, GLfloat z, char *t) {
  char *p;
  glRasterPos3f(x, y, z);
  for (p = t; *p; p++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
  }
}

GLvoid gl_circle_vertices(GLfloat radius) {
  GLfloat d = 0;
  for (d = 0; d < M_PI * 2; d += CIRCLE_INT)
    glVertex3f(sin(d) * radius, 0.0f, cos(d) * radius);
}

GLvoid gl_cylinder_vertices(GLfloat radius, GLfloat height) {
  GLfloat d = 0;
  for (d = 0; d < M_PI * 2; d += CIRCLE_INT) {
    glVertex3f(sin(d+CIRCLE_INT) * radius, 0.0f, cos(d+CIRCLE_INT) * radius);
    glVertex3f(sin(d+CIRCLE_INT) * radius, height, cos(d+CIRCLE_INT) * radius);
    glVertex3f(sin(d) * radius, height, cos(d) * radius);
    glVertex3f(sin(d) * radius, 0.0f, cos(d) * radius);
  }
}

GLvoid gl_circle_vertices_texture(GLfloat radius) {
  GLfloat d = 0;
  for (d = 0; d < M_PI * 2; d += CIRCLE_INT) {
    glTexCoord2d(sin(d) * 0.5f + 0.5f,
		 cos(d) * 0.5f + 0.5f);
    glVertex3f(sin(d) * radius, 0.0f, cos(d) * radius);
  }
}
