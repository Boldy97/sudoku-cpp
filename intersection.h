#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <unordered_set>

#include "group.h"

class Intersection {
  public:
    static Intersection* create(Group*, Group*);
    std::unordered_set<Cell*> l;
    std::unordered_set<Cell*> r;
    std::unordered_set<Cell*> i;
    std::unordered_set<Cell*> ll;
    std::unordered_set<Cell*> rr;
  private:
};

#endif
