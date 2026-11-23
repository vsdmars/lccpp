// LeetCode 1547: Minimum Cost To Cut A Stick
// https://leetcode.com/problems/minimum-cost-to-cut-a-stick/
//
// Problem:
//   You are given a wooden stick of length `n` (labeled from `0` to `n`) and an
//   array `cuts` of distinct integer positions strictly between `0` and `n`
//   where cuts must be made. Cuts can be performed in any order. Performing a
//   cut on a stick segment costs the current length of that segment and splits
//   it into two smaller segments whose lengths sum to the original length.
//   Return the minimum total cost required to perform all cuts.
//   Constraints: 2 <= n <= 10^6, 1 <= cuts.length <= min(n - 1, 100),
//   1 <= cuts[i] <= n - 1, and all values in `cuts` are distinct.
//
// Algorithm:
//   Interval Dynamic Programming (top-down memoized DFS):
//   1. Augment `cuts` with the two stick endpoints `0` and `n`, then sort all
//      boundary positions into `points[0..m-1]` where `m = cuts.size() + 2`.
//   2. Define `dfs(left, right)` as the minimum total cost to execute every
//      required cut strictly inside the open interval `(points[left],
//      points[right])`.
//   3. Base case: when `right - left < 2`, no cut lies strictly between
//      `points[left]` and `points[right]`, so the cost is `0`.
//   4. Transition: whichever cut `mid` (`left < mid < right`) is performed
//      first on the segment `[points[left], points[right]]`, it incurs the full
//      segment length `points[right] - points[left]` and decomposes the problem
//      into two independent sub-intervals `(left, mid)` and `(mid, right)`:
//        dp(left, right) = (points[right] - points[left])
//                          + min_{left < mid < right} [dp(left, mid) + dp(mid, right)].
//   5. Memoize each `(left, right)` pair so every interval is solved at most
//      once.
//
//   Complexity (let k = cuts.size(), m = k + 2):
//   - Time:  O(k^3) — O(k^2) distinct intervals, each scanning O(k) split points
//            (sorting takes O(k log k), dominated by O(k^3)).
//   - Space: O(k^2) — memoization table of size m x m plus O(k) recursion stack.
//
// Data Structures:
//   - `std::vector<int> points`: sorted list of all cut coordinates plus
//     endpoints `0` and `n`, mapping interval indices directly to stick lengths.
//   - `std::vector<std::vector<int>> memo`: 2D memoization table storing the
//     minimum cost for each sub-interval `(left, right)`, initialized to `-1`.
//
// Real-World Applications:
//   - Timber, steel, and sheet-metal cutting stock optimization where saw
//     wear or kerf-processing cost scales with the length of the workpiece.
//   - Database query optimization (optimal binary join trees and range
//     partitioning) and compiler optimal expression-tree splitting.
//   - Text/media segmentation and optimal binary search tree construction
//     where splitting cost is proportional to segment weight.
//
// Related LeetCode Problems:
//   - #312 Burst Balloons [done] (classic interval DP choosing a split point in a subarray)
//   - #1000 Minimum Cost to Merge Stones [new] (interval DP minimizing cumulative segment-merging costs)
//   - #1039 Minimum Score Triangulation of Polygon [new] (interval DP partitioning a range via an inner vertex)
//   - #664 Strange Printer [new] (interval DP combining optimal sub-intervals)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum total cost to perform all cuts in `cuts` on a stick
  // spanning `[0, n]`.
  //
  // Invariant: `points` is strictly increasing with `points.front() == 0` and
  // `points.back() == n`. Each memoized state `(left, right)` represents the
  // optimal cost to finish all cuts strictly between `points[left]` and
  // `points[right]`.
  //
  // Time: O(k^3), Space: O(k^2), where k = cuts.size().
  int minCost(int n, const std::vector<int>& cuts) {
    // Include the stick boundaries 0 and n so any active sub-stick is simply
    // bounded by two indices `left` and `right` in `points`.
    std::vector<int> points;
    points.reserve(cuts.size() + 2);
    points.push_back(0);
    points.insert(points.end(), cuts.begin(), cuts.end());
    points.push_back(n);
    std::ranges::sort(points);

    const std::size_t m = points.size();
    // `-1` marks uncomputed intervals (valid cut costs are always >= 0).
    std::vector<std::vector<int>> memo(m, std::vector<int>(m, -1));

    // Top-down interval DP: returns the minimum cost to make all cuts strictly
    // between `points[left]` and `points[right]`.
    auto dfs = [&](this auto&& self, std::size_t left,
                   std::size_t right) -> int {
      // Base case: adjacent boundary points have no cut strictly between them.
      if (right - left < 2) {
        return 0;
      }
      if (memo[left][right] != -1) {
        return memo[left][right];
      }

      // Regardless of which cut `mid` is chosen first, cutting this segment
      // always costs its full current length `points[right] - points[left]`.
      const int segment_cost = points[right] - points[left];
      int best_subcost = std::numeric_limits<int>::max();

      // Try every possible first cut `mid` strictly inside `(left, right)` and
      // recursively solve the two resulting independent sub-sticks.
      for (std::size_t mid = left + 1; mid < right; ++mid) {
        const int candidate = self(left, mid) + self(mid, right);
        best_subcost = std::min(best_subcost, candidate);
      }

      memo[left][right] = segment_cost + best_subcost;
      return memo[left][right];
    };

    return dfs(0, m - 1);
  }
};

LC_PROBLEM(1547, "Minimum Cost To Cut A Stick", "dp", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    // Optimal order [3, 5, 1, 4]: 7 + 4 + 3 + 2 = 16.
    ctx.expect_eq(sol.minCost(7, {1, 3, 4, 5}), 16);
  });

  ctx.test("example_2", [&] {
    // Optimal order [4, 6, 5, 2, 1]: total cost = 22.
    ctx.expect_eq(sol.minCost(9, {5, 6, 1, 4, 2}), 22);
  });

  ctx.test("single_cut", [&] {
    // Only one cut on a stick of length 6 always costs 6.
    ctx.expect_eq(sol.minCost(6, {3}), 6);
  });

  ctx.test("no_cuts", [&] {
    // Edge case: zero cuts require zero cost.
    ctx.expect_eq(sol.minCost(10, {}), 0);
  });

  ctx.test("every_unit_position", [&] {
    // Stick of length 4 cut at 1, 2, 3: cut at 2 (4), then 1 (2) and 3 (2).
    ctx.expect_eq(sol.minCost(4, {1, 2, 3}), 8);
  });

  ctx.test("symmetric_two_cuts", [&] {
    // Stick of length 10 cut at 2 and 8: either order costs 10 + 8 = 18.
    ctx.expect_eq(sol.minCost(10, {8, 2}), 18);
  });

  ctx.test("large_stick_balanced_cuts", [&] {
    // Stick of length 10^6 cut at quarters: 1,000,000 + 500,000 + 500,000.
    ctx.expect_eq(sol.minCost(1000000, {750000, 250000, 500000}), 2000000);
  });
}

}  // namespace
