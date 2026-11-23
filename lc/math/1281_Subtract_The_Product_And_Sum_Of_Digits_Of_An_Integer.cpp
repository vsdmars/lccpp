// LeetCode 1281: Subtract The Product And Sum Of Digits Of An Integer
// https://leetcode.com/problems/subtract-the-product-and-sum-of-digits-of-an-integer/
//
// Problem:
//   Given a positive integer `n`, return the difference between the product of
//   its base-10 digits and the sum of its base-10 digits:
//     `(d_0 * d_1 * ... * d_{k-1}) - (d_0 + d_1 + ... + d_{k-1})`.
//   Constraints: 1 <= n <= 10^5.
//
// Algorithm:
//   Repeatedly peel off the least significant decimal digit using `n % 10` and
//   advance `n /= 10` until `n` reaches `0`, updating a running product
//   (initialized to `1`) and a running sum (initialized to `0`) in a single
//   pass. Because `n <= 10^5`, the maximum possible product is `9^5 = 59,049`
//   and the maximum sum is `45`, both well within 32-bit signed `int` range.
//   An explicit guard for `n == 0` ensures both product and sum are treated as
//   `0` rather than returning `1`.
//   Time Complexity:  O(log_10(n)), proportional to the number of decimal digits.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Scalar integer accumulators (`product` and `sum`): track multiplicative
//     and additive folds over the decimal digits without allocating strings or
//     arrays.
//
// Real-World Applications:
//   - Checksum and identifier validation (e.g., Luhn, Verhoeff, ISBN, IBAN):
//     streaming base-10 digit extraction and arithmetic folding without string
//     allocations.
//   - Embedded systems and telemetry encoding: decomposing numeric register
//     values into BCD (Binary-Coded Decimal) digits for seven-segment displays
//     or compact protocols.
//
// Related LeetCode Problems:
//   - #258 Add Digits [done] (repeatedly sums base-10 digits / digital root)
//   - #202 Happy Number [done] (extracts base-10 digits iteratively to sum their squares)
//   - #9 Palindrome Number [done] (base-10 digit peeling via modulo and integer division)
//   - #2535 Difference Between Element Sum and Digit Sum of an Array [new] (aggregates digit sums across an array)
//   - #1295 Find Numbers With Even Number Of Digits [done] (counting decimal digits via repeated division by 10)

#include "framework/lc_framework.h"

namespace {

class Solution {
 public:
  // Computes (product of decimal digits of `n`) - (sum of decimal digits of `n`).
  // Time: O(log_10(n)), Space: O(1).
  int subtractProductAndSum(int n) {
    // Guard against n == 0, where a naive product=1 initializer would return 1.
    if (n == 0) {
      return 0;
    }

    int product = 1;
    int sum = 0;

    // Peel digits from least significant to most significant in base 10.
    while (n > 0) {
      const int digit = n % 10;
      product *= digit;
      sum += digit;
      n /= 10;
    }

    return product - sum;
  }
};

LC_PROBLEM(1281, "Subtract The Product And Sum Of Digits Of An Integer", "math",
           ctx) {
  Solution sol;

  ctx.test("example_1_234", [&] {
    // Product = 2 * 3 * 4 = 24, Sum = 2 + 3 + 4 = 9 -> 15.
    ctx.expect_eq(sol.subtractProductAndSum(234), 15);
  });

  ctx.test("example_2_4421", [&] {
    // Product = 4 * 4 * 2 * 1 = 32, Sum = 4 + 4 + 2 + 1 = 11 -> 21.
    ctx.expect_eq(sol.subtractProductAndSum(4421), 21);
  });

  ctx.test("contains_zero_digit", [&] {
    // Any zero digit zeroes the entire product: 0 - (1 + 0 + 5) = -6.
    ctx.expect_eq(sol.subtractProductAndSum(105), -6);
    ctx.expect_eq(sol.subtractProductAndSum(100000), -1);
  });

  ctx.test("single_digit_and_bounds", [&] {
    ctx.expect_eq(sol.subtractProductAndSum(1), 0);
    ctx.expect_eq(sol.subtractProductAndSum(9), 0);
    ctx.expect_eq(sol.subtractProductAndSum(0), 0);
    // Upper five-9s case: 9^5 - 45 = 59049 - 45 = 59004.
    ctx.expect_eq(sol.subtractProductAndSum(99999), 59004);
  });
}

}  // namespace
