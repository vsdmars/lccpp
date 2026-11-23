// LeetCode 374: Guess Number Higher Or Lower
// https://leetcode.com/problems/guess-number-higher-or-lower/
//
// Problem:
//   A secret integer `pick` is chosen from the inclusive range [1, n]. You
//   must identify `pick` by calling a pre-defined oracle API `guess(num)` that
//   returns:
//     -1 if `num > pick` (your guess is higher than the picked number),
//      1 if `num < pick` (your guess is lower than the picked number),
//      0 if `num == pick` (exact match).
//
// Examples:
//   Example 1:
//     Input:  n = 10, pick = 6
//     Output: 6
//     Explanation:
//       - Initialize the closed search interval to lo = 1 and hi = 10.
//       - Probe mid = 1 + (10 - 1) / 2 = 5; guess(5) returns 1 (too low), so narrow
//         to lo = 6, hi = 10.
//       - Probe mid = 6 + (10 - 6) / 2 = 8; guess(8) returns -1 (too high), so
//         narrow to lo = 6, hi = 7.
//       - Probe mid = 6 + (7 - 6) / 2 = 6; guess(6) returns 0, returning 6 after 3
//         oracle calls.
//
//   Example 2:
//     Input:  n = 1, pick = 1
//     Output: 1
//     Explanation:
//       - Initialize the single-element interval to lo = 1 and hi = 1.
//       - Probe mid = 1 + (1 - 1) / 2 = 1; guess(1) returns 0, immediately
//         returning 1 in 1 oracle call.
//
//   Example 3:
//     Input:  n = 2147483647, pick = 2147483647
//     Output: 2147483647
//     Explanation:
//       - Initialize lo = 1 and hi = 2147483647 (INT_MAX).
//       - Compute overflow-safe midpoints via lo + (hi - lo) / 2, starting at mid =
//         1073741824 where guess(1073741824) returns 1 and raises lo to 1073741825.
//       - Repeatedly advance lo = mid + 1 until lo = hi = 2147483647, where
//         guess(2147483647) returns 0 and returns 2147483647 in 31 oracle calls.
//
// Algorithm:
//   Standard three-way binary search over the closed interval [lo, hi] = [1, n]:
//     - Compute overflow-safe midpoint `mid = lo + ((hi - lo) / 2)`.
//     - Query `cmp = guess(mid)`:
//         * If `cmp == 0`, return `mid` immediately.
//         * If `cmp > 0`, `pick > mid`, so restrict to [mid + 1, hi].
//         * If `cmp < 0`, `pick < mid`, so restrict to [lo, mid - 1].
//   Each iteration eliminates at least half of the remaining candidates.
//   Time: O(log n) oracle queries, Space: O(1).
//
// Data Structures:
//   Two integer interval endpoints (`lo`, `hi`) and a lightweight mock oracle
//   encapsulated inside `Solution` that counts API queries for testing.
//
// Real-World Applications:
//   - Binary search over black-box monotonic oracles such as automated TCP
//     congestion / MTU path discovery (PMTUD) and bandwidth probing.
//   - Calibration routines in embedded sensors and ADC successive-approximation
//     registers (SAR ADCs) converging on an analog voltage in log2(N) steps.
//   - Hyperparameter / threshold bracketing when only directional feedback
//     (too high / too low) is observable.
//
// Related LeetCode Problems:
//   - #278 First Bad Version [done] (two-way monotonic oracle binary search)
//   - #704 Binary Search [done] (classic three-way binary search on a sorted array)
//   - #375 Guess Number Higher or Lower II [done] (minimax interval DP minimizing worst-case cost)
//   - #1095 Find in Mountain Array [new] (interactive oracle binary search on a bitonic array)

#include "framework/lc_framework.h"

#include <limits>

namespace {

// Models LeetCode's interactive GuessGame API together with
// Solution::guessNumber and records query counts for unit testing.
class Solution {
 public:
  explicit Solution(int pick) : pick_(pick) {}

  // Finds the hidden number in [1, n] using three-way binary search.
  //
  // Invariant: The secret value `pick_` always lies within [lo, hi].
  // Time: O(log n), Space: O(1).
  int guessNumber(int n) {
    int lo = 1;
    int hi = n;

    while (lo <= hi) {
      // Avoid `(lo + hi) / 2` signed integer overflow when n is near INT_MAX.
      const int mid = lo + ((hi - lo) / 2);
      const int cmp = guess(mid);

      if (cmp == 0) {
        return mid;
      }
      if (cmp > 0) {
        // Secret number is strictly higher than mid.
        lo = mid + 1;
      } else {
        // Secret number is strictly lower than mid.
        hi = mid - 1;
      }
    }

    return lo;
  }

  // Returns how many times the mock guess() oracle was called.
  [[nodiscard]] int callCount() const noexcept { return call_count_; }

 private:
  // Simulated judge oracle following LeetCode #374's exact sign convention:
  //  -1 if num > pick_, 1 if num < pick_, 0 if num == pick_.
  int guess(int num) {
    ++call_count_;
    if (num > pick_) {
      return -1;
    }
    if (num < pick_) {
      return 1;
    }
    return 0;
  }

  int pick_ = 1;
  int call_count_ = 0;
};

LC_PROBLEM(374, "Guess Number Higher Or Lower", "binary_search", ctx) {
  ctx.test("docstring_and_python_examples", [&] {
    Solution sol6(6);
    ctx.expect_eq(sol6.guessNumber(10), 6);
    ctx.expect_true(sol6.callCount() <= 4);

    Solution sol7(7);
    ctx.expect_eq(sol7.guessNumber(10), 7);
    ctx.expect_true(sol7.callCount() <= 4);
  });

  ctx.test("small_ranges_and_endpoints", [&] {
    Solution sol_one(1);
    ctx.expect_eq(sol_one.guessNumber(1), 1);
    ctx.expect_eq(sol_one.callCount(), 1);

    Solution sol_low(1);
    ctx.expect_eq(sol_low.guessNumber(2), 1);

    Solution sol_high(2);
    ctx.expect_eq(sol_high.guessNumber(2), 2);
  });

  ctx.test("int_max_no_overflow", [&] {
    constexpr int kMaxInt = std::numeric_limits<int>::max();

    Solution sol_max(kMaxInt);
    ctx.expect_eq(sol_max.guessNumber(kMaxInt), kMaxInt);
    ctx.expect_true(sol_max.callCount() <= 32);

    Solution sol_arbitrary(1'702'766'719);
    ctx.expect_eq(sol_arbitrary.guessNumber(2'126'753'390), 1'702'766'719);
    ctx.expect_true(sol_arbitrary.callCount() <= 32);
  });
}

}  // namespace
