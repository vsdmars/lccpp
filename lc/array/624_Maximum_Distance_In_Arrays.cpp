// LeetCode 624: Maximum Distance In Arrays
// https://leetcode.com/problems/maximum-distance-in-arrays/
//
// Problem:
//   Given `m` arrays (`m >= 2`), each non-empty and sorted in ascending order,
//   pick two integers from two *different* arrays to maximize their absolute
//   difference `|a - b|`. Return this maximum distance.
//
// Examples:
//   Example 1:
//     Input:  arrays = [[1, 2, 3], [4, 5], [1, 2, 3]]
//     Output: 4
//     Explanation:
//       - Seed min_val = 1, max_val = 3 from arrays[0] = [1, 2, 3], and initialize
//         best = 0.
//       - At i = 1 ([4, 5]), compute best = max(0, 5 - 1, 3 - 4) = 4, then update
//         min_val = 1 and max_val = 5.
//       - At i = 2 ([1, 2, 3]), compute best = max(4, 3 - 1, 5 - 1) = 4 and keep
//         min_val = 1, max_val = 5, returning 4.
//
//   Example 2:
//     Input:  arrays = [[1, 10], [2, 3]]
//     Output: 8
//     Explanation:
//       - Seed min_val = 1, max_val = 10 from arrays[0] = [1, 10], and initialize
//         best = 0.
//       - At i = 1 ([2, 3]), compare strictly against prior arrays to get best =
//         max(0, 3 - 1, 10 - 2) = 8, avoiding the invalid same-array difference 10
//         - 1.
//       - Update min_val = 1 and max_val = 10, then exit the loop and return 8.
//
//   Example 3:
//     Input:  arrays = [[-10, -5], [-3, 0], [-2, 4]]
//     Output: 14
//     Explanation:
//       - Seed min_val = -10, max_val = -5 from arrays[0] = [-10, -5], and
//         initialize best = 0.
//       - At i = 1 ([-3, 0]), update best = max(0, 0 - (-10), -5 - (-3)) = 10, then
//         update min_val = -10 and max_val = 0.
//       - At i = 2 ([-2, 4]), update best = max(10, 4 - (-10), 0 - (-2)) = 14,
//         returning 14.
//
// Algorithm:
//   Because every individual array is already sorted in ascending order, only
//   its first element (`front()`, minimum) and last element (`back()`, maximum)
//   can contribute to a maximum distance.
//   Scan the arrays once from index `1` to `m - 1`, maintaining the global
//   minimum (`min_val`) and global maximum (`max_val`) seen strictly among
//   earlier arrays `arrays[0..i-1]`:
//     1. Update `best` with `max(arrays[i].back() - min_val, max_val -
//        arrays[i].front())`. Updating `best` *before* folding `arrays[i]`
//        into `min_val` and `max_val` guarantees the two endpoints always come
//        from strictly different arrays.
//     2. Fold `arrays[i].front()` into `min_val` and `arrays[i].back()` into
//        `max_val`.
//
//   Time Complexity: O(m), where `m` is the number of arrays (O(1) per array).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Three running scalar integers (`min_val`, `max_val`, `best`) tracking
//     prefix extrema across previously visited sorted arrays.
//
// Real-World Applications:
//   - Distributed database query planning and shard range-pruning (computing
//     maximum cross-partition value spread from per-partition min/max metadata).
//   - Multi-sensor telemetry drift detection (finding the largest cross-sensor
//     reading divergence from sorted per-device measurement windows).
//   - Portfolio cross-asset spread analysis comparing pre-sorted price ranges
//     across distinct financial instruments.
//
// Related LeetCode Problems:
//   - #121 Best Time to Buy and Sell Stock [done] (tracks running prefix minimum for max difference)
//   - #122 Best Time to Buy and Sell Stock II [done] (greedy single-pass extremum tracking)
//   - #632 Smallest Range Covering Elements from K Lists [done] (multi-list extrema across sorted arrays)
//   - #2016 Maximum Difference Between Increasing Elements [new] (prefix-min single-pass max gap)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum absolute difference between any two elements chosen
  // from two distinct sorted arrays in `arrays`.
  //
  // Invariant:
  //   At the start of iteration `i`, `min_val` and `max_val` hold the global
  //   minimum and maximum across `arrays[0..i-1]`, and `best` holds the maximum
  //   cross-array distance among all pairs of arrays in `arrays[0..i-1]`.
  //
  // Time Complexity: O(m) where `m = arrays.size()`.
  // Space Complexity: O(1).
  int maxDistance(const std::vector<std::vector<int>>& arrays) {
    if (arrays.size() < 2) {
      return 0;
    }

    // Seed prefix extrema from the first sorted array so no magic sentinel
    // constants are required and any valid integer range is handled cleanly.
    int min_val = arrays[0].front();
    int max_val = arrays[0].back();
    int best = 0;

    for (std::size_t i = 1; i < arrays.size(); ++i) {
      const int cur_min = arrays[i].front();
      const int cur_max = arrays[i].back();

      // Compare `arrays[i]` strictly against prior arrays before updating
      // `min_val`/`max_val`, enforcing the "two different arrays" rule.
      best = std::max({best, cur_max - min_val, max_val - cur_min});

      min_val = std::min(min_val, cur_min);
      max_val = std::max(max_val, cur_max);
    }

    return best;
  }
};

LC_PROBLEM(624, "Maximum Distance In Arrays", "array", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    const std::vector<std::vector<int>> arrays = {
        {1, 2, 3},
        {4, 5},
        {1, 2, 3},
    };
    ctx.expect_eq(sol.maxDistance(arrays), 4);
  });

  ctx.test("single_element_equal_arrays", [&] {
    const std::vector<std::vector<int>> arrays = {{1}, {1}};
    ctx.expect_eq(sol.maxDistance(arrays), 0);
  });

  ctx.test("global_min_and_max_in_same_array", [&] {
    // Array 0 contains both global min (1) and global max (10), so we cannot
    // take 10 - 1 = 9; best cross-array distance is 10 - 2 = 8.
    const std::vector<std::vector<int>> arrays = {
        {1, 10},
        {2, 3},
    };
    ctx.expect_eq(sol.maxDistance(arrays), 8);
  });

  ctx.test("negative_values", [&] {
    const std::vector<std::vector<int>> arrays = {
        {-10, -5},
        {-3, 0},
        {-2, 4},
    };
    ctx.expect_eq(sol.maxDistance(arrays), 14);
  });

  ctx.test("min_in_last_array_max_in_first", [&] {
    const std::vector<std::vector<int>> arrays = {
        {5, 9},
        {3, 4},
        {-2, 1},
    };
    ctx.expect_eq(sol.maxDistance(arrays), 11);
  });
}

}  // namespace
