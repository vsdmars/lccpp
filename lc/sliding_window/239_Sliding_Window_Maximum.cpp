// LeetCode 239: Sliding Window Maximum
// https://leetcode.com/problems/sliding-window-maximum/
//
// Problem:
//   Given an integer array nums (1 <= nums.length <= 10^5) and a sliding
//   window size k (1 <= k <= nums.length), the window moves from left to right
//   one position at a time. Return an array containing the maximum element in
//   each of the (nums.length - k + 1) windows.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 3, -1, -3, 5, 3, 6, 7], k = 3
//     Output: [3, 3, 5, 5, 6, 7]
//     Explanation:
//       - Process indices 0..2: 3 pops 0 from the back and 2 is pushed, leaving dq
//         = [1, 2] and emitting nums[1] = 3 for [1, 3, -1].
//       - At i = 3 (-3), dq becomes [1, 2, 3]; index 1 is still inside [1..3], so
//         emit nums[1] = 3.
//       - At i = 4 (5), 5 pops indices 3, 2, and 1 from the back so dq = [4] and
//         emits 5; at i = 5 (3), dq = [4, 5] and emits nums[4] = 5.
//       - At i = 6 (6) and i = 7 (7), each new maximum clears the deque and emits
//         nums[6] = 6 and nums[7] = 7, yielding [3, 3, 5, 5, 6, 7].
//
//   Example 2:
//     Input:  nums = [1, 3, 1, 2, 0, 5], k = 3
//     Output: [3, 3, 2, 5]
//     Explanation:
//       - After indices 0..2, dq = [1, 2] (values 3, 1) and the first window emits
//         nums[1] = 3.
//       - At i = 3 (2), 2 pops index 2 (1 <= 2) so dq = [1, 3] and emits nums[1] =
//         3.
//       - At i = 4 (0), 4 is pushed and expired front index 1 (1 <= 4 - 3) is
//         popped, leaving dq = [3, 4] and emitting nums[3] = 2.
//       - At i = 5 (5), 5 pops indices 4 and 3 so dq = [5] and emits 5, producing
//         [3, 3, 2, 5].
//
//   Example 3:
//     Input:  nums = [2, 2, 2, 2], k = 2
//     Output: [2, 2, 2]
//     Explanation:
//       - At i = 0, push index 0 so dq = [0].
//       - At i = 1, nums[0] <= nums[1] (2 <= 2) pops index 0 and pushes index 1,
//         keeping dq = [1] and emitting nums[1] = 2.
//       - At i = 2 and i = 3, each incoming 2 similarly replaces the sole back
//         index and emits 2, yielding [2, 2, 2].
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
