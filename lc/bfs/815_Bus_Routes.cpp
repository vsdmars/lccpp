// LeetCode 815: Bus Routes
// https://leetcode.com/problems/bus-routes/
//
// Problem:
//   You are given an array `routes` where `routes[i]` lists the bus stops
//   repeatedly visited by bus route i. Starting at stop `source` (not yet on
//   any bus), find the minimum number of buses you must board to reach stop
//   `target`. Return 0 if `source == target`, or -1 if `target` is
//   unreachable.
//
// Algorithm:
//   Model the transit network as a bipartite stop-route graph and run
//   level-order Breadth-First Search (BFS) where each BFS level corresponds to
//   taking one additional bus:
//     1. If `source == target`, return 0 immediately (0 buses needed even if
//        the stop does not appear on any route).
//     2. Build an inverted index `stop_to_routes` mapping each bus stop to all
//        route indices that pass through it.
//     3. If either `source` or `target` is absent from `stop_to_routes`,
//        return -1.
//     4. Seed the BFS queue with all routes serving `source` (distance = 1 bus)
//        and mark `source` and those routes as visited.
//     5. At each BFS level (`buses = 1, 2, ...`), inspect every stop on each
//        popped route:
//        - If `stop == target`, return `buses`.
//        - Otherwise, if `stop` has not yet been expanded, mark `stop` visited
//          and enqueue every unvisited route serving `stop`.
//   Tracking both `visited_route` and `visited_stop` prevents re-scanning hub
//   stops shared by many routes, keeping runtime strictly linear in total
//   route entries.
//   Time Complexity:  O(S), where S = sum of routes[i].size() across all
//                     routes (each stop-route pair is indexed once and
//                     inspected at most once during BFS).
//   Space Complexity: O(S) for the inverted index, visited sets, and queue.
//
// Data Structures:
//   - std::unordered_map<int, std::vector<std::size_t>>: Inverted index from
//     stop ID to list of route indices serving that stop.
//   - std::vector<std::uint8_t>: Boolean table marking already-boarded routes.
//   - std::unordered_set<int>: Tracks stops whose transfer routes have already
//     been enqueued, avoiding quadratic work at busy interchange hubs.
//   - std::queue<std::size_t>: FIFO queue of route indices for level-order BFS.
//
// Real-World Applications:
//   - Public transit trip planners (minimizing transfers across bus, subway,
//     and rail lines — analogous to RAPTOR's round-based transit routing).
//   - Multi-hop airline itinerary search optimizing minimum flight legs.
//   - Inter-VLAN / multi-network packet routing minimizing gateway hops.
//
// Related LeetCode Problems:
//   - #127 Word Ladder [done] (bipartite pattern-to-word BFS for minimum steps)
//   - #752 Open The Lock [done] (unweighted shortest-path BFS)
//   - #787 Cheapest Flights Within K Stops [done] (minimum-cost transit with transfer limits)
//   - #1345 Jump Game IV [done] (value-grouped portal jumps solved with visited-group BFS)
//   - #2059 Minimum Operations to Convert Number [new] (shortest-path BFS with deduplicated frontier)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the fewest buses required to travel from `source` to `target`,
  // or -1 if no sequence of bus transfers connects them.
  // Time: O(sum(routes[i].size())), Space: O(sum(routes[i].size())).
  int numBusesToDestination(const std::vector<std::vector<int>>& routes,
                            int source, int target) {
    if (source == target) {
      return 0;
    }

    // Build stop -> list of route indices serving that stop.
    std::unordered_map<int, std::vector<std::size_t>> stop_to_routes;
    for (std::size_t r = 0uz; r < routes.size(); ++r) {
      for (int stop : routes[r]) {
        stop_to_routes[stop].push_back(r);
      }
    }

    if (!stop_to_routes.contains(source) || !stop_to_routes.contains(target)) {
      return -1;
    }

    std::vector<std::uint8_t> visited_route(routes.size(), 0);
    std::unordered_set<int> visited_stop;
    std::queue<std::size_t> frontier;

    // Seed BFS with every bus route accessible directly from `source`.
    visited_stop.insert(source);
    enqueue_routes_at_stop(source, stop_to_routes, visited_route, frontier);

    int buses = 1;
    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      for (std::size_t i = 0uz; i < level_size; ++i) {
        const std::size_t route_idx = frontier.front();
        frontier.pop();

        if (explore_route(routes[route_idx], target, stop_to_routes,
                          visited_stop, visited_route, frontier)) {
          return buses;
        }
      }
      ++buses;
    }

    return -1;
  }

 private:
  // Enqueues every unvisited bus route passing through `stop`.
  static void enqueue_routes_at_stop(
      int stop,
      const std::unordered_map<int, std::vector<std::size_t>>& stop_to_routes,
      std::vector<std::uint8_t>& visited_route,
      std::queue<std::size_t>& frontier) {
    const auto it = stop_to_routes.find(stop);
    if (it == stop_to_routes.end()) {
      return;
    }
    for (std::size_t next_route : it->second) {
      if (visited_route[next_route] == 0) {
        visited_route[next_route] = 1;
        frontier.push(next_route);
      }
    }
  }

  // Scans all stops on one boarded route; returns true immediately if `target`
  // lies on this route, otherwise queues transfer routes at newly reached stops.
  static bool explore_route(
      const std::vector<int>& route_stops, int target,
      const std::unordered_map<int, std::vector<std::size_t>>& stop_to_routes,
      std::unordered_set<int>& visited_stop,
      std::vector<std::uint8_t>& visited_route,
      std::queue<std::size_t>& frontier) {
    for (int stop : route_stops) {
      if (stop == target) {
        return true;
      }
      // Expanding each stop at most once avoids O(R^2) work at shared hubs.
      if (visited_stop.insert(stop).second) {
        enqueue_routes_at_stop(stop, stop_to_routes, visited_route, frontier);
      }
    }
    return false;
  }
};

LC_PROBLEM(815, "Bus Routes", "bfs", ctx) {
  ctx.test("example_1_two_buses", [&] {
    Solution sol;
    ctx.expect_eq(sol.numBusesToDestination({{1, 2, 7}, {3, 6, 7}}, 1, 6), 2);
  });

  ctx.test("example_2_unreachable", [&] {
    Solution sol;
    const std::vector<std::vector<int>> routes = {
        {7, 12}, {4, 5, 15}, {6}, {15, 19}, {9, 12, 13},
    };
    ctx.expect_eq(sol.numBusesToDestination(routes, 15, 12), -1);
  });

  ctx.test("source_equals_target", [&] {
    Solution sol;
    ctx.expect_eq(sol.numBusesToDestination({{1, 2, 7}, {3, 6, 7}}, 1, 1), 0);
    // Even if the stop is not on any route, 0 buses are needed.
    ctx.expect_eq(sol.numBusesToDestination({{1, 2, 7}}, 999, 999), 0);
  });

  ctx.test("direct_single_bus_and_shortcut", [&] {
    Solution sol;
    const std::vector<std::vector<int>> routes = {
        {1, 2}, {2, 3}, {3, 4}, {1, 4},
    };
    ctx.expect_eq(sol.numBusesToDestination(routes, 1, 4), 1);
  });

  ctx.test("three_bus_chain", [&] {
    Solution sol;
    const std::vector<std::vector<int>> routes = {
        {1, 2}, {2, 3}, {3, 4},
    };
    ctx.expect_eq(sol.numBusesToDestination(routes, 1, 4), 3);
  });

  ctx.test("missing_source_or_target", [&] {
    Solution sol;
    ctx.expect_eq(sol.numBusesToDestination({{1, 2, 3}}, 1, 9), -1);
    ctx.expect_eq(sol.numBusesToDestination({{1, 2, 3}}, 9, 1), -1);
  });
}

}  // namespace
