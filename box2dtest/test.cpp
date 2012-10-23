#include "test.h"

// largely based off of the testbed app in Box2D

using namespace std;

#define BODY_HULL 0
#define BODY_THRUSTER_FORWARD 1
#define BODY_THRUSTER_REAR 2
#define BODY_THRUSTER_LEFT 3
#define BODY_THRUSTER_RIGHT 4

namespace {
  int32 windowWidth = 800;
  int32 windowHeight = 600;
  int32 windowMain;
  int32 framePeriod = 16;
  b2Vec2 viewCenter = b2Vec2(0.0f, 0.0f);
  float viewZoom = 0.5f;

  float settingsHz = 60.0f;
  int32 settingsVelocityIterations = 8;
  int32 settingsPositionIterations = 3;

  DebugDraw debugDraw;

  b2Vec2 gravity;
  b2World* simWorld;
  b2BodyDef myBodyDef;

  b2Body* bodies[5];
}

ostream& operator<<(ostream& os, const b2Vec2& v) {
  os << "[" << v.x << "," << v.y << "]";
  return os;
}

static void initWorld() {
  uint32 flags = 0;
  flags |= b2Draw::e_shapeBit;
  flags |= b2Draw::e_jointBit;
  flags |= b2Draw::e_centerOfMassBit;
  flags |= b2Draw::e_pairBit;
  //flags |= b2Draw::e_aabbBit;

  debugDraw.SetFlags(flags);

  gravity.Set(0,0);
  simWorld = new b2World(gravity);

  simWorld->SetDebugDraw(&debugDraw);

  myBodyDef.type = b2_dynamicBody;
  myBodyDef.position.Set(0, 0);
  myBodyDef.angle = 0;

  bodies[BODY_HULL] = simWorld->CreateBody(&myBodyDef);

  b2PolygonShape boxShape;
  boxShape.SetAsBox(1,2);
  
  b2FixtureDef boxFixtureDef;
  boxFixtureDef.shape = &boxShape;
  boxFixtureDef.density = 1;
  bodies[BODY_HULL]->CreateFixture(&boxFixtureDef);  

  boxShape.SetAsBox(0.2, 0.4);

  myBodyDef.angle = M_PI_2;
  myBodyDef.position.Set(0, 1.0);
  bodies[BODY_THRUSTER_FORWARD] = simWorld->CreateBody(&myBodyDef);
  bodies[BODY_THRUSTER_FORWARD]->CreateFixture(&boxFixtureDef);

  myBodyDef.angle = -M_PI_2;
  myBodyDef.position.Set(0, -1.0);  
  bodies[BODY_THRUSTER_REAR] = simWorld->CreateBody(&myBodyDef);
  bodies[BODY_THRUSTER_REAR]->CreateFixture(&boxFixtureDef);

  myBodyDef.angle = M_PI;
  myBodyDef.position.Set(-0.5, 0.5);  
  bodies[BODY_THRUSTER_LEFT] = simWorld->CreateBody(&myBodyDef);
  bodies[BODY_THRUSTER_LEFT]->CreateFixture(&boxFixtureDef);

  myBodyDef.angle = 0;
  myBodyDef.position.Set(0.5, 0.5);  
  bodies[BODY_THRUSTER_RIGHT] = simWorld->CreateBody(&myBodyDef);
  bodies[BODY_THRUSTER_RIGHT]->CreateFixture(&boxFixtureDef);

  b2WeldJointDef jointDef;
  jointDef.bodyA = bodies[BODY_HULL];
  jointDef.collideConnected = false;

  jointDef.bodyB = bodies[BODY_THRUSTER_REAR];
  jointDef.localAnchorA = bodies[BODY_HULL]->GetLocalPoint(b2Vec2(0,-1));
  jointDef.localAnchorB = bodies[BODY_THRUSTER_REAR]->GetLocalPoint(b2Vec2(0,0.2));
  jointDef.referenceAngle = bodies[BODY_THRUSTER_REAR]->GetAngle() - bodies[BODY_HULL]->GetAngle();
  //cout << bodies[BODY_THRUSTER_REAR]->GetLocalPoint(b2Vec2(0,0)) << endl;
  simWorld->CreateJoint(&jointDef);

  jointDef.bodyB = bodies[BODY_THRUSTER_FORWARD];
  jointDef.localAnchorA = bodies[BODY_HULL]->GetLocalPoint(b2Vec2(0,1));
  jointDef.localAnchorB = bodies[BODY_THRUSTER_FORWARD]->GetLocalPoint(b2Vec2(0,-0.2));
  jointDef.referenceAngle = bodies[BODY_THRUSTER_FORWARD]->GetAngle() - bodies[BODY_HULL]->GetAngle();
  simWorld->CreateJoint(&jointDef);

  jointDef.bodyB = bodies[BODY_THRUSTER_LEFT];
  jointDef.localAnchorA = bodies[BODY_HULL]->GetLocalPoint(b2Vec2(-0.5,0.5));
  jointDef.localAnchorB = bodies[BODY_THRUSTER_LEFT]->GetLocalPoint(b2Vec2(0.2,0));
  jointDef.referenceAngle = bodies[BODY_THRUSTER_LEFT]->GetAngle() - bodies[BODY_HULL]->GetAngle();
  simWorld->CreateJoint(&jointDef);

  jointDef.bodyB = bodies[BODY_THRUSTER_RIGHT];
  jointDef.localAnchorA = bodies[BODY_HULL]->GetLocalPoint(b2Vec2(0.5,0.5));
  jointDef.localAnchorB = bodies[BODY_THRUSTER_RIGHT]->GetLocalPoint(b2Vec2(-0.2,0));
  jointDef.referenceAngle = bodies[BODY_THRUSTER_RIGHT]->GetAngle() - bodies[BODY_HULL]->GetAngle();
  simWorld->CreateJoint(&jointDef);
}

static void simStep() {
  float32 timeStep = settingsHz > 0.0f ? 1.0f / settingsHz : float32(0.0f);

  simWorld->Step(timeStep, settingsVelocityIterations, settingsPositionIterations);
}

static void handleDisplay() {
  simStep();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();

  simWorld->DrawDebugData();

  /*
  glBegin(GL_QUADS);
  glVertex3f(-1,-1,0);
  glVertex3f(1,-1,0);
  glVertex3f(1,1,0);
  glVertex3f(-1,1,0);
  glEnd();
  */

  glutSwapBuffers();
}

b2Vec2 bodyDirection(b2Body* b) {
  return (b2Vec2(b->GetTransform().q.c,
		 b->GetTransform().q.s));
}

static void handleKeyboard(uchar key, int x, int y) {
  switch (key) {
  case 27:
    printf("quitting\n");
    exit(0);
    break;
  case 'w':
    bodies[BODY_THRUSTER_REAR]->ApplyLinearImpulse(-1 * bodyDirection(bodies[BODY_THRUSTER_REAR])
						   ,bodies[BODY_THRUSTER_REAR]->GetWorldCenter());
    break;
  case 's':
    bodies[BODY_THRUSTER_FORWARD]->ApplyLinearImpulse(-1 * bodyDirection(bodies[BODY_THRUSTER_FORWARD])
						      ,bodies[BODY_THRUSTER_FORWARD]->GetWorldCenter());
    break;
  case 'a':
    bodies[BODY_THRUSTER_RIGHT]->ApplyLinearImpulse(-0.1 * bodyDirection(bodies[BODY_THRUSTER_RIGHT])
						    ,bodies[BODY_THRUSTER_RIGHT]->GetWorldCenter());
    break;
  case 'd':
    bodies[BODY_THRUSTER_LEFT]->ApplyLinearImpulse(-0.1 * bodyDirection(bodies[BODY_THRUSTER_LEFT])
						    ,bodies[BODY_THRUSTER_LEFT]->GetWorldCenter());
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

static void handleMouse(int32 button, int32 state, int32 x, int32 y) {
  switch(button) {
  case GLUT_LEFT_BUTTON:
    break;
 }
}

static void handleMouseMotion(int32 x, int32 y) {

}

static void handleResize(int32 w, int32 h) {
  windowWidth = w;
  windowHeight = h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float32 ratio = float32(w) / float32(h);
  b2Vec2 extents(ratio * 25.0f, 25.0f);
  extents *= viewZoom;
  b2Vec2 lower = viewCenter - extents;
  b2Vec2 upper = viewCenter + extents;
  gluOrtho2D(lower.x, upper.x, lower.y, upper.y);

  printf("resized to [%f,%f]-[%f,%f]\n", lower.x, lower.y, upper.x, upper.y);
}

static void handleTimer(int t) {
  glutSetWindow(windowMain);
  glutPostRedisplay();
  glutTimerFunc(framePeriod, handleTimer, 0);
}

int main(int argc, char** argv) {
  cout << "hellos" << endl;

  initWorld();

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(windowWidth, windowHeight);

  windowMain = glutCreateWindow("test");

  glutDisplayFunc(handleDisplay);
  glutReshapeFunc(handleResize);

  glutMouseFunc(handleMouse);
  glutMotionFunc(handleMouseMotion);
  
  glutKeyboardFunc(handleKeyboard);
  glutKeyboardUpFunc(handleKeyboardUp);
  glutSpecialFunc(handleKeyboardSpecial);  

  glutTimerFunc(framePeriod, handleTimer, 0);
  glutMainLoop();

  return 0;
}
