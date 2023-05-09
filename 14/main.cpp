#include "../utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>
#include <ranges>

constexpr int padding = 1;
constexpr int GRID_WIDTH  = 70 + 2*padding;
constexpr int GRID_HEIGHT = 175 + padding;
constexpr int X_OFFSET = 479 - padding;;
// constexpr int GRID_WIDTH  = 10 + 2 * padding;
// constexpr int GRID_HEIGHT = 10 + padding;
// constexpr int X_OFFSET = 494 - padding;
constexpr int Y_OFFSET = 0;

struct Coord {
  int x;
  int y;
};

constexpr Coord startpos = {500, 0};

std::istream& operator>>(std::istream& source, Coord& c) {
  source >> c.x;
  source.ignore(1, ',');
  source >> c.y;

  return source;
}

void SizeGrid(int& min_x, int& max_x, int& min_y, int& max_y) {
  min_x = 500; max_x = 500;
  min_y = 0; max_y = 0;

  std::ifstream ifs("data.txt");
  std::string s;

  while (std::getline(ifs, s) and not s.empty()) {
    std::istringstream iss(s);
    while (not iss.eof()) {
      Coord c;
      iss >> c;
      iss.ignore(4);
      min_x = std::min(min_x, c.x);
      max_x = std::max(max_x, c.x);
      min_y = std::min(min_y, c.y);
      max_y = std::max(max_y, c.y);
    }
  }
}

void ParseLine(std::string s, int (*grid)[GRID_HEIGHT][GRID_WIDTH]) {
  Coord start;
  Coord end;
  std::istringstream line(s);
  line >> start;
  line.ignore(1, ' ');
  while (line.peek() == '-') {
    line.ignore(3, ' ');
    line >> end;
    for (int x = std::min(start.x, end.x); x <= std::max(start.x, end.x); ++x) {
      for (int y = std::min(start.y, end.y); y <= std::max(start.y, end.y); ++y) {
        (*grid)[y - Y_OFFSET][x - X_OFFSET] = true;
      }
    }

    start = end;
    line.ignore(1, ' ');
  }
}

enum State {
  HALF_FILLED = 2,
  EMPTY = 0,
  TAKEN = 1
};

int DropSand(int (*grid)[GRID_HEIGHT][GRID_WIDTH]) {
  Coord sand = startpos;
  int grain_count = 0;
  while ((sand.x >= X_OFFSET) and (sand.x < (X_OFFSET + GRID_WIDTH)) and (sand.y >= Y_OFFSET) and (sand.y < (Y_OFFSET + GRID_HEIGHT))) {
    // Stop at bottom
    if (sand.y + 1 == Y_OFFSET + GRID_HEIGHT) {
      (*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] = TAKEN;
      grain_count = 1;
    }

    // Moves down
    else if ((*grid)[sand.y - Y_OFFSET + 1][sand.x - X_OFFSET] != TAKEN) { ++sand.y; continue;}

    // Falls off left side
    else if (sand.x <= X_OFFSET) {
      if ((*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] == EMPTY) {
        (*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] = HALF_FILLED;
        grain_count = GRID_HEIGHT - (sand.y - Y_OFFSET) - 1;
      }
      else if ((*grid)[sand.y - Y_OFFSET + 1][sand.x - X_OFFSET + 1] != TAKEN) {++sand.y; ++sand.x; continue;}
      else { (*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] = TAKEN; grain_count=1;}
    }

    // Moves down-left
    else if ((*grid)[sand.y - Y_OFFSET + 1][sand.x - X_OFFSET - 1] != TAKEN) { ++sand.y; --sand.x; continue;}

    // Falls off right side
    else if (sand.x - X_OFFSET + 1 >= GRID_WIDTH) {
      if ((*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] == EMPTY) {
        (*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] = HALF_FILLED;
        grain_count = GRID_HEIGHT - (sand.y - Y_OFFSET) - 1;
      }
      else if ((*grid)[sand.y - Y_OFFSET + 1][sand.x - X_OFFSET - 1] != TAKEN) { ++sand.y; --sand.x; continue;}
      else { (*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] = TAKEN; grain_count=1;}
    }

    // Moves down-right
    else if ((*grid)[sand.y - Y_OFFSET + 1][sand.x - X_OFFSET + 1] != TAKEN) {++sand.y;++sand.x;continue;}

    // Stops
    else {
      (*grid)[sand.y - Y_OFFSET][sand.x - X_OFFSET] = TAKEN;
      grain_count = 1;
    }
    if (sand.x = startpos.x and sand.y == startpos.y) {
      grain_count = 0;
    }
    return grain_count;
  }
  assert(0);
  return 0;
}

int main() {
  int min_x, max_x, min_y, max_y;
  SizeGrid(min_x, max_x, min_y, max_y);
  Print(min_x, max_x, min_y, max_y);

  int padding = 1;
  assert((max_x - min_x + 1 + 2 * padding) == GRID_WIDTH);
  assert((max_y - min_y + 1 + padding) == GRID_HEIGHT);
  assert(X_OFFSET == min_x - padding);
  assert(Y_OFFSET == min_y);

  int grid[GRID_HEIGHT][GRID_WIDTH] = {0};

  std::ifstream ifs("data.txt");
  while (not ifs.eof()) {
    std::string s;
    std::getline(ifs, s);
    ParseLine(std::move(s), &grid);
  }

  assert(grid[startpos.y - Y_OFFSET][startpos.x - X_OFFSET] == EMPTY);

  for (auto y : IN_RANGE(GRID_HEIGHT)) {
    for (auto x : IN_RANGE(GRID_WIDTH)) {
      std::cout << grid[y][x];
    }
    std::cout << '\n';
  }

  int i = 0;
  while (int c = DropSand(&grid)) {
    i += c;


  }
  std::cout << i+1 << '\n';
  for (auto y : IN_RANGE(GRID_HEIGHT)) {
    for (auto x : IN_RANGE(GRID_WIDTH)) {
      std::cout << grid[y][x];
    }
    std::cout << '\n';
  }




  return 0;
}
