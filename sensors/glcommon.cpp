#include "glcommon.h"

float stringLength(void* font, string s) {
  float ret = 0;
  int len = (int) s.length();
  int i;
  for (i = 0; i < len; i++) {
    ret += glutStrokeWidth(font, s[i]);
  }
  return ret;
}

glm::vec2 coordWorldToScreen(glm::vec3 world) {
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);

  double projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);

  double modelview[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
  
  glm::vec2 ret = glm::vec2(0,0);
  double temp[3];

  gluProject(world.x, world.y, world.z,
	     modelview, projection, viewport,
	     &(temp[0]), &(temp[1]), &(temp[2]));
	     
  ret.x = temp[0];
  ret.y = temp[1];

  return ret;
}

glm::vec2 coordScreenToWorld(glm::vec2 screen) {
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);

  double projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);

  double modelview[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

  screen.y = viewport[3] - screen.y;

  double pos[3];
  gluUnProject(screen.x, screen.y, 0.0,
	       modelview,
	       projection,
	       viewport,
	       &pos[0], &pos[1], &pos[2]);

  return (glm::vec2(pos[0], pos[1])); 
}

glm::vec2 coordScreenToWorld(glm::vec2 screen, double* modelview) {
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);

  double projection[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projection);

  screen.y = viewport[3] - screen.y;

  double pos[3];
  gluUnProject(screen.x, screen.y, 0.0,
	       modelview,
	       projection,
	       viewport,
	       &pos[0], &pos[1], &pos[2]);

  return (glm::vec2(pos[0], pos[1])); 
}

void drawStringAtWorld(string s, glm::vec3 pos, int a, float fsize, float padding) {
  int len, i;

  void* font = GLUT_STROKE_ROMAN;

  float height = 119.05;
  float width = stringLength(font, s);

  //  glm::vec2 screenPos = coordWorldToScreen(pos);

  //  screenPos.x = 0;
  //  screenPos.y = 0;

  //  glDisable(GL_DEPTH_TEST);
  //  glDisable(GL_TEXTURE_2D);

  //  glRasterPos2f(pos.x, pos.y); // set position

  padding *= height;

  glPushMatrix();

  glTranslatef(pos.x, pos.y, 0);
  glScalef(fsize / height,
	   fsize / height,
	   fsize / height);

  switch (a) {
  case ANCHOR_LEFT_MID:
    glTranslatef(padding, height * -0.5, 0);
    break;
  case ANCHOR_RIGHT_MID:
    glTranslatef(-width - padding, height * -0.5, 0);
    break;
  case ANCHOR_MID_BOTTOM:
    glTranslatef(width * -0.5, padding, 0);
    break;
  case ANCHOR_MID_TOP:
    glTranslatef(width * -0.5, -height - padding, 0);
    break;
  }
  
  len = (int) s.length();
  for (i = 0; i < len; i++) { // output character by character
    glutStrokeCharacter(font, s[i]);
  }

  glPopMatrix();

  //  glEnable(GL_DEPTH_TEST);
  //  glEnable(GL_TEXTURE_2D);
}

void rectangleVertices(float x, float y, float w, float h) {
  glVertex3f(x, y, 0);
  glVertex3f(x+w,y,0);
  glVertex3f(x+w,y+h,0);
  glVertex3f(x,y+h,0);
}
