#include "group.h"

using namespace std;

Group::Group() {

}

// public

void Group::add(Cell* cell) {
  this->cells.insert(cell);
  cell->addGroup(this);
}

bool Group::contains(Cell* cell) const {
  return this->cells.find(cell) != this->cells.end();
}

void Group::fill(int value) {
  for(Cell* cell : this->cells) {
    cell->remove(value);
  }
}

bool Group::fillOnlyOption() {
  array<int,4> optionCounts = {0};
  // fill optionCounts
  for(Cell* cell : this->cells) {
    int overflow = cell->getOptions();
    for(int i=0;i<optionCounts.size() && overflow;i++) {
      optionCounts[i] ^= overflow;
      overflow &= ~(optionCounts[i]);
    }
  }
  // check if 1000 occurs in any column
  int valueBinary = optionCounts[0];
  for(int i=1;i<optionCounts.size();i++) {
    valueBinary &= ~optionCounts[i];
  }
  if(valueBinary == 0) {
    return false;
  }
  // fill
  for(Cell* cell : this->cells) {
    int value = valueBinary & cell->getOptions();
    if(valueBinary & cell->getOptions()) {
      cell->fill(__builtin_ctz(value));
      return true;
    }
  }
  cout << "Unexpected behaviour in Group.fillOnlyOption" << endl;
  return false;
}

bool Group::removeNakedPair(const unordered_set<Cell*>& cells) {
  bool acted = false;
  int options = (*cells.begin())->getOptions();
  for(Cell* cell : this->cells) {
    if(cells.find(cell) != cells.end()) {
      continue;
    }
    if(cell->isEmpty()) {
      continue;
    }
    acted = acted || cell->removeAll(options);
  }
  return acted;
}

// private
