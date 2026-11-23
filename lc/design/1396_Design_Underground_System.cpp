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
// Examples:
//   Example 1:
//     Input:  ["UndergroundSystem", "checkIn", "checkIn", "checkIn", "checkOut",
//             "checkOut", "checkOut", "getAverageTime", "getAverageTime",
//             "checkIn", "getAverageTime", "checkOut", "getAverageTime"], [[], [45,
//             "Leyton", 3], [32, "Paradise", 8], [27, "Leyton", 10], [45,
//             "Waterloo", 15], [27, "Waterloo", 20], [32, "Cambridge", 22],
//             ["Paradise", "Cambridge"], ["Leyton", "Waterloo"], [10, "Leyton",
//             24], ["Leyton", "Waterloo"], [10, "Waterloo", 38], ["Leyton",
//             "Waterloo"]]
//     Output: [null, null, null, null, null, null, null, 14.0, 11.0, null, 11.0,
//             null, 12.0]
//     Explanation:
//       - `checkIn` records active trips `45 -> ("Leyton", 3)`, `32 -> ("Paradise",
//         8)`, and `27 -> ("Leyton", 10)` in `active_trips_`.
//       - Checking out passenger 45 at `"Waterloo"` (t = 15) and passenger 27 at
//         `"Waterloo"` (t = 20) erases both from `active_trips_` and accumulates
//         durations `15 - 3 = 12` and `20 - 10 = 10` under `"Leyton>Waterloo"`
//         (`total_time = 22`, `trip_count = 2`).
//       - Checking out passenger 32 at `"Cambridge"` (t = 22) records duration `22
//         - 8 = 14` under `"Paradise>Cambridge"` (`total_time = 14`, `trip_count =
//         1`), so the first two queries return `14 / 1 = 14.0` and `22 / 2 = 11.0`.
//       - `checkIn(10, "Leyton", 24)` only updates `active_trips_`, leaving
//         `"Leyton>Waterloo"` at `11.0` until `checkOut(10, "Waterloo", 38)` adds
//         `38 - 24 = 14` (`total_time = 36`, `trip_count = 3`) and
//         `getAverageTime("Leyton", "Waterloo")` returns `36 / 3 = 12.0`.
//
//   Example 2:
//     Input:  ["UndergroundSystem", "checkIn", "checkOut", "getAverageTime",
//             "checkIn", "checkOut", "getAverageTime", "checkIn", "checkOut",
//             "getAverageTime"], [[], [10, "Leyton", 3], [10, "Paradise", 8],
//             ["Leyton", "Paradise"], [5, "Leyton", 10], [5, "Paradise", 16],
//             ["Leyton", "Paradise"], [2, "Leyton", 21], [2, "Paradise", 30],
//             ["Leyton", "Paradise"]]
//     Output: [null, null, null, 5.0, null, null, 5.5, null, null, 20.0 / 3.0]
//     Explanation:
//       - Passenger 10 completes `"Leyton"` (t = 3) to `"Paradise"` (t = 8),
//         setting `route_stats_["Leyton>Paradise"]` to `total_time = 5, trip_count
//         = 1` and returning `5 / 1 = 5.0`.
//       - Passenger 5 completes `"Leyton"` (t = 10) to `"Paradise"` (t = 16) with
//         duration `6`, updating `"Leyton>Paradise"` to `total_time = 11,
//         trip_count = 2` and returning `11 / 2 = 5.5`.
//       - Passenger 2 completes `"Leyton"` (t = 21) to `"Paradise"` (t = 30) with
//         duration `9`, updating `"Leyton>Paradise"` to `total_time = 20,
//         trip_count = 3` and returning `20.0 / 3.0`.
//
//   Example 3:
//     Input:  ["UndergroundSystem", "checkIn", "checkOut", "checkIn", "checkOut",
//             "checkIn", "checkOut", "getAverageTime", "getAverageTime"], [[], [1,
//             "Alpha", 0], [1, "Beta", 10], [1, "Beta", 15], [1, "Alpha", 45], [1,
//             "Alpha", 50], [1, "Beta", 70], ["Alpha", "Beta"], ["Beta", "Alpha"]]
//     Output: [null, null, null, null, null, null, null, 15.0, 30.0]
//     Explanation:
//       - Passenger 1 travels `"Alpha"` (t = 0) to `"Beta"` (t = 10), erasing ID 1
//         from `active_trips_` on checkout and setting `"Alpha>Beta"` to
//         `total_time = 10, trip_count = 1`.
//       - Passenger 1 then checks in at `"Beta"` (t = 15) and out at `"Alpha"` (t =
//         45), recording duration `30` under the separate reverse key
//         `"Beta>Alpha"` (`total_time = 30, trip_count = 1`).
//       - On the third trip from `"Alpha"` (t = 50) to `"Beta"` (t = 70), duration
//         `20` updates `"Alpha>Beta"` to `total_time = 30, trip_count = 2`.
//       - Querying `getAverageTime("Alpha", "Beta")` returns `30 / 2 = 15.0`, while
//         `getAverageTime("Beta", "Alpha")` returns `30 / 1 = 30.0`.
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
