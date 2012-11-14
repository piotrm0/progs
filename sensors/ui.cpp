#include "ui.h"

using namespace std;

ostream& operator<<(ostream& os, ui::Element& e) {
  os << "Element "
     << " pos: " << e.position
     << " size:" << e.size
     << " lower:" << e.lower
     << " upper:" << e.upper;
  return os;
}

namespace ui {

  Element::Element() {
    position = glm::vec2(0,0);
    size     = glm::vec2(0,0);

    lower = glm::vec2(0,0);
    upper = glm::vec2(0,0);

    visible = true;
    highlighted = false;
    isMouseOver = false;
    isSelected = false;

    clickHandler = NULL;
    overHandler = NULL;
    motionHandler = NULL;
    downHandler = NULL;
    upHandler = NULL;
  }

  void Element::setDownHandler(down_handler* h) { downHandler = h; }
  void Element::setUpHandler(up_handler* h) { upHandler = h; }
  void Element::setClickHandler(click_handler* h) { clickHandler = h; }
  void Element::setOverHandler(over_handler* h) { overHandler = h; }
  void Element::setMotionHandler(motion_handler* h) { motionHandler = h; }

  bool Element::isOver(glm::vec2 pos) {
    return (glm::all(glm::greaterThanEqual(pos, lower)) &&
	    glm::all(glm::lessThanEqual(pos, upper)));
  }

  Element* Element::handleMouseClick(glm::vec2 pos, uint button, uint state) {
    list<Element*>::iterator it;
    Element* e;
    Element* captured = NULL;

    cout << "Element::handleMouseClick" << endl;

    for (it = children.begin();
	 it != children.end() &&
	   NULL == captured;
	 ++it) {
      e = *it;
      if (e->isMouseOver) {
	captured = e->handleMouseClick(pos, button, state);
      }
    }

    cout << "handled children" << endl;

    if (NULL != clickHandler) {
      (*clickHandler)(this, pos, button, state);
    }
 
    cout << "clickHandler handled" << endl;

    return captured;
  }

  void Element::handleMouseMotion(glm::vec2 pos) {
    if (isMouseOver) {
      if (! isOver(pos)) {
	handleMouseOut(pos);
      }
    } else if (isOver(pos)) {
      handleMouseOver(pos);
    }

    list<Element*>::iterator it;
    Element* e;

    for (it = children.begin();
	 it != children.end();
	 ++it) {
      e = *it;
      e->handleMouseMotion(pos);
    }
  }

  void Element::handleMouseDown(glm::vec2 pos) {
    cout << "Element::handleMouseDown" << endl;
    if (NULL != downHandler) {
      (*downHandler)(this, pos);
    }
  }

  void Element::handleMouseUp(glm::vec2 posDown, glm::vec2 posUp) {
    if (NULL != upHandler) {
      (*upHandler)(this, posDown, posUp);
    }
  }

  void Element::handleMouseOver(glm::vec2 pos) {
    // call some handler
    isMouseOver = true;
    highlighted = true;

    if (NULL != overHandler) {
      (*overHandler)(this, pos, MOUSE_OVER);
    }

  }

  void Element::handleMouseOut(glm::vec2 pos) {
    // call some handler
    isMouseOver = false;
    highlighted = false;

    if (NULL != overHandler) {
      (*overHandler)(this, pos, MOUSE_OUT);
    }
  }

  void Element::overElements(glm::vec2 pos, list<Element*>* collector) {
    if (! isOver(pos)) {
      return;
    }

    collector->push_back(this);

    list<Element*>::iterator it;
    Element* e;

    if (it != children.end()) {
      e = *it;
      e->arrange();
      lower = e->lower;
      upper = e->upper;
      size = e->size;
    }

    for (children.begin();
	 it != children.end();
	 it++) {
      e = *it;

      e->overElements(pos, collector);
    }
  }

  void Element::draw(glm::vec2 lower, glm::vec2 upper) {
    list<Element*>::iterator it;
    Element* e;

    for (it = children.begin();
	 it != children.end();
	 ++it) {
      e = *it;

      e->draw(lower, upper);
    }    

    if (highlighted) {
      glColor4f(1,0,0,0.5);
      glBegin(GL_LINE_LOOP);
      rectangleVertices(position.x, position.y,
			size.x, size.y);
      glEnd();
    }
  }

  void Element::addChild(Element* c) {
    children.push_back(c);
    c->parent = this;
    c->root = root;
    c->manager = manager;
  }
  void Element::removeChild(Element* c) {
    children.remove(c);
    c->parent = NULL;
    c->root = NULL;
    c->manager = NULL;
  }

  void Element::arrange() {
    list<Element*>::iterator it;
    Element* e;

    it = children.begin();
    if (it != children.end()) {
      e = *it;
      e->arrange();

      lower = e->lower;
      upper = e->upper;
      size = e->size;
    }

    for (;it != children.end();
	 it++) {
      e = *it;
      e->arrange();
    
      if (lower.x > e->lower.x) {
	lower.x = e->lower.x;
      }
      if (lower.y > e->lower.y) {
	lower.y = e->lower.y;
      }
      if (upper.x < e->upper.x) {
	upper.x = e->upper.x;
      }
      if (upper.y < e->upper.y) {
	upper.y = e->upper.y;
      }
    }

    size = upper - lower;
  }

  Rectangle::Rectangle() {
  }

  Circle::Circle() {
  }

  Vertical::Vertical() {
    padding = 0;
  }

  void Vertical::arrange() {
    list<Element*>::iterator it;
    Element* e;

    glm::vec2 tempPos = position;

    for (it = children.begin();
	 it != children.end();
	 ++it) {
      e = *it;

      e->position = tempPos;

      tempPos.y += e->size.y;
      tempPos.y += padding;

      e->arrange();
    }

    Rectangle::arrange();
  }

  Button::Button(string l) {
    label = l;
    isPressed = false;
  }

  void Button::arrange() {
    lower = position;
    upper = position + size;
  }

  Element* Button::handleMouseClick(glm::vec2 pos, uint button, uint state) {
    cout << "Button::handleMouseClick" << endl;
    //Element::handleMouseClick(pos, button, state);
    return this;
  }

  void Button::handleMouseDown(glm::vec2 pos) {
    Element::handleMouseDown(pos);
    isPressed = true;
  }

  void Button::handleMouseUp(glm::vec2 posDown, glm::vec2 posUp) {
    Element::handleMouseUp(posDown, posUp);
    isPressed = false;
  }

  void Button::draw(glm::vec2 lower, glm::vec2 upper) {
    Element::draw(lower, upper);

    float height = size.y;
    float width = size.x;

    float col = 0;
    if (isPressed) {
      col = 1;
    }

    glPushMatrix();

    glTranslatef(position.x, position.y, 0);

    glColor4f(col,0,0,0.25); glBegin(GL_QUADS); rectangleVertices(0,0,height,height); glEnd();
    glColor4f(col,0,0,1); glBegin(GL_LINE_LOOP); rectangleVertices(0,0,height,height); glEnd();
    glColor4f(col,0,0,0.05); glBegin(GL_QUADS); rectangleVertices(0,0,width,height); glEnd();
    glColor4f(col,0,0,0.5); glBegin(GL_LINE_LOOP); rectangleVertices(0,0,width,height); glEnd();

    glColor4f(0,0,0,1);
    drawStringAtWorld(label, glm::vec3(height,height/2,0), ANCHOR_LEFT_MID, height*0.8, 0.1f);

    glPopMatrix();
  }

  void Collection::arrange() {
    list<Element*>::iterator it;
    Element* e;

    for (it = children.begin();
	 it != children.end();
	 it++) {
      e = *it;
      e->arrange();
    }
  }

  Pane::Pane() {
    viewZoom = 1.0f;
    viewCenter = glm::vec2(0,0);
    tempViewCenter = glm::vec2(0,0);

    worldLower = glm::vec2(0,0);
    worldUpper = glm::vec2(10,10);
  }

  void Pane::arrange() {
    //Element::arrange();

    position = glm::vec2(0,0);
    upper = root->upper;
    lower = root->lower;
    size = upper - lower;

    glm::vec2 extents(10.0f, 10.0f);

    float ratio;

    float w = manager->windowWidth;
    float h = manager->windowHeight;

    if (w < h) {
      ratio = h / w;
      extents.y *= ratio;
    } else {
      ratio = w / h;
      extents.x *= ratio;
    }

    extents *= viewZoom;
    worldLower = viewCenter - extents;
    worldUpper = viewCenter + extents;

    cout << "viewZoom = " << viewZoom << endl;
    cout << "viewCenter = " << viewCenter << endl;
    cout << "arranged, lower = " << lower << endl;
    cout << "arranged, upper = " << upper << endl;
    cout << "arranged, worldLower = " << worldLower << endl;
    cout << "arranged, worldUpper = " << worldUpper << endl;
  }

  void Pane::draw(glm::vec2 lower, glm::vec2 upper) {
    Element::draw(lower, upper);

    if (NULL != drawHandler) {
      glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
      gluOrtho2D(worldLower.x, worldUpper.x, worldLower.y, worldUpper.y);

      glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
      glTranslatef(viewCenter.x, viewCenter.y, 0);

      (*drawHandler)(this);

      glMatrixMode(GL_MODELVIEW); glPopMatrix();
      glMatrixMode(GL_PROJECTION); glPopMatrix();
    }
  }

  Manager* Manager::manager;

  namespace Manager {
    uint windowWidth;
    uint windowHeight;
    int windowMain;
    int framePeriod;

    glm::vec2 lower;
    glm::vec2 upper;
    glm::vec2 size;

    Element* root;

    keyboard_handler* keyboardHandler;
    keyboard_special_handler* keyboardSpecialHandler;

    Element* downElement;
    glm::vec2 downPos;
  }

  Manager::Manager(uint w, uint h, int* argc, char** argv) {  
    downElement = NULL;
    windowWidth = w;
    windowHeight = h;
    lower = glm::vec2(0,0);
    upper = glm::vec2(w,h);
    size = upper - lower;
    framePeriod = 16;

    glutInit(argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);

    windowMain = glutCreateWindow("test");

    glutDisplayFunc(&rootHandleDisplay);
    glutReshapeFunc(&rootHandleResize);

    glutMouseFunc(&rootHandleMouse);
    glutPassiveMotionFunc(&rootHandleMousePassiveMotion);
    glutMotionFunc(&rootHandleMouseMotion);
  
    glutKeyboardFunc(&rootHandleKeyboard);
    glutKeyboardUpFunc(&rootHandleKeyboardUp);
    glutSpecialFunc(&rootHandleKeyboardSpecial);  

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glutTimerFunc(framePeriod, &rootHandleTimer, 0);
  }

  void Manager::mainLoop() {
    glutMainLoop();
  }

  void Manager::setRoot(Element* e) {
    e->manager = manager;
    e->root = e;
    root = e;
    arrange();
  }

  void Manager::arrange() {
    lower = glm::vec2(0,0);
    upper = glm::vec2(windowWidth, windowHeight);
    size = upper - lower;
    
    if (NULL != root) {
      root->position = lower;
      root->lower = lower;
      root->upper = upper;
      root->size = size;
    }

    root->arrange();
  }

  void Manager::draw(glm::vec2 lower, glm::vec2 upper) {
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    root->draw(lower, upper);
  }

  void Manager::rootHandleDisplay() {
    glClearColor(0xff, 0xff, 0xff, 0xff);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw(glm::vec2(0,0), glm::vec2(windowWidth, windowHeight));

    glutSwapBuffers();
  }

  void Manager::setKeyboardHandler(keyboard_handler* h) { keyboardHandler = h; };
  void Manager::setKeyboardSpecialHandler(keyboard_special_handler* h) { keyboardSpecialHandler = h; };

  void Manager::rootHandleKeyboard(uchar key, int x, int y) {
    switch (key) {
    case 27:
      printf("quitting\n");
      exit(0);
      break;
    default:
      if (NULL != keyboardHandler) {
	(*keyboardHandler)(key, glm::vec2(x, y));
      }
      break;
    }
  }

  void Manager::rootHandleKeyboardSpecial(int key, int x, int y) {
    switch (key) {
    default:
      if (NULL != keyboardSpecialHandler) {
	(*keyboardSpecialHandler)(key, glm::vec2(x, y));
      }
      break;
    }
  }

  void Manager::rootHandleKeyboardUp(uchar key, int x, int y) {  
    switch (key) {
    default:
      if (NULL != keyboardHandler) {
	(*keyboardHandler)(key, glm::vec2(x, y));
      }
      break;
    }
  }
  
  void Manager::rootHandleMousePassiveMotion(int32 x, int32 y) {
    root->handleMouseMotion(glm::vec2(x,windowHeight - y));
  }

  void Manager::rootHandleMouseMotion(int32 x, int32 y) {
    //    if (mouseDragging) {
    //      glm::vec2 cCoord = coordScreenToWorld(glm::vec2(x,y), mouseDragStartModelview);
      //tempViewCenter = cCoord - mouseDragStart;
    //    }
    rootHandleMousePassiveMotion(x, y);
  }

  void Manager::rootHandleMouse(int32 button, int32 state, int32 x, int32 y) {
    if (handleMouseClick(glm::vec2(x, windowHeight - y), button, state)) {
      return;
    }

    switch(button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
	
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glTranslatef(viewCenter.x, viewCenter.y, 0);
	//glGetDoublev(GL_MODELVIEW_MATRIX, mouseDragStartModelview);
	
	//mouseDragStart = coordScreenToWorld(glm::vec2(x, y), mouseDragStartModelview);
	//      mouseDragging = true;
	
	//handleMouseMotion(x, y);
	
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
      } else if (state == GLUT_UP) {
	//if (mouseDragging) {
	//viewCenter = viewCenter + tempViewCenter;
	//mouseDragging = false;
	
	glutSetCursor(GLUT_CURSOR_INHERIT);
      }
      break; 
    }
  }

  void Manager::rootHandleResize(int32 w, int32 h) {
    glViewport(0, 0, w, h);

    windowWidth = w;
    windowHeight = h;

    arrange();
  }

  void Manager::rootHandleTimer(int t) {
    glutSetWindow(windowMain);
    glutPostRedisplay();
    glutTimerFunc(framePeriod, rootHandleTimer, 0);
  }

  Element* Manager::handleMouseClick(glm::vec2 pos, uint button, uint state) {    
    Element* e = NULL;
    
    cout << "Manager::handleMouseClick" << endl;

    if (button == GLUT_LEFT_BUTTON &&
	state == GLUT_DOWN) {
      e = root->handleMouseClick(pos, button, state);
      cout << "got an e=" << e << endl;
      if (NULL != e) {
	cout << "e=" << (*e) << endl;
	cout << "not null" << endl;
	downElement = e;
	downPos = pos;
	e->handleMouseDown(pos);
      }
    } else if (state == GLUT_UP) {
      if (NULL != downElement) {
	downElement->handleMouseUp(downPos, pos);
	downElement = NULL;
      }
    }

    cout << "Manager::handleMouseClick done" << endl;

    return e;
  }

  Element* Manager::getObjectAtScreen(glm::vec2 pos) {
    return NULL;
  }
}
