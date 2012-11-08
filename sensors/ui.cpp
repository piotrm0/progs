#include "ui.h"

using namespace ui;

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
}

ostream& operator<<(ostream& os, Element* e) {
  os << "Element "
     << " pos: " << &e->position
     << " size:" << &e->size
     << " lower:" << &e->lower
     << " upper:" << &e->upper;
  return os;
}

void Element::setClickHandler(click_handler* h) { clickHandler = h; }
void Element::setOverHandler(over_handler* h) { overHandler = h; }
void Element::setMotionHandler(motion_handler* h) { motionHandler = h; }

bool Element::isOver(glm::vec2 pos) {
  return (glm::all(glm::greaterThanEqual(pos, lower)) &&
	  glm::all(glm::lessThanEqual(pos, upper)));
}

bool Element::handleMouseClick(glm::vec2 pos, uint button, uint state) {
  list<Element*>::iterator it;
  Element* e;
  bool captured = false;

  for (it = children.begin();
       it != children.end() && !captured;
       ++it) {
    e = *it;
    if (e->isMouseOver) {
      captured = e->handleMouseClick(pos, button, state);
    }
  }

  if (NULL != clickHandler) {
    (*clickHandler)(pos, button, state);
  }
 
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

void Element::handleMouseOver(glm::vec2 pos) {
  // call some handler
  isMouseOver = true;
  highlighted = true;

  if (NULL != overHandler) {
    (*overHandler)(pos, MOUSE_OVER);
  }

}

void Element::handleMouseOut(glm::vec2 pos) {
  // call some handler
  isMouseOver = false;
  highlighted = false;

  if (NULL != overHandler) {
    (*overHandler)(pos, MOUSE_OUT);
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

void Element::draw() {
  //printf("drawing base element\n");
  glColor4f(1,0,0,(highlighted ? 0.5 : 0.1));
  glBegin(GL_QUADS);
  rectangleVertices(position.x, position.y,
		    size.x, size.y);
  glEnd();

  glColor4f(1,0,0,0.5);
  glBegin(GL_LINE_LOOP);
  rectangleVertices(position.x, position.y,
		    size.x, size.y);
  glEnd();
}

void Element::addChild(Element* c) {
  children.push_back(c);
}
void Element::removeChild(Element* c) {
  children.remove(c);
}

void Element::arrange() {
  list<Element*>::iterator it;
  Element* e;

  printf("base arrange\n");

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

void Vertical::draw() {
  Element::draw();

  list<Element*>::iterator it;
  Element* e;

  for (it = children.begin();
       it != children.end();
       ++it) {
    e = *it;

    e->draw();
  }
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
}

void Button::arrange() {
  lower = position;
  upper = position + size;
}

void Button::draw() {
  Element::draw();

  float height = size.y;
  float width = size.x;

  //  printf("drawing button\n");

  glPushMatrix();

  glTranslatef(position.x, position.y, 0);

  glColor4f(0,0,0,0.25); glBegin(GL_QUADS); rectangleVertices(0,0,height,height); glEnd();
  glColor4f(0,0,0,1); glBegin(GL_LINE_LOOP); rectangleVertices(0,0,height,height); glEnd();
  glColor4f(0,0,0,0.05); glBegin(GL_QUADS); rectangleVertices(0,0,width,height); glEnd();
  glColor4f(0,0,0,0.5); glBegin(GL_LINE_LOOP); rectangleVertices(0,0,width,height); glEnd();

  glColor4f(0,0,0,1);
  drawStringAtWorld(label, glm::vec3(height,height/2,0), ANCHOR_LEFT_MID, height*0.8, 0.1f);

  glPopMatrix();
}

bool Button::handleMouseClick(glm::vec2 pos, uint button, uint state) {
  cout << "pressed " << label << endl;
  return true;
}

Manager::Manager() {  
}

Element* Manager::getObjectAtScreen(glm::vec2 pos) {
  return NULL;
}
