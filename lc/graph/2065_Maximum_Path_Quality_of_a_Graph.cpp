// LeetCode 2065: Maximum Path Quality of a Graph
// https://leetcode.com/problems/maximum-path-quality-of-a-graph/
//
// Problem:
//   Given an undirected weighted graph with `n` nodes (`0..n-1`), non-negative
//   node values `values[i]`, undirected edges `edges[j] = [u, v, time]`, and a
//   time budget `maxTime`, return the maximum path quality of any valid walk
//   starting and ending at node `0` with total travel time `<= maxTime`. Nodes
//   and edges may be revisited multiple times, but each node's value is added
//   to the path quality at most once. Constraints: 1 <= n <= 1000,
//   0 <= values[i] <= 10^8, 10 <= time_j, maxTime <= 100, and every node has
//   degree at most 4.
//
// Examples:
//   Example 1:
//     Input:  values = [0, 32, 10, 43], edges = [[0, 1, 10], [1, 2, 15], [0, 3,
//             10]], maxTime = 49
//     Output: 75
//     Explanation:
//       - Start dfs(0, 49, 0) with visit_count = [1, 0, 0, 0] and best = 0.
//       - Explore 0 -> 1 (time 10, cur_quality = 32, remaining_time = 39) and
//         return 1 -> 0 (time 10, remaining_time = 29), updating best = 32 at node
//         0 with visit_count = [2, 1, 0, 0].
//       - From node 0 at remaining_time = 29, take 0 -> 3 (time 10, visit_count[3]
//         = 1, cur_quality = 32 + 43 = 75) and return 3 -> 0 (time 10,
//         remaining_time = 9).
//       - Update best = 75 upon returning to node 0; shorter/dead-end branches
//         (such as visiting node 2) cannot return to 0 with higher quality, so 75
//         is returned.
//
//   Example 2:
//     Input:  values = [5, 10, 15, 20], edges = [[0, 1, 10], [1, 2, 10], [0, 3,
//             10]], maxTime = 30
//     Output: 25
//     Explanation:
//       - Start dfs(0, 30, 5) with visit_count = [1, 0, 0, 0] and best = 5.
//       - The walk 0 -> 1 -> 0 spends 20 time to reach node 0 with cur_quality =
//         15, leaving 10 time which is insufficient for the 20-time round trip to
//         node 3.
//       - Taking 0 -> 3 -> 0 spends 10 + 10 = 20 <= 30 time, collecting values[3] =
//         20 once for cur_quality = 25 and updating best = 25.
//
//   Example 3:
//     Input:  values = [1, 2, 3, 4], edges = [[0, 1, 10], [1, 2, 11], [2, 3, 12],
//             [1, 3, 13]], maxTime = 50
//     Output: 7
//     Explanation:
//       - Start dfs(0, 50, 1) with visit_count = [1, 0, 0, 0] and best = 1.
//       - Exploring 0 -> 1 -> 2 -> 3 spends 10 + 11 + 12 = 33 time and leaves only
//         17 time at node 3, which cannot return to node 0 via 1 (needs 13 + 10 =
//         23).
//       - Taking 0 -> 1 -> 3 spends 10 + 13 = 23 time (cur_quality = 1 + 2 + 4 = 7,
//         visit_count = [1, 1, 0, 1]), then returns 3 -> 1 (visit_count[1] becomes
//         2 so gain = 0) -> 0 (total time 46 <= 50), updating best = 7.
//
// Algorithm:
//   Time-bounded backtracking Depth-First Search (DFS) rooted at node `0`. Key
//   constraint insight: because `time_j >= 10` and `maxTime <= 100`, every
//   feasible walk traverses at most `floor(maxTime / 10) <= 10` edges, and
//   each vertex has degree <= 4, bounding the search tree to at most 4^10
//   states (pruned further whenever `travel_time > remaining_time`). Whenever
//   the walk reaches node `0`, `best` is updated with `cur_quality`. Rather
//   than copying a visited set on each step, a `visit_count` table credits
//   `values[v]` only on the first entry (`0 -> 1`) and decrements cleanly on
//   backtrack.
//   Time Complexity: O(V + E + 4^(maxTime / minEdgeTime)), bounded by
//     O(V + E + 4^10) under the problem constraints.
//   Space Complexity: O(V + E) for the weighted adjacency list, `visit_count`
//     array, and recursion stack (depth <= maxTime / minEdgeTime <= 10).
//
// Data Structures:
//   - `std::vector<std::vector<std::pair<int, int>>> adj`: Undirected weighted
//     adjacency list storing `(neighbor, travel_time)` pairs per vertex.
//   - `std::vector<int> visit_count`: Per-node multiplicity counter along the
//     current walk so node values are summed only on first visit and cleanly
//     reverted during backtracking.
//   - Recursive call stack (`dfs` lambda): Tracks `(current_node,
//     remaining_time, cur_quality)` across the bounded-depth search tree.
//
// Real-World Applications:
//   - Drone reconnaissance and orienteering routing: departing a base depot
//     (`node 0`) and returning within a strict battery/fuel budget while
//     maximizing unique points-of-interest reward collected.
//   - Field-service technician or tourist itinerary optimization where hubs
//     may be traversed repeatedly but site value is realized only once.
//   - Bounded-horizon game AI and resource-collection planning in low-degree
//     graphs under strict turn or time budgets.
//
// Related LeetCode Problems:
//   - #1219 Path with Maximum Gold [done] (bounded backtracking DFS maximizing
//     collected path value)
//   - #1928 Minimum Cost to Reach Destination in Time [new] (weighted graph path
//     optimization constrained by `maxTime`)
//   - #787 Cheapest Flights Within K Stops [done] (budget-constrained graph traversal)
//   - #847 Shortest Path Visiting All Nodes [new] (graph walks permitting revisits
//     while tracking uniquely visited nodes)
//   - #797 All Paths From Source to Target [done] (DFS path exploration with
//     backtracking)

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
