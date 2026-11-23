// LeetCode 739: Daily Temperatures
// https://leetcode.com/problems/daily-temperatures/
//
// Problem:
//   Given an integer array `temperatures` representing daily temperatures,
//   return an array `answer` of the same length where `answer[i]` is the number
//   of days you must wait after day `i` to encounter a strictly warmer
//   temperature. If no future day is strictly warmer, set `answer[i] = 0`.
//
// Algorithm:
//   Traverse `temperatures` from right to left (`n - 1` down to `0`) while
//   maintaining a monotonic stack of future day indices whose temperatures are
//   strictly increasing from top (nearest) to bottom (farthest):
//     1. Pop every index `j` from the stack with `temperatures[i] >= temperatures[j]`,
//        because day `i` is both closer and at least as warm as day `j`, so `j`
//        can never be the next warmer day for any index `< i`.
//     2. If the stack is non-empty, its top index `j` is the earliest future
//        day strictly warmer than day `i`, so `answer[i] = j - i`.
//     3. Push `i` onto the stack.
//   Time Complexity:  O(n), because each day index is pushed once and popped at
//                     most once.
//   Space Complexity: O(n) worst-case stack size (for strictly decreasing input).
//
// Data Structures:
//   - Monotonic index stack (`std::vector<std::size_t>`): stores candidate
//     future warmer day indices in strictly increasing temperature order from
//     top to bottom.
//
// Real-World Applications:
//   - Financial time-series analytics (computing time-to-breakout or days until
//     an asset price exceeds a prior level).
//   - Weather and sensor telemetry dashboards highlighting wait times to peak
//     exceedance events.
//   - Capacity planning and SLA monitoring (measuring time until load or
//     latency surpasses a previous high-water mark).
//
// Related LeetCode Problems:
//   - #496 Next Greater Element I [done] (next greater value via monotonic stack)
//   - #503 Next Greater Element II [done] (circular next greater element)
//   - #901 Online Stock Span [new] (previous greater-or-equal distance)
//   - #84 Largest Rectangle in Histogram [done] (next/prev smaller indices)
//   - #1475 Final Prices With a Special Discount in a Shop [new] (next smaller-or-equal)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // For each day `i`, computes how many days until a strictly warmer day
  // occurs, or `0` if none exists.
  // Time: O(n), Space: O(n).
  std::vector<int> dailyTemperatures(const std::vector<int>& temperatures) {
    const std::size_t n = temperatures.size();
    std::vector<int> result(n, 0);
    if (n == 0uz) {
      return result;
    }

    // Monotonic stack of future day indices with strictly increasing
    // temperatures from top (back) to bottom (front).
    std::vector<std::size_t> stack;
    stack.reserve(n);

    for (std::size_t i = n; i-- > 0uz;) {
      // Pop future days that are not strictly warmer than day `i`; they are
      // dominated by day `i` for all earlier days as well.
      while (!stack.empty() && temperatures[i] >= temperatures[stack.back()]) {
        stack.pop_back();
      }

      if (!stack.empty()) {
        result[i] = static_cast<int>(stack.back() - i);
      }

      stack.push_back(i);
    }

    return result;
  }
};

LC_PROBLEM(739, "Daily Temperatures", "stack", ctx) {
  ctx.test("example_standard", [&] {
    Solution sol;
    ctx.expect_eq(sol.dailyTemperatures({73, 74, 75, 71, 69, 72, 76, 73}),
                  std::vector<int>{1, 1, 4, 2, 1, 1, 0, 0});
  });

  ctx.test("source_build_case", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.dailyTemperatures({89, 62, 70, 58, 47, 47, 46, 76, 100, 70}),
        std::vector<int>{8, 1, 5, 4, 3, 2, 1, 1, 0, 0});
  });

  ctx.test("duplicate_plateau", [&] {
    Solution sol;
    ctx.expect_eq(sol.dailyTemperatures({73, 74, 75, 71, 69, 69, 72, 76, 73}),
                  std::vector<int>{1, 1, 5, 3, 2, 1, 1, 0, 0});
  });

  ctx.test("strictly_increasing_and_decreasing", [&] {
    Solution sol;
    ctx.expect_eq(sol.dailyTemperatures({30, 40, 50, 60}),
                  std::vector<int>{1, 1, 1, 0});
    ctx.expect_eq(sol.dailyTemperatures({90, 80, 70, 60}),
                  std::vector<int>{0, 0, 0, 0});
  });

  ctx.test("all_equal_and_single_day", [&] {
    Solution sol;
    ctx.expect_eq(sol.dailyTemperatures({70, 70, 70}),
                  std::vector<int>{0, 0, 0});
    ctx.expect_eq(sol.dailyTemperatures({50}), std::vector<int>{0});
    ctx.expect_eq(sol.dailyTemperatures({}), std::vector<int>{});
  });
}

}  // namespace
