#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

long long moddish(long long k, long long n) {
  return ((k %= n) <= 0) ? k + n : k;
}

using in_t = long long int;
main() {

  std::ifstream ifs("input.txt");
  std::vector<in_t> input;
  std::vector<in_t> indices;

  int idx = 0;
  while (!ifs.eof() and ifs.good()) {
    in_t i;
    ifs >> i;
    input.push_back(i * 811589153);
    indices.push_back(idx++);
  }

  for (auto l : input) {
    std::cout << l << ',';
  }
  std::cout << '\n';

  const int N = input.size();
  int count = 10;
  while (count-- > 0) {
    for (int i = 0; i < N; ++i) {
      in_t i_pos = 0;
      while (indices[i_pos] != i) {
        ++i_pos;
      }

      indices.erase(std::next(indices.begin(), i_pos));
      auto it =
          std::next(indices.begin(), moddish((i_pos + input[i]), (N - 1)));
      indices.insert(it, i);

    }
  }

  int offset = 0;
  while (input[indices[offset]] != 0) {
    ++offset;
  }
  std::cout << (input[indices[(offset + 1000) % N]] +
                input[indices[(offset + 2000) % N]] +
                input[indices[(offset + 3000) % N]]);

  return 0;
}
