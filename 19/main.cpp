
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

#define EXAMPLE

#include "input.h"

struct Blueprint {
  int id;

  // cost_ingredient_product
  int cost_ore_ore;
  int cost_ore_clay;
  int cost_ore_obs;
  int cost_clay_obs;
  int cost_ore_geo;
  int cost_obs_geo;
};

std::ostream &operator<<(std::ostream &source, const Blueprint &b) {
  source << "Blueprint " << b.id << ": Each ore robot costs " << b.cost_ore_ore
         << " ore. Each clay robot costs " << b.cost_ore_clay
         << " ore. Each obsidian robot costs " << b.cost_ore_obs << " ore and "
         << b.cost_clay_obs << " clay. Each geode robot costs "
         << b.cost_ore_geo << " ore and " << b.cost_obs_geo << " obsidian.";
  return source;
}

[[nodiscard]] std::vector<Blueprint> Parse(const std::string_view in) {
  std::vector<Blueprint> rv;

  const char *head = in.data();
  const char *const end = in.data() + in.size();

  while (head < end) {
    Blueprint b;
    head += 10;
    auto result = std::from_chars(head, end, b.id);
    assert(result.ec != std::errc::invalid_argument);

    head = result.ptr + 23;
    result = std::from_chars(head, end, b.cost_ore_ore);
    assert(result.ec != std::errc::invalid_argument);

    head = result.ptr + 28;
    result = std::from_chars(head, end, b.cost_ore_clay);
    assert(result.ec != std::errc::invalid_argument);

    head = result.ptr + 32;
    result = std::from_chars(head, end, b.cost_ore_obs);
    assert(result.ec != std::errc::invalid_argument);

    head = result.ptr + 9;
    result = std::from_chars(head, end, b.cost_clay_obs);
    assert(result.ec != std::errc::invalid_argument);

    head = result.ptr + 30;
    result = std::from_chars(head, end, b.cost_ore_geo);
    assert(result.ec != std::errc::invalid_argument);

    head = result.ptr + 9;
    result = std::from_chars(head, end, b.cost_obs_geo);
    assert(result.ec != std::errc::invalid_argument);

    head = result.ptr + 11;
    rv.push_back(b);
  }
  return rv;
}

int main() {

  auto bs = Parse(input);
  std::cout << input << '\n';
  for (auto b : bs) {
    std::cout << b << '\n';
  }

  return 0;
}
