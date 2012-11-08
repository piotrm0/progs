#include <string>
#include <glm/glm.hpp>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#define ANCHOR_LEFT_BOTTOM 0
#define ANCHOR_LEFT_MID 1
#define ANCHOR_RIGHT_MID 2
#define ANCHOR_MID_BOTTOM 3
#define ANCHOR_MID_TOP 4

using namespace std;

void drawStringAtWorld(string s, glm::vec3 pos, int a, float height, float padding);
float stringLength(void* font, string s);
glm::vec2 coordScreenToWorld(glm::vec2 screen, double* modelview);
glm::vec2 coordScreenToWorld(glm::vec2 screen);
glm::vec2 coordWorldToScreen(glm::vec3 world);

void rectangleVertices(float x, float y, float w, float h);
