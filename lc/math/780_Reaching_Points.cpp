// LeetCode 780: Reaching Points
// https://leetcode.com/problems/reaching-points/
//
// Problem:
//   Given four positive integers (sx, sy) and (tx, ty) in [1, 10^9], a single
//   move transforms a point (x, y) into either (x, x + y) or (x + y, y).
//   Return true if and only if there exists a finite sequence of moves (possibly
//   zero moves) transforming (sx, sy) into (tx, ty).
//
// Examples:
//   Example 1:
//     Input:  sx = 1, sy = 1, tx = 3, ty = 5
//     Output: true
//     Explanation:
//       - Enter the loop with `tx = 3 > 1` and `ty = 5 > 1`; since `tx <= ty`,
//         reduce `ty %= tx` (`5 % 3 = 2`), giving `(3, 2)`.
//       - Continue the loop with `tx = 3 > 1` and `ty = 2 > 1`; since `tx > ty`,
//         reduce `tx %= ty` (`3 % 2 = 1`), giving `(1, 2)`.
//       - Exit the loop because `tx == sx` (`1 > 1` is false).
//       - Verify `(sx == tx && sy <= ty && (ty - sy) % sx == 0)` with `(1 == 1 && 1
//         <= 2 && (2 - 1) % 1 == 0)`, returning `true`.
//
//   Example 2:
//     Input:  sx = 3, sy = 3, tx = 12, ty = 9
//     Output: true
//     Explanation:
//       - Enter the loop with `tx = 12 > 3` and `ty = 9 > 3`; since `tx > ty`,
//         reduce `tx %= ty` (`12 % 9 = 3`), giving `(3, 9)`.
//       - Exit the loop because `tx == sx` (`3 > 3` is false).
//       - Check `(sx == tx && sy <= ty && (ty - sy) % sx == 0)`: `3 == 3`, `3 <=
//         9`, and `(9 - 3) % 3 = 0`, so return `true`.
//
//   Example 3:
//     Input:  sx = 1, sy = 1, tx = 2, ty = 2
//     Output: false
//     Explanation:
//       - Enter the loop with `tx = 2 > 1` and `ty = 2 > 1`; since `tx > ty` is
//         false, execute the `else` branch `ty %= tx` (`2 % 2 = 0`), giving `(2,
//         0)`.
//       - Exit the loop immediately because `ty = 0` violates `ty > sy` (`0 > 1` is
//         false).
//       - Neither `sx == tx` (`1 == 2`) nor `sy == ty` (`1 == 0`) holds in the
//         final return check, so return `false`.
//
// Algorithm:
//   Work backward from (tx, ty) toward (sx, sy) using modulo reduction, analogous
//   to the Euclidean GCD algorithm:
//   - Because all coordinates are strictly positive, any point (tx, ty) with
//     tx != ty has a unique valid predecessor: (tx - ty, ty) if tx > ty, or
//     (tx, ty - tx) if ty > tx.
//   - While both tx > sx and ty > sy, repeated subtraction of the smaller
//     coordinate from the larger cannot overshoot the other threshold, so we
//     compress all consecutive subtractions in one step via `tx %= ty` or
//     `ty %= tx`.
//   - Once the loop exits, at least one coordinate is <= its starting value.
//     If tx == sx and ty >= sy, the only remaining moves can add sx to y, so
//     (tx, ty) is reachable iff (ty - sy) % sx == 0. Symmetrically, if
//     ty == sy and tx >= sx, it is reachable iff (tx - sx) % sy == 0.
//   Time: O(log(max(tx, ty))), Space: O(1).
//
// Data Structures:
//   None (in-place scalar integer variables).
//
// Real-World Applications:
//   - Inverse state-space reachability in deterministic rewriting systems and
//     linear recurrence verification.
//   - Number-theoretic lattice/Euclidean reduction used in continued fractions,
//     modular arithmetic, and Stern-Brocot / Calkin-Wilf rational tree navigation.
//
// Related LeetCode Problems:
//   - #365 Water and Jug Problem [new] (reachability governed by Bezout's identity and GCD)
//   - #1250 Check If It Is a Good Array [new] (checking GCD = 1 for integer linear combinations)
//   - #2543 Check if Point Is Reachable [new] (backward reduction with halving and GCD on grid points)
//   - #1071 Greatest Common Divisor of Strings [new] (Euclidean reduction applied to string periods)

#include "framework/lc_framework.h"

namespace {

class Solution {
 public:
  // Determines whether (tx, ty) is reachable from (sx, sy) using moves
  // (x, y) -> (x + y, y) and (x, y) -> (x, x + y).
  //
  // Invariant: Every backward reduction step preserves reachability from
  // (sx, sy) while strictly reducing max(tx, ty) in Euclidean fashion.
  //
  // Time: O(log(max(tx, ty))), Space: O(1).
  bool reachingPoints(int sx, int sy, int tx, int ty) {
    // Reduce the larger coordinate modulo the smaller while both coordinates
    // remain strictly above their starting targets. Since sx >= 1 and sy >= 1,
    // both tx and ty are guaranteed strictly positive inside the loop.
    while (tx > sx && ty > sy) {
      if (tx > ty) {
        tx %= ty;
      } else {
        // Also handles tx == ty: sets ty = 0 and exits immediately, since no
        // positive predecessor can produce equal coordinates.
        ty %= tx;
      }
    }

    // Once one coordinate has reached its start value, the other coordinate
    // can only decrease in multiples of the fixed coordinate.
    return (sx == tx && sy <= ty && (ty - sy) % sx == 0) ||
           (sy == ty && sx <= tx && (tx - sx) % sy == 0);
  }
};

LC_PROBLEM(780, "Reaching Points", "math", ctx) {
  Solution sol;

  ctx.test("example_1_reachable",
           [&] { ctx.expect_true(sol.reachingPoints(1, 1, 3, 5)); });

  ctx.test("example_2_unreachable_equal_target",
           [&] { ctx.expect_false(sol.reachingPoints(1, 1, 2, 2)); });

  ctx.test("example_3_already_at_target",
           [&] { ctx.expect_true(sol.reachingPoints(1, 1, 1, 1)); });

  ctx.test("target_smaller_than_start", [&] {
    ctx.expect_false(sol.reachingPoints(3, 5, 1, 1));
    ctx.expect_false(sol.reachingPoints(5, 2, 5, 1));
  });

  ctx.test("single_axis_multiple_steps", [&] {
    ctx.expect_true(sol.reachingPoints(3, 7, 3, 28));
    ctx.expect_false(sol.reachingPoints(3, 7, 3, 29));
    ctx.expect_true(sol.reachingPoints(2, 5, 17, 5));
  });

  ctx.test("non_coprime_start_and_target", [&] {
    // (3, 3) -> (3, 6) -> (3, 9) -> (12, 9)
    ctx.expect_true(sol.reachingPoints(3, 3, 12, 9));
    // Backward from (8, 4) hits (4, 4), which cannot reach (2, 2) without 0.
    ctx.expect_false(sol.reachingPoints(2, 2, 8, 4));
  });

  ctx.test("large_coordinates_log_time", [&] {
    ctx.expect_true(sol.reachingPoints(1, 1, 1'000'000'000, 1));
    ctx.expect_false(sol.reachingPoints(35, 13, 455'955'547, 420'098'884));
  });
}

}  // namespace
