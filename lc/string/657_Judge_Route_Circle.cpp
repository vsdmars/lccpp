// LeetCode 657: Judge Route Circle
// https://leetcode.com/problems/judge-route-circle/
//
// Problem:
//   A robot starts at the origin `(0, 0)` on a 2D Cartesian plane and executes
//   a sequence of unit-length moves encoded in a string `moves`, where each
//   character is `'U'` (up), `'D'` (down), `'L'` (left), or `'R'` (right).
//   Return `true` if the robot returns to `(0, 0)` after completing all moves,
//   and `false` otherwise. (Later renamed "Robot Return to Origin" on
//   LeetCode.)
//   Constraints: 1 <= moves.size() <= 2 * 10^4.
//
// Algorithm:
//   Single-pass net displacement tracking:
//   Because 2D translations are commutative and independent along the x- and
//   y-axes, the robot returns to the origin iff total right steps equal total
//   left steps (`x == 0`) and total up steps equal total down steps (`y == 0`).
//   Scan `moves` once, incrementing/decrementing `x` on `'R'`/`'L'` and `y` on
//   `'U'`/`'D'`, and finally check `x == 0 && y == 0`.
//   Time Complexity: O(N), where N = moves.size().
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two scalar integer accumulators (`x`, `y`) tracking net horizontal and
//     vertical displacement from the origin.
//
// Real-World Applications:
//   - Robotics and CNC toolpath verification checking whether a programmed
//     closed-loop motion profile returns cleanly to its home reference point.
//   - Vector graphics (SVG/PostScript) path parsers verifying whether a series
//     of relative pen offsets forms a closed contour.
//   - Dead-reckoning telemetry sanity checks in autonomous warehouse AGVs.
//
// Related LeetCode Problems:
//   - #1041 Robot Bounded In Circle [new] (checking boundedness with directional turning commands)
//   - #874 Walking Robot Simulation [new] (2D grid movement simulation with turns and obstacles)
//   - #1496 Path Crossing [new] (detecting whether a 2D unit-step walk revisits any coordinate)
//   - #2120 Execution of All Suffix Instructions Staying in a Grid [new] (simulating U/D/L/R move sequences within bounds)

#include "framework/lc_framework.h"

#include <string_view>

namespace {

class Solution {
 public:
  // Returns true iff executing all unit moves in `moves` brings the robot back
  // to the starting origin (0, 0).
  // Time: O(N), Space: O(1).
  bool judgeCircle(std::string_view moves) {
    int x = 0;
    int y = 0;

    for (const char move : moves) {
      switch (move) {
        case 'U':
          ++y;
          break;
        case 'D':
          --y;
          break;
        case 'R':
          ++x;
          break;
        case 'L':
          --x;
          break;
        default:
          break;
      }
    }

    // Origin is reached iff horizontal and vertical displacements both cancel.
    return x == 0 && y == 0;
  }
};

LC_PROBLEM(657, "Judge Route Circle", "string", ctx) {
  ctx.test("example_1_up_down", [&] {
    Solution sol;
    ctx.expect_true(sol.judgeCircle("UD"));
  });

  ctx.test("example_2_left_left", [&] {
    Solution sol;
    ctx.expect_false(sol.judgeCircle("LL"));
  });

  ctx.test("python_build_unbalanced_vertical", [&] {
    Solution sol;
    ctx.expect_false(sol.judgeCircle("UDDDD"));
  });

  ctx.test("closed_loop_and_empty", [&] {
    Solution sol;
    ctx.expect_true(sol.judgeCircle("URDL"));
    ctx.expect_true(sol.judgeCircle("RRDDLLUU"));
    ctx.expect_true(sol.judgeCircle(""));
  });

  ctx.test("single_axis_mismatch", [&] {
    Solution sol;
    ctx.expect_false(sol.judgeCircle("LDRRLRUULR"));
  });
}

}  // namespace
