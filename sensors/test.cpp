#include "test.h"

#include <cstdio>
#include <iostream>
#include <list>
//#include <boost/lambda/lambda.hpp>
#include <glm/glm.hpp>

#include "ui.h"
#include "common.h"
#include "glcommon.h"

using namespace std;

//class Sensor {
//}

#define TOOL_DRAG 0
#define TOOL_ZOOM 1

namespace {
  int32 windowWidth = 800;
  int32 windowHeight = 600;
  int32 windowMain;

  int32 framePeriod = 16;
  //float settingsHz = 60.0f;
  glm::vec2 viewCenter = glm::vec2(0.0f, 0.0f);
  glm::vec2 tempViewCenter = glm::vec2(0,0);
  glm::vec2 extentsLower(0,0);
  glm::vec2 extentsUpper(0,0);
  float viewZoom = 1.0f;

  GLuint tex_2d;

  //GLfloat tile_shine[]   = {0.0f, 0.0f, 0.0f, 0.0f};
  //GLfloat tile_emit[]    = {0.3f, 0.3f, 0.3f, 1.0f};
  //GLfloat tile_diffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};

  float r1 = 0.0;

  bool mouseDragging = false;
  glm::vec2 mouseDragStart(0,0);
  double mouseDragStartModelview[16];

  int currentTool = TOOL_DRAG;

  ui::Manager* uiManager;
  ui::Vertical* toolbar;
  ui::Pane* board;
}

static void initWorld();

static void drawGrid();

static void setupProjection();
static void setupScreenProjection();
static void setupCamera();

static void handleDisplay();
static void handleKeyboard(uchar key, int x, int y);
static void handleKeyboardSpecial(int key, int x, int y);
static void handleKeyboardUp(uchar key, int x, int y);
static void handleMouseMotion(int32 x, int32 y);
static void handleMousePassiveMotion(int32 x, int32 y);
static void handleMouse(int32 button, int32 state, int32 x, int32 y);
static void handleResize(int32 w, int32 h);
static void handleTimer(int t);

/*ostream& operator<<(ostream& os, const b2Vec2& v) {
  os << "[" << v.x << "," << v.y << "]";
  return os;
  }*/

void handleToolbarClick(ui::Element* e, glm::vec2 posDown, glm::vec2 posUp) {
  cout << "upped: " << (*e) << endl;
};

void drawBoard(ui::Element* e) {

}

static void initUI() {
  uiManager = new ui::Manager();

  toolbar = new ui::Vertical();

  toolbar->padding = 2;

  ui::Button* b = new ui::Button("Drag");
  b->size = glm::vec2(100, 20);  
  b->setUpHandler(&handleToolbarClick);

  toolbar->addChild(b);
  
  b = new ui::Button("Zoom");
  b->size = glm::vec2(100, 20);
  b->setUpHandler(&handleToolbarClick);

  toolbar->addChild(b);
  toolbar->position = glm::vec2(10, 10);

  uiManager->addChild(toolbar);

  board = new ui::Pane();
  board->drawHandler = &drawBoard;
  board->position = glm::vec2(0,0);
  board->upper = glm::vec2(windowWidth, windowHeight);
  board->lower = glm::vec2(0,0);
  board->arrange();

  uiManager->addChild(board);

  uiManager->arrange();
}

static void initGfx() {
  tex_2d = SOIL_load_OGL_texture
    ("gfx/img_test.png",
     SOIL_LOAD_RGB,
     SOIL_CREATE_NEW_ID,
     0);
  //     SOIL_FLAG_MIPMAPS
  // );
  
  if (0 == tex_2d) {
    printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  //glDisable(GL_DEPTH_TEST);
}

static void initWorld() {

}

//static void simStep() {
//  float32 timeStep = settingsHz > 0.0f ? 1.0f / settingsHz : float32(0.0f);
//}

static void drawGrid() {
  glBegin(GL_LINES);
  glColor4f(0,0,0,0.05);

  /*
  float bigx = extentsUpper.x - extentsLower.x;
  float bigy = extentsUpper.y - extentsLower.y;

  float big = bigx;
  if (bigy > bigx) {
    big = bigy;
  }
  */

  //float gridSpacing = big / 16;

  char temp[32];

  for (float x = -10; x <= 10; x += 1) {
    glVertex3f(x, -10.1, 0);
    glVertex3f(x, 10.1, 0);
  }

  for (float y = -10; y <= 10; y += 1) {
    glVertex3f(-10.1, y, 0);
    glVertex3f(10.1, y, 0);
  }

  glVertex3f(-10, 0, 0);
  glVertex3f(10, 0, 0);

  glVertex3f(0, -10, 0);
  glVertex3f(0, 10, 0);

  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f(-10,-10,0);
  glVertex3f(10,-10,0);
  glVertex3f(10,10,0);
  glVertex3f(-10,10,0);
  glEnd();

  glColor4f(0,0,0,0.25); 
  for (float x = -10; x <= 10; x += 1) {
    sprintf(temp, "%0.0f", x);
    drawStringAtWorld(temp, glm::vec3(x,-10,0), ANCHOR_MID_TOP, 0.3, 0.5);
    drawStringAtWorld(temp, glm::vec3(x,10,0), ANCHOR_MID_BOTTOM, 0.3, 0.5);
  }
  for (float y = -10; y <= 10; y += 1) {
    sprintf(temp, "%0.0f", y);
    drawStringAtWorld(temp, glm::vec3(-10,y,0), ANCHOR_RIGHT_MID, 0.3, 0.5);
    drawStringAtWorld(temp, glm::vec3(10,y,0), ANCHOR_LEFT_MID, 0.3, 0.5);
  }
}

static void setupCamera() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(viewCenter.x, viewCenter.y, 0);

  //  glPushMatrix();

  if (mouseDragging) {
    glTranslatef(tempViewCenter.x, tempViewCenter.y, 0);
  }
}

static void handleDisplay() {
  glClearColor(0xff, 0xff, 0xff, 0xff);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tile_diffuse);
  //glMaterialfv(GL_FRONT, GL_EMISSION, tile_emit);

  //  if (! mouseDragging) {

  //  } 
  setupCamera();

  drawGrid();

  glPushMatrix();

  glRotatef(r1,0,0,1);
  r1 += 0.5;

  //glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex_2d);

  glColor4f(1,1,1,0.5);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-2,-2,0);
  glTexCoord2f(1.0f, 0.0f); glVertex3f(2,-2,0);
  glTexCoord2f(1.0f, 1.0f); glVertex3f(2,2,0);
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-2,2,0);
  glEnd();

  glDisable(GL_TEXTURE_2D);

  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  setupScreenProjection();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  uiManager->draw();

  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glutSwapBuffers();
}

static void handleKeyboard(uchar key, int x, int y) {
  switch (key) {
  case 27:
    printf("quitting\n");
    exit(0);
    break;
  case '+':
    viewZoom *= 0.8;
    setupProjection();
    break;

  case '-':
    viewZoom *= 1.0 / 0.8;
    setupProjection();
    break;
  }
}
      
static void handleKeyboardSpecial(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_LEFT:
    break;
  }
}

static void handleKeyboardUp(uchar key, int x, int y) {  
}

static void handleMousePassiveMotion(int32 x, int32 y) {
  uiManager->handleMouseMotion(glm::vec2(x,windowHeight - y));
}

static void handleMouseMotion(int32 x, int32 y) {
  if (mouseDragging) {
    glm::vec2 cCoord = coordScreenToWorld(glm::vec2(x,y), mouseDragStartModelview);
    tempViewCenter = cCoord - mouseDragStart;
  }
  handleMousePassiveMotion(x, y);
}

static void setupScreenProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, windowWidth, 0, windowHeight);
}


static void setupProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glm::vec2 extents(10.0f, 10.0f);

  float ratio;

  if (windowWidth < windowHeight) {
    ratio = float(windowHeight) / float(windowWidth);
    extents.y *= ratio;
  } else {
    ratio = float(windowWidth) / float(windowHeight);
    extents.x *= ratio;
  }

  extents *= viewZoom;
  extentsLower = viewCenter - extents;
  extentsUpper = viewCenter + extents;
  gluOrtho2D(extentsLower.x, extentsUpper.x, extentsLower.y, extentsUpper.y);

  //printf("resized to [%f,%f]-[%f,%f]\n", lower.x, lower.y, upper.x, upper.y);
}

static void handleMouse(int32 button, int32 state, int32 x, int32 y) {
  if (uiManager->handleMouseClick(glm::vec2(x, windowHeight - y), button, state)) {
    return;
  }

  switch(button) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(viewCenter.x, viewCenter.y, 0);
      glGetDoublev(GL_MODELVIEW_MATRIX, mouseDragStartModelview);

      mouseDragStart = coordScreenToWorld(glm::vec2(x, y), mouseDragStartModelview);
      mouseDragging = true;

      handleMouseMotion(x, y);

      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    } else if (state == GLUT_UP) {
      if (mouseDragging) {
	viewCenter = viewCenter + tempViewCenter;
	mouseDragging = false;

	glutSetCursor(GLUT_CURSOR_INHERIT);
      }
    }
    break;
  case 3: // wheel up
    printf("zoomin in...");
    viewZoom *= 1.5;
    setupProjection();
    break;
  case 4: // wheel down
    viewZoom *= 0.5;
    setupProjection();
    break;
  }
}

static void handleResize(int32 w, int32 h) {
  windowWidth = w;
  windowHeight = h;

  glViewport(0, 0, w, h);

  setupProjection();
}

static void handleTimer(int t) {
  glutSetWindow(windowMain);
  glutPostRedisplay();
  glutTimerFunc(framePeriod, handleTimer, 0);
}

int main(int argc, char** argv) {
  cout << "hellos" << endl;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(windowWidth, windowHeight);

  windowMain = glutCreateWindow("test");

  glutDisplayFunc(handleDisplay);
  glutReshapeFunc(handleResize);

  glutMouseFunc(handleMouse);
  glutPassiveMotionFunc(handleMousePassiveMotion);
  glutMotionFunc(handleMouseMotion);
  
  glutKeyboardFunc(handleKeyboard);
  glutKeyboardUpFunc(handleKeyboardUp);
  glutSpecialFunc(handleKeyboardSpecial);  

  glutTimerFunc(framePeriod, handleTimer, 0);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  initUI();
  initGfx();
  initWorld();

  glutMainLoop();

  return 0;
}
