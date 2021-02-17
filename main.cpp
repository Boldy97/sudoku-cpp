#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

#include "main.h"
#include "grid.h"

using namespace std;

int main(int argc, char* argv[]) {
  bool loggingRegular = false;
  bool loggingTiming = false;
  int countLine = 1;
  int countSkip = 0;
  int countRepeat = 1;
  for(int i=0;i<argc;i++) {
    string arg = string(argv[i]);
    if(arg == "-o") {
      loggingRegular = true;
    }
    if(arg == "-t") {
      loggingTiming = true;
    }
    if(arg == "-n") {
      countLine = strtol(argv[i+1], NULL, 10);
    }
    if(arg == "-s") {
      countSkip = strtol(argv[i+1], NULL, 10);
    }
    if(arg == "-r") {
      countRepeat = strtol(argv[i+1], NULL, 10);
    }
  }
  run(loggingRegular, loggingTiming, countLine, countSkip, countRepeat);
  return 0;
}

void run(bool loggingRegular, bool loggingTiming, int countLine, int countSkip, int countRepeat) {
  Grid grid;
  string line;
  unsigned long totalDuration = 0;
  for(int i=0;i<countSkip+countLine;i++) {
    getline(cin, line);
    line = line.substr(0,81);

    if(i<countSkip) {
      continue;
    }

    for(int j=0;j<countRepeat;j++) {
      grid.reset();
      auto t0 = chrono::high_resolution_clock::now();
      grid.fill(line);
      //if(loggingRegular) cout << grid;
      grid.solve();
      auto t1 = chrono::high_resolution_clock::now();
      if(loggingRegular) cout << grid;

      chrono::duration<float> fs = t1 - t0;
      chrono::microseconds duration = std::chrono::duration_cast<chrono::microseconds>(fs);
      if(loggingTiming) cout << duration.count() << " µs" << endl;
      totalDuration += duration.count();
    }
  }

  //cout << "Total duration : " << totalDuration << " µs" << endl;
  cout << "Average duration : " << totalDuration/(countLine*countRepeat) << " µs" << endl;
}

// TODO go over all code again, to see if it cant be improved with bit operations

// TODO keep track of guesses in grid, not in cell. more efficient lookup when backtracking => not worth it, most time is spent not guessing, even for things with a lot of guesses
// TODO HARD find pair subsets
// TODO HARD detect x-wing

// profiling
// g++ *.cpp -o main -pg && ./main -s 2126 -r 10 < sudokus.txt && gprof ./main | py gprof2dot.py | dot -Tpng -o output.png

// benchmark
// g++ *.cpp -o main && ./main -r 100 < sudokus.txt && ./main -s 2126 -r 50 < sudokus.txt && ./main -n 1000 < sudokus.txt

// g++ *.cpp -o main && ./main -o -t < easy-sudoku.txt
// g++ *.cpp -o main && ./main -o -t < hard-sudoku.txt

// easy / hard / 1k
// 8.5ms / 340ms / 35.6ms
// --- optionCount tracking ---
// 6.3ms / 252ms / 26.0ms
// --- guess 2 and stepNakedPair  ---
// 6.9ms / 253ms / 26.3ms
// --- improved stepOnlyOptionInGroup --- 16.4%
// 5.2ms / 207ms / 22.0ms
// --- early exit from guessing --- 28.7%
// 4.3ms / 147ms / 15.7ms
// -- ?? merging 2 methods ?? --- 42.0%
// 2.17ms / 73.6ms / 9.11ms
// --- better isValid detection --- 4.5%
// 2.11ms / 71.1ms / 8.70ms
// --- removeNakedPair check for empty before removing options --- 2.5%
// 1.99ms / 68.9ms / 8.48ms
// --- easier Cell::getOnlyOption --- 4.1%
// 1.90ms / 66.2ms / 8.14ms
