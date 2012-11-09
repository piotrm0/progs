#include <list>
#include <iostream>
#include "common.h"
#include "glcommon.h"

#define MOUSE_OVER 0
#define MOUSE_OUT  1

namespace ui {
  class Element;

  typedef void (click_handler)(Element* e, glm::vec2 pos, uint button, uint state);
  typedef void (over_handler)(Element* e, glm::vec2 pos, uint state);
  typedef void (motion_handler)(Element* e, glm::vec2 pos);
  //typedef void (handler)(Element* e, glm::vec2 pos);

  typedef void (draw_handler)(Element* e);

  typedef void (down_handler)(Element* e, glm::vec2 pos);
  typedef void (up_handler)(Element* e, glm::vec2 posDown, glm::vec2 posUp);

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

    down_handler* downHandler;
    up_handler* upHandler;

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
    virtual void handleMouseDown(glm::vec2 pos);
    virtual void handleMouseUp(glm::vec2 pos, glm::vec2 pos);
    virtual void handleMouseMotion(glm::vec2 pos);
    virtual Element* handleMouseClick(glm::vec2 pos, uint button, uint state);

    void setDownHandler(down_handler* h);
    void setUpHandler(up_handler* h);
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
    void arrange();
  };

  class Collection: public Rectangle {
  private:
  public:
  };

  class Button: public Rectangle {
  public:
    bool isPressed;
    std::string label;
    Button(std::string l);
    void draw();
    void arrange();
    Element* handleMouseClick(glm::vec2 pos, uint button, uint state);
    void handleMouseUp(glm::vec2 posDown, glm::vec2 posUp);
    void handleMouseDown(glm::vec2 pos);
  };

  class Pane: public Rectangle {
  public:
    Pane();

    float viewZoom;
    glm::vec2 viewCenter;
    glm::vec2 tempViewCenter;
    draw_handler* drawHandler;

    void draw();
    void arrange();
  };

  class Manager: public Element {
  public:
    Element* downElement;
    glm::vec2 downPos;

    Manager();
    Element* getObjectAtScreen(glm::vec2 pos);
    Element* handleMouseClick(glm::vec2 pos, uint button, uint state);
  };
  
}

ostream& operator<<(ostream& os, ui::Element& e);
