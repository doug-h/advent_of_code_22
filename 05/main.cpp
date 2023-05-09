#include "../utility.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <bitset>

/*
        [Q] [B]         [H]        
    [F] [W] [D] [Q]     [S]        
    [D] [C] [N] [S] [G] [F]        
    [R] [D] [L] [C] [N] [Q]     [R]
[V] [W] [L] [M] [P] [S] [M]     [M]
[J] [B] [F] [P] [B] [B] [P] [F] [F]
[B] [V] [G] [J] [N] [D] [B] [L] [V]
[D] [P] [R] [W] [H] [R] [Z] [W] [S]
 1   2   3   4   5   6   7   8   9
*/

using Stack = std::vector<char>;

struct Move { int count; int from; int to; };

std::istream&
operator>>( std::istream& source, Move& m ) {

  source.ignore(5, ' ');
  source >> m.count;

  source.ignore(1, ' ');
  source.ignore(5, ' ');
  source >> m.from;

  source.ignore(1, ' ');
  source.ignore(3, ' ');
  source >> m.to;

  return source;
}

std::ostream &operator<<(std::ostream &stream, Move m) {
  return stream << "move " << m.count << " from " << m.from << " to " << m.to;
}

std::vector<Stack> stacks(9);

void MakeMove(const Move m) {
    Stack& from = stacks[m.from-1];
    Stack& to = stacks[m.to-1];

    to.insert(to.end(), from.end()-m.count, from.end());
    from.resize(from.size()-m.count);
}

int main() {

  
  std::vector<Move> moves;

  stacks[0] = {'D', 'B', 'J', 'V'};
  stacks[1] = {'P', 'V', 'B', 'W', 'R', 'D', 'F'};
  stacks[2] = {'R', 'G', 'F', 'L', 'D', 'C', 'W', 'Q'};
  stacks[3] = {'W', 'J', 'P', 'M', 'L', 'N', 'D', 'B'};
  stacks[4] = {'H', 'N', 'B', 'P', 'C', 'S', 'Q'};
  stacks[5] = {'R', 'D', 'B', 'S', 'N', 'G'};
  stacks[6] = {'Z', 'B', 'P', 'M', 'Q', 'F', 'S', 'H'};
  stacks[7] = {'W', 'L', 'F'};
  stacks[8] = {'S', 'V', 'F', 'M', 'R'};

  std::ifstream ifs("data.txt");
  if (!ifs) { assert(0); }

  while (not ifs.eof()) {
    std::string s;
    std::getline(ifs, s);
    std::istringstream iss(s);

    Move m;
    iss >> m;
    moves.push_back(m);
  }

  for(const auto m: moves) {  MakeMove(m); }

  std::string output = "";
  for(auto s : stacks) {
    for (auto c : s) {
      std::cout << c;
    }
    output += s.back();
    std::cout << '\n';
  }

  std::cout << output;

  return 0;
}
