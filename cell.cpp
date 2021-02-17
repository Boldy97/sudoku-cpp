#include "cell.h"

using namespace std;
//using namespace plf;

Cell::Cell(int index) {
  this->index = index;
  this->reset();
}

// public

bool Cell::isValid() {
  return this->valid;
}

int Cell::getIndex() {
  return this->index;
}

int Cell::getGuess() {
  return this->guess;
}

void Cell::setGuess(int value) {
  this->guess = value;
}

void Cell::addGroup(Group* group) {
  this->groups.insert(group);
}

int Cell::getOptions() {
  return this->options;
}

int Cell::getOptionCount() {
  return this->optionCount;
}

void Cell::reset() {
  this->valid = true;
  this->value = 0;
  this->options = 1022;
  this->guess = 0;
  this->optionCount = 9;
  this->valueStack = stack<int>();
  this->optionsStack = stack<int>();
  this->guessStack = stack<int>();
  this->optionCountStack = stack<int>();
}

void Cell::fill(int value) {
  this->value = value;
  if(value == 0) {
    return;
  }
  this->options = 0;
  this->optionCount = 0;
  for(Group* group : this->groups) {
    group->fill(value);
  }
}

bool Cell::isEmpty() {
  return this->value == 0;
}

bool Cell::remove(int value) {
  if(!this->hasOption(value)) {
    return false;
  }
  this->options &= ~(1 << value);
  this->optionCount--;
  if(this->optionCount == 0) {
    this->valid = false;
  }
  return true;
}

bool Cell::removeAll(int options) {
  int newOptions = this->options & ~options;
  if(newOptions == this->options) {
    return false;
  }
  this->options = newOptions;
  this->recalculateOptionCount();
  return true;
}

int Cell::getOnlyOption() {
  if(this->optionCount != 1) {
    return 0;
  }
  return __builtin_ctz(this->options);
}

bool Cell::hasOption(int value) {
  return ((this->options >> value) & 1) == 1;
}

void Cell::push() {
  this->valueStack.push(this->value);
  this->optionsStack.push(this->options);
  this->guessStack.push(this->guess);
  this->optionCountStack.push(this->optionCount);
  this->guess = 0;
}

void Cell::pop() {
  this->valid = true;
  this->value = this->valueStack.top();
  this->options = this->optionsStack.top();
  this->guess = this->guessStack.top();
  this->optionCount = this->optionCountStack.top();
  this->valueStack.pop();
  this->optionsStack.pop();
  this->guessStack.pop();
  this->optionCountStack.pop();
}

int Cell::getFirstOption() {
  unsigned int v = this->options;
  static const int MultiplyDeBruijnBitPosition[32] = {
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
  };
  return MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];
}

ostream& operator<<(ostream& os, const Cell& cell) {
  return os << cell.value;
}

int Cell::getOptionsUnion(const std::unordered_set<Cell*>& cells) {
  int result = 0;
  for(Cell* cell : cells) {
    result = result || cell->options;
  }
  return result;
}

bool Cell::hasOption(const std::unordered_set<Cell*>& cells, int value) {
  for(Cell* cell : cells) {
    if(cell->hasOption(value)) {
      return true;
    }
  }
  return false;
}

bool Cell::remove(const std::unordered_set<Cell*>& cells, int value) {
  bool result = false;
  for(Cell* cell : cells) {
    result = result || cell->remove(value);
  }
  return result;
}

unordered_map<int,unordered_set<Cell*>> Cell::getDistinctTwoOptions(const array<Cell*,81>& cells) {
  unordered_map<int,unordered_set<Cell*>> result;
  for(Cell* cell : cells) {
    if(cell->optionCount != 2) {
      continue;
    }
    if(result.find(cell->options) == result.end()) {
      unordered_set<Cell*> set;
      set.insert(cell);
      result[cell->options] = set;
    } else {
      result[cell->options].insert(cell);
    }
  }
  return result;
}

Group* Cell::getSharedGroup(const unordered_set<Cell*>& cells) {
  unordered_set<Group*> groups;
  for(Cell* cell : cells) {
    for(Group* group : cell->groups) {
      if(groups.find(group) != groups.end()) {
        return group;
      }
      groups.insert(group);
    }
  }
  return NULL;
}

// private

void Cell::recalculateOptionCount() {
  this->optionCount = 0;
  int n = this->options;
  while(n) {
    n &= (n - 1);
    this->optionCount++;
  }
  if(this->optionCount == 0) {
    this->valid = false;
  }
}
