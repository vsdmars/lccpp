// LeetCode 747: Largest Number At Least Twice Of Others
// https://leetcode.com/problems/largest-number-at-least-twice-of-others/
//
// Problem:
//   Given an integer array `nums` where the largest integer is unique,
//   determine whether the largest element in the array is at least twice as
//   large as every other element in the array. If it is, return the index of
//   the largest element; otherwise, return `-1`.
//   Constraints: 1 <= nums.size() <= 50; 0 <= nums[i] <= 100.
//
// Algorithm:
//   The maximum element is at least twice every other element if and only if
//   it is at least twice the second-largest element. In a single linear pass,
//   maintain the largest value (`largest`), its index (`largest_idx`), and the
//   second-largest value (`second_largest`). When an element `x >= largest` is
//   seen, demote `largest` into `second_largest` before updating `largest` and
//   `largest_idx`; otherwise, if `x > second_largest`, update only
//   `second_largest`. After the scan, verify `largest >= 2 * second_largest`
//   using 64-bit arithmetic to avoid signed overflow.
//   Time Complexity:  O(n), single pass through `nums`.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Scalar top-2 tracker (`largest`, `second_largest`, `largest_idx`):
//     retains the two largest order statistics in constant space without
//     sorting or allocating a heap.
//
// Real-World Applications:
//   - Election and consensus margin checks: verifying whether the leading
//     candidate or quorum vote has a >= 2x supermajority over the runner-up.
//   - Anomaly and outlier detection: flagging a metric spike (e.g., latency or
//     CPU usage) that exceeds twice the second-highest observation.
//   - Feature-matching ratio tests (Lowe's ratio test in computer vision):
//     accepting a best descriptor match only when it beats the second-best
//     match by a clear multiplicative margin.
//
// Related LeetCode Problems:
//   - #414 Third Maximum Number [done] (single-pass tracking of top-k order statistics)
//   - #215 Kth Largest Element in an Array [done] (generalized k-th largest selection)
//   - #1464 Maximum Product of Two Elements in an Array [new] (single-pass top-2 maximum tracking)
//   - #628 Maximum Product of Three Numbers [new] (tracking extreme values in one linear scan)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the index of the largest element if it is >= 2 * every other
  // element, or -1 otherwise.
  // Time: O(n), Space: O(1).
  int dominantIndex(const std::vector<int>& nums) {
    if (nums.empty()) {
      return -1;
    }
    if (nums.size() == 1uz) {
      // With no other elements to compare against, the sole element dominates.
      return 0;
    }

    int largest = std::numeric_limits<int>::min();
    int second_largest = std::numeric_limits<int>::min();
    std::size_t largest_idx = 0uz;

    for (std::size_t idx = 0uz; idx < nums.size(); ++idx) {
      const int value = nums[idx];
      if (value >= largest) {
        // Demote the previous maximum so duplicate maxima are also captured as
        // `second_largest` and accurately fail the 2x test (unless both are 0).
        second_largest = largest;
        largest = value;
        largest_idx = idx;
      } else if (value > second_largest) {
        second_largest = value;
      }
    }

    // Use 64-bit multiplication to prevent signed overflow on large values.
    const auto largest64 = static_cast<std::int64_t>(largest);
    const std::int64_t twice_second =
        std::int64_t{2} * static_cast<std::int64_t>(second_largest);
    if (largest64 < twice_second) {
      return -1;
    }
    return static_cast<int>(largest_idx);
  }
};

LC_PROBLEM(747, "Largest Number At Least Twice Of Others", "array", ctx) {
  Solution sol;

  ctx.test("example_1_dominant_exists", [&] {
    ctx.expect_eq(sol.dominantIndex({3, 6, 1, 0}), 1);
  });

  ctx.test("example_2_not_twice_second_largest", [&] {
    ctx.expect_eq(sol.dominantIndex({1, 2, 3, 4}), -1);
  });

  ctx.test("python_build_cases", [&] {
    ctx.expect_eq(sol.dominantIndex({0, 0, 3, 2}), -1);
    ctx.expect_eq(sol.dominantIndex({2, 6, 1, 0}), 1);
  });

  ctx.test("exact_double_boundary", [&] {
    ctx.expect_eq(sol.dominantIndex({4, 8, 2}), 1);
    ctx.expect_eq(sol.dominantIndex({10, 5, 1}), 0);
    ctx.expect_eq(sol.dominantIndex({1, 5, 10}), 2);
  });

  ctx.test("single_element_and_empty", [&] {
    ctx.expect_eq(sol.dominantIndex({1}), 0);
    ctx.expect_eq(sol.dominantIndex({0}), 0);
    ctx.expect_eq(sol.dominantIndex({}), -1);
  });

  ctx.test("duplicate_maximum_not_dominant", [&] {
    ctx.expect_eq(sol.dominantIndex({5, 1, 5}), -1);
  });
}

}  // namespace
