// LeetCode 305: Number of Islands II
// https://leetcode.com/problems/number-of-islands-ii/
//
// Problem:
//   Start with an `m x n` 2D grid initially filled completely with water. Given
//   a sequence of `positions` where each `[r, c]` turns cell `(r, c)` into land
//   (an `addLand` operation), return a vector of integers recording the total
//   number of 4-directionally connected islands after each operation. Duplicate
//   operations on an existing land cell leave the grid and count unchanged.
//
// Algorithm:
//   Model dynamic 4-directional grid connectivity using Disjoint Set Union
//   (Union-Find) over flattened 1D cell IDs `id = r * n + c`, with path
//   compression and union by rank.
//   Maintain a running `islands` counter initialized to 0. For each `[r, c]`:
//     1. If `parent[id] != -1`, the cell is already land; append `islands` and
//        skip.
//     2. Otherwise mark `parent[id] = id` (creating a singleton island) and
//        increment `islands`.
//     3. Check all 4 orthogonal neighbors (`kDirs`). For each in-bounds
//        neighbor that is already land (`parent[nid] != -1`), call `unite(id,
//        nid)`; if the two cells belonged to different islands, decrement
//        `islands`.
//     4. Append the updated `islands` count to `result`.
//   Time Complexity:  O(m * n + k * alpha(m * n)), where k = positions.size()
//                     (O(m * n) to initialize arrays, O(alpha(m * n)) amortized
//                     per operation).
//   Space Complexity: O(m * n) for the DSU arrays (plus O(k) for the output).
//
// Data Structures:
//   - `std::vector<int> parent`: flattened `m * n` DSU parent table where `-1`
//     acts as a sentinel for water and non-negative entries store parent IDs.
//   - `std::vector<int> rank`: union-by-rank heights for balanced merges.
//   - `std::array<std::pair<int, int>, 4> kDirs`: 4-neighbor coordinate offsets.
//   - `std::vector<int> result`: island count recorded after each `addLand`.
//
// Real-World Applications:
//   - Incremental image segmentation and connected-component labeling as pixels
//     or foreground regions stream in.
//   - Percolation simulations in physics and materials science tracking cluster
//     merging as sites open dynamically.
//   - GIS / map tiling systems maintaining landmass or service-coverage zones
//     under real-time terrain or sensor updates.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (static connected-component count on a fixed grid)
//   - #695 Max Area of Island [done] (tracks largest connected land area on a grid)
//   - #827 Making A Large Island [done] (merges adjacent islands via one water flip)
//   - #959 Regions Cut By Slashes [new] (Union-Find region merging on a grid)
//   - #1970 Last Day Where You Can Still Cross [new] (dynamic grid connectivity via
//     reverse-time Union-Find)

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
  // Dynamic connectivity using Union-Find with path compression & union by rank.
  // Time: O(k * alpha(m * n)), Space: O(m * n) where k = positions.size().
  std::vector<int> numIslands2(int m, int n,
                               const std::vector<std::vector<int>>& positions) {
    const auto total = static_cast<std::size_t>(m) * static_cast<std::size_t>(n);
    // `parent[id] == -1` indicates water; otherwise it stores the DSU parent.
    std::vector<int> parent(total, -1);
    std::vector<int> rank(total, 0);

    auto find = [&](this auto&& self, int x) -> int {
      const auto idx = static_cast<std::size_t>(x);
      if (parent[idx] != x) {
        parent[idx] = self(parent[idx]);
      }
      return parent[idx];
    };

    auto unite = [&](int a, int b) -> bool {
      int ra = find(a);
      int rb = find(b);
      if (ra == rb) {
        return false;
      }
      if (rank[static_cast<std::size_t>(ra)] <
          rank[static_cast<std::size_t>(rb)]) {
        std::swap(ra, rb);
      }
      parent[static_cast<std::size_t>(rb)] = ra;
      if (rank[static_cast<std::size_t>(ra)] ==
          rank[static_cast<std::size_t>(rb)]) {
        ++rank[static_cast<std::size_t>(ra)];
      }
      return true;
    };

    std::vector<int> result;
    result.reserve(positions.size());
    int islands = 0;

    for (const auto& pos : positions) {
      const int r = pos[0];
      const int c = pos[1];
      const int id = (r * n) + c;

      // Ignore duplicate addLand operations on an existing land cell.
      if (parent[static_cast<std::size_t>(id)] != -1) {
        result.push_back(islands);
        continue;
      }

      parent[static_cast<std::size_t>(id)] = id;
      ++islands;

      for (const auto& [dr, dc] : kDirs) {
        const int nr = r + dr;
        const int nc = c + dc;
        if (nr < 0 || nr >= m || nc < 0 || nc >= n) {
          continue;
        }
        const int nid = (nr * n) + nc;
        if (parent[static_cast<std::size_t>(nid)] != -1 && unite(id, nid)) {
          --islands;
        }
      }

      result.push_back(islands);
    }

    return result;
  }
};

LC_PROBLEM(305, "Number of Islands II", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: standard 3x3 grid", [&] {
    ctx.expect_eq(sol.numIslands2(3, 3, {{0, 0}, {0, 1}, {1, 2}, {2, 1}}),
                  std::vector<int>{1, 1, 2, 3});
  });

  ctx.test("duplicate land position", [&] {
    ctx.expect_eq(sol.numIslands2(3, 3, {{0, 0}, {0, 1}, {1, 2}, {1, 2}}),
                  std::vector<int>{1, 1, 2, 2});
  });

  ctx.test("bridge merges three separate islands into one", [&] {
    ctx.expect_eq(sol.numIslands2(3, 3, {{0, 0}, {0, 2}, {1, 1}, {0, 1}}),
                  std::vector<int>{1, 2, 3, 1});
  });
}

}  // namespace
