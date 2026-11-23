// LeetCode 1396: Design Underground System
// https://leetcode.com/problems/design-underground-system/
//
// Problem:
//   Design an `UndergroundSystem` class that tracks passenger trips on a
//   railway network and answers average travel-time queries:
//     - `checkIn(id, stationName, t)`: passenger `id` enters `stationName` at
//       timestamp `t` (at most one active trip per passenger at a time).
//     - `checkOut(id, stationName, t)`: passenger `id` exits `stationName` at
//       timestamp `t`, completing a direct trip from their check-in station.
//     - `getAverageTime(startStation, endStation)`: returns the mean travel
//       time over all completed direct trips from `startStation` to
//       `endStation` (directional: A -> B is distinct from B -> A).
//
// Algorithm:
//   Maintain two hash tables:
//     1. `active_trips_` maps each currently checked-in passenger `id` to
//        `{station, check_in_time}`.
//     2. `route_stats_` maps each directed route key `"start>end"` to running
//        aggregates `{total_time, trip_count}`. Because station names contain
//        only ASCII alphanumeric characters, `'>'` is a collision-free
//        separator.
//   On `checkIn`, insert/overwrite `active_trips_[id]`.
//   On `checkOut`, look up and erase `active_trips_[id]`, compute
//   `duration = t - check_in_time`, and accumulate `total_time += duration` and
//   `++trip_count` in `route_stats_[start + ">" + end]`.
//   On `getAverageTime`, return `total_time / trip_count` in O(1) time without
//   storing or re-summing individual trip durations.
//   Time Complexity:  O(L) expected per `checkIn`, `checkOut`, and
//                     `getAverageTime`, where L <= 10 is station name length
//                     (effectively O(1)).
//   Space Complexity: O(P + R) where P is concurrent checked-in passengers and
//                     R is the number of distinct directed station pairs seen.
//
// Data Structures:
//   - `std::unordered_map<int, CheckInRecord> active_trips_`: tracks in-flight
//     passenger journeys.
//   - `std::unordered_map<std::string, RouteStats> route_stats_`: maintains
//     running sum and count per directed station pair for O(1) averages.
//
// Real-World Applications:
//   - Public transit smart-card analytics (Oyster/Clipper/Suica origin-
//     destination travel-time aggregation).
//   - Distributed tracing and APM telemetry (computing mean latency between
//     service spans keyed by `(caller, callee)` and request ID).
//   - Ride-hailing / logistics ETA estimation between zone pairs.
//
// Related LeetCode Problems:
//   - #146 LRU Cache [done] (O(1) hash-map backed stateful system design)
//   - #362 Design Hit Counter [done] (time-stamped event aggregation)
//   - #981 Time Based Key-Value Store [done] (key-timestamp lookup structures)
//   - #1472 Design Browser History [done] (stateful O(1) design class)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace {

constexpr double kEpsilon = 1e-5;

// Tracks in-flight passenger check-ins and running directed-route travel-time
// aggregates for O(1) average queries.
class UndergroundSystem {
 public:
  // Records that customer `id` checked into `station_name` at time `t`.
  // Time: O(L) expected, Space: O(L) per active passenger.
  void checkIn(int id, std::string station_name, int t) {
    active_trips_[id] = CheckInRecord{
        .station = std::move(station_name),
        .time = t,
    };
  }

  // Completes customer `id`'s active trip at `station_name` and time `t`,
  // updating the running sum and trip count for the directed route.
  // Time: O(L) expected, Space: O(L) if the directed route is new.
  void checkOut(int id, const std::string& station_name, int t) {
    const auto it = active_trips_.find(id);
    if (it == active_trips_.end()) {
      return;
    }

    const std::string route_key = makeRouteKey(it->second.station, station_name);
    const std::int64_t duration =
        static_cast<std::int64_t>(t) - static_cast<std::int64_t>(it->second.time);
    // Erase the completed journey so `active_trips_` only holds in-flight IDs.
    active_trips_.erase(it);

    RouteStats& stats = route_stats_[route_key];
    stats.total_time += duration;
    ++stats.trip_count;
  }

  // Returns the exact running mean travel time from `start_station` to
  // `end_station` across all completed direct trips.
  // Time: O(L) expected, Space: O(1) auxiliary.
  [[nodiscard]] double getAverageTime(const std::string& start_station,
                                      const std::string& end_station) const {
    const RouteStats& stats =
        route_stats_.at(makeRouteKey(start_station, end_station));
    return static_cast<double>(stats.total_time) /
           static_cast<double>(stats.trip_count);
  }

 private:
  struct CheckInRecord {
    std::string station;
    int time = 0;
  };

  struct RouteStats {
    std::int64_t total_time = 0;
    int trip_count = 0;
  };

  // Builds a collision-free directed route key using '>' (not allowed in valid
  // alphanumeric station names).
  static std::string makeRouteKey(std::string_view start,
                                  std::string_view end) {
    std::string key;
    key.reserve(start.size() + 1uz + end.size());
    key.append(start);
    key.push_back('>');
    key.append(end);
    return key;
  }

  std::unordered_map<int, CheckInRecord> active_trips_;
  std::unordered_map<std::string, RouteStats> route_stats_;
};

bool approxEq(double a, double b) { return std::fabs(a - b) <= kEpsilon; }

LC_PROBLEM(1396, "Design Underground System", "design", ctx) {
  ctx.test("example_1", [&] {
    UndergroundSystem sys;
    sys.checkIn(45, "Leyton", 3);
    sys.checkIn(32, "Paradise", 8);
    sys.checkIn(27, "Leyton", 10);
    sys.checkOut(45, "Waterloo", 15);
    sys.checkOut(27, "Waterloo", 20);
    sys.checkOut(32, "Cambridge", 22);

    ctx.expect_true(approxEq(sys.getAverageTime("Paradise", "Cambridge"), 14.0));
    ctx.expect_true(approxEq(sys.getAverageTime("Leyton", "Waterloo"), 11.0));

    sys.checkIn(10, "Leyton", 24);
    ctx.expect_true(approxEq(sys.getAverageTime("Leyton", "Waterloo"), 11.0));

    sys.checkOut(10, "Waterloo", 38);
    ctx.expect_true(approxEq(sys.getAverageTime("Leyton", "Waterloo"), 12.0));
  });

  ctx.test("example_2", [&] {
    UndergroundSystem sys;
    sys.checkIn(10, "Leyton", 3);
    sys.checkOut(10, "Paradise", 8);
    ctx.expect_true(approxEq(sys.getAverageTime("Leyton", "Paradise"), 5.0));

    sys.checkIn(5, "Leyton", 10);
    sys.checkOut(5, "Paradise", 16);
    ctx.expect_true(approxEq(sys.getAverageTime("Leyton", "Paradise"), 5.5));

    sys.checkIn(2, "Leyton", 21);
    sys.checkOut(2, "Paradise", 30);
    ctx.expect_true(
        approxEq(sys.getAverageTime("Leyton", "Paradise"), 20.0 / 3.0));
  });

  ctx.test("directional_routes_and_repeat_passenger", [&] {
    UndergroundSystem sys;
    // Customer 1 goes A -> B (10s), then returns B -> A (30s), then A -> B (20s).
    sys.checkIn(1, "Alpha", 0);
    sys.checkOut(1, "Beta", 10);
    sys.checkIn(1, "Beta", 15);
    sys.checkOut(1, "Alpha", 45);
    sys.checkIn(1, "Alpha", 50);
    sys.checkOut(1, "Beta", 70);

    ctx.expect_true(approxEq(sys.getAverageTime("Alpha", "Beta"), 15.0));
    ctx.expect_true(approxEq(sys.getAverageTime("Beta", "Alpha"), 30.0));
  });
}

}  // namespace
