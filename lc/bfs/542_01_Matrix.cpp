// LeetCode 542: 01 Matrix
// https://leetcode.com/problems/01-matrix/
//
// Problem:
//   Given an m x n binary matrix `mat` containing only 0s and 1s (with at
//   least one 0 guaranteed), return an m x n matrix where each cell holds the
//   shortest Manhattan distance to a cell with value 0. Adjacent cells are
//   connected 4-directionally (up, down, left, right) at distance 1.
//
// Algorithm:
//   Two clean O(m * n) approaches (both noted in the original solution):
//   1. Multi-Source BFS (`updateMatrix`, primary):
//      Treat every 0-cell simultaneously as a distance-0 source and enqueue all
//      of them up front, marking every 1-cell as unvisited (-1). Expanding the
//      FIFO queue layer by layer ("peeling the onion") assigns each newly
//      reached cell its exact shortest distance `dist[cur] + 1` on first visit.
//      Time Complexity:  O(m * n) (each cell enqueued and dequeued once).
//      Space Complexity: O(m * n) for the BFS queue and distance grid.
//   2. Two-Pass Dynamic Programming (`updateMatrixDp`):
//      Any shortest Manhattan path can be decomposed into orthogonal steps
//      from either the top-left quadrant or the bottom-right quadrant.
//      - Pass 1 (top-left -> bottom-right): relax each cell from its top and
//        left neighbors.
//      - Pass 2 (bottom-right -> top-left): relax each cell from its bottom
//        and right neighbors.
//      Time Complexity:  O(m * n).
//      Space Complexity: O(1) auxiliary space beyond the output matrix.
//
// Data Structures:
//   - std::vector<std::vector<int>>: Distance matrix storing final shortest
//     distances (and -1 sentinel for unvisited cells during BFS).
//   - std::queue<std::pair<std::size_t, std::size_t>>: Multi-source BFS queue.
//   - std::array<std::pair<int, int>, 4>: Cardinal neighbor offsets.
//
// Real-World Applications:
//   - Euclidean/Manhattan distance transforms in computer vision and image
//     processing (morphological dilation, chamfer matching, skeletonization).
//   - Robotics obstacle clearance maps (costmaps) computing distance from every
//     free cell to the nearest obstacle for safe path planning.
//   - Geographic Information Systems (GIS) proximity queries (distance from
//     every grid parcel to the nearest hospital, fire station, or cell tower).
//
// Related LeetCode Problems:
//   - #286 Walls and Gates [done] (identical multi-source BFS from gates with obstacles)
//   - #994 Rotting Oranges [done] (multi-source BFS measuring time to reach all cells)
//   - #1162 As Far from Land as Possible [new] (multi-source BFS maximizing minimum distance)
//   - #1765 Map of Highest Peak [new] (exact reformulation of 01 Matrix with water = 0)
//   - #490 The Maze [done] (grid BFS with rolling-ball transitions)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{-1, 0}, {1, 0}, {0, -1}, {0, 1}},
};

class Solution {
 public:
  // Computes nearest-zero distance for every cell via multi-source BFS.
  // Time: O(m * n), Space: O(m * n).
  std::vector<std::vector<int>> updateMatrix(
      const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) {
      return {};
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));
    std::queue<std::pair<std::size_t, std::size_t>> frontier;

    // Seed the queue with every 0-cell at distance 0 so the BFS wavefront
    // radiates outward from all zeros simultaneously.
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (mat[r][c] == 0) {
          dist[r][c] = 0;
          frontier.emplace(r, c);
        }
      }
    }

    while (!frontier.empty()) {
      const auto [r, c] = frontier.front();
      frontier.pop();
      relax_neighbors(r, c, dist, frontier);
    }

    return dist;
  }

  // Computes nearest-zero distance via two-pass dynamic programming (top-left
  // sweep followed by bottom-right sweep).
  // Time: O(m * n), Space: O(1) auxiliary beyond the output matrix.
  std::vector<std::vector<int>> updateMatrixDp(
      const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) {
      return {};
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();
    // Maximum Manhattan distance in an m x n grid is (m - 1) + (n - 1), so
    // rows + cols is a safe finite upper bound that never overflows on + 1.
    const int kInf = static_cast<int>(rows + cols);

    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, kInf));

    // Pass 1: propagate shortest paths arriving from above or from the left.
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (mat[r][c] == 0) {
          dist[r][c] = 0;
          continue;
        }
        if (r > 0uz) {
          dist[r][c] = std::min(dist[r][c], dist[r - 1uz][c] + 1);
        }
        if (c > 0uz) {
          dist[r][c] = std::min(dist[r][c], dist[r][c - 1uz] + 1);
        }
      }
    }

    // Pass 2: propagate shortest paths arriving from below or from the right.
    for (std::size_t r = rows; r > 0uz; --r) {
      const std::size_t row = r - 1uz;
      for (std::size_t c = cols; c > 0uz; --c) {
        const std::size_t col = c - 1uz;
        if (row + 1uz < rows) {
          dist[row][col] = std::min(dist[row][col], dist[row + 1uz][col] + 1);
        }
        if (col + 1uz < cols) {
          dist[row][col] = std::min(dist[row][col], dist[row][col + 1uz] + 1);
        }
      }
    }

    return dist;
  }

 private:
  // Enqueues every unvisited 4-neighbor of (r, c) at distance dist[r][c] + 1.
  static void relax_neighbors(
      std::size_t r, std::size_t c, std::vector<std::vector<int>>& dist,
      std::queue<std::pair<std::size_t, std::size_t>>& frontier) {
    const int rows = static_cast<int>(dist.size());
    const int cols = static_cast<int>(dist[0].size());
    const int next_dist = dist[r][c] + 1;

    for (const auto& [dr, dc] : kDirs) {
      const int nr = static_cast<int>(r) + dr;
      const int nc = static_cast<int>(c) + dc;
      if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
        continue;
      }

      const auto ur = static_cast<std::size_t>(nr);
      const auto uc = static_cast<std::size_t>(nc);
      if (dist[ur][uc] == -1) {
        dist[ur][uc] = next_dist;
        frontier.emplace(ur, uc);
      }
    }
  }
};

LC_PROBLEM(542, "01 Matrix", "bfs", ctx) {
  ctx.test("example_1_isolated_one", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0},
    };
    ctx.expect_eq(sol.updateMatrix(mat), expected);
    ctx.expect_eq(sol.updateMatrixDp(mat), expected);
  });

  ctx.test("example_2_bottom_row_ones", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {
        {0, 0, 0},
        {0, 1, 0},
        {1, 1, 1},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0},
        {0, 1, 0},
        {1, 2, 1},
    };
    ctx.expect_eq(sol.updateMatrix(mat), expected);
    ctx.expect_eq(sol.updateMatrixDp(mat), expected);
  });

  ctx.test("python_build_case", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {
        {0, 0, 0},
        {0, 1, 1},
        {1, 1, 1},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0},
        {0, 1, 1},
        {1, 2, 2},
    };
    ctx.expect_eq(sol.updateMatrix(mat), expected);
    ctx.expect_eq(sol.updateMatrixDp(mat), expected);
  });

  ctx.test("single_zero_in_bottom_right_corner", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 0},
    };
    const std::vector<std::vector<int>> expected = {
        {5, 4, 3, 2},
        {4, 3, 2, 1},
        {3, 2, 1, 0},
    };
    ctx.expect_eq(sol.updateMatrix(mat), expected);
    ctx.expect_eq(sol.updateMatrixDp(mat), expected);
  });

  ctx.test("bfs_and_dp_agree_on_large_10x10_grid", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {
        {1, 0, 1, 1, 0, 0, 1, 0, 0, 1}, {0, 1, 1, 0, 1, 0, 1, 0, 1, 1},
        {0, 0, 1, 0, 1, 0, 0, 1, 0, 0}, {1, 0, 1, 0, 1, 1, 1, 1, 1, 1},
        {0, 1, 0, 1, 1, 0, 0, 0, 0, 1}, {0, 0, 1, 0, 1, 1, 1, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 0, 0, 1, 1}, {1, 0, 0, 0, 1, 1, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 1, 0, 1, 0, 0, 1, 1},
    };
    const auto bfs_res = sol.updateMatrix(mat);
    const auto dp_res = sol.updateMatrixDp(mat);
    ctx.expect_eq(bfs_res, dp_res);
    // Spot-check cell (9, 0): nearest 0 is (7, 1) -> |9-7| + |0-1| = 3.
    ctx.expect_eq(bfs_res[9][0], 3);
    // Spot-check cell (0, 0): adjacent to (0, 1) and (1, 0) -> distance 1.
    ctx.expect_eq(bfs_res[0][0], 1);
  });

  ctx.test("single_cell_zero", [&] {
    Solution sol;
    ctx.expect_eq(sol.updateMatrix({{0}}), std::vector<std::vector<int>>{{0}});
    ctx.expect_eq(sol.updateMatrixDp({{0}}),
                  std::vector<std::vector<int>>{{0}});
  });
}

}  // namespace
