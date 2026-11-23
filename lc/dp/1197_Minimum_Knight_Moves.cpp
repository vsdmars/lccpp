// LeetCode 1197: Minimum Knight Moves
// https://leetcode.com/problems/minimum-knight-moves/
//
// Problem:
//   On an infinite chess board with integer coordinates, a knight starts at
//   `(0, 0)` and moves in the standard L-shape (`(+-2, +-1)` or `(+-1, +-2)`).
//   Given a target square `(x, y)` with `|x| + |y| <= 300`, return the minimum
//   number of moves required to reach `(x, y)`.
//
// Algorithm:
//   Symmetry-reduced top-down DFS with memoization. Because the board and move
//   set are symmetric across both axes, reaching `(x, y)` takes the same number
//   of moves as reaching `(|x|, |y|)` in the first quadrant.
//   Working backward from `(cx, cy)` toward the origin, only the two inward
//   moves `(cx - 1, cy - 2)` and `(cx - 2, cy - 1)` (folded back into the first
//   quadrant via `abs`) are needed, because moving strictly outward is never
//   optimal once outside the `cx + cy <= 2` neighborhood:
//     - Base case `cx + cy == 0`: `(0, 0)` requires 0 moves.
//     - Base case `cx + cy == 2`: `(2, 0)`, `(0, 2)`, and `(1, 1)` each require
//       exactly 2 moves (capturing the parity/corner wrap-around near the
//       origin, which also handles `(1, 0)` and `(0, 1)` cleanly in 1+2 = 3).
//     - Recurrence: `dp(cx, cy) = 1 + min(dp(|cx - 1|, |cy - 2|),
//                                         dp(|cx - 2|, |cy - 1|))`.
//   Because any coordinate `>= 2` strictly decreases and any coordinate `< 2`
//   folds to at most `2`, all visited states stay within
//   `[0, max(|x|, 2)] x [0, max(|y|, 2)]`.
//   Time Complexity:  O(|x| * |y|) states, each computed once in O(1).
//   Space Complexity: O(|x| * |y|) for the flat memoization grid and call stack.
//
// Data Structures:
//   - Flat 2D memoization table (`std::vector<int>`) indexed by
//     `(cx * cols) + cy`, initialized to `-1` to cache solved subproblems
//     without hash-map overhead.
//
// Real-World Applications:
//   - Game AI and grid-based robotics pathfinding exploiting spatial symmetry
//     and non-holonomic movement primitives.
//   - Shortest-path heuristics and admissible pattern databases for A* search.
//   - Lattice navigation and discrete motion planning on infinite or large grids.
//
// Related LeetCode Problems:
//   - #935 Knight Dialer [done] (DP counting knight paths on a keypad)
//   - #688 Knight Probability in Chessboard [done] (DP computing knight stay probability)
//   - #1091 Shortest Path in Binary Matrix [done] (unweighted grid shortest path)
//   - #752 Open the Lock [done] (shortest-path state-space search)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum knight moves from `(0, 0)` to `(x, y)` using
  // first-quadrant symmetry and top-down memoized DP.
  // Time: O(|x| * |y|), Space: O(|x| * |y|).
  int minKnightMoves(int x, int y) {
    const int target_x = std::abs(x);
    const int target_y = std::abs(y);

    // Coordinates `< 2` can reflect to `2` via `|c - 2|`, while coordinates
    // `>= 2` strictly decrease, so bounding by `max(target, 2) + 1` covers all
    // reachable states in the recursion.
    const std::size_t rows = static_cast<std::size_t>(std::max(target_x, 2)) + 1uz;
    const std::size_t cols = static_cast<std::size_t>(std::max(target_y, 2)) + 1uz;
    static constexpr int kUnvisited = -1;
    std::vector<int> memo(rows * cols, kUnvisited);

    auto dfs = [&](this auto&& self, int cx, int cy) -> int {
      // Base case 1: already at the origin.
      if (cx + cy == 0) {
        return 0;
      }
      // Base case 2: squares `(2, 0)`, `(0, 2)`, and `(1, 1)` all require
      // exactly 2 moves (stepping briefly outside the first quadrant).
      if (cx + cy == 2) {
        return 2;
      }

      const std::size_t key =
          (static_cast<std::size_t>(cx) * cols) + static_cast<std::size_t>(cy);
      if (memo[key] != kUnvisited) {
        return memo[key];
      }

      // Take the shorter of the two inward L-moves, reflecting across axes to
      // remain in the non-negative first quadrant.
      const int move_a = self(std::abs(cx - 1), std::abs(cy - 2));
      const int move_b = self(std::abs(cx - 2), std::abs(cy - 1));
      memo[key] = std::min(move_a, move_b) + 1;
      return memo[key];
    };

    return dfs(target_x, target_y);
  }
};

LC_PROBLEM(1197, "Minimum Knight Moves", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_single_move", [&] {
    ctx.expect_eq(sol.minKnightMoves(2, 1), 1);
  });

  ctx.test("example_2_diagonal_target", [&] {
    ctx.expect_eq(sol.minKnightMoves(5, 5), 4);
  });

  ctx.test("python_build_adjacent_square", [&] {
    ctx.expect_eq(sol.minKnightMoves(0, 1), 3);
    ctx.expect_eq(sol.minKnightMoves(1, 0), 3);
  });

  ctx.test("origin_and_near_origin_cases", [&] {
    ctx.expect_eq(sol.minKnightMoves(0, 0), 0);
    ctx.expect_eq(sol.minKnightMoves(1, 1), 2);
    ctx.expect_eq(sol.minKnightMoves(2, 0), 2);
    ctx.expect_eq(sol.minKnightMoves(0, 2), 2);
    ctx.expect_eq(sol.minKnightMoves(0, 3), 3);
  });

  ctx.test("quadrant_symmetry_and_far_target", [&] {
    ctx.expect_eq(sol.minKnightMoves(-2, -1), 1);
    ctx.expect_eq(sol.minKnightMoves(-5, 5), 4);
    ctx.expect_eq(sol.minKnightMoves(5, -5), 4);
    ctx.expect_eq(sol.minKnightMoves(300, 0), 150);
  });
}

}  // namespace
