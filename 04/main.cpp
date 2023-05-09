#include "../utility.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <bitset>

bool Contains(int s1, int e1, int s2, int e2) {
  return (((s1 >= s2) and (e1 <= e2))  or  ((s2 >= s1) and (e2 <= e1)));
}
bool Overlap(int s1, int e1, int s2, int e2) {
  return !((e2 < s1) or (e1 < s2));
}

struct Assignment  {
  Assignment(int s1, char pad_1, int e1, char pad_2, int s2, char pad_3, int e2) : elf_1{s1, e1}, elf_2{s2, e2}{
  }

  bool Overlap() { return ::Overlap(elf_1.front(), elf_1.back(), elf_2.front(), elf_2.back()); }

  using Section = std::vector<int>;
  Section elf_1;
  Section elf_2;
};

std::ostream &operator<<(std::ostream &stream, Assignment a) {
  return stream << '(' << a.elf_1.front() << ',' << a.elf_1.back() << ')' << ',' << '(' << a.elf_2.front() << ',' << a.elf_2.back() << ')';
}

int main() {

  auto data = ChunkInput<Assignment, int, char, int, char, int, char, int>("data.txt");
  auto lines = data[0];

std::cout << std::accumulate(lines.begin(), lines.end(), 0, [](int acc, Assignment a){ return acc + a.Overlap(); });


  return 0;
}
