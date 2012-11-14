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
  ui::Collection* uiRoot;
  ui::Vertical* uiToolbar;
  ui::Pane* uiBoard;
}

static void initGfx();
static void initUI(int* argc, char** argv);
static void initWorld();
static void drawGrid();

/*ostream& operator<<(ostream& os, const b2Vec2& v) {
  os << "[" << v.x << "," << v.y << "]";
  return os;
  }*/

void handleToolbarClick(ui::Element* e, glm::vec2 posDown, glm::vec2 posUp) {
  cout << "upped: " << (*e) << endl;
};

void drawBoard(ui::Element* e) {
  //ui::Pane* p = (ui::Pane*) e;

  //if (mouseDragging) {
  //    glTranslatef(tempViewCenter.x, tempViewCenter.y, 0);
  //  }

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
}

static void initUI(int* argc, char** argv) {
  uiManager = new ui::Manager(800, 600, argc, argv);

  printf("made manager\n");

  ui::Manager::manager = uiManager;

  uiRoot = new ui::Collection();
  uiRoot->position = glm::vec2(0,0);
  uiRoot->size = glm::vec2(800,600);
  uiManager->setRoot(uiRoot);

  uiToolbar = new ui::Vertical();
  uiToolbar->padding = 2;
  uiToolbar->position = glm::vec2(10, 10);
  uiToolbar->size = glm::vec2(100, 20);

  ui::Button* b = new ui::Button("Drag");
  b->size = glm::vec2(100, 20);  
  b->setUpHandler(&handleToolbarClick);

  uiToolbar->addChild(b);
  
  b = new ui::Button("Zoom");
  b->size = glm::vec2(100, 20);
  b->setUpHandler(&handleToolbarClick);

  uiToolbar->addChild(b);

  uiRoot->addChild(uiToolbar);

  uiBoard = new ui::Pane();
  uiBoard->drawHandler = &drawBoard;

  uiRoot->addChild(uiBoard);

  printf("finished initUI\n");
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

  printf("finished initGfx\n");
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

int main(int argc, char** argv) {
  cout << "hellos" << endl;

  initUI(&argc, argv);
  initGfx();
  initWorld();

  uiManager->mainLoop();

  return 0;
}
