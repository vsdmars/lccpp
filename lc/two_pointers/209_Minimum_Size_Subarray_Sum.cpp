// LeetCode 209: Minimum Size Subarray Sum
// https://leetcode.com/problems/minimum-size-subarray-sum/
//
// Problem:
//   Given a positive integer `target` and an array `nums` of positive
//   integers, return the minimal length of a contiguous subarray whose sum is
//   greater than or equal to `target`. If no such subarray exists, return 0.
//
// Algorithm:
//   Variable-size sliding window (`[left, right]`) relying critically on the
//   fact that every element of `nums` is strictly positive (`nums[i] > 0`).
//   Positivity guarantees monotonicity: expanding `right` strictly increases
//   `window_sum`, and advancing `left` strictly decreases it.
//   For each `right` from `0` to `nums.size() - 1`:
//     1. Add `nums[right]` to `window_sum`.
//     2. While `window_sum >= target`, record `right - left + 1` as a
//        candidate minimum length, subtract `nums[left]`, and increment
//        `left` to tighten the window as much as possible.
//   (Note: if negative numbers were allowed, a monotonic deque over prefix
//   sums — LeetCode #862 — would be needed instead.)
//   Time Complexity:  O(n) — both `left` and `right` move monotonically from
//                     `0` to `n`, visiting each element at most twice.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Two `std::size_t` sliding-window cursors (`left`, `right`) and a 64-bit
//   running sum accumulator (`std::int64_t`) to avoid 32-bit signed overflow.
//
// Real-World Applications:
//   - Financial / bandwidth burst detection finding the shortest time interval
//     during which cumulative volume or bytes transferred meets a threshold.
//   - Resource allocation and cloud billing analysis locating the tightest
//     contiguous window of tasks/requests exceeding a quota or SLA budget.
//   - Streaming signal processing triggering alerts on minimal-duration energy
//     spikes above a threshold.
//
// Related LeetCode Problems:
//   - #862 Shortest Subarray with Sum at Least K [done] (generalization allowing negative values via monotonic deque)
//   - #76 Minimum Window Substring [done] (minimal-length sliding window over character frequencies)
//   - #560 Subarray Sum Equals K [done] (exact target sum with negative values via prefix-sum hash map)
//   - #325 Maximum Size Subarray Sum Equals k [done] (longest subarray with exact sum via prefix sums)
//   - #713 Subarray Product Less Than K [new] (sliding window exploiting positive-number monotonicity for products)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the length of the shortest contiguous subarray of positive integers
  // with sum >= `target`, or 0 if the entire array sums to less than `target`.
  //
  // Invariant: `window_sum` equals the sum of `nums[left .. right]`. Because
  // all elements are positive, once `window_sum >= target`, shrinking `left`
  // explores all shorter valid windows ending at `right`.
  //
  // Complexity: O(n) time, O(1) extra space.
  [[nodiscard]] int minSubArrayLen(int target, const std::vector<int>& nums) {
    constexpr std::size_t kInf = std::numeric_limits<std::size_t>::max();
    std::size_t min_len = kInf;
    std::int64_t window_sum = 0;
    std::size_t left = 0uz;

    for (std::size_t right = 0uz; right < nums.size(); ++right) {
      window_sum += nums[right];

      // Greedily shrink from the left while the window still meets `target`.
      while (left <= right && window_sum >= target) {
        min_len = std::min(min_len, right - left + 1uz);
        window_sum -= nums[left];
        ++left;
      }
    }

    return min_len == kInf ? 0 : static_cast<int>(min_len);
  }
};

LC_PROBLEM(209, "Minimum Size Subarray Sum", "two_pointers", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1_and_python_build", [&] {
    ctx.expect_eq(sol.minSubArrayLen(7, {2, 3, 1, 2, 4, 3}), 2);
    ctx.expect_eq(sol.minSubArrayLen(11, {10, 1}), 2);
    ctx.expect_eq(sol.minSubArrayLen(7, {2, 3, 1, 1, 4, 3}), 2);
    ctx.expect_eq(sol.minSubArrayLen(15, {1, 2, 3, 4, 5}), 5);
  });

  ctx.test("leetcode_example_2_single_element_hit", [&] {
    ctx.expect_eq(sol.minSubArrayLen(4, {1, 4, 4}), 1);
  });

  ctx.test("leetcode_example_3_impossible", [&] {
    ctx.expect_eq(sol.minSubArrayLen(11, {1, 1, 1, 1, 1, 1, 1, 1}), 0);
  });

  ctx.test("edge_cases_empty_and_single", [&] {
    ctx.expect_eq(sol.minSubArrayLen(5, {}), 0);
    ctx.expect_eq(sol.minSubArrayLen(5, {5}), 1);
    ctx.expect_eq(sol.minSubArrayLen(6, {5}), 0);
  });

  ctx.test("large_values_no_overflow", [&] {
    constexpr int kBillion = 1'000'000'000;
    ctx.expect_eq(sol.minSubArrayLen(2'000'000'000,
                                     {kBillion, kBillion, kBillion}),
                  2);
  });
}

}  // namespace
