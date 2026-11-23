// LeetCode 396: Rotate Function
// https://leetcode.com/problems/rotate-function/
//
// Problem:
//   Given an integer array `nums` of length n (1 <= n <= 10^5, -100 <= nums[i]
//   <= 100), let B_k be the array obtained by rotating `nums` clockwise by k
//   positions, and define:
//     F(k) = 0 * B_k[0] + 1 * B_k[1] + ... + (n - 1) * B_k[n - 1].
//   Return the maximum value of F(0), F(1), ..., F(n - 1).
//
// Algorithm:
//   Rather than recomputing F(k) in O(n) for each of the n rotations (which
//   would take O(n^2)), derive the O(1) recurrence between consecutive
//   rotations. Moving from B_{k-1} to B_k shifts every element right by one
//   index (adding 1 * element to the weighted sum) except the element wrapping
//   from index n - 1 to index 0, which is `nums[n - k]` and loses (n - 1)
//   copies of its weight (plus the 1 copy already added by `total_sum`):
//     F(k) = F(k - 1) + total_sum - n * nums[n - k].
//   Compute `total_sum` and `F(0)` in one initial pass, then iterate k from 1
//   to n - 1 updating F(k) and tracking the maximum. Intermediate arithmetic
//   uses 64-bit integers to avoid any intermediate overflow.
//   Time Complexity: O(n).
//   Space Complexity: O(1).
//
// Data Structures:
//   Constant scalar accumulators (`total_sum`, `current_f`, `max_f`) over the
//   input `std::vector<int>`.
//
// Real-World Applications:
//   Sliding-window weighted linear filters and ramp correlations in digital
//   signal processing, incremental center-of-mass updates under cyclic buffer
//   rotations, and rolling hash / weighted checksum recalculation.
//
// Related LeetCode Problems:
//   - #189 Rotate Array [done] (performing in-place cyclic rotations of an array)
//   - #53 Maximum Subarray [done] (linear-time recurrence tracking a running maximum)
//   - #1423 Maximum Points You Can Obtain from Cards [new] (sliding-window cyclic sum optimization)
//   - #238 Product of Array Except Self [done] (algebraic reuse across array positions in O(n))

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Computes max_{0 <= k < n} F(k) using the O(1) cyclic recurrence
  // F(k) = F(k - 1) + sum(nums) - n * nums[n - k].
  // Time: O(n), Space: O(1).
  int maxRotateFunction(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    const std::size_t len = nums.size();
    const auto n = static_cast<std::int64_t>(len);

    // Phase 1: compute total element sum and the base value F(0).
    std::int64_t total_sum = 0;
    std::int64_t current_f = 0;
    for (std::size_t i = 0uz; i < len; ++i) {
      const auto val = static_cast<std::int64_t>(nums[i]);
      total_sum += val;
      current_f += static_cast<std::int64_t>(i) * val;
    }

    std::int64_t max_f = current_f;

    // Phase 2: advance from F(k - 1) to F(k) for each clockwise rotation k.
    // The element leaving index (n - 1) to wrap around to index 0 is
    // `nums[len - k]`, so its coefficient drops by n while every other element
    // gains +1.
    for (std::size_t k = 1uz; k < len; ++k) {
      const auto wrapped = static_cast<std::int64_t>(nums[len - k]);
      current_f = (current_f + total_sum) - (n * wrapped);
      max_f = std::max(max_f, current_f);
    }

    return static_cast<int>(max_f);
  }
};

LC_PROBLEM(396, "Rotate Function", "math", ctx) {
  ctx.test("example_1_build", [&] {
    Solution sol;
    // F(0)=25, F(1)=16, F(2)=23, F(3)=26.
    ctx.expect_eq(sol.maxRotateFunction({4, 3, 2, 6}), 26);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxRotateFunction({100}), 0);
  });

  ctx.test("all_negative", [&] {
    Solution sol;
    // F(0)=-8, F(1)=-5, F(2)=-5.
    ctx.expect_eq(sol.maxRotateFunction({-1, -2, -3}), -5);
  });

  ctx.test("mixed_signs", [&] {
    Solution sol;
    // F(0)=-8, F(1)=6, F(2)=-8, F(3)=-2.
    ctx.expect_eq(sol.maxRotateFunction({1, -2, 3, -4}), 6);
  });

  ctx.test("strictly_increasing", [&] {
    Solution sol;
    // By rearrangement inequality, F(0) = sum_{i=0}^9 i*(i+1) = 330 is maximal.
    ctx.expect_eq(sol.maxRotateFunction({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), 330);
  });
}

}  // namespace
