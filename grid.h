#ifndef GRID_H
#define GRID_H

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>

#include "cell.h"
#include "group.h"
#include "intersection.h"

class Grid {
  public:
    Grid();
    void reset();
    void fill(std::string& text);
    void solve();
    void printOptions();
    friend std::ostream& operator<<(std::ostream&, const Grid&);
  private:
    int guesses;
    int backtracks;
    std::array<Cell*,81> cells;
    std::unordered_set<Group*> groups;
    std::unordered_set<Intersection*> intersections;
    void initialiseCells();
    void initialiseGroups();
    void initialiseIntersections();
    bool isNotSolved();
    void step();
    bool stepOnlyOption();
    bool stepOnlyOptionInGroup();
    bool stepOnlyOptionInGroupBinary();
    bool stepGroupIntersect();
    bool stepNakedPair();
    bool stepGuess(int);
    void push();
    void pop();
    void backtrack();
    Cell* getCellWithLeastOptions();
    Cell* getCellWithGuess();
    bool isValid();
};

#endif
