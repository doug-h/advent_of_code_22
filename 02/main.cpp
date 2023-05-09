#include "../utility.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

enum RPS {
  ROCK = 1,
  PAPER = 2,
  SCISSORS = 3
};

enum Ending {
  LOSE = 0,
  DRAW = 3,
  WIN = 6
};

RPS RightMove(RPS o, Ending e) {
  int offset = RPS(((e/3)+2)%3);
  return RPS((int(o)+offset-1)%3+1);
}


#define XOR(b1,b2) (!(b1) != !(b2))

struct Move {
  Move(char o, char e) : opponent(RPS(int(o) - int('A') + int(ROCK))), 
                         response(RightMove(RPS(opponent), Ending(3*(int(e) - int('X')) + int(LOSE)))) 
  {}


  int Points() const{
    int points = response;

    bool even = (opponent+response)%2==0;
    if (opponent == response) { points += 3; }
    else if( XOR(response > opponent, even) ) {
      points += 6;
    }
    return points;
  }

  RPS opponent;
  RPS response;
};


int main() {

  auto data = ChunkInput<Move, char, char>("data.txt");
  auto moves = data[0];


  std::cout << std::accumulate(moves.begin(), moves.end(), 0, [](int acc, Move m){ return acc + m.Points(); });


  return 0;
}
