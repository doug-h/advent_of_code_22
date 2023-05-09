#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <ranges>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "wind.input"

constexpr int width = 7;
using Row = uint8_t;

struct Rock {
  Rock(const std::array<std::array<int, 4>, 4> &pattern) : w(0), h(0) {
    for (int i = 0; i < 4; ++i) {
      Row r_i = 0;
      for (int j = 0; j < 4; ++j) {
        r_i |= (pattern[i][j]);
        r_i <<= (j < 3);

        w = std::max(w, (j + 1) * pattern[i][j]);
        h = std::max(h, (4 - i) * pattern[i][j]);
      }
      values[i] = r_i;
    }
  }

  int w, h;
  std::array<Row, 4> values;
};

std::ostream &operator<<(std::ostream &source, const Row &r) {
  for (int j = width - 1; j >= 0; --j) {
    source << ((r & (1 << j)) ? '#' : '.');
  }
  return source;
}
std::ostream &operator<<(std::ostream &source, const Rock &r) {
  for (int j = 0; j < 4; ++j) {
    source << r.values[j] << '\n';
  }
  return source;
}

template <typename T> T Shift(T t, int shift) {
  return (shift > 0) ? (t << shift) : (t >> -shift);
}

std::array<Rock, 5> rock_patterns = {
    Rock({{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}}}),
    Rock({{{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}}}),
    Rock({{{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}}}),
    Rock({{{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}}}),
    Rock({{{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 0, 0}, {1, 1, 0, 0}}})};

// Add rock according to wind
int AddRock(std::vector<Row> &cave, int wind_count, int rock_count) {
  int turn_count = 0;
  int x = 2;
  int y = cave.size();
  const int before = cave.size();

  const Rock &rock = rock_patterns[rock_count];

  bool can_move = true;
  while (can_move) {
    const char wind = input[(wind_count+turn_count)%input.size()];
    x += (wind == '>') * 2 - 1;
    x = std::clamp(x, 0, width - rock.w);
    ++turn_count;

    for (int i = 0; can_move and (i < rock.h) and ((y + i) < before); ++i) {
      can_move =
          can_move and !(Shift(Row(rock.values[3 - i]), (3 - x)) & cave[y + i]);
    }
    if (!can_move) {
      x -= (wind == '>') * 2 - 1;
      can_move = true;
    }
    if (!y) {
      break;
    }

    --y;
    for (int i = 0; can_move and (i < rock.h) and ((y + i) < before); ++i) {
      can_move =
          can_move and !(Shift(rock.values[3 - i], (3 - x)) & cave[y + i]);
    }
    if (!can_move) {
      ++y;
    }
  }

  cave.resize(std::max(size_t(y + rock.h + 3), cave.size()));
  for (int i = 0; i < rock.h; ++i) {
    cave[y + i] |= Shift(rock.values[3 - i], (3 - x));
  }

  return turn_count;
}

std::array<int, 7> Profile(const std::vector<Row> &cave) {
  std::array<int, 7> profiles{};
  int i = cave.size() - 3;
  while (i >= 0 and std::any_of(profiles.begin(), profiles.end(),
                                [](int a) { return a == 0; })) {
    for (int j = 0; j < 7; ++j) {
      if (!profiles[j] and ((1 << (6 - j)) & cave[i])) {
        profiles[j] = (cave.size() - i);
      }
    }
    --i;
  }
  return profiles;
}
size_t Hash(int turn, int rocks, const std::vector<Row> &cave) {
  auto profiles = Profile(cave);
  size_t seed = 0;
  seed ^= std::hash<int>{}(turn) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= std::hash<int>{}(rocks) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[0]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[1]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[2]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[3]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[4]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[5]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[6]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  seed ^= std::hash<int64_t>{}(profiles[7]) + 0x9e3779b9 + (seed << 6) +
          (seed >> 2);
  return seed;
}

int main() {

  const int64_t rock_limit = 1000000000000;
  std::vector<Row> cave(3, 0);
  std::unordered_map<size_t, int64_t> cache;
  std::unordered_map<size_t, size_t> heights;
  std::unordered_map<size_t, size_t> rocks;

  int64_t turn = 0;
  int64_t rock = 0;

  int wind_count = 0;
  int rock_count = 0;

  size_t height_offset = 0;
  bool cycle_found = false;
  do {
    int dt = AddRock(cave, wind_count, rock_count);
    turn += dt;
    ++rock;

    wind_count = (wind_count + dt) % input.size();
    rock_count = rock % rock_patterns.size();


    if (!cycle_found) {
      size_t key = Hash(wind_count, rock_count, cave);
      if (cache.count(key)) {
        cycle_found = true;
        std::cout << "Cycle found " << cache[key] << ' ' << turn << '\n';
        std::cout << "Rocks " << rock - rocks[key] << '\n';
        int64_t rocks_during_cycle = rock - rocks[key];
        int64_t cycles_to_skip = (rock_limit - rock) / rocks_during_cycle;
        height_offset += cycles_to_skip * (cave.size() - heights[key]);
        rock += cycles_to_skip * rocks_during_cycle;
        std::cout << (turn) << '\n';
      } else {
        cache[key] = turn;
        heights[key] = cave.size();
        rocks[key] = rock;
      }
    }
  } while (rock < rock_limit);

  std::cout << rock << '\n';
  std::cout << (cave.size() - 3 + height_offset);
}
