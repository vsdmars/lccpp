// LeetCode 69: Sqrtx
// https://leetcode.com/problems/sqrtx/
//
// Problem:
//   Given a non-negative 32-bit integer `x`, compute and return the integer
//   square root `floor(sqrt(x))` (the decimal part is truncated) without using
//   any built-in exponentiation or square-root functions.
//
// Algorithm:
//   Integer Newton-Raphson (Heron's / Babylonian) method for finding the
//   positive root of `f(r) = r^2 - x = 0`. Starting from an upper bound
//   `r = x` in 64-bit arithmetic (to prevent overflow when computing `r * r`
//   or `r + x / r` for `x` up to `INT_MAX`), repeatedly update
//   `r = (r + x / r) / 2` while `r * r > x`. Because `f(r)` is strictly convex
//   for `r > 0`, the integer Newton iterates decrease monotonically and
//   converge quadratically to `floor(sqrt(x))` in O(log log x) steps.
//   Time Complexity: O(log x) worst-case (quadratically fast, <= 17 iterations
//     for any 32-bit signed integer).
//   Space Complexity: O(1).
//
// Data Structures:
//   A single 64-bit signed integer (`std::int64_t`) holding the current Newton
//   estimate `r` so `r * r` cannot overflow 32-bit `int`.
//
// Real-World Applications:
//   - Fixed-point and integer math kernels in graphics, DSP, and embedded
//     firmware without floating-point units.
//   - Prime factorization bounds (`limit = isqrt(n)`), cryptographic big-int
//     square roots, and geometric distance approximations.
//   - Root-finding solvers across physics engines and numerical optimization.
//
// Related LeetCode Problems:
//   - #367 Valid Perfect Square [done] (exact integer square root check via Newton or binary search)
//   - #50 Powx N [done] (logarithmic-time numerical exponentiation)
//   - #29 Divide Two Integers [done] (overflow-safe integer arithmetic)
//   - #633 Sum of Square Numbers [new] (two-pointer search bounded by isqrt(c))

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>

namespace {

class Solution {
 public:
  // Computes `floor(sqrt(x))` for a non-negative 32-bit integer `x` using
  // integer Newton's method. Uses 64-bit intermediates so `r * r` never
  // overflows for `x` up to `INT_MAX`.
  // Time: O(log x) (quadratically converging), Space: O(1).
  int mySqrt(int x) {
    if (x <= 1) {
      return x;
    }

    const std::int64_t target = x;
    std::int64_t r = target;

    // Invariant: `r >= floor(sqrt(target))` throughout the loop. Each Newton
    // step strictly decreases `r` until `r * r <= target`, at which point
    // `r == floor(sqrt(target))`.
    while ((r * r) > target) {
      r = (r + (target / r)) / 2;
    }

    return static_cast<int>(r);
  }
};

LC_PROBLEM(69, "Sqrtx", "math", ctx) {
  Solution sol;

  ctx.test("examples", [&] {
    ctx.expect_eq(sol.mySqrt(4), 2);
    ctx.expect_eq(sol.mySqrt(8), 2);
    ctx.expect_eq(sol.mySqrt(9), 3);
  });

  ctx.test("small_values", [&] {
    ctx.expect_eq(sol.mySqrt(0), 0);
    ctx.expect_eq(sol.mySqrt(1), 1);
    ctx.expect_eq(sol.mySqrt(2), 1);
    ctx.expect_eq(sol.mySqrt(3), 1);
    ctx.expect_eq(sol.mySqrt(15), 3);
    ctx.expect_eq(sol.mySqrt(16), 4);
  });

  ctx.test("int32_boundaries", [&] {
    // 46340^2 = 2147395600 <= INT_MAX < 46341^2 = 2147488281.
    ctx.expect_eq(sol.mySqrt(2147395599), 46339);
    ctx.expect_eq(sol.mySqrt(2147395600), 46340);
    ctx.expect_eq(sol.mySqrt(std::numeric_limits<int>::max()), 46340);
  });
}

}  // namespace
