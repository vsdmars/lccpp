// https://leetcode.com/problems/min-cost-to-connect-all-points/description/
//
// Given an array `points` representing integer 2D coordinates where
// `points[i] = [xi, yi]`, the cost of connecting two points is their Manhattan
// distance `|xi - xj| + |yi - yj|`. Return the minimum total cost to connect
// all points into a single spanning tree.

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
