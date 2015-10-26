#include <utility>
#include <cassert>
#include "Cell.h"

long double Cell::getDistance() const {
  assert(initialized);
  return distance;
}

void Cell::setDistance(long double d) {
  assert(initialized);
  distance = d;
}

pair<int, int> Cell::getPrevious() const {
  assert(initialized);
  return previous;
}

void Cell::setPrevious(pair<int, int> p) {
  previous = p;
  initialized = true;
}
