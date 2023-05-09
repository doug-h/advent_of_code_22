#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <fstream>
#include <iostream>
#include <istream>
#include <queue>
#include <ranges>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "input.h"

constexpr int cube_size = 20;
struct In {
  int x, y, z;
};

std::ostream &operator<<(std::ostream &source, const In &in) {
  source << '(' << in.x << ',' << in.y << ',' << in.z << ')';
  return source;
}

[[nodiscard]] std::vector<In> ParseInput(const std::string_view input_data) {
  std::vector<In> blocks;

  const char *head = input_data.data();
  const char *const end = input_data.data() + input_data.size();

  while (head < end) {
    In in;
    auto result = std::from_chars(head, end, in.x);
    assert(result.ec != std::errc::invalid_argument);
    head = result.ptr + 1;

    result = std::from_chars(head, end, in.y);
    assert(result.ec != std::errc::invalid_argument);
    head = result.ptr + 1;

    result = std::from_chars(head, end, in.z);
    assert(result.ec != std::errc::invalid_argument);
    head = result.ptr + 1;

    blocks.push_back(in);
  }

  return blocks;
}

using Side = std::array<std::array<uint32_t, cube_size>, cube_size>;
std::array<Side, 3> CreateCube(const std::vector<In> &blocks) {
  std::array<Side, 3> cube{};

  Side &x_side = cube[0];
  Side &y_side = cube[1];
  Side &z_side = cube[2];

  for (auto b : blocks) {
    x_side[b.y][b.z] |= (1 << b.x);
    y_side[b.x][b.z] |= (1 << b.y);
    z_side[b.x][b.y] |= (1 << b.z);
  }

  return cube;
}

int CountSlice(uint32_t slice) {
  int count = 0;
  uint32_t n1 = slice & ~(slice << 1);
  uint32_t n2 = slice & ~(slice >> 1);

  for (int i = 0; i < cube_size; ++i) {
    count += bool(n1 & (1 << i));
    count += bool(n2 & (1 << i));
  }

  return count;
}
int CountFaces(const std::array<Side, 3> &cube) {
  int count = 0;

  for (int i = 0; i < cube_size; ++i) {
    for (int j = 0; j < cube_size; ++j) {
      count += CountSlice(cube[0][i][j]);
      count += CountSlice(cube[1][i][j]);
      count += CountSlice(cube[2][i][j]);
    }
  }

  return count;
}

bool BlockAt(int i, int j, int k, const Side &side) {
  return (i >= 0 and i < cube_size and j >= 0 and j < cube_size and k >= 0 and
          k < cube_size and ((1 << i) & side[j][k]));
}
bool BlockAt(In coord, const std::array<Side, 3> &cube) {
  return BlockAt(coord.x, coord.y, coord.z, cube[0]);
}

int FloodCount(const std::array<Side, 3> &cube) {
  // Walk round (-1, 20)^3 cube, counting sides
  int count = 0;
  std::queue<In> queue;
  std::array<std::array<uint32_t, cube_size + 2>, cube_size + 2> visited {};

  std::vector<In> offsets = {{-1, 0, 0}, {1, 0, 0},  {0, -1, 0},
                             {0, 1, 0},  {0, 0, -1}, {0, 0, 1}};
  In start = {-1, -1, -1};
  queue.push(start);

  while (!queue.empty()) {
    const In p = queue.front();
    queue.pop();
    for (In f : offsets) {
      In neighbour = {p.x + f.x, p.y + f.y, p.z + f.z};
      // Out of box
      if (neighbour.x < -1 or neighbour.x > cube_size or neighbour.y < -1 or
          neighbour.y > cube_size or neighbour.z < -1 or
          neighbour.z > cube_size) {
        continue;
      }
      // Already seen
      if ((1 << (neighbour.x + 1)) &
          visited[neighbour.y + 1][neighbour.z + 1]) {
        assert(!BlockAt(neighbour, cube));
        continue;
      }
      // Space contains block
      if (BlockAt(neighbour, cube)) {
        // Count adjacent face
        ++count;
        continue;
      }

      // All checks pass, neighbour is new empty space within bounds
      queue.push(neighbour);
      visited[neighbour.y + 1][neighbour.z + 1] |= (1 << (neighbour.x + 1));
    }
  }

  return count;
}

int main() {

  auto blocks = ParseInput(input);
  auto cube = CreateCube(blocks);

  std::cout << CountFaces(cube) << '\n';
  std::cout << FloodCount(cube) << '\n';

  return 0;
}
