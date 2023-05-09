#include <cassert>
#include <charconv>
#include <fstream>
#include <iostream>
#include <queue>
#include <regex>
#include <string_view>
#include <unordered_map>
#include <vector>

struct RoomInput {
  std::string name;
  int flow_rate;
  std::vector<std::string> tunnels;
};
struct Room {
  int flow_rate;

  std::vector<int> tunnels;
};

static std::vector<std::string> names; // for debugging
std::vector<Room> ParseInput(const char *file) {
  std::vector<Room> rv;

  std::ifstream ifs(file);
  assert(ifs);
  std::regex re(
      R"(Valve ([A-Z]{2}) has flow rate=([0-9]+); tunnels? leads? to valves? ([A-Z]{2}(?:, [A-Z]{2})*))");

  std::vector<RoomInput> temp_input;
  std::string line;
  while (not ifs.eof()) {
    std::getline(ifs, line);

    RoomInput r;
    std::smatch match;
    if (std::regex_search(line, match, re)) {
      const std::string name = match[1].str();
      const std::string_view sv_flow = match[2].str();
      std::istringstream iss(match[3].str());

      r.name = name;
      std::from_chars(sv_flow.begin(), sv_flow.end(), r.flow_rate);

      std::string s_tunnel_id;
      while (iss >> s_tunnel_id) {
        std::string_view sv(s_tunnel_id.c_str(), 2);
        r.tunnels.emplace_back(sv);
      }
      temp_input.push_back(r);
    }
  }
  std::sort(temp_input.begin(), temp_input.end(),
            [](RoomInput a, RoomInput b) { return a.flow_rate > b.flow_rate; });

  // A lot of this can be removed
  for (auto r : temp_input) {
    names.push_back(r.name);
  }

  for (int i = 0; i < temp_input.size(); ++i) {
    Room r{temp_input[i].flow_rate, {}};
    for (std::string t : temp_input[i].tunnels) {
      int index = std::distance(names.begin(),
                                std::find(names.begin(), names.end(), t));
      r.tunnels.push_back(index);
    }
    rv.push_back(r);
  }

  return rv;
}

int Distance(int start, int end, const std::vector<Room> &rooms) {
  std::vector<int> distance(rooms.size(), -1);
  distance[start] = 0;

  std::queue<int> queue;
  queue.push(start);
  while (not queue.empty()) {
    const Room &r = rooms[queue.front()];
    if (queue.front() == end) {
      return distance[end];
    }
    for (auto t : r.tunnels) {
      if (distance[t] == -1) {
        queue.push(t);
        distance[t] = distance[queue.front()] + 1;
      }
    }
    queue.pop();
  }
  return -1;
}

struct Cave {
  int flow_rate;
  int shortest_path;
  int nearest_neighbout;
};

std::vector<std::vector<int>> distances;
std::vector<Cave> CompressGraph(const std::vector<Room> &rooms) {
  std::vector<Cave> rv;
  int n_caves =
      std::distance(rooms.begin(),
                    std::find_if(rooms.begin(), rooms.end(), [](const Room &r) {
                      return r.flow_rate == 0;
                    }));

  rv.resize(n_caves);
  distances.resize(n_caves);
  for (int i = 0; i < n_caves; ++i) {
    rv[i] = {rooms[i].flow_rate, std::numeric_limits<int>::max(), -1};

    distances[i].resize(n_caves);
    for (int j = 0; j < n_caves; ++j) {
      distances[i][j] = Distance(i, j, rooms);
      if (distances[i][j] > 1 and distances[i][j] < rv[i].shortest_path) {
        rv[i].shortest_path = distances[i][j];
        rv[i].nearest_neighbout = j;
      }
    }
  }

  return rv;
};

struct State {
  uint64_t valves = 0;
  int total_steam = 0;
  int time_left = 30;
  int position = 0;
  int steam_per_min = 0; // could be calc'd from valves
  int actor = 0;         // could restructure so time = 26*actors
                         //
  bool operator==(const State &other) const {
    return (time_left == other.time_left and position == other.position and
            valves == other.valves and actor == other.actor);
  }
};

// struct DebugState : State {
//   std::vector<State> history = {};
// };

template <> struct std::hash<State> {
  std::size_t operator()(State const &s) const {
    std::size_t h1 = std::hash<int>{}(s.valves);
    std::size_t h2 = std::hash<int>{}(s.time_left);
    std::size_t h3 = std::hash<int>{}(s.position);
    std::size_t h4 = std::hash<int>{}(s.actor);
    std::size_t seed = 0;
    seed ^= h1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    return seed;
  }
};

static std::unordered_map<State, int> best_totals;

int UpperBound(State present, const std::vector<Cave> &caves) {
  // Can be tightened...
  int total = present.steam_per_min * present.time_left + present.total_steam;
  for (int i = 0; i < caves.size(); ++i) {
    if (!(present.valves & (1 << i)) and
        distances[i][present.position] + 1 < present.time_left) {
      total += caves[i].flow_rate *
               (present.time_left - distances[i][present.position] - 1);
    }
  }

  return total;
}

int main() {
  auto rooms = ParseInput("input.txt");
  auto caves = CompressGraph(rooms);
  const size_t n_caves = caves.size();
  const int n_actors = 2;

  uint64_t all_open = (1 << n_caves) - 1;
  int best_total = 0;
  State best;

  int i_AA =
      std::distance(names.begin(), std::find(names.begin(), names.end(), "AA"));
  std::queue<State> queue;
  for (int i = 0; i < n_caves; ++i) {
    // Start at a cave with valve open
    State start;
    start.time_left = 26;
    start.position = i;
    start.time_left -= Distance(i_AA, i, rooms) + 1;
    start.valves = (1 << i);
    start.steam_per_min = caves[i].flow_rate;

    queue.push(start);
  }
  while (not queue.empty()) {
    State present = queue.front();
    // present.history.push_back((State)present);
    queue.pop();

    // Memo-ise
    if (best_totals.count(present) and
        best_totals[present] >= present.total_steam) {
      continue;
    }
    best_totals[present] = present.total_steam;

    // Early exit
    if (present.actor == 1 and UpperBound(present, caves) <= best_total) {
      continue;
    }

    for (int i = 0; i < n_caves; ++i) {
      if (i == present.position) {
        continue;
      }
      const int d = distances[present.position][i];
      if (present.time_left > d) {

        if (!(present.valves & (1 << i))) {
          State next = present;

          next.total_steam += present.steam_per_min * (d + 1);
          next.time_left -= (d + 1);
          next.position = i;
          next.steam_per_min += caves[i].flow_rate;
          next.valves |= (1 << i);
          queue.push(next);
        }

        State next = present;

        next.total_steam += present.steam_per_min * d;
        next.time_left -= d;
        next.position = i;
        queue.push(next);
      }
    }
    present.total_steam += present.steam_per_min * present.time_left;
    if (best_total < present.total_steam) {
      best = present;
    }
    best_total = std::max(best_total, present.total_steam);

    // std::cout << best_total << '\n';

    if (present.actor == 0) {
      for (int i = 0; i < n_caves; ++i) {
        State start = present;
        start.time_left = 26;
        ++start.actor;

        start.position = i;
        start.time_left -= Distance(i_AA, i, rooms);
        start.steam_per_min = 0;
        if (!(present.valves & (1 << i))) {
          start.steam_per_min = caves[i].flow_rate;
          start.time_left -= 1;
        }
        start.valves |= (1 << i);
        queue.push(start);
      }
    }
  }
  assert(n_caves <= 8 * sizeof(uint64_t));
  std::cout << best_total << '\n';
}
