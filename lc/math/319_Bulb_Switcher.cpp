// LeetCode 319: Bulb Switcher
// https://leetcode.com/problems/bulb-switcher/
//
// Problem:
//   There are `n` bulbs (0 <= n <= 10^9) initially turned off. In round i
//   (for i = 1, 2, ..., n), you toggle every i-th bulb (turning off bulbs on
//   and on bulbs off). Return the number of bulbs that remain on after all n
//   rounds.
//
// Algorithm:
//   Label the bulbs 1 through n. Bulb k is toggled in round d if and only if d
//   divides k, so the final state of bulb k is ON if and only if k has an odd
//   number of positive divisors.
//   Divisors of k pair up as (d, k / d) with d != k / d unless d * d == k.
//   Therefore, k has an odd number of divisors if and only if k is a perfect
//   square. The count of perfect squares in [1, n] is floor(sqrt(n)).
//   Because IEEE-754 `double` has a 53-bit significand and `std::sqrt` is
//   correctly rounded, `static_cast<int>(std::sqrt(static_cast<double>(n)))`
//   is exact for all 32-bit signed integers n >= 0.
//   Time Complexity: O(1).
//   Space Complexity: O(1).
//
// Data Structures:
//   None; reduces the entire simulation to a closed-form scalar calculation.
//
// Real-World Applications:
//   Divisor-parity reasoning in number-theoretic cryptography, analytical
//   reduction of periodic toggle/sieve schedules, and avoiding O(n log n)
//   simulations when periodic state flips admit a closed-form invariant.
//
// Related LeetCode Problems:
//   - #367 Valid Perfect Square [done] (testing whether an integer is a perfect square)
//   - #672 Bulb Switcher II [done] (state-space analysis of constrained bulb toggle operations)
//   - #1375 Number of Times Binary String Is Prefix-Aligned [new] (bulb-switching prefix state invariant)
//   - #69 Sqrt [done] (x) (computing integer floor square root directly)

#include "framework/lc_framework.h"

#include <cmath>

namespace {

class Solution {
 public:
  // Returns the number of bulbs remaining on after n toggle rounds.
  // Only bulbs at perfect-square 1-based indices are toggled an odd number of
  // times, so the answer is floor(sqrt(n)).
  // Time: O(1), Space: O(1).
  int bulbSwitch(int n) {
    if (n <= 0) {
      return 0;
    }

    // Count perfect squares 1^2, 2^2, ..., k^2 <= n.
    return static_cast<int>(std::sqrt(static_cast<double>(n)));
  }
};

LC_PROBLEM(319, "Bulb Switcher", "math", ctx) {
  ctx.test("example_1_n_3", [&] {
    Solution sol;
    ctx.expect_eq(sol.bulbSwitch(3), 1);
  });

  ctx.test("example_2_n_0", [&] {
    Solution sol;
    ctx.expect_eq(sol.bulbSwitch(0), 0);
  });

  ctx.test("example_3_n_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.bulbSwitch(1), 1);
  });

  ctx.test("square_boundaries", [&] {
    Solution sol;
    ctx.expect_eq(sol.bulbSwitch(4), 2);
    ctx.expect_eq(sol.bulbSwitch(8), 2);
    ctx.expect_eq(sol.bulbSwitch(9), 3);
    ctx.expect_eq(sol.bulbSwitch(15), 3);
    ctx.expect_eq(sol.bulbSwitch(16), 4);
  });

  ctx.test("large_n", [&] {
    Solution sol;
    ctx.expect_eq(sol.bulbSwitch(99999999), 9999);
    // 31622^2 = 999,950,884 <= 10^9 < 31623^2 = 1,000,014,129.
    ctx.expect_eq(sol.bulbSwitch(1000000000), 31622);
  });
}

}  // namespace
