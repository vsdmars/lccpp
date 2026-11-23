// LeetCode 738: Monotone Increasing Digits
// https://leetcode.com/problems/monotone-increasing-digits/
//
// Problem:
//   Given a non-negative integer n (0 <= n <= 10^9), return the largest
//   integer less than or equal to n whose base-10 digits are monotone
//   increasing (every pair of adjacent digits satisfies x <= y).
//
// Algorithm:
//   Convert n into its decimal digit string and scan from right to left to
//   find any inversion where digits[i - 1] > digits[i]. Whenever an inversion
//   is detected, decrement digits[i - 1] by 1 and record i as the new start
//   position for trailing '9's. Scanning right-to-left ensures that if
//   decrementing digits[i - 1] breaks monotonicity with an earlier equal digit
//   (e.g., 332 -> 322 -> 222), the borrow cascades automatically. Finally, set
//   every digit from the recorded cutoff index to the end to '9' to maximize
//   the value while staying <= n, and parse the resulting string back to int.
//   Time Complexity: O(d), where d <= 10 is the number of decimal digits in n.
//   Space Complexity: O(d) to store the decimal digit representation.
//
// Data Structures:
//   std::string: mutable base-10 digit array enabling O(1) indexed access,
//   in-place right-to-left borrow propagation, and suffix filling with '9'.
//
// Real-World Applications:
//   - Numeric bound tightening and lexicographic ceiling/floor search in
//     constraint solvers and range-partitioned key-value stores.
//   - Generating maximum valid monotonic sequence numbers, version codes, or
//     ordered identifiers under an upper-bound budget.
//
// Related LeetCode Problems:
//   - #402 Remove K Digits [done] (greedy monotonic digit construction)
//   - #316 Remove Duplicate Letters [done] (greedy lexicographic monotonic stack)
//   - #556 Next Greater Element III [done] (digit manipulation on base-10 string)
//   - #1323 Maximum 69 Number [new] (single-pass greedy digit optimization)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>

namespace {

class Solution {
 public:
  // Returns the largest integer <= n whose base-10 digits are non-decreasing.
  // Time: O(d), Space: O(d), where d is the number of decimal digits in n.
  int monotoneIncreasingDigits(int n) {
    // Single-digit numbers (0..9) are already trivially monotone increasing.
    if (n < 10) {
      return n;
    }

    std::string digits = std::to_string(n);
    // Tracks the leftmost index from which all subsequent digits become '9'.
    std::size_t fill_nine_from = digits.size();

    // Walk right-to-left so any borrow from a cliff propagates cleanly across
    // runs of identical digits (e.g., "332" -> "322" -> "222", cutoff = 1).
    for (std::size_t i = digits.size() - 1; i > 0uz; --i) {
      if (digits[i - 1] > digits[i]) {
        --digits[i - 1];
        fill_nine_from = i;
      }
    }

    // Maximizing every position strictly after the last decremented digit with
    // '9' yields the largest monotone number <= n.
    for (std::size_t i = fill_nine_from; i < digits.size(); ++i) {
      digits[i] = '9';
    }

    // std::stoi automatically drops any leading '0' (e.g., "099" -> 99).
    return std::stoi(digits);
  }
};

LC_PROBLEM(738, "Monotone Increasing Digits", "greedy", ctx) {
  Solution sol;

  ctx.test("example_1_ten", [&] {
    ctx.expect_eq(sol.monotoneIncreasingDigits(10), 9);
  });

  ctx.test("example_2_already_monotone", [&] {
    ctx.expect_eq(sol.monotoneIncreasingDigits(1234), 1234);
  });

  ctx.test("example_3_cascading_borrow", [&] {
    ctx.expect_eq(sol.monotoneIncreasingDigits(332), 299);
  });

  ctx.test("python_build_cases", [&] {
    ctx.expect_eq(sol.monotoneIncreasingDigits(1287), 1279);
    ctx.expect_eq(sol.monotoneIncreasingDigits(1988), 1899);
  });

  ctx.test("edge_zero_and_single_digit", [&] {
    ctx.expect_eq(sol.monotoneIncreasingDigits(0), 0);
    ctx.expect_eq(sol.monotoneIncreasingDigits(7), 7);
    ctx.expect_eq(sol.monotoneIncreasingDigits(9), 9);
  });

  ctx.test("edge_powers_of_ten_and_max_bound", [&] {
    ctx.expect_eq(sol.monotoneIncreasingDigits(100), 99);
    ctx.expect_eq(sol.monotoneIncreasingDigits(1000000000), 999999999);
    ctx.expect_eq(sol.monotoneIncreasingDigits(999999998), 899999999);
  });
}

}  // namespace
