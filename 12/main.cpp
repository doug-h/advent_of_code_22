#include "../utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>

constexpr size_t width = 77;
constexpr size_t height = 41;

struct Coord {
  int x, y;
};

int main() {

  char map[height][width];
  int distance[height][width];
  bool visited[height][width] = {0};

  std::fill_n((int *)distance, height * width, std::numeric_limits<int>::max());

  std::ifstream ifs("data.txt");

  Coord start;
  Coord end;
  for (auto row = 0; row < height; ++row) {
    for (auto col = 0; col < width; ++col) {
      char c;
      ifs >> c;
      if (c == 'S') {
        start = {col, row};
        c = 'a';
      } else if (c == 'E') {
        end = {col, row};
        c = 'z';
      }
      map[row][col] = c;
    }
  }

  distance[end.y][end.x] = 0;
  std::vector<Coord> coord_stack = {end};

  int best = std::numeric_limits<int>::max();

  while (not coord_stack.empty()) {
    Coord c = coord_stack.back();
    if(map[c.y][c.x] == 'a') { best = std::min(best, distance[c.y][c.x]); }
    coord_stack.erase(coord_stack.end() - 1);
    visited[c.y][c.x] = true;
    for (int dy = -1; dy <= 1; ++dy) {
      if (c.y + dy < 0 or c.y + dy >= height) {
        continue;
      }
      for (int dx = -1; dx <= 1; ++dx) {
        if (c.x + dx < 0 or c.x + dx >= width) {
          continue;
        }
        if ((dx == 0) == ((dy == 0))) {
          continue;
        }
        Coord n = {c.x + dx, c.y + dy};
        int height = map[c.y][c.x] - map[n.y][n.x];
        if (height <= 1) {
          if (distance[c.y][c.x] + 1 < distance[n.y][n.x]) {
            coord_stack.push_back(n);
            distance[n.y][n.x] = distance[c.y][c.x] + 1;
            visited[n.y][n.x] = true;
          }
        }
      }
    }
  }

  for (auto row = 0; row < height; ++row) {
    for (auto col = 0; col < width; ++col) {
      if (distance[row][col] == std::numeric_limits<int>::max()) {
        distance[row][col] = 88;
      }
      printf("%03d,", distance[row][col]);
    }
    std::cout << '\n';
  }
  std::cout << best;
  return 0;
}
