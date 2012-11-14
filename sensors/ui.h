#include <list>
#include <iostream>
#include <functional>
#include "common.h"
#include "glcommon.h"

#define MOUSE_OVER 0
#define MOUSE_OUT  1

namespace ui {
  class Element;
  class Manager;

  typedef void (click_handler)(Element* e, glm::vec2 pos, uint button, uint state);
  typedef void (over_handler)(Element* e, glm::vec2 pos, uint state);
  typedef void (motion_handler)(Element* e, glm::vec2 pos);

  typedef void (draw_handler)(Element* e);

  typedef void (down_handler)(Element* e, glm::vec2 pos);
  typedef void (up_handler)(Element* e, glm::vec2 posDown, glm::vec2 posUp);

  typedef void (keyboard_handler)(int key, glm::vec2 pos);
  typedef void (keyboard_special_handler)(uchar key, glm::vec2 pos);

  class Element {
  private:
    
  public:
    bool visible;
    bool highlighted;
    bool isMouseOver;
    bool isSelected;
    
    list<Element*> children;
    Element* parent;
    Element* root;
    Manager* manager;

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
    virtual void draw(glm::vec2 lower, glm::vec2 upper);
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
  public:
    void arrange();
  };

  class Button: public Rectangle {
  public:
    bool isPressed;
    std::string label;
    Button(std::string l);
    void draw(glm::vec2 lower, glm::vec2 upper);
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
    glm::vec2 worldLower;
    glm::vec2 worldUpper;

    draw_handler* drawHandler;

    void draw(glm::vec2 lower, glm::vec2 upper);
    void arrange();
  };

  class Manager {
  public:
    static Manager* manager;

    static uint windowWidth;
    static uint windowHeight;
    static int windowMain;
    static int framePeriod;

    static glm::vec2 lower;
    static glm::vec2 upper;
    static glm::vec2 size;

    static Element* root;

    static keyboard_handler* keyboardHandler;
    static keyboard_special_handler* keyboardSpecialHandler;

    static Element* downElement;
    static glm::vec2 downPos;

    Manager(uint w, uint h, int* argc, char** argv);
    static Element* getObjectAtScreen(glm::vec2 pos);
    static Element* handleMouseClick(glm::vec2 pos, uint button, uint state);
    static void draw(glm::vec2 lower, glm::vec2 upper);

    static void setRoot(Element* e);

    static void arrange();

    static void setKeyboardHandler(keyboard_handler* h);
    static void setKeyboardSpecialHandler(keyboard_special_handler* h);

    static void mainLoop();

    static void rootHandleDisplay();
    static void rootHandleKeyboard(uchar key, int x, int y);
    static void rootHandleKeyboardSpecial(int key, int x, int y);
    static void rootHandleKeyboardUp(uchar key, int x, int y);
    static void rootHandleMousePassiveMotion(int x, int y);
    static void rootHandleMouseMotion(int x, int y);
    static void rootHandleMouse(int button, int state, int x, int y);
    static void rootHandleResize(int w, int h);
    static void rootHandleTimer(int t);
  };
  
}

ostream& operator<<(ostream& os, ui::Element& e);
