// LeetCode 973: K Closest Points To Origin
// https://leetcode.com/problems/k-closest-points-to-origin/
//
// Problem:
//   Given an array of 2D integer points `points` where `points[i] = [xi, yi]`
//   and an integer `k`, return the `k` points closest to the origin `(0, 0)`
//   under Euclidean distance. The output may be returned in any order.
//
// Examples:
//   Example 1:
//     Input:  points = [[1, 3], [-2, 2]], k = 1
//     Output: [[-2, 2]]
//     Explanation:
//       - For index 0 ([1, 3]), compute squared_distance = 1^2 + 3^2 = 10 and push
//         (10, 0) into max_heap.
//       - For index 1 ([-2, 2]), compute squared_distance = (-2)^2 + 2^2 = 8 and
//         push (8, 1) into max_heap.
//       - Since max_heap.size() = 2 > limit (1), pop the root (10, 0), leaving only
//         (8, 1).
//       - Drain max_heap to return [[-2, 2]].
//
//   Example 2:
//     Input:  points = [[3, 3], [5, -1], [-2, 4]], k = 2
//     Output: [[-2, 4], [3, 3]]
//     Explanation:
//       - Push (18, 0) for [3, 3] and (26, 1) for [5, -1] into max_heap, keeping
//         size 2 <= limit (2).
//       - Compute squared_distance([-2, 4]) = (-2)^2 + 4^2 = 20 and push (20, 2)
//         into max_heap, increasing size to 3 > 2.
//       - Evict the farthest root (26, 1) ([5, -1]), leaving (20, 2) and (18, 0) in
//         max_heap.
//       - Pop the remaining entries from max_heap to return [[-2, 4], [3, 3]].
//
//   Example 3:
//     Input:  points = [[0, 0], [1, 1], [-1, -1], [2, 2]], k = 3
//     Output: [[-1, -1], [0, 0], [1, 1]]
//     Explanation:
//       - Compute squared distances 0 for [0, 0], 2 for [1, 1], 2 for [-1, -1], and
//         8 for [2, 2].
//       - Push (0, 0), (2, 1), and (2, 2) into max_heap, filling it to limit = 3.
//       - Push (8, 3) for [2, 2], causing max_heap.size() = 4 > 3, and immediately
//         pop the root (8, 3).
//       - Drain the three remaining entries from max_heap, yielding [[-1, -1], [0,
//         0], [1, 1]] in canonicalized order.
//
// Algorithm:
//   Because square root is strictly increasing on non-negative values,
//   comparing exact squared distances `x^2 + y^2` (in 64-bit integer
//   arithmetic) avoids floating-point error and overflow.
//
//   1. Bounded Max-Heap of Size k (primary):
//      Scan all points, pushing `(dist_sq, index)` into a max-heap. Whenever
//      the heap grows beyond size `k`, pop the farthest point at the root.
//      After scanning all N points, the heap holds the k closest points.
//      Time:  O(N * log(k))
//      Space: O(k)
//
//   2. All-Points Min-Heap (alternative matching Python `rewrite`):
//      Heapify all N `(dist_sq, index)` entries into a min-heap in O(N) time
//      and extract the minimum k times.
//      Time:  O(N + k * log(N))
//      Space: O(N)
//
// Data Structures:
//   - Max-heap (`std::priority_queue<PointDist>`) capped at k elements to
//     evict the farthest candidate in O(log k).
//   - Min-heap (`std::priority_queue` with `std::greater<>`) built via linear
//     heapify for the alternative extraction pass.
//
// Real-World Applications:
//   - Geospatial services (ride-hailing dispatch, restaurant/store locators)
//     finding the k nearest candidates to a query coordinate.
//   - k-Nearest Neighbors (k-NN) inference and vector similarity search.
//   - Collision detection broad-phase pruning in physics engines and robotics.
//
// Related LeetCode Problems:
//   - #215 Kth Largest Element in an Array [done] (top-k / quickselect selection)
//   - #347 Top K Frequent Elements [done] (bounded heap top-k extraction)
//   - #692 Top K Frequent Words [done] (bounded heap with custom comparator)
//   - #703 Kth Largest Element in a Stream [done] (online bounded heap)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the k closest points to the origin using a bounded max-heap of
  // size k. Time: O(N * log(k)), Space: O(k).
  std::vector<std::vector<int>> kClosest(
      const std::vector<std::vector<int>>& points, int k) {
    if (points.empty() || k <= 0) {
      return {};
    }
    const auto limit = static_cast<std::size_t>(k);
    if (limit >= points.size()) {
      return points;
    }

    std::priority_queue<PointDist> max_heap;
    for (std::size_t i = 0uz; i < points.size(); ++i) {
      max_heap.push(
          PointDist{.dist_sq = squared_distance(points[i]), .index = i});
      // Evict the farthest point whenever the candidate set exceeds k.
      if (max_heap.size() > limit) {
        max_heap.pop();
      }
    }

    std::vector<std::vector<int>> result;
    result.reserve(max_heap.size());
    while (!max_heap.empty()) {
      result.push_back(points[max_heap.top().index]);
      max_heap.pop();
    }
    return result;
  }

  // Alternative implementation using linear-time min-heap construction followed
  // by k extractions (mirrors Python `rewrite`).
  // Time: O(N + k * log(N)), Space: O(N).
  std::vector<std::vector<int>> kClosestMinHeap(
      const std::vector<std::vector<int>>& points, int k) {
    if (points.empty() || k <= 0) {
      return {};
    }
    const auto limit = static_cast<std::size_t>(k);
    if (limit >= points.size()) {
      return points;
    }

    std::vector<PointDist> entries;
    entries.reserve(points.size());
    for (std::size_t i = 0uz; i < points.size(); ++i) {
      entries.push_back(
          PointDist{.dist_sq = squared_distance(points[i]), .index = i});
    }

    std::priority_queue<PointDist, std::vector<PointDist>, std::greater<>>
        min_heap(std::greater<>{}, std::move(entries));

    std::vector<std::vector<int>> result;
    result.reserve(limit);
    for (std::size_t i = 0uz; i < limit; ++i) {
      result.push_back(points[min_heap.top().index]);
      min_heap.pop();
    }
    return result;
  }

 private:
  struct PointDist {
    std::int64_t dist_sq = 0;
    std::size_t index = 0;

    [[nodiscard]] bool operator<(const PointDist& other) const noexcept {
      return dist_sq < other.dist_sq;
    }

    [[nodiscard]] bool operator>(const PointDist& other) const noexcept {
      return dist_sq > other.dist_sq;
    }
  };

  // Computes exact squared Euclidean distance in 64-bit arithmetic to avoid
  // overflow and floating-point precision loss.
  [[nodiscard]] static std::int64_t squared_distance(
      const std::vector<int>& point) noexcept {
    const auto x = static_cast<std::int64_t>(point[0]);
    const auto y = static_cast<std::int64_t>(point[1]);
    return (x * x) + (y * y);
  }
};

// Sorts point lists canonically so order-independent results can be compared.
std::vector<std::vector<int>> canonicalize(
    std::vector<std::vector<int>> points) {
  std::ranges::sort(points);
  return points;
}

LC_PROBLEM(973, "K Closest Points To Origin", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {{1, 3}, {-2, 2}};
    const std::vector<std::vector<int>> expected = {{-2, 2}};
    ctx.expect_eq(canonicalize(sol.kClosest(points, 1)), expected);
    ctx.expect_eq(canonicalize(sol.kClosestMinHeap(points, 1)), expected);
  });

  ctx.test("example_2_python_build", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {{3, 3}, {5, -1}, {-2, 4}};
    const std::vector<std::vector<int>> expected = {{-2, 4}, {3, 3}};
    ctx.expect_eq(canonicalize(sol.kClosest(points, 2)), expected);
    ctx.expect_eq(canonicalize(sol.kClosestMinHeap(points, 2)), expected);
  });

  ctx.test("origin_and_symmetric_points", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {
        {0, 0},
        {1, 1},
        {-1, -1},
        {2, 2},
    };
    const std::vector<std::vector<int>> expected = {{-1, -1}, {0, 0}, {1, 1}};
    ctx.expect_eq(canonicalize(sol.kClosest(points, 3)), expected);
    ctx.expect_eq(canonicalize(sol.kClosestMinHeap(points, 3)), expected);
  });

  ctx.test("boundary_k_values", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {{2, 1}, {-1, -2}};
    ctx.expect_eq(sol.kClosest(points, 0), std::vector<std::vector<int>>{});
    ctx.expect_eq(canonicalize(sol.kClosest(points, 2)),
                  canonicalize(points));
    ctx.expect_eq(canonicalize(sol.kClosestMinHeap(points, 5)),
                  canonicalize(points));
  });
}

}  // namespace
