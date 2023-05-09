#include "../utility.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>


constexpr int WIDTH = 40;
constexpr int HEIGHT = 9;

int main() {
  std::ifstream ifs("data.txt");

  int X = 1;
  int cycle = 1;
  int signal_strength = 0;

  while (not ifs.eof()) {
    char h = ifs.peek();
    if (h == 'n') {
      SkipLine(ifs);
      if (X + 1 >= cycle % 40 and X - 1 <= cycle % 40) {
        std::cout << '#';
      }
      else {
        std::cout << '.';
      }
      ++cycle;
      if(cycle%40 == 0) { std::cout << '\n'; }
    }
    else if (h == 'a') {
      int V;
      IgnoreTo(ifs, ' ');
      ifs >> V;
      SkipLine(ifs);
      if (X + 1 >= cycle % 40 and X - 1 <= cycle % 40) {
        std::cout << '#';
      }
      else {
        std::cout << '.';
      }
      ++cycle;
      if(cycle%40 == 0) { std::cout << '\n'; }

      X += V;
      if (X + 1 >= cycle % 40 and X - 1 <= cycle % 40) {
        std::cout << '#';
      }
      else {
        std::cout << '.';
      }
      ++cycle;
      if(cycle%40 == 0) { std::cout << '\n'; }
    }
    else {
      assert(0);
    }
  }


  return 0;
}
