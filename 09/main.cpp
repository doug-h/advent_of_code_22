#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>

template <typename T> int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}
struct Pos { int x; int y; };

struct Rope {
  std::vector<Pos> knots = std::vector<Pos>(10, Pos{0,0});

  enum Direction {
    LEFT,
    RIGHT,
    UP,
    DOWN
  };

  void Move(Direction d) {
    switch (d) {
    case LEFT:
      knots[0].x -= 1;
      break;
    case RIGHT:
      knots[0].x += 1;
      break;
    case UP:
      knots[0].y += 1;
      break;
    case DOWN:
      knots[0].y -= 1;
      break;
    }

    CatchUp();
  }


private:
  void CatchUp() {
    for (int knot = 0; knot < knots.size()-1; ++knot) {
      Pos& head = knots[knot];
      Pos& next = knots[knot+1];

      int dx = head.x - next.x;
      int dy = head.y - next.y;

      next.x += dx - sgn(dx);
      next.y += dy - sgn(dy);

      next.x += (dx != 0 and dy != 0 and dx * dx < dy * dy) ? sgn(dx) : 0;
      next.y += (dx != 0 and dy != 0 and dy * dy < dx * dx) ? sgn(dy) : 0;
    }

  }
};

void Print(Rope r) {
  for (int row = 15; row >= -5; --row) {
    for (int col = -11; col < 15; ++col) {
      if (col == r.knots[0].x and row == r.knots[0].y) {
        std::cout << 'H';
      }
      else if (col == r.knots[r.knots.size()-1].x and row == r.knots[r.knots.size()-1].y) {
        std::cout << 'T';
      }
      else {
        bool b = false;
        for(int i=1; i<r.knots.size()-1; ++i) {
          if (not b and col == r.knots[i].x and row == r.knots[i].y) {
            std::cout << i;
            b = true;
          }
        }
        if (not b) { std::cout << '.'; }
      }
    }
    std::cout << '\n';
  }
}


int main() {

  Rope r;

  std::map<std::tuple<int, int>, bool> visited;

  bool running = true;
  std::ifstream ifs("data.txt");

  while (not ifs.eof()) {
    std::string s;
    std::getline(ifs, s);
    std::istringstream iss(s);

    char c;
    int i;
    iss >> c >> i;
    for (auto _ = i; _--;) {
      switch (c) {
      case 'L' : r.Move(Rope::LEFT);  break;
      case 'R' : r.Move(Rope::RIGHT); break;
      case 'U' : r.Move(Rope::UP); break;
      case 'D' : r.Move(Rope::DOWN); break;
      }
      visited[std::tuple<int, int>(r.knots[r.knots.size()-1].y, r.knots[r.knots.size()-1].x)] = true;
      //Print(r);
      //std::cout << "----------------" << '\n';
    }
  }

  // for(auto [k,v] : visited) {
  //   auto [x,y] = k;
  // }

  std::cout << visited.size();

  // for(int i=4; i>=0; --i) {
  //   for(int j=0; j<6; ++j) {
  //     std::cout << vis[i][j];
  //   }
  //   std::cout << '\n';
  // }
  return 0;
}
