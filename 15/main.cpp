#include "../utility.h"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <charconv>

constexpr const char example_input[] = "\
Sensor at x=2, y=18: closest beacon is at x=-2, y=15\
Sensor at x=9, y=16: closest beacon is at x=10, y=16\
Sensor at x=13, y=2: closest beacon is at x=15, y=3\
Sensor at x=12, y=14: closest beacon is at x=10, y=16\
Sensor at x=10, y=20: closest beacon is at x=10, y=16\
Sensor at x=14, y=17: closest beacon is at x=10, y=16\
Sensor at x=8, y=7: closest beacon is at x=2, y=10\
Sensor at x=2, y=0: closest beacon is at x=2, y=10\
Sensor at x=0, y=11: closest beacon is at x=2, y=10\
Sensor at x=20, y=14: closest beacon is at x=25, y=17\
Sensor at x=17, y=20: closest beacon is at x=21, y=22\
Sensor at x=16, y=7: closest beacon is at x=15, y=3\
Sensor at x=14, y=3: closest beacon is at x=15, y=3\
Sensor at x=20, y=1: closest beacon is at x=15, y=3\
";
constexpr const char input[] = "\
Sensor at x=2765643, y=3042538: closest beacon is at x=2474133, y=3521072\
Sensor at x=2745662, y=2324735: closest beacon is at x=2491341, y=1883354\
Sensor at x=2015742, y=2904055: closest beacon is at x=2474133, y=3521072\
Sensor at x=3375262, y=3203288: closest beacon is at x=3321219, y=3415236\
Sensor at x=3276468, y=3892409: closest beacon is at x=3321219, y=3415236\
Sensor at x=952573, y=3147055: closest beacon is at x=-41010, y=2905006  \
Sensor at x=1823659, y=1779343: closest beacon is at x=1592718, y=2000000\
Sensor at x=1156328, y=865741: closest beacon is at x=1592718, y=2000000 \
Sensor at x=3938443, y=271482: closest beacon is at x=4081274, y=1177185 \
Sensor at x=2815232, y=1641178: closest beacon is at x=2491341, y=1883354\
Sensor at x=3984799, y=3424711: closest beacon is at x=3321219, y=3415236\
Sensor at x=1658825, y=3999931: closest beacon is at x=2474133, y=3521072\
Sensor at x=3199859, y=1285962: closest beacon is at x=4081274, y=1177185\
Sensor at x=3538649, y=2788193: closest beacon is at x=3725736, y=2414539\
Sensor at x=3522208, y=3336284: closest beacon is at x=3321219, y=3415236\
Sensor at x=3093758, y=3492396: closest beacon is at x=3321219, y=3415236\
Sensor at x=2464979, y=562119: closest beacon is at x=2491341, y=1883354 \
Sensor at x=3665010, y=1556840: closest beacon is at x=3735739, y=2128164\
Sensor at x=207525, y=3893957: closest beacon is at x=-41010, y=2905006  \
Sensor at x=3894678, y=1974599: closest beacon is at x=3735739, y=2128164\
Sensor at x=2185146, y=3822275: closest beacon is at x=2474133, y=3521072\
Sensor at x=31166, y=1467978: closest beacon is at x=-41010, y=2905006   \
Sensor at x=3242364, y=3335961: closest beacon is at x=3321219, y=3415236\
Sensor at x=3773718, y=3999789: closest beacon is at x=3321219, y=3415236\
Sensor at x=423046, y=2227938: closest beacon is at x=-41010, y=2905006  \
Sensor at x=1600225, y=2529059: closest beacon is at x=1592718, y=2000000\
Sensor at x=3291752, y=2241389: closest beacon is at x=3735739, y=2128164\
Sensor at x=2741333, y=3984346: closest beacon is at x=2474133, y=3521072\
Sensor at x=3935288, y=2292902: closest beacon is at x=3725736, y=2414539\
Sensor at x=291635, y=140996: closest beacon is at x=212146, y=-1154950  \
Sensor at x=3966296, y=2600346: closest beacon is at x=3725736, y=2414539\
Sensor at x=2228916, y=1461096: closest beacon is at x=2491341, y=1883354\
";
// Input like {11 chars, X, 4 chars, Y}

struct Coord {
  int x;
  int y;

  bool operator==(const Coord& other) const {
    return (x == other.x and y == other.y);
  }
  const Coord operator+(const Coord& other) const {
    return { x + other.x, y + other.y };
  }
  const Coord operator-(const Coord& other) const {
    return { x - other.x, y - other.y };
  }
  auto& operator+=(const Coord& other) {
    x += other.x; y += other.y;
    return *this;
  }
  auto& operator-=(const Coord& other) {
    x -= other.x; y -= other.y;
    return *this;
  }
};

struct Sensor: Coord {
  int closest;
};

struct Grid {
  std::vector<Sensor> sensors;
  std::vector<Coord> beacons;
  Coord min;
  Coord max;
};

void ParseInput(const char input[], Grid& g) {
  int idx = 0;

  int min_x = std::numeric_limits<int>::max();
  int max_x = std::numeric_limits<int>::min();
  int min_y = std::numeric_limits<int>::max();
  int max_y = std::numeric_limits<int>::min();

  //const int size = std::size(input);
  //static_assert(std::numeric_limits<decltype(idx)>::max() >= size);


  while (char c1 = input[idx]) {
    Coord sensor, beacon;
    if (c1 == 'x') {
      idx += 2;

      int end = FindNext(input, idx, ',');

      std::from_chars(input + idx, input + end, sensor.x);


      idx = FindNext(input, end, '=') + 1;
      end = FindNext(input, idx, ':');

      std::from_chars(input + idx, input + end, sensor.y);

      idx = FindNext(input, end, '=') + 1;
      end = FindNext(input, idx, ',');

      std::from_chars(input + idx, input + end, beacon.x);

      idx = FindNext(input, end, '=') + 1;
      end = FindNext(input, idx, 'S');

      std::from_chars(input + idx, input + end, beacon.y);

      g.beacons.push_back(beacon);
      Coord d = sensor - beacon;
      int dist = abs(d.x) + abs(d.y);
      g.sensors.push_back({ sensor, dist });

      min_x = std::min(min_x, sensor.x - dist);
      max_x = std::max(max_x, sensor.x + dist);
      min_y = std::min(min_y, sensor.y);
      max_y = std::max(max_y, sensor.y);

      idx = end;

    }
    else { ++idx; }
  }
  g.min = Coord{ min_x, min_y };
  g.max = Coord{ max_x, max_y };
}

struct Range {
  // Ordered range, includes endpoints
  int start;
  int end;
  int length() const { return end - start + 1; }
};


std::vector<Range> RemoveOverlaps(std::vector<Range> intervals) {
  // Requires input sorted by interval start increasing (when tied, choose longest first).
  if (intervals.size() <= 1) { return std::move(intervals); }

  // Inclusive range for the dead zone (values chosen so length == 0 initially)
  Range dead_zone = { -1,-2 };
  // Put all consumed ranges into the dead zone, then swap elements over it

  bool finished = false;
  int idx = 0;
  while (true) {
    Range& r1 = intervals[idx];
    Range& r2 = intervals[idx + 1];

    // Not overlapping
    if (r1.end < r2.start) {
      ++idx;
    }
    else {
      r1.end = std::max(r1.end,r2.end);
      if(dead_zone.start == -1) {
        dead_zone = {idx+1, idx+1};
      }
      else {
        --dead_zone.start;
      }
    }
    if (idx + 1 + dead_zone.length() >= intervals.size()) {
      break;
    }

    if(idx+1 == dead_zone.start) {
      //Swap dead element with next living one
      Range& living = intervals[idx + 1 + dead_zone.length()];
      Range& dead = intervals[idx + 1];
      Range tmp = dead;
      dead = living;
      living = tmp;

      if(dead_zone.start != -1) {
        ++dead_zone.start;
        ++dead_zone.end;
      }
    }
  }
  intervals.resize(intervals.size() - dead_zone.length());
  return std::move(intervals);
}


int main() {
  Grid g;

  ParseInput(input, g);
  //std::sort(g.sensors.begin(), g.sensors.end(), [](Sensor a, Sensor b) { return a.x < b.x; });

  // Print(g.min.x, g.min.y);
  // Print(g.max.x, g.max.y);

  Coord answer;

  Coord min = { 0, 0 };
  Coord max = { 4000000, 4000000 };

  for (int y = min.y; y <= max.y; ++y) {
    std::vector<Range> blocked{};
    for (auto s : g.sensors) {
      int range = s.closest - abs(s.y - y);

      // Overlaps with row
      if (range >= 0) {
        blocked.push_back({ s.x - range, s.x + range });
      }
    }
    std::sort(blocked.begin(), blocked.end(), [](Range a, Range b) { return a.start < b.start; });
    auto blocked2 = RemoveOverlaps(blocked);
    if (blocked2.size() != 1) {
      assert(blocked2.size() == 2);
      for (auto b : blocked2) {
        std::cout << b.start << ',' << b.end << '\n';
      }
      answer = {blocked2[0].end+1, y};
      assert(answer.x == blocked2[1].start-1);
      break;
    }
  }
  std::cout << answer.x << ',' << answer.y << ',' << ((long long)(answer.x)*4000000+answer.y) << '\n';
  return 0;
}
