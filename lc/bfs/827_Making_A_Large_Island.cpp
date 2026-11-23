// LeetCode 827: Making A Large Island
// https://leetcode.com/problems/making-a-large-island/
//
// Problem:
//   Given an n x n binary matrix `grid` where 0 represents water and 1
//   represents land, you are allowed to change at most one 0 cell into a 1.
//   Return the size (area in number of cells) of the largest 4-directionally
//   connected island of 1s achievable after at most one flip.
//   Constraints: 1 <= n <= 500, grid[i][j] in {0, 1}.
//
// Algorithm:
//   Two-phase connected-component labeling + boundary merge evaluation:
//   1. Island Labeling (BFS Flood Fill): Scan the grid. Whenever an unvisited
//      land cell (value 1) is found, run a BFS to label every cell in that
//      4-connected island in-place with a unique integer ID starting at 2, and
//      record its total cell count in `island_sizes[id]`. Track the largest
//      existing island size in `best` (this also handles the all-1s case).
//   2. Candidate Flip Evaluation: Scan every water cell (value 0). Inspect its
//      up to four orthogonal neighbors, collect the distinct neighboring island
//      IDs (deduplicating so a single island touching the cell on multiple
//      sides is only counted once), and compute `1 + sum(island_sizes[id])`.
//      Update `best` with the maximum candidate area.
//   Time Complexity:  O(n^2) — each cell is visited once during flood fill and
//                     once during the candidate flip scan.
//   Space Complexity: O(n^2) — worst-case BFS queue and island size table.
//
// Data Structures:
//   - 2D grid (`std::vector<std::vector<int>>`): reused in-place to map each
//     land cell `(r, c)` directly to its integer island ID (`>= 2`) in O(1).
//   - Flat lookup table (`std::vector<int>`): indexed by island ID to store
//     each component's area with O(1) random access.
//   - FIFO queue (`std::queue<std::pair<int, int>>`): drives BFS flood fill.
//   - Fixed-size array (`std::array<int, 4>`): deduplicates up to four
//     neighboring island IDs per water cell without heap allocations.
//
// Real-World Applications:
//   - Geographic Information Systems (GIS) & urban planning: finding the single
//     optimal land-bridge or road-segment placement that maximizes connected
//     serviceable area or habitat connectivity.
//   - Circuit & PCB routing: identifying a single jumper or via that merges the
//     largest disconnected conductive copper regions on a grid layer.
//   - Wireless sensor & mesh network repair: selecting one relay node location
//     to maximize the size of the largest connected communication cluster.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (4-directional connected-component counting in a binary grid)
//   - #695 Max Area of Island [done] (computing maximum island area without flipping any cell)
//   - #934 Shortest Bridge [done] (labeling islands first and expanding across water with BFS)
//   - #305 Number of Islands II [done] (dynamically adding land cells and merging islands via DSU)
//   - #1020 Number of Enclaves [new] (grid flood fill to classify boundary-connected land)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Four orthogonal directions: right, left, down, up.
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}},
};

class Solution {
 public:
  // Returns the largest 4-connected island area achievable by flipping at most
  // one 0 cell in `grid` to 1.
  // Time: O(R * C), Space: O(R * C).
  int largestIsland(std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    // IDs 0 and 1 are reserved for water and unlabeled land, so real island
    // IDs start at 2. `island_sizes[id]` stores the cell count of island `id`.
    std::vector<int> island_sizes = {0, 0};
    int best = 0;

    // Phase 1: Label every connected island with a unique ID >= 2 and record
    // its area. Tracking `best` here ensures grids with no 0s return R * C.
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        if (grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] ==
            1) {
          const int island_id = static_cast<int>(island_sizes.size());
          const int area = floodFillIsland(grid, r, c, island_id);
          island_sizes.push_back(area);
          best = std::max(best, area);
        }
      }
    }

    // Phase 2: Try flipping each water cell (0) to land (1) and sum the areas
    // of all distinct neighboring islands.
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        if (grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] ==
            0) {
          best = std::max(best, mergedIslandSize(grid, island_sizes, r, c));
        }
      }
    }

    return best;
  }

 private:
  // Labels the 4-connected island containing `(start_r, start_c)` with
  // `island_id` using iterative BFS and returns its total cell count.
  static int floodFillIsland(std::vector<std::vector<int>>& grid, int start_r,
                             int start_c, int island_id) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    std::queue<std::pair<int, int>> frontier;
    frontier.emplace(start_r, start_c);
    grid[static_cast<std::size_t>(start_r)]
        [static_cast<std::size_t>(start_c)] = island_id;

    int area = 0;
    while (!frontier.empty()) {
      const auto [r, c] = frontier.front();
      frontier.pop();
      ++area;

      for (const auto [dr, dc] : kDirs) {
        const int nr = r + dr;
        const int nc = c + dc;
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
          continue;
        }
        auto& cell =
            grid[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
        if (cell == 1) {
          // Stamp the ID immediately on enqueue to prevent duplicate visits.
          cell = island_id;
          frontier.emplace(nr, nc);
        }
      }
    }

    return area;
  }

  // Computes the combined island area obtained by flipping the water cell
  // `(r, c)` to land, deduplicating adjacent island IDs in O(1) stack space.
  static int mergedIslandSize(const std::vector<std::vector<int>>& grid,
                              const std::vector<int>& island_sizes, int r,
                              int c) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    // A cell has at most 4 orthogonal neighbors, so a tiny fixed array avoids
    // per-cell heap allocations for hash sets.
    std::array<int, 4> seen_ids = {0, 0, 0, 0};
    std::size_t seen_count = 0;
    int merged_area = 1;  // Include the flipped cell itself.

    for (const auto [dr, dc] : kDirs) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
        continue;
      }

      const int gid =
          grid[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
      if (gid < 2) {
        continue;
      }

      // Only add this island's area if we have not already counted `gid` from
      // another orthogonal neighbor of `(r, c)`.
      const bool already_seen =
          std::ranges::any_of(seen_ids.begin(),
                              seen_ids.begin() +
                                  static_cast<std::ptrdiff_t>(seen_count),
                              [gid](int id) { return id == gid; });
      if (!already_seen) {
        seen_ids[seen_count] = gid;
        ++seen_count;
        merged_area += island_sizes[static_cast<std::size_t>(gid)];
      }
    }

    return merged_area;
  }
};

LC_PROBLEM(827, "Making A Large Island", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_diagonal_islands", [&] {
    std::vector<std::vector<int>> grid = {{1, 0}, {0, 1}};
    ctx.expect_eq(sol.largestIsland(grid), 3);
  });

  ctx.test("example2_expand_single_island", [&] {
    std::vector<std::vector<int>> grid = {{1, 1}, {1, 0}};
    ctx.expect_eq(sol.largestIsland(grid), 4);
  });

  ctx.test("example3_all_ones", [&] {
    std::vector<std::vector<int>> grid = {{1, 1}, {1, 1}};
    ctx.expect_eq(sol.largestIsland(grid), 4);
  });

  ctx.test("all_zeros_flips_one_cell", [&] {
    std::vector<std::vector<int>> grid = {{0, 0}, {0, 0}};
    ctx.expect_eq(sol.largestIsland(grid), 1);
  });

  ctx.test("same_island_touches_water_on_multiple_sides", [&] {
    // Flipping the center 0 touches the same U-shaped island on 3 sides and
    // must NOT double-count it; total area should be 5 + 1 = 6.
    std::vector<std::vector<int>> grid = {
        {1, 1, 1},
        {1, 0, 1},
        {0, 0, 0},
    };
    ctx.expect_eq(sol.largestIsland(grid), 6);
  });

  ctx.test("five_by_five_complex_grid", [&] {
    // From Python reference build(): flipping (3, 3) bridges the large
    // 14-cell island with the isolated land cell at (4, 3) -> 16.
    std::vector<std::vector<int>> grid = {
        {1, 0, 0, 1, 1}, {1, 0, 0, 1, 0}, {1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1}, {0, 0, 0, 1, 0},
    };
    ctx.expect_eq(sol.largestIsland(grid), 16);
  });

  ctx.test("single_cell_edges", [&] {
    std::vector<std::vector<int>> zero_grid = {{0}};
    std::vector<std::vector<int>> one_grid = {{1}};
    ctx.expect_eq(sol.largestIsland(zero_grid), 1);
    ctx.expect_eq(sol.largestIsland(one_grid), 1);
  });
}

}  // namespace
