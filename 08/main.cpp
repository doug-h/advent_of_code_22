#include "../utility.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>

constexpr int plot_width  = 99;

int main() {

  int trees[plot_width][plot_width];


  std::ifstream ifs("data.txt");
  if (!ifs) { assert(0); }

  for (int row = 0; row < plot_width; ++row) {
    std::string s;
    std::getline(ifs, s);

    assert(s.size() == plot_width);
    for (int idx = 0; idx < plot_width; ++idx) {
      assert(s[idx] >= '0' and s[idx] <= '9');
      trees[row][idx] = (s[idx] - '0');
    }
  }

#if 0
  bool sight[plot_width][plot_width];



  for (int row = 0; row < plot_width; ++row) {
    int  highest_east = -1;
    for (int idx = 0; idx < plot_width; ++idx) {
      const bool visable = (trees[row][idx] > highest_east);
      sight[row][idx] = visable;
      highest_east = (visable) ? trees[row][idx] : highest_east;
    }
    int  highest_west = -1;
    for (int idx = plot_width - 1; idx >= 0; --idx) {
      const bool visable = (trees[row][idx] > highest_west);
      sight[row][idx] |= visable;
      highest_west = (visable) ? trees[row][idx] : highest_west;
    }
  }

  for (int column = 0; column < plot_width; ++column) {
    int highest_north = -1;
    for (int idx = 0; idx < plot_width; ++idx) {
      const bool visable = (trees[idx][column] > highest_north);
      sight[idx][column] |= visable;
      highest_north = (visable) ? trees[idx][column] : highest_north;

    }
    int highest_south = -1;
    for (int idx = plot_width - 1; idx >= 0; --idx) {
      const bool visable = (trees[idx][column] > highest_south);
      sight[idx][column] |= visable;
      highest_south = (visable) ? trees[idx][column] : highest_south;
    }
  }

  int total = 0;
  for (int row = 0; row < plot_width; ++row) {
    for (int idx = 0; idx < plot_width; ++idx) {
      std::cout << sight[row][idx];
      total += sight[row][idx];
    }
    std::cout << '\n';
  }
  std::cout << total;
#endif

  // Visability score is product of score for each cardinal direction,
  //  so we could ignore edges since score = a*b*c*0 = 0
  int visablility[plot_width][plot_width] = {0};

  for (int row = 0; row < plot_width; ++row) {
    for (int idx = 0; idx < plot_width; ++idx) {

      const int high_limit = trees[row][idx];
      visablility[row][idx] = 1;

      // RIGHT
      int vis = 0;
      for (int offset = 1; offset < plot_width - idx; ++offset) {
        ++vis;
        if (trees[row][idx + offset] >= high_limit) {
          break;
        }
      }
      visablility[row][idx] *= vis;
      // LEFT
      vis = 0;
      for (int offset = -1; idx + offset >= 0; --offset) {
        ++vis;
        if (trees[row][idx + offset] >= high_limit) {
          break;
        }
      }
      visablility[row][idx] *= vis;
      // DOWN
      vis = 0;
      for (int offset = 1; offset < plot_width - row; ++offset) {
        ++vis;
        if (trees[row + offset][idx] >= high_limit) {
          break;
        }
      }
      visablility[row][idx] *= vis;
      // UP
      vis = 0;
      for (int offset = -1; row + offset >= 0; --offset) {
        ++vis;
        if (trees[row + offset][idx] >= high_limit) {
          break;
        }
      }
      visablility[row][idx] *= vis;
    }
  }

  int highest = -1;
  for (int row = 0; row < plot_width; ++row) {
    for (int idx = 0; idx < plot_width; ++idx) {

      //std::cout << visablility[row][idx] << ',';
      highest = std::max(highest, visablility[row][idx]);
      if (visablility[row][idx] >= 2016) { std::cout << row << ',' << idx << '\n'; }
    }
    //std::cout << '\n';
  }
  std::cout << highest << '\n';

  return 0;
}
