// LeetCode 414: Third Maximum Number
// https://leetcode.com/problems/third-maximum-number/
//
// Problem:
//   Given a non-empty integer array `nums`, return the third distinct maximum
//   number in the array. If fewer than three distinct numbers exist, return
//   the overall maximum number instead. The solution must run in O(n) time.
//
// Algorithm:
//   Maintain three `std::optional<int>` slots (`first`, `second`, `third`)
//   representing the largest, second-largest, and third-largest distinct
//   values seen so far during a single left-to-right scan.
//
//   For each element `num` in `nums`:
//     1. If `num` already equals any occupied slot, skip it so duplicates
//        never occupy multiple rank positions.
//     2. Otherwise, insert `num` into the highest slot it exceeds, shifting
//        the displaced ranks downward (`first -> second -> third`).
//
//   Using `std::optional<int>` instead of a sentinel like `INT_MIN` cleanly
//   handles inputs that legitimately contain `std::numeric_limits<int>::min()`.
//   At the end of the scan, return `*third` if populated, otherwise `*first`.
//
//   Time Complexity:  O(n) — single linear pass with O(1) work per element.
//   Space Complexity: O(1) — only three optional scalar slots are maintained.
//
// Data Structures:
//   - Three `std::optional<int>` variables (`first`, `second`, `third`): act
//     as a fixed-capacity (k = 3) sorted top-k buffer without heap allocation
//     or sentinel ambiguity.
//
// Real-World Applications:
//   - Leaderboard and podium tracking (gold/silver/bronze distinct scores) in
//     streaming telemetry and competitive gaming without sorting full datasets.
//   - Monitoring pipelines tracking top-k distinct latency spikes or error
//     rates in constant memory over unbounded event streams.
//   - Financial tick processors maintaining best-k distinct bid/ask levels.
//
// Related LeetCode Problems:
//   - #215 Kth Largest Element in an Array [done] (generalizes k = 3 to arbitrary k via quickselect or min-heap)
//   - #703 Kth Largest Element in a Stream [done] (maintains top-k dynamically as new elements arrive)
//   - #628 Maximum Product of Three Numbers [new] (also tracks fixed top-3 maxima and bottom-2 minima in O(n))
//   - #1913 Maximum Product Difference Between Two Pairs [new] (single-pass tracking of top-2 and bottom-2 values)

#include "framework/lc_framework.h"

#include <limits>
#include <optional>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the third distinct maximum in `nums`, or the maximum if fewer than
  // three distinct values exist.
  //
  // Invariant: Whenever populated, `*first > *second > *third`, and if a lower
  // rank is populated then every higher rank is also populated.
  //
  // Time: O(n), Space: O(1).
  int thirdMax(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    std::optional<int> first;
    std::optional<int> second;
    std::optional<int> third;

    for (const int num : nums) {
      // Ignore duplicates so identical values share a single distinct rank.
      const bool already_tracked =
          (first.has_value() && num == *first) ||
          (second.has_value() && num == *second) ||
          (third.has_value() && num == *third);
      if (already_tracked) {
        continue;
      }

      // Cascade displaced maxima downward to preserve strict ordering.
      if (!first.has_value() || num > *first) {
        third = second;
        second = first;
        first = num;
      } else if (!second.has_value() || num > *second) {
        third = second;
        second = num;
      } else if (!third.has_value() || num > *third) {
        third = num;
      }
    }

    // Fall back to the overall maximum when fewer than 3 distinct values exist.
    return third.has_value() ? *third : *first;
  }
};

LC_PROBLEM(414, "Third Maximum Number", "array", ctx) {
  ctx.test("example_1_three_distinct", [&] {
    Solution sol;
    ctx.expect_eq(sol.thirdMax({3, 2, 1}), 1);
  });

  ctx.test("example_2_fewer_than_three", [&] {
    Solution sol;
    ctx.expect_eq(sol.thirdMax({1, 2}), 2);
  });

  ctx.test("example_3_with_duplicates", [&] {
    Solution sol;
    ctx.expect_eq(sol.thirdMax({2, 2, 3, 1}), 1);
  });

  ctx.test("contains_int_min_as_third_max", [&] {
    Solution sol;
    const int kMinInt = std::numeric_limits<int>::min();
    ctx.expect_eq(sol.thirdMax({1, 2, kMinInt}), kMinInt);
  });

  ctx.test("all_equal_elements", [&] {
    Solution sol;
    ctx.expect_eq(sol.thirdMax({5, 5, 5, 5}), 5);
  });

  ctx.test("two_distinct_with_duplicates", [&] {
    Solution sol;
    ctx.expect_eq(sol.thirdMax({5, 2, 2}), 5);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.thirdMax({-7}), -7);
  });

  ctx.test("all_negative_numbers", [&] {
    Solution sol;
    ctx.expect_eq(sol.thirdMax({-1, -2, -3, -4}), -3);
  });
}

}  // namespace
