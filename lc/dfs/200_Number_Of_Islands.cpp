// LeetCode 200: Number Of Islands
// https://leetcode.com/problems/number-of-islands/
//
// Problem:
//   Given an m x n 2D binary grid representing a map of '1's (land) and '0's
//   (water), return the number of islands. An island is formed by connecting
//   adjacent land cells horizontally or vertically (diagonals do not connect)
//   and is assumed to be surrounded by water outside all four edges.
//
// Examples:
//   Example 1:
//     Input:  grid =
//             [["1","1","1","1","0"],["1","1","0","1","0"],["1","1","0","0","0"],["0","0","0","0","0"]]
//     Output: 1
//     Explanation:
//       - The row-major scan encounters '1' at (0, 0), increments count to 1, and
//         invokes sinkIsland(grid, 0, 0, 4, 5).
//       - sinkIsland traverses all 4-directionally connected land cells across rows
//         0..2 and overwrites every visited '1' with '0' in place.
//       - Because all nine land cells belong to this single component, the rest of
//         the grid scan sees only '0' and returns 1.
//
//   Example 2:
//     Input:  grid =
//             [["1","1","0","0","0"],["1","1","0","0","0"],["0","0","1","0","0"],["0","0","0","1","1"]]
//     Output: 3
//     Explanation:
//       - At (0, 0), count becomes 1 and sinkIsland flips the 2x2 land block at
//         (0,0), (0,1), (1,0), (1,1) to '0'.
//       - Continuing the scan reaches (2, 2), increments count to 2, and sinks that
//         single cell since kDirs only checks orthogonal neighbors.
//       - At (3, 3), count becomes 3 and sinkIsland sinks (3, 3) and (3, 4),
//         completing the scan and returning 3.
//
//   Example 3:
//     Input:  grid = [["1","0","1"],["0","1","0"],["1","0","1"]]
//     Output: 5
//     Explanation:
//       - At (0, 0), count increments to 1 and sinkIsland flips (0, 0) to '0',
//         stopping immediately because orthogonal neighbors (0, 1) and (1, 0) are
//         '0'.
//       - Each remaining '1' at (0, 2), (1, 1), (2, 0), and (2, 2) likewise has no
//         4-directional '1' neighbors.
//       - Each of those four cells triggers its own sinkIsland call, raising count
//         to 5.
//
// Algorithm:
//   Connected-component counting via in-place Depth-First Search (DFS) flood
//   fill ("sink and count").
//   Scan every cell (r, c) in the grid. Whenever an unvisited land cell ('1')
//   is encountered, increment the island counter and launch a 4-directional
//   DFS that "sinks" the entire connected land component by flipping every
//   reachable '1' to '0'. Because each land cell is sunk immediately upon
//   discovery, no cell is explored more than once and no separate visited
//   matrix is required.
//   Time Complexity:  O(m * n), as each cell is scanned once and sunk at most
//                     once across all DFS invocations.
//   Space Complexity: O(m * n) worst-case recursion stack depth (when the
//                     entire grid is a single land component), O(1) extra
//                     visited storage.
//
// Data Structures:
//   - 2D character grid (`std::vector<std::vector<char>>`): represents the map
//     and doubles as an in-place visited table by overwriting '1' with '0'.
//   - Fixed 4-direction offset table (`std::array<std::pair<int, int>, 4>`).
//   - Implicit DFS call stack.
//
// Real-World Applications:
//   - Connected-component labeling in computer vision and medical imaging
//     (segmenting foreground blobs, cells, or lesions in binary masks).
//   - Geographic Information Systems (GIS): counting disjoint landmasses,
//     watersheds, or wildfire clusters from satellite rasters.
//   - Circuit-board and VLSI netlist verification: identifying isolated
//     conductive copper islands or unintended shorts.
//
// Related LeetCode Problems:
//   - #695 Max Area of Island [done] (DFS measuring each island's cell count)
//   - #130 Surrounded Regions [done] (border-seeded DFS flood fill)
//   - #305 Number of Islands II [done] (dynamic island counting with Union-Find)
//   - #694 Number of Distinct Islands [done] (DFS with normalized shape hashing)
//   - #463 Island Perimeter [done] (grid boundary edge counting)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Counts 4-directionally connected '1' components in `grid` by sinking each
  // discovered island to '0' in-place.
  // Time: O(m * n), Space: O(m * n) worst-case DFS stack.
  int numIslands(std::vector<std::vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const auto rows = static_cast<int>(grid.size());
    const auto cols = static_cast<int>(grid[0].size());
    int count = 0;

    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        const auto ur = static_cast<std::size_t>(r);
        const auto uc = static_cast<std::size_t>(c);
        if (grid[ur][uc] == '1') {
          // Found an unvisited island: count it and sink its entire component.
          ++count;
          sinkIsland(grid, r, c, rows, cols);
        }
      }
    }
    return count;
  }

 private:
  // Recursively floods all land cells reachable from (r, c), turning them '0'.
  static void sinkIsland(std::vector<std::vector<char>>& grid, int r, int c,
                         int rows, int cols) {
    const auto ur = static_cast<std::size_t>(r);
    const auto uc = static_cast<std::size_t>(c);
    grid[ur][uc] = '0';

    for (const auto& [dr, dc] : kDirs) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
        const auto unr = static_cast<std::size_t>(nr);
        const auto unc = static_cast<std::size_t>(nc);
        if (grid[unr][unc] == '1') {
          sinkIsland(grid, nr, nc, rows, cols);
        }
      }
    }
  }
};

LC_PROBLEM(200, "Number Of Islands", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_single_island", [&] {
    std::vector<std::vector<char>> grid = {
        {'1', '1', '1', '1', '0'},
        {'1', '1', '0', '1', '0'},
        {'1', '1', '0', '0', '0'},
        {'0', '0', '0', '0', '0'},
    };
    ctx.expect_eq(sol.numIslands(grid), 1);
  });

  ctx.test("example_2_three_islands", [&] {
    std::vector<std::vector<char>> grid = {
        {'1', '1', '0', '0', '0'},
        {'1', '1', '0', '0', '0'},
        {'0', '0', '1', '0', '0'},
        {'0', '0', '0', '1', '1'},
    };
    ctx.expect_eq(sol.numIslands(grid), 3);
  });

  ctx.test("diagonals_do_not_connect", [&] {
    std::vector<std::vector<char>> grid = {
        {'1', '0', '1'},
        {'0', '1', '0'},
        {'1', '0', '1'},
    };
    ctx.expect_eq(sol.numIslands(grid), 5);
  });

  ctx.test("all_water_and_all_land", [&] {
    std::vector<std::vector<char>> water = {
        {'0', '0'},
        {'0', '0'},
    };
    std::vector<std::vector<char>> land = {
        {'1', '1'},
        {'1', '1'},
    };
    ctx.expect_eq(sol.numIslands(water), 0);
    ctx.expect_eq(sol.numIslands(land), 1);
  });

  ctx.test("empty_and_single_cell", [&] {
    std::vector<std::vector<char>> empty_grid;
    std::vector<std::vector<char>> one_land = {{'1'}};
    std::vector<std::vector<char>> one_water = {{'0'}};
    ctx.expect_eq(sol.numIslands(empty_grid), 0);
    ctx.expect_eq(sol.numIslands(one_land), 1);
    ctx.expect_eq(sol.numIslands(one_water), 0);
  });
}

}  // namespace
