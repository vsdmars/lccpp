// LeetCode 2544: Alternating Digit Sum
// https://leetcode.com/problems/alternating-digit-sum/
//
// Problem:
//   Given a positive integer `n` (1 <= n <= 10^9), assign signs to its base-10
//   digits from left to right such that the most significant digit has a
//   positive (`+`) sign and every subsequent digit has the opposite sign of
//   its left neighbor (`+d_0 - d_1 + d_2 - ...`). Return the resulting signed
//   sum.
//
// Examples:
//   Example 1:
//     Input:  n = 521
//     Output: 4
//     Explanation:
//       - Convert n = 521 to string "521" and initialize parity buckets sums = {0,
//         0} with bucket = 0.
//       - Process '5' into sums[0] = 5 (bucket -> 1), '2' into sums[1] = 2 (bucket
//         -> 0), and '1' into sums[0] = 6 (bucket -> 1).
//       - Subtract the buckets to return sums[0] - sums[1] = 6 - 2 = 4.
//
//   Example 2:
//     Input:  n = 886996
//     Output: 0
//     Explanation:
//       - Convert n = 886996 to string "886996" and initialize sums = {0, 0} and
//         bucket = 0.
//       - Toggling bucket after each character puts even-position digits ('8', '6',
//         '9') into sums[0] = 8 + 6 + 9 = 23.
//       - Odd-position digits ('8', '9', '6') accumulate into sums[1] = 8 + 9 + 6 =
//         23.
//       - Return sums[0] - sums[1] = 23 - 23 = 0.
//
//   Example 3:
//     Input:  n = 19
//     Output: -8
//     Explanation:
//       - Convert n = 19 to string "19" with sums = {0, 0} and bucket = 0.
//       - Add '1' to sums[0] = 1, toggle bucket to 1, then add '9' to sums[1] = 9.
//       - Return sums[0] - sums[1] = 1 - 9 = -8.
//
// Algorithm:
//   Primary approach (mirrors the Python two-bucket parity accumulator):
//     Convert `n` to its decimal string representation so digits are visited
//     from most significant to least significant. Maintain a 2-element bucket
//     array `sums` (`sums[0]` for even/positive positions, `sums[1]` for
//     odd/negative positions) and flip the active bucket with `bucket ^= 1uz`
//     after each digit. Finally return `sums[0] - sums[1]`.
//   Time Complexity:  O(d) where d = floor(log10(n)) + 1 (at most 10 digits).
//   Space Complexity: O(d) for the decimal string.
//
//   Alternative O(1)-space arithmetic approach (`alternateDigitSumConstantSpace`):
//     Extract digits right-to-left via `n % 10`, alternating `sign` at each
//     step. When the loop ends, `sign` holds the sign that was applied to the
//     most significant digit; multiplying `sum * sign` aligns the MSD to `+1`
//     without needing to know the digit count beforehand.
//   Time Complexity:  O(d).
//   Space Complexity: O(1).
//
// Data Structures:
//   - `std::string` from `std::to_string(n)` for MSB-to-LSB digit iteration.
//   - `std::array<int, 2>` storing the accumulated positive-position and
//     negative-position digit sums.
//
// Real-World Applications:
//   - Divisibility-by-11 checks and checksum validation (e.g., ISBN-10,
//     routing numbers, and barcode check-digit verification use alternating
//     weighted digit sums).
//   - Streaming parity-weighted telemetry aggregation where signal polarity
//     alternates per sample.
//
// Related LeetCode Problems:
//   - #1281 Subtract the Product and Sum of Digits of an Integer [done] (aggregates base-10 digits in a single scan)
//   - #258 Add Digits [done] (digit-sum reduction and modular arithmetic properties)
//   - #2180 Count Integers With Even Digit Sum [new] (digit-sum parity over positive integers)
//   - #9 Palindrome Number [done] (digit extraction and positional processing in O(1) space)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>

namespace {

class Solution {
 public:
  // Computes the alternating digit sum by accumulating even-indexed (positive)
  // and odd-indexed (negative) digits into two parity buckets.
  //
  // Invariant:
  //   Before processing index `i`, `bucket == (i % 2)`, `sums[0]` holds the
  //   sum of all even-indexed digits seen so far, and `sums[1]` holds the sum
  //   of all odd-indexed digits seen so far.
  //
  // Time:  O(log10(n))
  // Space: O(log10(n))
  int alternateDigitSum(int n) {
    // Two parity buckets: sums[0] collects `+` positions, sums[1] collects `-`.
    std::array<int, 2> sums{0, 0};
    std::size_t bucket = 0uz;

    const std::string digits = std::to_string(n);
    for (const char ch : digits) {
      sums[bucket] += (ch - '0');
      // Toggle between bucket 0 and bucket 1 (C++ equivalent of Python ~flip
      // indexing into a 2-element list).
      bucket ^= 1uz;
    }

    return sums[0] - sums[1];
  }

  // O(1)-space arithmetic alternative using right-to-left digit extraction.
  //
  // Invariant:
  //   Each extracted digit receives the opposite sign of the previous digit.
  //   When `n` reaches 0, `sign` equals the sign applied to the most
  //   significant digit; multiplying `sum * sign` guarantees the MSD is `+`.
  //
  // Time:  O(log10(n))
  // Space: O(1)
  int alternateDigitSumConstantSpace(int n) {
    int sum = 0;
    int sign = 1;

    while (n > 0) {
      sign = -sign;
      sum += (sign * (n % 10));
      n /= 10;
    }

    // If the MSD ended up with -1, negate the entire sum to make MSD positive.
    return sum * sign;
  }
};

LC_PROBLEM(2544, "Alternating Digit Sum", "math", ctx) {
  Solution sol;

  ctx.test("example1_python_build", [&] {
    // (+5) + (-2) + (+1) = 4.
    ctx.expect_eq(sol.alternateDigitSum(521), 4);
    ctx.expect_eq(sol.alternateDigitSumConstantSpace(521), 4);
  });

  ctx.test("example2_all_ones_odd_length", [&] {
    // (+1) + (-1) + (+1) = 1.
    ctx.expect_eq(sol.alternateDigitSum(111), 1);
    ctx.expect_eq(sol.alternateDigitSumConstantSpace(111), 1);
  });

  ctx.test("example3_even_length_cancels_to_zero", [&] {
    // (+8) + (-8) + (+6) + (-9) + (+9) + (-6) = 0.
    ctx.expect_eq(sol.alternateDigitSum(886996), 0);
    ctx.expect_eq(sol.alternateDigitSumConstantSpace(886996), 0);
  });

  ctx.test("single_digit", [&] {
    ctx.expect_eq(sol.alternateDigitSum(9), 9);
    ctx.expect_eq(sol.alternateDigitSumConstantSpace(9), 9);
  });

  ctx.test("negative_result", [&] {
    // (+1) + (-9) = -8.
    ctx.expect_eq(sol.alternateDigitSum(19), -8);
    ctx.expect_eq(sol.alternateDigitSumConstantSpace(19), -8);
  });

  ctx.test("max_constraint_power_of_ten", [&] {
    // (+1) - 0 + 0 - ... - 0 = 1.
    ctx.expect_eq(sol.alternateDigitSum(1000000000), 1);
    ctx.expect_eq(sol.alternateDigitSumConstantSpace(1000000000), 1);
  });
}

}  // namespace
