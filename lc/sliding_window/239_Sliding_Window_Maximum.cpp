// LeetCode 239: Sliding Window Maximum
// https://leetcode.com/problems/sliding-window-maximum/
//
// Problem:
//   Given an integer array nums (1 <= nums.length <= 10^5) and a sliding
//   window size k (1 <= k <= nums.length), the window moves from left to right
//   one position at a time. Return an array containing the maximum element in
//   each of the (nums.length - k + 1) windows.
//
// Algorithm:
//   Maintain a monotonic decreasing deque of array indices. For each index i:
//   1. Pop indices from the back while their values are <= nums[i], because
//      those earlier, smaller-or-equal elements can never be the maximum of
//      the current window or any future window.
//   2. Push i onto the back of the deque.
//   3. If the front index has fallen out of the current window (index <= i - k),
//      pop it from the front.
//   4. Once the first full window is formed (i + 1 >= k), nums[dq.front()] is
//      the maximum for the window ending at i.
//   Each index enters and leaves the deque at most once, yielding linear time.
//   Time Complexity: O(n), where n = nums.size().
//   Space Complexity: O(k) auxiliary space for the deque (excluding output).
//
// Data Structures:
//   std::deque<std::size_t>: double-ended monotonic queue storing candidate
//   indices in increasing order of index and strictly decreasing order of value.
//
// Real-World Applications:
//   - Real-time telemetry and monitoring: tracking rolling peak CPU, latency,
//     or network bandwidth over a fixed trailing time window in O(1) amortized
//     time per sample.
//   - Financial tick analytics (rolling high/low bands) and 1D morphological
//     dilation filters in image/signal processing.
//
// Related LeetCode Problems:
//   - #480 Sliding Window Median [done] (sliding window order statistic)
//   - #1438 Longest Continuous Subarray With Absolute Diff <= Limit [done] (two monotonic deques)
//   - #862 Shortest Subarray with Sum at Least K [done] (monotonic deque on prefix sums)
//   - #739 Daily Temperatures [done] (monotonic stack)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum value for every contiguous window of length k in nums.
  // Time: O(n), Space: O(k) auxiliary.
  std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
    if (nums.empty() || k <= 0) {
      return {};
    }

    const auto win = static_cast<std::size_t>(k);
    if (win > nums.size()) {
      return {};
    }

    std::vector<int> result;
    result.reserve((nums.size() - win) + 1uz);

    // Monotonic deque of indices: values strictly decrease from front to back,
    // so dq.front() is always the index of the current window's maximum.
    std::deque<std::size_t> dq;

    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      // Discard dominated elements that are both older and <= nums[i].
      while (!dq.empty() && nums[dq.back()] <= nums[i]) {
        dq.pop_back();
      }
      dq.push_back(i);

      // Evict the front index once it slides strictly outside [i - win + 1, i].
      if (i >= win && dq.front() <= i - win) {
        dq.pop_front();
      }

      // Emit the window maximum starting once the first k elements are seen.
      if (i + 1uz >= win) {
        result.push_back(nums[dq.front()]);
      }
    }

    return result;
  }
};

LC_PROBLEM(239, "Sliding Window Maximum", "sliding_window", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    const std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    const std::vector<int> expected = {3, 3, 5, 5, 6, 7};
    ctx.expect_eq(sol.maxSlidingWindow(nums, 3), expected);
  });

  ctx.test("python_build_case", [&] {
    const std::vector<int> nums = {1, 3, 1, 2, 0, 5};
    const std::vector<int> expected = {3, 3, 2, 5};
    ctx.expect_eq(sol.maxSlidingWindow(nums, 3), expected);
  });

  ctx.test("window_size_one_and_full", [&] {
    ctx.expect_eq(sol.maxSlidingWindow({1}, 1), std::vector<int>{1});
    ctx.expect_eq(sol.maxSlidingWindow({4, -2, 7, 1}, 1),
                  std::vector<int>({4, -2, 7, 1}));
    ctx.expect_eq(sol.maxSlidingWindow({4, -2, 7, 1}, 4), std::vector<int>{7});
  });

  ctx.test("strictly_decreasing_and_duplicates", [&] {
    ctx.expect_eq(sol.maxSlidingWindow({5, 4, 3, 2, 1}, 2),
                  std::vector<int>({5, 4, 3, 2}));
    ctx.expect_eq(sol.maxSlidingWindow({2, 2, 2, 2}, 2),
                  std::vector<int>({2, 2, 2}));
    ctx.expect_eq(sol.maxSlidingWindow({9, 10, 9, -7, -4, -8, 2, -6}, 5),
                  std::vector<int>({10, 10, 9, 2}));
  });
}

}  // namespace
