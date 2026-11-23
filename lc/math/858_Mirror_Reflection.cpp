// LeetCode 858: Mirror Reflection
// https://leetcode.com/problems/mirror-reflection/
//
// Problem:
//   A square room of side length `p` (1 <= p <= 1000) has mirrors on all four
//   walls and receptors at three corners:
//     - Receptor 0: southeast corner (p, 0)
//     - Receptor 1: northeast corner (p, p)
//     - Receptor 2: northwest corner (0, p)
//   A laser ray starts from the southwest corner (0, 0) and first meets the
//   east wall at distance `q` (0 <= q <= p) from receptor 0 (i.e., at (p, q)).
//   Return the number of the receptor (0, 1, or 2) that the ray meets first.
//
// Algorithm:
//   Unfold the mirror room into an infinite grid of p x p cells so the laser
//   travels in a straight line y = (q / p) * x without bouncing:
//   - The ray first hits a grid corner (n * p, m * p) at the smallest positive
//     integer `n` and non-negative integer `m` satisfying m * p = n * q.
//   - Dividing out g = gcd(p, q) yields the exact minimal solution directly:
//       n = p / g  (number of horizontal room traversals)
//       m = q / g  (number of vertical room extensions)
//     (Note: when q == 0, gcd(p, 0) = p gives n = 1, m = 0, correctly returning
//      receptor 0 as the ray goes straight along the south wall.)
//   - Because gcd(n, m) = 1, n and m cannot both be even. Parity determines the
//     exact corner after folding back into the original room:
//       * n even, m odd  -> left wall, top corner  -> receptor 2
//       * n odd,  m even -> right wall, bottom     -> receptor 0
//       * n odd,  m odd  -> right wall, top corner -> receptor 1
//   Time: O(log(min(p, q))), Space: O(1).
//
// Data Structures:
//   None (scalar arithmetic with `std::gcd`).
//
// Real-World Applications:
//   - Optical cavity ray-tracing and billiard trajectory analysis via unfolded
//     fundamental domains (flat-surface unfolding).
//   - Phase-locked loop and discrete-time aliasing analysis where rational
//     frequency ratios determine boundary collisions.
//
// Related LeetCode Problems:
//   - #1071 Greatest Common Divisor of Strings [new] (GCD reduction for periodic alignment)
//   - #365 Water and Jug Problem [new] (GCD invariants in discrete state reachability)
//   - #149 Max Points on a Line [done] (reducing rational slopes by GCD)
//   - #2001 Number of Pairs of Interchangeable Rectangles [new] (canonical aspect ratios via GCD)

#include "framework/lc_framework.h"

#include <numeric>

namespace {

class Solution {
 public:
  // Returns the first receptor (0, 1, or 2) hit by a laser fired from (0, 0)
  // through (p, q) inside a p x p mirrored square room.
  //
  // Invariant: In the unfolded grid, the first lattice corner hit is
  // (n * p, m * p) with n = p / gcd(p, q) and m = q / gcd(p, q); since
  // gcd(n, m) == 1, at least one of n, m is odd.
  //
  // Time: O(log(min(p, q))), Space: O(1).
  int mirrorReflection(int p, int q) {
    // Reduce the slope q / p to lowest terms so m * p == n * q holds with
    // minimal positive n = p / g and m = q / g (including q == 0 -> n = 1, m = 0).
    const int g = std::gcd(p, q);
    const int n = p / g;  // Horizontal room count (odd = east wall, even = west wall).
    const int m = q / g;  // Vertical room count (odd = north wall, even = south wall).

    // Even horizontal traversals land back on the west wall (only receptor 2).
    if (n % 2 == 0) {
      return 2;
    }
    // Odd horizontal traversals land on the east wall: even vertical extensions
    // land at the south corner (receptor 0), odd at the north corner (receptor 1).
    if (m % 2 == 0) {
      return 0;
    }
    return 1;
  }
};

LC_PROBLEM(858, "Mirror Reflection", "math", ctx) {
  Solution sol;

  ctx.test("example_1_p2_q1",
           [&] { ctx.expect_eq(sol.mirrorReflection(2, 1), 2); });

  ctx.test("example_2_p3_q1",
           [&] { ctx.expect_eq(sol.mirrorReflection(3, 1), 1); });

  ctx.test("receptor_0_even_vertical_odd_horizontal", [&] {
    ctx.expect_eq(sol.mirrorReflection(3, 2), 0);
    ctx.expect_eq(sol.mirrorReflection(6, 4), 0);
  });

  ctx.test("zero_height_hits_receptor_0_directly", [&] {
    ctx.expect_eq(sol.mirrorReflection(1, 0), 0);
    ctx.expect_eq(sol.mirrorReflection(1000, 0), 0);
  });

  ctx.test("diagonal_hits_receptor_1_directly", [&] {
    ctx.expect_eq(sol.mirrorReflection(1, 1), 1);
    ctx.expect_eq(sol.mirrorReflection(7, 7), 1);
  });

  ctx.test("common_factor_scaling_invariant", [&] {
    // Scaling both p and q by any positive integer keeps the ray path identical.
    ctx.expect_eq(sol.mirrorReflection(4, 2), 2);
    ctx.expect_eq(sol.mirrorReflection(9, 3), 1);
    ctx.expect_eq(sol.mirrorReflection(1000, 500), 2);
  });
}

}  // namespace
