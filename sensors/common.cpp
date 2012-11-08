#include "common.h"

using namespace std;

ostream& operator<<(ostream& os, glm::vec2* e) {
  os << e->x << "," << e->y;
  return os;
}
