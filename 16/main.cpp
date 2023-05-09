#include <cassert>
#include <charconv>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct RoomInput {
  std::string name;
  int flow_rate;
  std::vector<std::string> tunnels;
};

struct Room {
  int index;
  int flow_rate;

  std::vector<int> tunnels;
};

// For printing
static std::vector<std::string> names;

std::ostream &operator<<(std::ostream &source, const Room &r) {
  source << "Room " << names[r.index] << ':' << r.flow_rate
         << ". Connected to:";
  for (auto i : r.tunnels) {
    source << ' ' << names[i];
  }
  source << '.';

  return source;
}

[[nodiscard]] std::vector<Room> ParseInput(const char *file) {
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

  // Now we replace XY labels with an index so rooms can be accessed
  // continuously.
  for (auto r : temp_input) {
    names.push_back(r.name);
  }
  std::sort(names.begin(), names.end());

  // Obviously a slow way of doing this
  for (const RoomInput &i : temp_input) {
    int index = std::distance(names.begin(),
                              std::find(names.begin(), names.end(), i.name));
    Room r{index, i.flow_rate, {}};
    for (std::string t : i.tunnels) {
      index = std::distance(names.begin(),
                            std::find(names.begin(), names.end(), t));
      r.tunnels.push_back(index);
    }
    rv.push_back(r);
  }

  std::sort(rv.begin(), rv.end(),
            [](Room a, Room b) { return a.index < b.index; });
  return rv;
}

int Distance(int start, int end, std::vector<Room> &rooms) {
  std::vector<int> distance(rooms.size(), -1);
  distance[start] = 0;

  std::queue<int> queue;
  queue.push(start);
  while (not queue.empty()) {
    Room &r = rooms[queue.front()];
    if (r.index == end) {
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

struct Edge {
  int index;
  int distance;
};

struct Node {
  int index;
  int flow_rate;
  std::vector<Edge> neighbours;
};

std::ostream &operator<<(std::ostream &source, const Node &r) {
  source << "Node " << names[r.index] << ':' << r.flow_rate
         << ". Connected to:";
  for (auto [index, distance] : r.neighbours) {
    if (index == -1) {
      continue;
    }
    source << ' ' << names[index] << ',' << distance;
  }
  source << '.';

  return source;
}

[[nodiscard]] std::vector<Node> ConstructNodes(std::vector<Room> &rooms) {
  std::vector<Node> rv;
  for (auto r : rooms) {
    if (r.flow_rate or r.index == 0) {
      Node node{r.index, r.flow_rate, {}};
      rv.push_back(node);
    } else {
      Node empty;
      empty.index = -1;
      rv.push_back(empty);
    }
  }
  for (auto &n1 : rv) {
    if (n1.index == -1) {
      continue;
    }
    for (const auto n2 : rv) {
      if (n2.index == -1 or n1.index == n2.index) {
        continue;
      }
      int d = Distance(n1.index, n2.index, rooms);
      Edge e{n2.index, d};
      n1.neighbours.push_back(e);
    }
    n1.neighbours.push_back({n1.index, 1});
  }

  return rv;
}

struct Actor : Node {
  int turns_till_active;
};

struct State {
  int total_steam;
  int steam_per_min;
  int minutes_left;
  Actor node;
  Actor elephant;
  std::vector<int> valves_open;
  std::vector<int> history;

  bool operator==(const State &other) const {
    return ( // total_steam == other.total_steam and
        minutes_left == other.minutes_left and
        node.index == other.node.index and
        elephant.index == other.elephant.index and
        valves_open == other.valves_open);
  }
};

template <> struct std::hash<State> {
  std::size_t operator()(State const &s) const {
    std::size_t h1 = std::hash<int>{}(s.minutes_left);

    std::size_t h2 = std::hash<int>{}(s.node.index);
    std::size_t h3 = std::hash<int>{}(s.node.turns_till_active);
    std::size_t h4 = std::hash<int>{}(s.elephant.index);
    std::size_t h5 = std::hash<int>{}(s.elephant.turns_till_active);
    std::size_t seed = 0;
    if (s.node.index < s.elephant.index) {
      seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h5 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    else {
      seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h5 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    for (auto v : s.valves_open) {
      seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    return seed;
  }
};

int main() {
  const int num_mins = 26;
  auto rooms = ParseInput("example_input.txt");
  auto nodes = ConstructNodes(rooms);

  auto start = nodes[0];

  std::vector<State> stack;
  std::unordered_map<State, int> heap;
  stack.push_back({0,
                   0,
                   num_mins,
                   {start, 0},
                   {start, 0},
                   std::vector<int>(nodes.size(), false),
                   {start.index, start.index, num_mins, 0, 0}});

  State max = stack.back();
  max.total_steam = 1000;
  while (not stack.empty()) {
    State current = stack.back();
    int index = current.node.index;

    stack.pop_back();

    // Memoise
    if (heap.count(current) and heap[current] >= current.total_steam) {
      continue;
    }
    heap[current] = current.total_steam;

    // Test early exit
    int max_possible =
        current.total_steam + current.steam_per_min * current.minutes_left;
    for (auto n : current.node.neighbours) {
      if (n.index == -1) {
        continue;
      }
      int distance = std::min(n.distance,
                              Distance(n.index, current.elephant.index, rooms));
      max_possible += std::max(0, (current.minutes_left - distance) *
                                      nodes[n.index].flow_rate);
    }
    if (max_possible <= max.total_steam) {
      continue;
    }

    assert(index != -1);

    bool player_to_move = current.node.turns_till_active == 0;
    bool eleph_to_move = current.elephant.turns_till_active == 0;

    // Zero if an actor can move this minute
    int time_to_move = std::min(current.node.turns_till_active,
                                current.elephant.turns_till_active);
    if (current.minutes_left > time_to_move) {
      // Step to next decision
      if (not player_to_move and not eleph_to_move) {
        State next = current;
        next.total_steam += next.steam_per_min * time_to_move;
        next.minutes_left -= time_to_move;
        next.node.turns_till_active -= time_to_move;
        next.elephant.turns_till_active -= time_to_move;
        next.history.push_back(next.node.index);
        next.history.push_back(next.elephant.index);
        next.history.push_back(next.minutes_left);
        next.history.push_back(next.total_steam);
        next.history.push_back(next.steam_per_min);
        stack.push_back(next);

        continue;
      }

      if (player_to_move and eleph_to_move) {
        for (auto n : current.node.neighbours) {
          if (n.index == -1) {
            continue;
          }

          // Nodes are their own neighbours with distance=1, so treat n1->n1 as
          // opening valve or doing nothing
          for (auto e : current.elephant.neighbours) {
            if (e.index == -1) {
              continue;
            }
            State next = current;
            int distance = std::min(n.distance, e.distance);
            next.total_steam += next.steam_per_min * (distance - 1);
            if (n.index == next.node.index and not next.valves_open[n.index]) {
              next.valves_open[n.index] = true;
              next.steam_per_min += next.node.flow_rate;
            }
            if (e.index == next.elephant.index and
                not next.valves_open[e.index]) {
              next.valves_open[e.index] = true;
              next.steam_per_min += next.elephant.flow_rate;
            }
            if (next.minutes_left > std::max(n.distance, e.distance)) {
              next.minutes_left -= (distance);
              next.node = {nodes[n.index], n.distance - 1};
              next.elephant = {nodes[e.index], e.distance - 1};
              next.history.push_back(next.node.index);
              next.history.push_back(next.elephant.index);
              next.history.push_back(next.minutes_left);
              next.history.push_back(next.total_steam);
              next.history.push_back(next.steam_per_min);
              stack.push_back(next);
            }
          }
        }
        // Just move player as elephant is travelling
      } else if (player_to_move) {
        for (auto n : current.node.neighbours) {
          State next = current;
          if (n.index == -1) {
            continue;
          }
          next.total_steam += next.steam_per_min * (n.distance - 1);
          if (n.index == next.node.index and not next.valves_open[n.index]) {
            next.valves_open[n.index] = true;
            next.steam_per_min += next.node.flow_rate;
          }
          if (n.distance <= next.elephant.turns_till_active and
              next.minutes_left > n.distance) {
            next.minutes_left -= n.distance;
            next.node = {nodes[n.index], n.distance - 1};
            next.elephant.turns_till_active -= (n.distance);
            next.history.push_back(next.node.index);
            next.history.push_back(next.elephant.index);
            next.history.push_back(next.minutes_left);
            next.history.push_back(next.total_steam);
            next.history.push_back(next.steam_per_min);
            stack.push_back(next);
          }
        }
        // Just move elephant as player is travelling
      } else {
        for (auto e : current.elephant.neighbours) {
          if (e.index == -1) {
            continue;
          }
          State next = current;
          next.total_steam += next.steam_per_min * e.distance;
          if (e.index == next.elephant.index and
              not next.valves_open[e.index]) {
            next.valves_open[e.index] = true;
            next.steam_per_min += next.elephant.flow_rate;
          }
          if (e.distance <= next.node.turns_till_active and
              next.minutes_left > e.distance) {
            next.minutes_left -= e.distance;
            next.node.turns_till_active -= (e.distance);
            next.elephant = {nodes[e.index], e.distance - 1};
            next.history.push_back(next.node.index);
            next.history.push_back(next.elephant.index);
            next.history.push_back(next.minutes_left);
            next.history.push_back(next.total_steam);
            next.history.push_back(next.steam_per_min);
            stack.push_back(next);
          }
        }
      }
    }
    if (current.total_steam > max.total_steam) {
      max = current;
      for (int i = 0; i < max.history.size(); i += 5) {
        std::cout << names[max.history[i]] << ' ';
        std::cout << names[max.history[i + 1]] << ' ';
        std::cout << (1 + num_mins - max.history[i + 2]) << ' ';
        std::cout << (max.history[i + 3]) << ' ';
        std::cout << (max.history[i + 4]) << '\n';
      }
    }
  }

  std::cout << max.total_steam << '\n';

  return 0;
}
