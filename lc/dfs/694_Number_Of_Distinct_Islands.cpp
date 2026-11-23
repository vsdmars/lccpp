// LeetCode 694: Number Of Distinct Islands
// https://leetcode.com/problems/number-of-distinct-islands/
//
// Problem:
//   Given an m x n binary matrix `grid` where `1` represents land and `0`
//   represents water, an island is a 4-directionally connected component of
//   `1`s. Two islands are considered identical if and only if one can be
//   translated (shifted without rotation or reflection) to match the other.
//   Return the number of distinct island shapes in `grid`.
//
// Examples:
//   Example 1:
//     Input:  grid = [[1, 1, 0, 0, 0], [1, 1, 0, 0, 0], [0, 0, 0, 1, 1], [0, 0, 0,
//             1, 1]]
//     Output: 1
//     Explanation:
//       - Row-major scanning first encounters land at (0, 0) and runs
//         dfsEncode(grid, 0, 0, 'S', sig), zeroing out the top-left 2x2 block.
//       - Following kDirs order (R, L, D, U) from (0, 0) -> R(0, 1) -> D(1, 1) ->
//         L(1, 0) and appending 'B' on each return produces signature "SRDLBBBB",
//         which is inserted into signatures.
//       - Scanning later finds (2, 3), explores the bottom-right 2x2 block in the
//         identical relative order to build "SRDLBBBB", and inserts it into
//         signatures with no size increase, returning 1.
//
//   Example 2:
//     Input:  grid = [[1, 1, 0, 1, 1], [1, 0, 0, 0, 0], [0, 0, 0, 0, 1], [1, 1, 0,
//             1, 1]]
//     Output: 3
//     Explanation:
//       - The top-left L-shaped island at (0, 0) visits (0, 0) -> R(0, 1) and (0,
//         0) -> D(1, 0), producing signature "SRBDBB".
//       - The top-right 1x2 horizontal bar at (0, 3) records signature "SRBB".
//       - The bottom-right reversed-L island at (2, 4) traverses (2, 4) -> D(3, 4)
//         -> L(3, 3), recording "SDLBBB".
//       - The bottom-left 1x2 horizontal bar at (3, 0) also records "SRBB", so
//         signatures holds {"SRBDBB", "SRBB", "SDLBBB"} and returns 3.
//
//   Example 3:
//     Input:  grid = [[1, 1, 0], [0, 1, 1], [0, 0, 0], [1, 1, 1], [0, 1, 0]]
//     Output: 2
//     Explanation:
//       - The Z-shaped island starting at (0, 0) traverses (0, 0) -> R(0, 1) ->
//         D(1, 1) -> R(1, 2) and unwinds four frames, generating signature
//         "SRDRBBBB".
//       - The T-shaped island starting at (3, 0) steps R to (3, 1), branches R to
//         (3, 2) (backtracks with 'B'), then branches D to (4, 1) (backtracks with
//         'B'), and unwinds to yield "SRRBDBBB".
//       - The backtrack markers 'B' keep the two 4-cell signatures distinct in
//         signatures, so numDistinctIslands returns 2.
//
// Algorithm:
//   Scan the grid in row-major order. Whenever an unvisited land cell `1` is
//   found, launch a deterministic 4-directional DFS that both marks visited
//   cells as `0` and appends a canonical path signature to a string. Because
//   every island is entered at its lexicographically smallest (top-most, then
//   left-most) cell and explored in a fixed direction order (`R, L, D, U`),
//   recording each entry move character (`'S'` for start, `'R'/'L'/'D'/'U'`
//   for steps) AND a backtrack marker `'B'` upon returning from each recursive
//   call produces an exact Euler-tour serialization of the DFS spanning tree.
//   The backtrack marker `'B'` is essential to distinguish branching shapes
//   (such as a T-shape vs. a Z-shape) that share the same move sequence.
//   Inserting each completed signature into a hash set deduplicates translated
//   copies in linear time.
//   Time Complexity:  O(m * n), since each grid cell is visited at most once
//                     and contributes O(1) characters to its island signature.
//   Space Complexity: O(m * n) for the DFS recursion stack and stored shape
//                     signatures.
//
// Data Structures:
//   - 2D grid (`std::vector<std::vector<int>>`): mutated in-place (`1 -> 0`)
//     to track visited land cells without an auxiliary visited matrix.
//   - Signature string (`std::string`): incrementally built canonical DFS
//     Euler-tour encoding of an island's translated shape.
//   - Hash set (`std::unordered_set<std::string>`): counts unique island
//     signatures.
//
// Real-World Applications:
//   - VLSI / PCB layout verification: identifying unique repeating circuit
//     footprints and via patterns across a silicon die.
//   - Computer vision & OCR connected-component clustering: deduplicating
//     identical glyph bitmaps or sprite templates under translation.
//   - Satellite & GIS land-parcel analysis: cataloging distinct building
//     footprints or habitat patch geometries.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (count total 4-connected islands)
//   - #695 Max Area of Island [done] (find largest 4-connected island area)
//   - #711 Number of Distinct Islands II [new] (deduplicate under rotations & reflections)
//   - #463 Island Perimeter [done] (boundary measurement of a grid island)
//   - #827 Making A Large Island [done] (component labeling & bridge merging)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

struct DirStep {
  int dr = 0;
  int dc = 0;
  char tag = ' ';
};

// Fixed 4-directional traversal order used to generate canonical signatures.
constexpr std::array<DirStep, 4> kDirs = {{
    {.dr = 0, .dc = 1, .tag = 'R'},
    {.dr = 0, .dc = -1, .tag = 'L'},
    {.dr = 1, .dc = 0, .tag = 'D'},
    {.dr = -1, .dc = 0, .tag = 'U'},
}};

class Solution {
 public:
  // Counts the number of translation-distinct 4-connected islands in `grid`.
  // Time: O(m * n) | Space: O(m * n).
  int numDistinctIslands(std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    std::unordered_set<std::string> signatures;
    for (std::size_t r = 0uz; r < grid.size(); ++r) {
      for (std::size_t c = 0uz; c < grid[0].size(); ++c) {
        if (grid[r][c] == 1) {
          std::string sig;
          dfsEncode(grid, static_cast<int>(r), static_cast<int>(c), 'S', sig);
          signatures.insert(std::move(sig));
        }
      }
    }
    return static_cast<int>(signatures.size());
  }

 private:
  // Explores an island via deterministic DFS, recording each incoming step
  // `move_tag` and a backtrack marker `'B'` so the resulting Euler-tour string
  // uniquely determines the island's translated shape.
  static void dfsEncode(std::vector<std::vector<int>>& grid, int r, int c,
                        char move_tag, std::string& sig) {
    grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] = 0;
    sig.push_back(move_tag);

    for (const auto& step : kDirs) {
      const int nr = r + step.dr;
      const int nc = c + step.dc;
      if (nr >= 0 && std::cmp_less(nr, grid.size()) && nc >= 0 &&
          std::cmp_less(nc, grid[0].size()) &&
          grid[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)] ==
              1) {
        dfsEncode(grid, nr, nc, step.tag, sig);
      }
    }

    // Backtrack delimiter disambiguates tree branching structure (e.g. T vs Z).
    sig.push_back('B');
  }
};

LC_PROBLEM(694, "Number Of Distinct Islands", "dfs", ctx) {
  ctx.test("example_1_from_python_build", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 1, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1},
        {0, 0, 0, 1, 1},
    };
    Solution sol;
    ctx.expect_eq(sol.numDistinctIslands(grid), 1);
  });

  ctx.test("example_2_three_distinct_shapes", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 0, 1, 1},
    };
    Solution sol;
    ctx.expect_eq(sol.numDistinctIslands(grid), 3);
  });

  ctx.test("backtrack_disambiguation_z_vs_t_shape", [&] {
    // Classic trap: without backtrack markers, Z-shape and T-shape produce the
    // exact same move sequence (S-R-D-R vs S-R-R-D / branching collision).
    std::vector<std::vector<int>> grid = {
        {1, 1, 0},
        {0, 1, 1},
        {0, 0, 0},
        {1, 1, 1},
        {0, 1, 0},
    };
    Solution sol;
    ctx.expect_eq(sol.numDistinctIslands(grid), 2);
  });

  ctx.test("reflections_and_rotations_are_distinct", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 1, 0, 0, 1},
        {1, 0, 0, 1, 1},
    };
    Solution sol;
    ctx.expect_eq(sol.numDistinctIslands(grid), 2);
  });

  ctx.test("all_water_and_single_cell", [&] {
    Solution sol;
    std::vector<std::vector<int>> water = {{0, 0}, {0, 0}};
    ctx.expect_eq(sol.numDistinctIslands(water), 0);

    std::vector<std::vector<int>> single = {{1}};
    ctx.expect_eq(sol.numDistinctIslands(single), 1);
  });
}

}  // namespace
