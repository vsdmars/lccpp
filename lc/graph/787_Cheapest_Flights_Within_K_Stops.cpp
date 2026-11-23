// LeetCode 787: Cheapest Flights Within K Stops
// https://leetcode.com/problems/cheapest-flights-within-k-stops/
//
// Problem:
//   Given `n` cities labeled `0..n-1` and a list of directed weighted flights
//   `[from, to, price]`, find the minimum total price to travel from `src` to
//   `dst` with at most `k` intermediate stops (meaning at most `k + 1` flight
//   legs). Return `-1` if no valid route exists within the stop limit.
//
// Algorithm:
//   This file implements two classic shortest-path algorithms:
//     1. `findCheapestPrice` (Modified Dijkstra with Stop-Count Pruning):
//        Explores `(cost, stops, city)` states from a min-heap ordered by
//        total `cost`. Because states are popped in non-decreasing cost order,
//        the first time `dst` is popped yields the minimum valid cost. Key
//        invariant: track `best_stops[u]`, the fewest edges used to pop city
//        `u` so far; any later pop at `u` is at least as expensive, so if its
//        `stops >= best_stops[u]` (or `stops > k`), it is strictly dominated
//        and pruned.
//        Time: O(E * k * log(V * k)), Space: O(V + E * k) including heap.
//     2. `dp` (Bellman-Ford over `k + 1` Rounds):
//        Maintains the minimum cost `dist[v]` to reach each city using at most
//        `step` edges. In each of `k + 1` rounds, relaxes all flights from a
//        read-only snapshot `dist` into `next_dist` so that each round extends
//        paths by at most one additional flight leg.
//        Time: O(k * E), Space: O(V).
//
// Data Structures:
//   - `std::vector<std::vector<std::pair<int, int>>> adj` (`findCheapestPrice`):
//     Directed weighted adjacency list mapping `from -> (to, price)`.
//   - `std::priority_queue<State, std::vector<State>, std::greater<>> pq`
//     (`findCheapestPrice`): Min-heap ordering candidate states by total cost.
//   - `std::vector<int> best_stops` (`findCheapestPrice`): Records minimum
//     edges used on any prior cheaper pop of each city to prune dominated work.
//   - `std::vector<int> dist`, `next_dist` (`dp`): Rolling Bellman-Ford
//     distance tables across consecutive edge-budget layers.
//
// Real-World Applications:
//   - Travel and freight routing engines: computing lowest-fare itineraries
//     subject to maximum layover or transfer limits.
//   - Network packet routing with TTL/hop limits: finding minimum-latency or
//     lowest-cost paths bounded by a maximum hop count.
//   - Supply-chain logistics: minimizing multi-leg shipping cost when
//     perishable goods can tolerate only a fixed number of hub transfers.
//
// Related LeetCode Problems:
//   - #743 Network Delay Time (standard single-source shortest paths with
//     Dijkstra or Bellman-Ford)
//   - #1514 Path with Maximum Probability (modified Dijkstra maximizing path
//     probability)
//   - #1631 Path With Minimum Effort (minimax shortest path via Dijkstra)
//   - #1928 Minimum Cost to Reach Destination in Time (resource-constrained
//     shortest path trading off cost and elapsed time)
//   - #2093 Minimum Cost to Reach City With Discounts (state-space Dijkstra
//     tracking remaining discount budget)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Dijkstra's algorithm with stop-count pruning.
  // Because states are popped in ascending order of total cost, the first time
  // `dst` is popped is guaranteed optimal. Pruning any state that reaches a
  // city `u` with >= stops than a previously popped (cheaper) visit avoids
  // exponential blowup while preserving optimality.
  // Time: O(E * k * log(V * k)), Space: O(V + E).
  [[nodiscard]] int findCheapestPrice(
      int n, const std::vector<std::vector<int>>& flights, int src, int dst,
      int k) const {
    if (src == dst) {
      return 0;
    }

    const auto count = static_cast<std::size_t>(n);
    std::vector<std::vector<std::pair<int, int>>> adj(count);
    for (const auto& f : flights) {
      adj[static_cast<std::size_t>(f[0])].emplace_back(f[1], f[2]);
    }

    struct State {
      int cost = 0;
      int stops = 0;  // Number of edges used so far.
      int city = 0;

      // Min-heap comparator by total cost.
      bool operator>(const State& other) const noexcept {
        return cost > other.cost;
      }
    };

    std::priority_queue<State, std::vector<State>, std::greater<>> pq;
    // Minimum number of edges used to pop each city so far.
    std::vector<int> best_stops(count, std::numeric_limits<int>::max());
    pq.push(State{.cost = 0, .stops = 0, .city = src});

    while (!pq.empty()) {
      const State cur = pq.top();
      pq.pop();

      if (cur.city == dst) {
        return cur.cost;
      }
      // Cannot take another edge if we already used `k + 1` flights, or if we
      // already visited this city cheaper with fewer or equal flights.
      const auto u = static_cast<std::size_t>(cur.city);
      if (cur.stops > k || cur.stops >= best_stops[u]) {
        continue;
      }
      best_stops[u] = cur.stops;

      for (const auto& [next, price] : adj[u]) {
        pq.push(State{
            .cost = cur.cost + price,
            .stops = cur.stops + 1,
            .city = next,
        });
      }
    }

    return -1;
  }

  // Bellman-Ford DP over `k + 1` flight-leg relaxations (mirrors dp() in
  // 787_Cheapest_Flights_Within_K_Stops.py).
  // Snapshotting `prev` each round ensures at most one extra flight is added
  // per iteration.
  // Time: O(k * E), Space: O(V).
  int dp(int n, const std::vector<std::vector<int>>& flights, int src, int dst,
         int k) {
    constexpr int kInf = std::numeric_limits<int>::max() / 2;
    const auto count = static_cast<std::size_t>(n);
    std::vector<int> dist(count, kInf);
    dist[static_cast<std::size_t>(src)] = 0;

    for (int step = 0; step <= k; ++step) {
      std::vector<int> next_dist = dist;
      for (const auto& f : flights) {
        const auto u = static_cast<std::size_t>(f[0]);
        const auto v = static_cast<std::size_t>(f[1]);
        const int price = f[2];
        if (dist[u] < kInf) {
          next_dist[v] = std::min(next_dist[v], dist[u] + price);
        }
      }
      dist = std::move(next_dist);
    }

    const int ans = dist[static_cast<std::size_t>(dst)];
    return ans >= kInf ? -1 : ans;
  }
};

LC_PROBLEM(787, "Cheapest Flights Within K Stops", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: 1 stop beats direct flight", [&] {
    const std::vector<std::vector<int>> flights = {
        {0, 1, 100}, {1, 2, 100}, {0, 2, 500}};
    ctx.expect_eq(sol.findCheapestPrice(3, flights, 0, 2, 1), 200);
    ctx.expect_eq(sol.dp(3, flights, 0, 2, 1), 200);
  });

  ctx.test("example 2: 0 stops forces direct flight", [&] {
    const std::vector<std::vector<int>> flights = {
        {0, 1, 100}, {1, 2, 100}, {0, 2, 500}};
    ctx.expect_eq(sol.findCheapestPrice(3, flights, 0, 2, 0), 500);
    ctx.expect_eq(sol.dp(3, flights, 0, 2, 0), 500);
  });

  ctx.test("python sample: cheaper longer path vs stop budget", [&] {
    const std::vector<std::vector<int>> flights = {
        {0, 1, 5}, {1, 2, 5}, {0, 3, 2}, {3, 1, 2}, {1, 4, 1}, {4, 2, 1}};
    // 0 -> 1 -> 4 -> 2 costs 5 + 1 + 1 = 7 with 2 stops.
    // (0 -> 3 -> 1 -> 4 -> 2 costs 6 but needs 3 stops.)
    ctx.expect_eq(sol.findCheapestPrice(5, flights, 0, 2, 2), 7);
    ctx.expect_eq(sol.dp(5, flights, 0, 2, 2), 7);
  });

  ctx.test("unreachable destination returns -1", [&] {
    const std::vector<std::vector<int>> flights = {{0, 1, 100}};
    ctx.expect_eq(sol.findCheapestPrice(3, flights, 0, 2, 1), -1);
    ctx.expect_eq(sol.dp(3, flights, 0, 2, 1), -1);
  });
}

}  // namespace
