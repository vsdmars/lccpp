// LeetCode 367: Valid Perfect Square
// https://leetcode.com/problems/valid-perfect-square/
//
// Problem:
//   Given a positive integer `num` (1 <= num <= 2^31 - 1), return true if `num`
//   is a perfect square (i.e., there exists an integer r such that r * r == num)
//   and false otherwise, without using any built-in square root library
//   function such as `std::sqrt`.
//
// Algorithm:
//   Use integer Newton-Raphson iteration (Heron's method) to compute
//   r = floor(sqrt(num)). Starting from r = num, while r * r > num, update:
//     r = (r + num / r) / 2.
//   By the AM-GM inequality, each step stays >= floor(sqrt(num)) while strictly
//   decreasing whenever r * r > num, so the loop converges quadratically in
//   O(log log num) iterations (bounded by O(log num)) to r = floor(sqrt(num)).
//   Using 64-bit signed integers prevents overflow in `r * r` when `num` is up
//   to 2^31 - 1. Finally, `num` is a perfect square iff `r * r == num`.
//   Time Complexity: O(log num) (quadratically convergent Newton iterations).
//   Space Complexity: O(1).
//
// Data Structures:
//   None; uses two 64-bit scalar integer variables.
//
// Real-World Applications:
//   Arbitrary-precision integer square roots in cryptographic libraries
//   (e.g., quadratic sieve / Pollard's rho factorization), deterministic
//   fixed-point math in embedded systems without an FPU, and exact geometric
//   predicate evaluation.
//
// Related LeetCode Problems:
//   - #69 Sqrt [done] (x) (computes floor(sqrt(x)) with the same Newton/binary-search method)
//   - #319 Bulb Switcher [done] (counts perfect squares up to n)
//   - #633 Sum of Square Numbers [new] (checks whether c = a^2 + b^2 using integer squares)
//   - #279 Perfect Squares [done] (minimum number of perfect squares summing to n)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>

namespace {

class Solution {
 public:
  // Returns true iff `num` is an exact integer square using Newton's method.
  // Time: O(log num), Space: O(1).
  bool isPerfectSquare(int num) {
    if (num < 1) {
      return false;
    }

    // Widen to 64-bit so `r * r` cannot overflow for num <= 2^31 - 1.
    const auto x = static_cast<std::int64_t>(num);
    std::int64_t r = x;

    // Loop invariant: r >= floor(sqrt(x)). Each step strictly decreases r
    // while r * r > x, converging to r == floor(sqrt(x)).
    while ((r * r) > x) {
      r = (r + (x / r)) / 2;
    }

    return (r * r) == x;
  }
};

LC_PROBLEM(367, "Valid Perfect Square", "math", ctx) {
  ctx.test("example_1_perfect_square_16", [&] {
    Solution sol;
    ctx.expect_true(sol.isPerfectSquare(16));
  });

  ctx.test("example_2_non_square_14", [&] {
    Solution sol;
    ctx.expect_false(sol.isPerfectSquare(14));
  });

  ctx.test("small_values", [&] {
    Solution sol;
    ctx.expect_true(sol.isPerfectSquare(1));
    ctx.expect_false(sol.isPerfectSquare(2));
    ctx.expect_false(sol.isPerfectSquare(3));
    ctx.expect_true(sol.isPerfectSquare(4));
    ctx.expect_true(sol.isPerfectSquare(9));
  });

  ctx.test("large_32bit_boundaries", [&] {
    Solution sol;
    // 46340^2 = 2,147,395,600 is the largest perfect square in signed 32-bit int.
    ctx.expect_true(sol.isPerfectSquare(2147395600));
    ctx.expect_false(sol.isPerfectSquare(2147395599));
    ctx.expect_false(sol.isPerfectSquare(std::numeric_limits<int>::max()));
    ctx.expect_true(sol.isPerfectSquare(808201));  // 899^2
  });
}

}  // namespace
