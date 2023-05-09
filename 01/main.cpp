#include "../utility.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>


int main() {

  std::vector<std::vector<int>> elves;

  elves = ChunkInput<int>("data.txt");

  // number of top elves to consider
  constexpr int largest_elves = 3;
  std::vector<int> totals(3, 0);

  for (const auto& elf : elves) {
    int elf_total = std::accumulate(elf.begin(), elf.end(), 0);
    if (elf_total > totals.front()) {
      totals.front() = elf_total;
      std::sort(totals.begin(), totals.end());
    }
  }

  std::cout << totals[0] << ',' << totals[1] << ',' << totals[2] << '\n';
  std::cout << std::accumulate(totals.begin(), totals.end(), 0) << '\n';




  return 0;
}
