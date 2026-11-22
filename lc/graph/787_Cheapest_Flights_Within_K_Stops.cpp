// https://leetcode.com/problems/cheapest-flights-within-k-stops/
//
// Given `n` cities and directed weighted flights `[from, to, price]`, find the
// cheapest price from `src` to `dst` with at most `k` intermediate stops (i.e.
// at most `k + 1` flight legs). Return -1 if no such route exists.

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
