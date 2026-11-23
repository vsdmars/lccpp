// LeetCode 643: Maximum Average Subarray I
// https://leetcode.com/problems/maximum-average-subarray-i/
//
// Problem:
//   Given an integer array `nums` of length n and an integer `k`, find a
//   contiguous subarray of exact length `k` that has the maximum average value
//   and return that maximum average as a floating-point number.
//   Constraints: 1 <= k <= n <= 10^5, -10^4 <= nums[i] <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 12, -5, -6, 50, 3], k = 4
//     Output: 12.75
//     Explanation:
//       - Sum the initial window nums[0..3] = [1, 12, -5, -6] to get window_sum = 2
//         and max_sum = 2.
//       - At i = 4, add nums[4] = 50 and subtract nums[0] = 1 so window_sum becomes
//         51 and max_sum updates to 51.
//       - At i = 5, add nums[5] = 3 and subtract nums[1] = 12 so window_sum becomes
//         42 while max_sum stays 51.
//       - Divide max_sum by k = 4 once at the end to return 51.0 / 4.0 = 12.75.
//
//   Example 2:
//     Input:  nums = [-1, -2, -3, -4], k = 2
//     Output: -1.5
//     Explanation:
//       - Sum the initial window nums[0..1] = [-1, -2] to initialize window_sum =
//         -3 and max_sum = -3.
//       - Slide to i = 2 (window [-2, -3]): window_sum = -3 + (-3) - (-1) = -5,
//         keeping max_sum = -3.
//       - Slide to i = 3 (window [-3, -4]): window_sum = -5 + (-4) - (-2) = -7,
//         keeping max_sum = -3 and returning -3.0 / 2.0 = -1.5.
//
//   Example 3:
//     Input:  nums = [4, 2, 1, 3, 3], k = 5
//     Output: 2.6
//     Explanation:
//       - Sum the initial window nums[0..4] = [4, 2, 1, 3, 3] to obtain window_sum
//         = 13 and max_sum = 13.
//       - Because window_len = 5 equals nums.size(), the sliding loop for i in [5,
//         5) runs zero iterations and returns 13.0 / 5.0 = 2.6.
//
// Algorithm:
//   Fixed-size sliding window. Since the divisor `k` is constant for every
//   window, maximizing the average is equivalent to maximizing the sum of a
//   length-k window.
//   1. Compute the sum of the initial window nums[0..k-1] and set `max_sum`
//      to that value.
//   2. Advance the window one element at a time for i in [k, n - 1]: add the
//      entering element `nums[i]` and subtract the leaving element
//      `nums[i - k]`, updating `max_sum` at each step.
//   3. Divide `max_sum` by `k` once at the end to minimize floating-point
//      rounding operations.
//   Time Complexity:  O(n) — each element enters and leaves the window once.
//   Space Complexity: O(1) — only scalar accumulators are used.
//
// Data Structures:
//   - Input `std::vector<int>` accessed via two implicit window endpoints
//     (`i` and `i - k`) with a 64-bit running sum accumulator.
//
// Real-World Applications:
//   - Time-series monitoring and alerting: finding peak k-second moving
//     average CPU load, latency, or network throughput bursts.
//   - Quantitative finance: computing peak simple moving average (SMA)
//     returns across fixed trading windows.
//   - Digital signal processing: boxcar / moving-average filter peak energy
//     detection in audio and sensor streams.
//
// Related LeetCode Problems:
//   - #644 Maximum Average Subarray II [new] (subarray of length >= k via binary search on average)
//   - #209 Minimum Size Subarray Sum [done] (variable-length sliding window)
//   - #239 Sliding Window Maximum [done] (maximum element inside a fixed-size window via monotonic deque)
//   - #1343 Number of Sub-arrays of Size K and Average Greater than or Equal to Threshold [new] (fixed-size sliding window count)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr double kEpsilon = 1e-5;

class Solution {
 public:
  // Finds the maximum average value over all contiguous subarrays of length k
  // using an O(n) fixed-size sliding window.
  //
  // Invariant: At the end of iteration i >= k - 1, `window_sum` equals the
  // sum of nums[i - k + 1 .. i], and `max_sum` holds the maximum window sum
  // observed up through right endpoint i.
  // Time: O(n), Space: O(1).
  double findMaxAverage(const std::vector<int>& nums, int k) {
    if (k <= 0 || nums.size() < static_cast<std::size_t>(k)) {
      return 0.0;
    }

    const auto window_len = static_cast<std::size_t>(k);

    // Phase 1: Sum the initial window [0, window_len).
    std::int64_t window_sum = 0;
    for (std::size_t i = 0uz; i < window_len; ++i) {
      window_sum += nums[i];
    }

    std::int64_t max_sum = window_sum;

    // Phase 2: Slide the window across the remainder of `nums`, adding the
    // incoming right element and removing the outgoing left element.
    for (std::size_t i = window_len; i < nums.size(); ++i) {
      window_sum += static_cast<std::int64_t>(nums[i]) - nums[i - window_len];
      max_sum = std::max(max_sum, window_sum);
    }

    return static_cast<double>(max_sum) / static_cast<double>(k);
  }
};

LC_PROBLEM(643, "Maximum Average Subarray I", "math", ctx) {
  Solution sol;

  auto expect_near = [&](double actual, double expected) {
    ctx.expect_true(std::fabs(actual - expected) <= kEpsilon);
  };

  ctx.test("example_1", [&] {
    expect_near(sol.findMaxAverage({1, 12, -5, -6, 50, 3}, 4), 12.75);
  });

  ctx.test("example_2_single_element", [&] {
    expect_near(sol.findMaxAverage({5}, 1), 5.0);
  });

  ctx.test("all_negative_values", [&] {
    expect_near(sol.findMaxAverage({-1, -2, -3, -4}, 2), -1.5);
  });

  ctx.test("window_equals_array_length", [&] {
    expect_near(sol.findMaxAverage({4, 2, 1, 3, 3}, 5), 2.6);
  });

  ctx.test("window_size_one", [&] {
    expect_near(sol.findMaxAverage({-10, 0, 7, 3}, 1), 7.0);
  });
}

}  // namespace
