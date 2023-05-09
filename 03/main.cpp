#include "../utility.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <bitset>

constexpr uint32_t CharToBit(char c) {
  return (c <= 'Z') ? c - 'A' + 32 : c - 'a';
}

constexpr char BitToChar(uint32_t idx) {
  return (idx <= 32) ? idx + 'a' : idx - 32 + 'A';
}

constexpr int Priority(char c) {
  return (c <= 'Z') ? c - 'A' + 27 : c - 'a' + 1;
}


struct Rucksack {
  Rucksack(const std::vector<char>& contents_list) : contents(0) {

    const size_t stride = contents_list.size() / 2;

    for (int i = 0; i < stride; ++i) {
      contents |= (1ULL << CharToBit(contents_list[i]));
      contents |= (1ULL << CharToBit(contents_list[i + stride]));
    }
  }

  // Store contents as (00000000)ZYX...A(00000000)zyx...a
  uint64_t contents;
};


std::ostream &operator<<(std::ostream &stream, Rucksack r) {
  return stream << std::bitset<64>(r.contents);
}

int main() {

  auto lines = CharLineInput("data.txt");
  for (auto c : lines[0]) { std::cout << c; }
  std::cout << '\n';

  Rucksack r (lines[0]);
  std::cout << "000000";
  for (char c = 'Z'; c >= 'A'; --c) { std::cout << c; }
  std::cout << "000000";
  for (char c = 'z'; c >= 'a'; --c) { std::cout << c; }
  std::cout << '\n';


  int total = 0;
  for (auto l=lines.begin(); l!=lines.end(); ++l) {
    Rucksack r1 (*l);
    Rucksack r2 (*++l);
    Rucksack r3 (*++l);

    uint64_t overlap = r1.contents & r2.contents & r3.contents;

    int idx = 0;
    while((overlap & 1) == 0 and idx++ < 65) { overlap >>= 1; }
    total +=  Priority(BitToChar(idx));
  }

  std::cout << total;

  return 0;
}
