#include <list>
#include <iostream>
#include "common.h"
#include "glcommon.h"

namespace ui {

typedef void (click_handler)(glm::vec2 pos, uint button, uint state);
typedef void (over_handler)(glm::vec2 pos, uint state);
typedef void (motion_handler)(glm::vec2 pos);

#define MOUSE_OVER 0
#define MOUSE_OUT  1

class Element {
 private:

 public:
  bool visible;
  bool highlighted;
  bool isMouseOver;
  bool isSelected;

  list<Element*> children;
  list<Element*> active;

  click_handler* clickHandler;
  over_handler* overHandler;
  motion_handler* motionHandler;

  glm::vec2 position;
  glm::vec2 size;

  glm::vec2 lower;
  glm::vec2 upper;

  Element();
  virtual void draw();
  virtual void arrange();
  virtual bool isOver(glm::vec2 pos);
  virtual void overElements(glm::vec2 pos, list<Element*>* collector);
  virtual void handleMouseOver(glm::vec2 pos);
  virtual void handleMouseOut(glm::vec2 pos);
  virtual void handleMouseMotion(glm::vec2 pos);
  virtual bool handleMouseClick(glm::vec2 pos, uint button, uint state);

  void setClickHandler(click_handler* h);
  void setOverHandler(over_handler* h);
  void setMotionHandler(motion_handler* h);

  void addChild(Element* c);
  void removeChild(Element* c);
};

class Rectangle: public Element {
 public:
  Rectangle();
};

class Circle: public Element {
 public:
  Circle();
};

class Vertical: public Rectangle {
 public:
  float padding;

  Vertical();
  void draw();
  void arrange();
};

class Collection: public Rectangle {
 private:
 public:
};

class Button: public Rectangle {
 public:
  std::string label;
  Button(std::string l);
  void draw();
  void arrange();
  bool handleMouseClick(glm::vec2, uint button, uint state);
};

 class Manager: public Element {
 public:
  Manager();
  Element* getObjectAtScreen(glm::vec2 pos);
};

}
