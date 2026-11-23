// LeetCode 329: Longest Increasing Path In A Matrix
// https://leetcode.com/problems/longest-increasing-path-in-a-matrix/
//
// Problem:
//   Given an m x n integer matrix, find the length of the longest strictly
//   increasing path. From each cell you may move in four orthogonal directions
//   (left, right, up, down) without wrapping around or moving diagonally.
//
// Algorithm:
//   Top-Down Depth-First Search with Memoization (DAG Longest Path). Model the
//   grid as a directed graph with an edge `(r, c) -> (nr, nc)` whenever the
//   neighbor is 4-adjacent and `matrix[nr][nc] > matrix[r][c]`. Because values
//   increase strictly along every edge, the graph is guaranteed to be a
//   Directed Acyclic Graph (DAG) — no cycle is possible and no in-flight
//   `visited` set is needed. Define `memo[r][c]` as the length of the longest
//   strictly increasing path starting at `(r, c)`:
//     memo[r][c] = 1 + max({0} U {memo[nr][nc] : matrix[nr][nc] > matrix[r][c]})
//   Each cell's result is computed at most once, and the global answer is the
//   maximum over all cells.
//   Time Complexity:  O(m * n) — every cell and its up to 4 outgoing edges are
//                     evaluated at most once.
//   Space Complexity: O(m * n) for the memoization table and recursion stack.
//
// Data Structures:
//   - 2D `std::vector<std::vector<int>>` memoization table (`m x n`) initialized
//     to 0 (every valid path has length >= 1, so 0 uniquely marks uncomputed
//     cells).
//   - Fixed `std::array<std::pair<int, int>, 4>` of orthogonal step offsets.
//
// Real-World Applications:
//   - Hydrological flow-path and watershed accumulation modeling on Digital
//     Elevation Models (DEMs) in GIS terrain analysis.
//   - Critical-path length computation in task dependency DAGs and timing
//     analysis of combinational hardware circuits.
//   - Gradient-ascent trajectory length analysis in image processing and
//     scientific grid simulations.
//
// Related LeetCode Problems:
//   - #2328 Number of Increasing Paths in a Grid [new] (counts all strictly increasing paths on the same implicit DAG)
//   - #300 Longest Increasing Subsequence [done] (1D longest strictly increasing subsequence DP)
//   - #207 Course Schedule [done] (DAG cycle detection / topological ordering)
//   - #1340 Jump Game V [done] (top-down memoized DFS on a 1D strictly decreasing DAG)
//   - #361 Bomb Enemy [done] (2D grid dynamic programming)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs{{
    {0, 1},
    {0, -1},
    {1, 0},
    {-1, 0},
}};

class Solution {
 public:
  // Returns the length of the longest strictly increasing 4-directional path
  // in `matrix`.
  // Invariant: `memo[r][c] > 0` holds the exact longest strictly increasing
  // path starting at `(r, c)`; `0` indicates `(r, c)` has not been solved yet.
  // Time: O(m * n), Space: O(m * n).
  int longestIncreasingPath(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0uz].empty()) {
      return 0;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0uz].size();
    std::vector<std::vector<int>> memo(rows, std::vector<int>(cols, 0));

    // Recursive DFS with memoization over the strictly-increasing DAG.
    auto dfs = [&](this auto&& self, std::size_t r, std::size_t c) -> int {
      if (memo[r][c] != 0) {
        return memo[r][c];
      }

      // Every cell alone forms a valid path of length 1.
      int best = 1;
      for (const auto& [dr, dc] : kDirs) {
        const int nr = static_cast<int>(r) + dr;
        const int nc = static_cast<int>(c) + dc;
        if (nr < 0 || nc < 0 || !std::cmp_less(nr, rows) ||
            !std::cmp_less(nc, cols)) {
          continue;
        }
        const auto ur = static_cast<std::size_t>(nr);
        const auto uc = static_cast<std::size_t>(nc);
        // Strictly greater neighbor guarantees acyclicity (DAG property).
        if (matrix[ur][uc] > matrix[r][c]) {
          best = std::max(best, 1 + self(ur, uc));
        }
      }

      memo[r][c] = best;
      return best;
    };

    int global_best = 0;
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        global_best = std::max(global_best, dfs(r, c));
      }
    }

    return global_best;
  }
};

LC_PROBLEM(329, "Longest Increasing Path In A Matrix", "dp", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {9, 9, 4},
        {6, 6, 8},
        {2, 1, 1},
    };
    // Path: 1 -> 2 -> 6 -> 9
    ctx.expect_eq(sol.longestIncreasingPath(matrix), 4);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {3, 4, 5},
        {3, 2, 6},
        {2, 2, 1},
    };
    // Path: 3 -> 4 -> 5 -> 6
    ctx.expect_eq(sol.longestIncreasingPath(matrix), 4);
  });

  ctx.test("four_by_four_snake", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {7, 6, 1, 1},
        {2, 7, 6, 0},
        {1, 3, 5, 1},
        {6, 6, 3, 2},
    };
    // Path: 0 -> 1 -> 2 -> 3 -> 5 -> 6 -> 7
    ctx.expect_eq(sol.longestIncreasingPath(matrix), 7);
  });

  ctx.test("small_and_edge_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestIncreasingPath({}), 0);
    ctx.expect_eq(sol.longestIncreasingPath({{1}}), 1);
    ctx.expect_eq(sol.longestIncreasingPath({{5, 1}}), 2);
    ctx.expect_eq(sol.longestIncreasingPath({{5, 5}, {5, 5}}), 1);
    ctx.expect_eq(sol.longestIncreasingPath({{-3, -2, -1}}), 3);
  });
}

}  // namespace
