#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "group.h"
//#include "plf_stack.h"

class Group;


class Cell {
  public:
    Cell(int);
    bool isValid();
    int getIndex();
    int getGuess();
    void setGuess(int);
    void addGroup(Group* group);
    int getOptions();
    int getOptionCount();
    void reset();
    void fill(int);
    bool isEmpty();
    bool remove(int);
    bool removeAll(int);
    int getOnlyOption();
    bool hasOption(int);
    void push();
    void pop();
    int getFirstOption();
    friend std::ostream& operator<<(std::ostream&, const Cell&);
    static int getOptionsUnion(const std::unordered_set<Cell*>&);
    static bool hasOption(const std::unordered_set<Cell*>&, int);
    static bool remove(const std::unordered_set<Cell*>&, int);
    static std::unordered_map<int,std::unordered_set<Cell*>> getDistinctTwoOptions(const std::array<Cell*,81>&);
    static Group* getSharedGroup(const std::unordered_set<Cell*>&);
  private:
    bool valid;
    int index;
    int value;
    int options;
    int guess;
    int optionCount;
    std::unordered_set<Group*> groups;
    std::stack<int> valueStack;
    std::stack<int> optionsStack;
    std::stack<int> guessStack;
    std::stack<int> optionCountStack;
    void recalculateOptionCount();
};

#endif
