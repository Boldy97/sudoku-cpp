#include "grid.h"

using namespace std;

Grid::Grid() {
  this->initialiseCells();
  this->initialiseGroups();
  this->initialiseIntersections();
}

// public

void Grid::reset() {
  this->guesses = 0;
  this->backtracks = 0;
  for(Cell* cell : this->cells) {
    cell->reset();
  }
}

void Grid::fill(string& text) {
  for(int i=0;i<81;i++) {
    if(text[i] == '.') {
      text[i] = '0';
    }
    this->cells[i]->fill(text[i]-'0');
  }
}

void Grid::solve() {
  while(this->isNotSolved()) {
    this->step();
  }
}

void Grid::printOptions() {
  for(int i=0;i<9;i++) {
    for(int j=0;j<9;j++) {
      Cell* cell = this->cells[9*i+j];
      cout << "(" << i << "," << j << ")" << ":" << *cell << ":[";
      for(int k=1;k<10;k++) {
        if(cell->hasOption(k)) {
          cout << k << ",";
        }
      }
      cout << "]" << endl;
    }
    cout << "-------------------" << endl;
  }
  cout << endl;
}

ostream& operator<<(ostream& os, const Grid& grid) {
  os << "-------------------" << endl;
  int clueCount = 0;
  for(Cell* cell : grid.cells) {
    if(!cell->isEmpty()) {
      clueCount++;
    }
  }
  os << "filled in clues : " << clueCount << endl;
  os << "-------------------" << endl;
  os << "Guesses : " << grid.guesses << "   Backtracks : " << grid.backtracks << endl;
  for(int i=0;i<9;i++) {
    if(i%3 == 0) {
      os << "-------------------" << endl;
    }
    for(int j=0;j<9;j++) {
      os << (j%3 == 0 ? '|' : ' ') << *(grid.cells[9*i+j]);
    }
    os << "|" << endl;
  }
  os << "-------------------" << endl;
  for(int i=0;i<81;i++) {
    os << *(grid.cells[i]);
  }
  return os << endl << "-------------------" << endl;
}

// private

void Grid::initialiseCells() {
  for(int i=0;i<81;i++) {
    this->cells[i] = new Cell(i);
  }
}

void Grid::initialiseGroups() {
  // rows
  for(int i=0;i<9;i++) { // row
    Group* group = new Group();
    this->groups.insert(group);
    for(int j=0;j<9;j++) { // column
      group->add(this->cells[9*i+j]);
    }
  }
  // columns
  for(int i=0;i<9;i++) { // column
    Group* group = new Group();
    this->groups.insert(group);
    for(int j=0;j<9;j++) { // row
      group->add(this->cells[9*j+i]);
    }
  }
  // squares
  for(int i=0;i<3;i++) { // row
    for(int j=0;j<3;j++) { // column
      Group* group = new Group();
      this->groups.insert(group);
      for(int k=0;k<3;k++) { // inner row
        for(int l=0;l<3;l++) { // inner column
          group->add(this->cells[9*(3*i+k)+3*j+l]);
        }
      }
    }
  }
}

void Grid::initialiseIntersections() {
  for(auto itr = this->groups.begin(); itr != this->groups.end(); itr++) {
    Group* group1 = *itr;
    auto jtr = itr;
    for(jtr++; jtr != this->groups.end(); jtr++) {
      Group* group2 = *jtr;
      Intersection* intersection = Intersection::create(group1, group2);
      if(intersection != NULL) {
        this->intersections.insert(intersection);
      }
    }
  }
}

bool Grid::isNotSolved() {
  for(Cell* cell : this->cells) {
    if(cell->isEmpty()) {
      return true;
    }
  }
  return false;
}

void Grid::step() {
  if(!this->isValid()) {
    this->backtrack();
    return;
  }
  if(this->stepOnlyOption()) {
    return;
  }
  if(this->stepOnlyOptionInGroup()) {
    return;
  }
  if(this->stepGroupIntersect()) {
    return;
  }
  if(this->stepGuess(2)) {
    return;
  }
  if(this->stepNakedPair()) {
    return;
  }
  this->stepGuess(9);
}

bool Grid::stepOnlyOption() {
  // detect if there is only one option for that cell
  for(Cell* cell : this->cells) {
    int value = cell->getOnlyOption();
    if(value != 0) {
      cell->fill(value);
      return true;
    }
  }
  return false;
}

bool Grid::stepOnlyOptionInGroup() {
  // detect if there is only one option for a value in a group
  for(Group* group : this->groups) {
    if(group->fillOnlyOption()) {
      return true;
    }
  }
  return false;
}

bool Grid::stepGroupIntersect() {
  // fancy rule with group intersections
  for(Intersection* intersection : intersections) {
    int options = Cell::getOptionsUnion(intersection->i);
    for(int value=1;value<10;value++) {
      if(((options >> value) & 1) == 0) {
        continue;
      }
      if(!Cell::hasOption(intersection->ll, value)) {
        if(Cell::remove(intersection->rr, value)) {
          return true;
        }
      }
      if(!Cell::hasOption(intersection->rr, value)) {
        if(Cell::remove(intersection->ll, value)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Grid::stepNakedPair() {
  // find naked pairs
  bool acted = false;
  unordered_map<int,unordered_set<Cell*>> distinctOptions = Cell::getDistinctTwoOptions(this->cells);
  for(auto& it : distinctOptions) {
    if(it.second.size() < 2) {
      continue;
    }
    Group* group = Cell::getSharedGroup(it.second);
    if(group == NULL) {
      continue;
    }
    acted = acted || group->removeNakedPair(it.second);
  }
  return acted;
}

bool Grid::stepGuess(int maxOptions) {
  Cell* cell = getCellWithLeastOptions();
  if(cell == NULL) {
    this->backtrack();
    return true;
  }
  if(cell->getOptionCount() > maxOptions) {
    return false;
  }
  int value = cell->getFirstOption();
  cell->setGuess(value);
  this->push();
  cell->fill(value);
  return true;
}

void Grid::push() {
  this->guesses++;
  for(Cell* cell : this->cells) {
    cell->push();
  }
}

void Grid::pop() {
  this->backtracks++;
  for(Cell* cell : this->cells) {
    cell->pop();
  }
}

void Grid::backtrack() {
  this->pop();
  Cell* cell = this->getCellWithGuess();
  int guess = cell->getGuess();
  cell->setGuess(0);
  cell->remove(guess);
}

Cell* Grid::getCellWithLeastOptions() {
  int count = 10;
  Cell* result = NULL;
  for(Cell* cell : this->cells) {
    int c = cell->getOptionCount();
    if(c != 0 && c < count) {
      count = c;
      result = cell;
    }
  }
  return result;
}

Cell* Grid::getCellWithGuess() {
  for(Cell* cell : this->cells) {
    if(cell->getGuess() != 0) {
      return cell;
    }
  }
  return NULL;
}

bool Grid::isValid() {
  for(Cell* cell : this->cells) {
    if(!cell->isValid()) {
      return false;
    }
  }
  return true;
}
