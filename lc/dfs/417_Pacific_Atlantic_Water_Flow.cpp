// LeetCode 417: Pacific Atlantic Water Flow
// https://leetcode.com/problems/pacific-atlantic-water-flow/
//
// Problem:
//   Given an m x n integer matrix `heights` representing cell elevations on an
//   island, the Pacific Ocean touches the top and left borders and the Atlantic
//   Ocean touches the bottom and right borders. Rainwater can flow from a cell
//   to any 4-directionally adjacent cell whose elevation is less than or equal
//   to the current cell's elevation. Return all coordinates `[r, c]` from which
//   water can flow to both the Pacific and Atlantic oceans (in any order).
//
// Examples:
//   Example 1:
//     Input:  heights = [[1, 2, 2, 3, 5], [3, 2, 3, 4, 4], [2, 4, 5, 3, 1], [6, 7,
//             1, 4, 5], [5, 1, 1, 2, 4]]
//     Output: [[0, 4], [1, 3], [1, 4], [2, 2], [3, 0], [3, 1], [4, 0]]
//     Explanation:
//       - dfsUphill seeds the pacific grid from row 0 and column 0, walking only to
//         neighbors with height >= current height to mark Pacific-reachable cells
//         including (0, 4), (1, 3), (1, 4), (2, 2), (3, 0), (3, 1), and (4, 0).
//       - dfsUphill then seeds the atlantic grid from row 4 and column 4, marking
//         border cells (0, 4), (1, 4), and (4, 0) directly and climbing via (0,
//         4)/(1, 4) to (1, 3), (2, 4)=1 -> (2, 3)=3 -> (2, 2)=5, and (4, 0)=5 ->
//         (3, 0)=6 -> (3, 1)=7.
//       - Scanning all 25 cells in row-major order collects every coordinate where
//         both pacific[idx] and atlantic[idx] are 1, returning [[0, 4], [1, 3], [1,
//         4], [2, 2], [3, 0], [3, 1], [4, 0]].
//
//   Example 2:
//     Input:  heights = [[1, 2, 2], [3, 4, 5], [6, 1, 1]]
//     Output: [[0, 1], [0, 2], [1, 1], [1, 2], [2, 0]]
//     Explanation:
//       - Uphill DFS from the Pacific borders (row 0 and column 0) marks (0, 0),
//         (0, 1), (0, 2), (1, 0), (1, 1), (1, 2), and (2, 0), leaving only (2, 1)=1
//         and (2, 2)=1 unreached.
//       - Uphill DFS from the Atlantic borders (row 2 and column 2) reaches (0, 1),
//         (0, 2), (1, 1), (1, 2), (2, 0), (2, 1), and (2, 2), while failing to step
//         downhill into (0, 0)=1 or (1, 0)=3.
//       - Taking the intersection in row-major order excludes (0, 0), (1, 0), (2,
//         1), and (2, 2), returning [[0, 1], [0, 2], [1, 1], [1, 2], [2, 0]].
//
//   Example 3:
//     Input:  heights = [[1]]
//     Output: [[0, 0]]
//     Explanation:
//       - With rows = 1 and cols = 1, border seeding runs dfsUphill at (0, 0) for
//         both pacific and atlantic, setting pacific[0] = 1 and atlantic[0] = 1.
//       - The row-major intersection check finds both flags nonzero at (0, 0) and
//         returns [[0, 0]].
//
// Algorithm:
//   Reverse-flow multi-source DFS starting from each ocean's coastal border:
//   Instead of running an expensive downhill search from every interior cell,
//   reverse the flow direction and walk uphill (`heights[nr][nc] >= heights[r][c]`)
//   from all Pacific-border cells (row 0 and column 0) to mark every cell that
//   can drain into the Pacific, then repeat from all Atlantic-border cells
//   (row m - 1 and column n - 1). Every cell marked reachable by both passes
//   belongs to the answer.
//   Time Complexity: O(m * n), since each cell is visited at most once per
//     ocean traversal.
//   Space Complexity: O(m * n) for the two flat reachability grids and worst-
//     case DFS recursion stack.
//
// Data Structures:
//   - std::vector<std::uint8_t>: two flattened m * n reachability tables
//     (`pacific` and `atlantic`) providing O(1), cache-friendly visited checks.
//   - std::array<std::pair<int, int>, 4>: compile-time 4-directional neighbor
//     offsets.
//
// Real-World Applications:
//   - Geographic Information Systems (GIS) and hydrological watershed modeling
//     identifying continental divides where runoff splits between two basins.
//   - Network reachability analysis determining nodes with valid paths to
//     multiple redundant egress gateways via reverse-graph traversal.
//   - VLSI / PCB routing and terrain flood-zone analysis.
//
// Related LeetCode Problems:
//   - #130 Surrounded Regions [done] (boundary-seeded reverse DFS on a 2D grid)
//   - #1020 Number of Enclaves [new] (boundary-seeded grid reachability)
//   - #200 Number of Islands [done] (4-directional grid DFS flood fill)
//   - #407 Trapping Rain Water II [done] (2D elevation water-flow modeling)
//   - #1254 Number of Closed Islands [done] (distinguishing interior vs border-connected cells)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

// 4-directional grid offsets (right, left, down, up).
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Finds all grid coordinates `[r, c]` from which water can flow to both the
  // Pacific (top/left) and Atlantic (bottom/right) oceans.
  //
  // Invariant: After seeding DFS along an ocean's borders and stepping only to
  // neighbors with `heights[nr][nc] >= heights[r][c]`, `visited[r * cols + c]`
  // is 1 if and only if water can flow downhill from `(r, c)` to that ocean.
  //
  // Time: O(m * n); Space: O(m * n).
  std::vector<std::vector<int>> pacificAtlantic(
      const std::vector<std::vector<int>>& heights) {
    if (heights.empty() || heights[0].empty()) {
      return {};
    }

    const std::size_t rows = heights.size();
    const std::size_t cols = heights[0].size();
    std::vector<std::uint8_t> pacific(rows * cols, 0);
    std::vector<std::uint8_t> atlantic(rows * cols, 0);

    // Seed Pacific from left column (c = 0) and Atlantic from right column.
    for (std::size_t r = 0uz; r < rows; ++r) {
      dfsUphill(heights, r, 0uz, pacific);
      dfsUphill(heights, r, cols - 1uz, atlantic);
    }

    // Seed Pacific from top row (r = 0) and Atlantic from bottom row.
    for (std::size_t c = 0uz; c < cols; ++c) {
      dfsUphill(heights, 0uz, c, pacific);
      dfsUphill(heights, rows - 1uz, c, atlantic);
    }

    // Collect the intersection of Pacific-reachable and Atlantic-reachable
    // coordinates in row-major order.
    std::vector<std::vector<int>> result;
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        const std::size_t idx = (r * cols) + c;
        if (pacific[idx] != 0 && atlantic[idx] != 0) {
          result.push_back({static_cast<int>(r), static_cast<int>(c)});
        }
      }
    }
    return result;
  }

 private:
  // Marks `(r, c)` as reachable and recursively explores all unvisited
  // 4-neighbors whose height is at least `heights[r][c]`.
  static void dfsUphill(const std::vector<std::vector<int>>& heights,
                        std::size_t r, std::size_t c,
                        std::vector<std::uint8_t>& visited) {
    const std::size_t rows = heights.size();
    const std::size_t cols = heights[0].size();
    const std::size_t idx = (r * cols) + c;
    if (visited[idx] != 0) {
      return;
    }
    visited[idx] = 1;

    for (const auto [dr, dc] : kDirs) {
      const int nr = static_cast<int>(r) + dr;
      const int nc = static_cast<int>(c) + dc;
      if (nr < 0 || nc < 0) {
        continue;
      }

      const auto ur = static_cast<std::size_t>(nr);
      const auto uc = static_cast<std::size_t>(nc);
      if (ur >= rows || uc >= cols) {
        continue;
      }

      if (visited[(ur * cols) + uc] == 0 && heights[ur][uc] >= heights[r][c]) {
        dfsUphill(heights, ur, uc, visited);
      }
    }
  }
};

LC_PROBLEM(417, "Pacific Atlantic Water Flow", "dfs", ctx) {
  ctx.test("example_5x5", [&] {
    const std::vector<std::vector<int>> heights = {
        {1, 2, 2, 3, 5}, {3, 2, 3, 4, 4}, {2, 4, 5, 3, 1},
        {6, 7, 1, 4, 5}, {5, 1, 1, 2, 4}};
    const std::vector<std::vector<int>> expected = {
        {0, 4}, {1, 3}, {1, 4}, {2, 2}, {3, 0}, {3, 1}, {4, 0}};

    Solution sol;
    ctx.expect_eq(sol.pacificAtlantic(heights), expected);
  });

  ctx.test("single_cell", [&] {
    Solution sol;
    ctx.expect_eq(sol.pacificAtlantic({{1}}),
                  std::vector<std::vector<int>>{{0, 0}});
  });

  ctx.test("flat_plateau_all_cells_flow_both", [&] {
    const std::vector<std::vector<int>> heights = {{1, 1}, {1, 1}, {1, 1}};
    const std::vector<std::vector<int>> expected = {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}};

    Solution sol;
    ctx.expect_eq(sol.pacificAtlantic(heights), expected);
  });

  ctx.test("custom_3x3_grid", [&] {
    const std::vector<std::vector<int>> heights = {
        {1, 2, 2}, {3, 4, 5}, {6, 1, 1}};
    const std::vector<std::vector<int>> expected = {
        {0, 1}, {0, 2}, {1, 1}, {1, 2}, {2, 0}};

    Solution sol;
    ctx.expect_eq(sol.pacificAtlantic(heights), expected);
  });

  ctx.test("single_row_all_reach_both_oceans", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {
        {0, 0}, {0, 1}, {0, 2}, {0, 3}};
    ctx.expect_eq(sol.pacificAtlantic({{1, 2, 3, 4}}), expected);
  });
}

}  // namespace
