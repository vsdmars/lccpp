// LeetCode 1584: Min Cost to Connect All Points
// https://leetcode.com/problems/min-cost-to-connect-all-points/
//
// Problem:
//   Given an array `points` of distinct 2D integer coordinates where
//   `points[i] = [xi, yi]`, the cost of connecting two points `i` and `j` is
//   their Manhattan distance `|xi - xj| + |yi - yj|`. Return the minimum total
//   cost to connect all points so that exactly one simple path exists between
//   every pair of points (i.e., the Minimum Spanning Tree weight of the
//   complete graph on `points`). Constraints: 1 <= points.length <= 1000,
//   -10^6 <= xi, yi <= 10^6.
//
// Algorithm:
//   Prim's Minimum Spanning Tree (MST) algorithm using a binary min-heap and
//   best-known distance relaxation. Starting from point `0`, the loop pops the
//   minimum-cost frontier edge `(dist, idx)`, discards stale entries whose
//   endpoint is already in the MST, adds `dist` to `total_cost`, and computes
//   Manhattan distances from `idx` to all remaining unvisited points on the
//   fly. A candidate `(ndist, next)` is pushed to the heap only if it strictly
//   improves `min_dist[next]`. By the MST cut property, greedily selecting the
//   lightest edge crossing between visited and unvisited sets produces a
//   minimum-weight spanning tree without pre-building all O(N^2) edges.
//   Time Complexity: O(N^2 log N) worst case (up to O(N^2) heap pushes, each
//     O(log N)).
//   Space Complexity: O(N^2) worst case for heap entries (`O(N)` for `visited`
//     and `min_dist`).
//
// Data Structures:
//   - `std::priority_queue<Edge, std::vector<Edge>, std::greater<>> min_heap`:
//     Min-heap of `(distance, point_index)` pairs retrieving the cheapest edge
//     to expand the MST.
//   - `std::vector<bool> visited`: Tracks points already added to the MST to
//     skip outdated heap entries.
//   - `std::vector<int> min_dist`: Stores the smallest known Manhattan distance
//     from the current MST to each unvisited point to prune dominated pushes.
//
// Real-World Applications:
//   - VLSI chip design and PCB rectilinear wire routing: minimizing total
//     Manhattan wirelength when connecting pins across a grid-aligned layout.
//   - Urban telecommunications, fiber-optic, and water/gas pipe deployment
//     along Manhattan-style street grids at minimum infrastructure cost.
//   - Single-linkage hierarchical clustering in spatial data analysis and
//     machine learning via Minimum Spanning Tree construction.
//
// Related LeetCode Problems:
//   - #1135 Connecting Cities With Minimum Cost [new] (classic MST on an explicit
//     weighted undirected graph)
//   - #1168 Optimize Water Distribution in a Village [new] (MST augmented with a
//     virtual node modeling per-vertex installation costs)
//   - #1489 Find Critical and Pseudo-Critical Edges in Minimum Spanning Tree [new]
//     (classifying edges by their role across all MSTs)
//   - #743 Network Delay Time [new] (greedy min-heap relaxation structurally similar
//     to Prim's algorithm)
//   - #1579 Remove Max Number of Edges to Keep Graph Fully Traversable [new]
//     (spanning-tree connectivity optimization across edge types)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Prim's MST algorithm with a min-heap and best-known edge relaxation.
  // Time: O(N^2 log N) worst case, Space: O(N^2) worst case for heap entries.
  int minCostConnectPoints(const std::vector<std::vector<int>>& points) {
    const std::size_t n = points.size();
    if (n <= 1uz) {
      return 0;
    }

    using Edge = std::pair<int, std::size_t>;
    std::priority_queue<Edge, std::vector<Edge>, std::greater<>> min_heap;
    std::vector<bool> visited(n, false);
    std::vector<int> min_dist(n, std::numeric_limits<int>::max());

    min_dist[0] = 0;
    min_heap.emplace(0, 0uz);

    int total_cost = 0;
    std::size_t visited_count = 0uz;

    while (visited_count < n) {
      const auto [dist, idx] = min_heap.top();
      min_heap.pop();

      if (visited[idx]) {
        continue;
      }

      visited[idx] = true;
      ++visited_count;
      total_cost += dist;

      const int x = points[idx][0];
      const int y = points[idx][1];
      for (std::size_t next = 0uz; next < n; ++next) {
        if (visited[next]) {
          continue;
        }
        const int ndist =
            std::abs(points[next][0] - x) + std::abs(points[next][1] - y);
        if (ndist < min_dist[next]) {
          min_dist[next] = ndist;
          min_heap.emplace(ndist, next);
        }
      }
    }

    return total_cost;
  }
};

LC_PROBLEM(1584, "Min Cost To Connect All Points", "graph", ctx) {
  ctx.test("leetcode_example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {
        {0, 0}, {2, 2}, {3, 10}, {5, 2}, {7, 0}};
    ctx.expect_eq(sol.minCostConnectPoints(points), 20);
  });

  ctx.test("leetcode_example_2", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {{3, 12}, {-2, 5}, {-4, 1}};
    ctx.expect_eq(sol.minCostConnectPoints(points), 18);
  });

  ctx.test("trivial_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.minCostConnectPoints({}), 0);
    ctx.expect_eq(sol.minCostConnectPoints({{4, -9}}), 0);
  });

  ctx.test("large_coordinates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {{-1000000, -1000000},
                                                  {1000000, 1000000}};
    ctx.expect_eq(sol.minCostConnectPoints(points), 4000000);
  });

  ctx.test("square_and_collinear", [&] {
    Solution sol;
    ctx.expect_eq(sol.minCostConnectPoints({{0, 0}, {0, 2}, {2, 0}, {2, 2}}),
                  6);
    ctx.expect_eq(sol.minCostConnectPoints({{0, 0}, {1, 0}, {2, 0}, {5, 0}}),
                  5);
  });
}

}  // namespace
