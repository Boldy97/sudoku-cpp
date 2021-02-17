#ifndef GROUP_H
#define GROUP_H

#include <array>
#include <unordered_set>

#include "cell.h"

class Cell;

class Group {
  public:
    Group();
    void add(Cell*);
    bool contains(Cell*) const;
    void fill(int);
    bool fillOnlyOption();
    bool removeNakedPair(const std::unordered_set<Cell*>&);
  private:
    std::unordered_set<Cell*> cells;
  friend class Intersection;
};

#endif
