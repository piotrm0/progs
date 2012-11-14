#include <iostream>
#include <glm/glm.hpp>

typedef unsigned char uchar;
typedef unsigned int uint;

typedef int int32;
typedef float float32;

using namespace std;

ostream& operator<<(ostream& os, glm::vec2& e);

// http://cppkid.wordpress.com/2008/09/04/getters-and-setters-for-a-class-using-a-macro/
#define GETSET(type, var)\
private:\
type _##var;\
public: \
  type get##var() { return _##var; }\
  void set##var(type val) { _##var = val; }
