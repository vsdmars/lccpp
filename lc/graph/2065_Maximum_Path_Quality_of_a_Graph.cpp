// https://leetcode.com/problems/maximum-path-quality-of-a-graph/
//
// Given an undirected weighted graph with `n` nodes (values `values[i]`) and
// edges `[u, v, time]`, find the maximum path quality of any walk starting and
// ending at node 0 whose total travel time is <= `maxTime`. A node may be
// visited multiple times, but its value is added to the path quality at most
// once.
//
// Constraint note:
// `10 <= time_j, maxTime <= 100` and each node has degree <= 4, so every valid
// walk has at most 10 edges (at most ~4^10 states in the worst case), making
// bounded-time backtracking DFS fast and exact.

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Backtracking DFS bounded by remaining travel time.
  // Uses a `visit_count` table instead of copying a set on each step so node
  // values are added only on the first visit (count 0 -> 1).
  // Time: O(4^(maxTime / minEdgeTime)), Space: O(V + E).
  int maximalPathQuality(const std::vector<int>& values,
                         const std::vector<std::vector<int>>& edges,
                         int maxTime) {
    const std::size_t n = values.size();
    std::vector<std::vector<std::pair<int, int>>> adj(n);
    for (const auto& edge : edges) {
      adj[static_cast<std::size_t>(edge[0])].emplace_back(edge[1], edge[2]);
      adj[static_cast<std::size_t>(edge[1])].emplace_back(edge[0], edge[2]);
    }

    std::vector<int> visit_count(n, 0);
    visit_count[0] = 1;
    int best = values[0];

    auto dfs = [&](this auto&& self, int u, int remaining_time,
                   int cur_quality) -> void {
      if (u == 0) {
        best = std::max(best, cur_quality);
      }

      for (const auto& [v, travel_time] : adj[static_cast<std::size_t>(u)]) {
        if (travel_time > remaining_time) {
          continue;
        }
        const auto uv = static_cast<std::size_t>(v);
        const int gain = visit_count[uv] == 0 ? values[uv] : 0;
        ++visit_count[uv];
        self(v, remaining_time - travel_time, cur_quality + gain);
        --visit_count[uv];
      }
    };

    dfs(0, maxTime, values[0]);
    return best;
  }
};

LC_PROBLEM(2065, "Maximum Path Quality of a Graph", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: 0 -> 1 -> 0 -> 3 -> 0 gives 75", [&] {
    ctx.expect_eq(sol.maximalPathQuality(
                      {0, 32, 10, 43}, {{0, 1, 10}, {1, 2, 15}, {0, 3, 10}}, 49),
                  75);
  });

  ctx.test("example 2: 0 -> 3 -> 0 gives 25", [&] {
    ctx.expect_eq(
        sol.maximalPathQuality({5, 10, 15, 20},
                               {{0, 1, 10}, {1, 2, 10}, {0, 3, 10}}, 30),
        25);
  });

  ctx.test("example 3: revisit node 1 without double-counting", [&] {
    ctx.expect_eq(
        sol.maximalPathQuality(
            {1, 2, 3, 4}, {{0, 1, 10}, {1, 2, 11}, {2, 3, 12}, {1, 3, 13}}, 50),
        7);
  });
}

}  // namespace
