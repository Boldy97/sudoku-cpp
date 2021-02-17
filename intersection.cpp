#include "intersection.h"

using namespace std;

// public

Intersection* Intersection::create(Group* left, Group* right) {
  Intersection* result = new Intersection();
  result->l = left->cells;
  result->r = right->cells;
  for(Cell* cell : left->cells) {
    if(right->contains(cell)) {
      result->i.insert(cell);
    }
  }
  if(result->i.size() == 0) {
    return NULL;
  }
  for(Cell* cell : left->cells) {
    if(result->i.find(cell) == result->i.end()) {
      result->ll.insert(cell);
    }
  }
  for(Cell* cell : right->cells) {
    if(result->i.find(cell) == result->i.end()) {
      result->rr.insert(cell);
    }
  }
  return result;
}

// private
