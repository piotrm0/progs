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

    for (it = children.begin();
	 it != children.end() && !captured;
	 ++it) {
      e = *it;
      if (e->isMouseOver) {
	captured = e->handleMouseClick(pos, button, state);
      }
    }

    if (NULL != clickHandler) {
      (*clickHandler)(this, pos, button, state);
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

  void Element::handleMouseDown(glm::vec2 pos) {
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

  void Element::draw() {
    //printf("drawing base element\n");

    list<Element*>::iterator it;
    Element* e;

    for (it = children.begin();
	 it != children.end();
	 ++it) {
      e = *it;

      e->draw();
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
    Element::handleMouseClick(pos, button, state);
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

  void Button::draw() {
    Element::draw();

    float height = size.y;
    float width = size.x;

    float col = 0;
    if (isPressed) {
      col = 1;
    }

    //  printf("drawing button\n");

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

  Pane::Pane() {
    float viewZoom = 1.0f;
    glm::vec2 viewCenter = glm::vec2(0,0);
    glm::vec2 tempViewCenter = glm::vec2(0,0);
  }

  void Pane::draw() {
    if (NULL != drawHandler) {
      (*drawHandler)(this);
    }
  }

  void Pane::arrange() {
    size = upper - lower;
  }

  Manager::Manager() {  
    downElement = NULL;    
  }

  Element* Manager::handleMouseClick(glm::vec2 pos, uint button, uint state) {    
    Element* e = NULL;

    if (button == GLUT_LEFT_BUTTON &&
	state == GLUT_DOWN) {
      e = Element::handleMouseClick(pos, button, state);
      if (NULL != e) {
	downElement = e;
	downPos = pos;
      }
      e->handleMouseDown(pos);
    } else if (state == GLUT_UP) {
      if (NULL != downElement) {
	downElement->handleMouseUp(downPos, pos);
	downElement = NULL;
      }
    }

    return e;
  }

  Element* Manager::getObjectAtScreen(glm::vec2 pos) {
    return NULL;
  }

}
