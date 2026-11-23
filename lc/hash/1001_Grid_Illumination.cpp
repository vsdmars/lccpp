// LeetCode 1001: Grid Illumination
// https://leetcode.com/problems/grid-illumination/
//
// Problem:
//   You are given an `n x n` grid (`1 <= n <= 10^9`) and an array `lamps`
//   where `lamps[i] = [row_i, col_i]` indicates a lamp initially turned on
//   (duplicate coordinates may appear in `lamps` and represent the same single
//   lamp). Every active lamp illuminates its entire row, column, main diagonal,
//   and anti-diagonal (like a chess queen). For each query `[row, col]`:
//   1. Record `1` if `(row, col)` is currently illuminated, else `0`.
//   2. Turn off any active lamp at `(row, col)` or any of its 8 adjacent
//      neighbors (the 3x3 square centered at `(row, col)`).
//   Return the array of query answers.
//   Constraints: 1 <= n <= 10^9, 0 <= lamps.length, queries.length <= 20000.
//
// Examples:
//   Example 1:
//     Input:  n = 5, lamps = [[0, 0], [4, 4]], queries = [[1, 1], [1, 0]]
//     Output: [1, 0]
//     Explanation:
//       - Insert (0, 0) and (4, 4) into active_lamps_, yielding row_counts_ = {0:
//         1, 4: 1}, col_counts_ = {0: 1, 4: 1}, main_diag_counts_ = {0: 2}, and
//         anti_diag_counts_ = {0: 1, 8: 1}.
//       - For query [1, 1], main_diag_counts_ contains 1 - 1 = 0, so record 1 as
//         illuminated.
//       - Extinguish the 3x3 neighborhood [0..2] x [0..2] around (1, 1), removing
//         (0, 0), pruning row 0, col 0, and anti_diag 0, and decrementing
//         main_diag_counts_[0] from 2 to 1.
//       - For query [1, 0], row 1, col 0, main_diag 1, and anti_diag 1 are all
//         absent from the line maps, so record 0 and return [1, 0].
//
//   Example 2:
//     Input:  n = 5, lamps = [[0, 0], [0, 0]], queries = [[0, 0], [0, 1]]
//     Output: [1, 0]
//     Explanation:
//       - Adding the first [0, 0] inserts the key into active_lamps_ and sets
//         row_counts_[0], col_counts_[0], main_diag_counts_[0], and
//         anti_diag_counts_[0] to 1.
//       - Adding the duplicate [0, 0] sees active_lamps_.insert(key).second ==
//         false and returns early without double-counting any line counter.
//       - For query [0, 0], row_counts_.contains(0) is true so record 1, then
//         extinguish_around(0, 0) erases (0, 0) and prunes all four counts (1 -> 0)
//         from the maps.
//       - For query [0, 1], all four line keys (row 0, col 1, main_diag -1,
//         anti_diag 1) are absent, so record 0 and return [1, 0].
//
//   Example 3:
//     Input:  n = 1000000000, lamps = [[0, 999999999], [999999999, 0]], queries =
//             [[500000000, 499999999], [0, 0], [1, 1]]
//     Output: [1, 1, 0]
//     Explanation:
//       - Insert both corner lamps into the sparse hash structures without
//         allocating a 10^9 x 10^9 grid, setting anti_diag_counts_[999999999] = 2
//         and counts of 1 for rows and cols 0 and 999999999.
//       - For query [500000000, 499999999], r + c = 999999999 hits
//         anti_diag_counts_ so record 1; its 3x3 neighborhood hits neither corner
//         lamp.
//       - For query [0, 0], row_counts_.contains(0) and col_counts_.contains(0) are
//         true so record 1, and its 3x3 neighborhood again leaves both corner lamps
//         active.
//       - For query [1, 1], row 1, col 1, main_diag 0, and anti_diag 2 are all
//         absent from the line maps, so record 0 and return [1, 1, 0].
//
// Algorithm:
//   Sparse Line Reference-Counting with Deduplicated Lamp Set:
//   Because `n` can be up to 10^9, materializing an `n x n` grid is infeasible.
//   Instead, observe that a cell `(r, c)` is illuminated iff at least one
//   active lamp shares any of its four queen lines:
//   - row: `r`
//   - column: `c`
//   - main diagonal: `r - c`
//   - anti-diagonal: `r + c`
//   Maintain a hash set of unique active lamp coordinates (packed into 64-bit
//   keys) and four hash maps counting how many active lamps lie on each row,
//   column, main diagonal, and anti-diagonal:
//   1. For each lamp `[r, c]`, insert its packed key into `active_lamps_`. Only
//      if the insertion succeeds (deduplicating repeated lamp coordinates) do
//      we increment all four line counters.
//   2. For each query `[r, c]`, check in O(1) whether any of the four counters
//      for `(r, c)` is positive, then scan the 9 cells `(r + dr, c + dc)` for
//      `dr, dc in {-1, 0, 1}`. Whenever an active lamp is found in that 3x3
//      neighborhood, erase it from `active_lamps_` and decrement all four line
//      counters (pruning keys that reach 0).
//   Time Complexity: O(L + Q) expected, where L = lamps.size() and
//   Q = queries.size() (each query inspects at most 9 cells).
//   Space Complexity: O(L) for the active-lamp set and the four line counters.
//
// Data Structures:
//   - Hash set (`std::unordered_set<std::uint64_t>`): tracks unique active lamp
//     positions packed as `(uint64_t(row) << 32) | uint64_t(col)` for O(1)
//     deduplication and 3x3 neighborhood lookup/removal.
//   - Four hash maps (`std::unordered_map<std::int64_t, int>`): reference-count
//     active lamps along every occupied row, column, main diagonal (`r - c`),
//     and anti-diagonal (`r + c`).
//
// Real-World Applications:
//   - Sparse spatial indexing and line-of-sight queries in huge coordinate
//     spaces (CAD/EDA routing grids, GIS ray-casting, game boards).
//   - Reference-counted constraint tracking in incremental SAT/CSP solvers
//     (e.g., N-Queens conflict counters under local search moves).
//   - Sensor coverage monitoring: updating coverage counts dynamically as
//     beacons or antennas are toggled on and off.
//
// Related LeetCode Problems:
//   - #51 N-Queens [done] (same row/col/diag/anti-diag conflict invariants on a chessboard)
//   - #52 N-Queens II [new] (counting valid placements using diagonal tracking)
//   - #1222 Queens That Can Attack the King [new] (8-directional grid queen rays)
//   - #356 Line Reflection [done] (hashing 2D coordinates into sparse hash sets)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Encapsulates sparse illumination reference counts for an `n x n` grid.
class IlluminationGrid {
 public:
  explicit IlluminationGrid(int grid_size) : n_(grid_size) {}

  // Turns on the lamp at `(row, col)` if it is not already active.
  // Deduplication prevents double-counting when `lamps` contains repeats.
  void add_lamp(int row, int col) {
    const std::uint64_t key = encode_cell(row, col);
    if (!active_lamps_.insert(key).second) {
      return;
    }
    const auto r = static_cast<std::int64_t>(row);
    const auto c = static_cast<std::int64_t>(col);
    ++row_counts_[r];
    ++col_counts_[c];
    ++main_diag_counts_[r - c];
    ++anti_diag_counts_[r + c];
  }

  // Returns true iff at least one active lamp illuminates `(row, col)`.
  [[nodiscard]] bool is_illuminated(int row, int col) const {
    const auto r = static_cast<std::int64_t>(row);
    const auto c = static_cast<std::int64_t>(col);
    return row_counts_.contains(r) || col_counts_.contains(c) ||
           main_diag_counts_.contains(r - c) ||
           anti_diag_counts_.contains(r + c);
  }

  // Turns off every active lamp in the 3x3 neighborhood centered at `(row, col)`.
  void extinguish_around(int row, int col) {
    for (int dr = -1; dr <= 1; ++dr) {
      for (int dc = -1; dc <= 1; ++dc) {
        const int nr = row + dr;
        const int nc = col + dc;
        if (nr < 0 || nr >= n_ || nc < 0 || nc >= n_) {
          continue;
        }
        turn_off_lamp(nr, nc);
      }
    }
  }

 private:
  // Packs non-negative `(row, col)` into a collision-free 64-bit key.
  static std::uint64_t encode_cell(int row, int col) {
    return (static_cast<std::uint64_t>(row) << 32U) |
           static_cast<std::uint64_t>(col);
  }

  // Decrements a line counter and erases the entry once its count reaches 0 so
  // `contains()` remains an exact indicator of illumination.
  static void decrement_and_prune(std::unordered_map<std::int64_t, int>& counts,
                                  std::int64_t line_id) {
    const auto it = counts.find(line_id);
    if (it == counts.end()) {
      return;
    }
    --it->second;
    if (it->second == 0) {
      counts.erase(it);
    }
  }

  void turn_off_lamp(int row, int col) {
    if (active_lamps_.erase(encode_cell(row, col)) == 0uz) {
      return;
    }
    const auto r = static_cast<std::int64_t>(row);
    const auto c = static_cast<std::int64_t>(col);
    decrement_and_prune(row_counts_, r);
    decrement_and_prune(col_counts_, c);
    decrement_and_prune(main_diag_counts_, r - c);
    decrement_and_prune(anti_diag_counts_, r + c);
  }

  int n_ = 0;
  std::unordered_set<std::uint64_t> active_lamps_;
  std::unordered_map<std::int64_t, int> row_counts_;
  std::unordered_map<std::int64_t, int> col_counts_;
  std::unordered_map<std::int64_t, int> main_diag_counts_;
  std::unordered_map<std::int64_t, int> anti_diag_counts_;
};

class Solution {
 public:
  // Answers each illumination query in order, extinguishing adjacent lamps
  // after each query.
  // Time: O(L + Q) expected, Space: O(L).
  std::vector<int> gridIllumination(
      int n, const std::vector<std::vector<int>>& lamps,
      const std::vector<std::vector<int>>& queries) {
    IlluminationGrid grid(n);
    for (const auto& lamp : lamps) {
      grid.add_lamp(lamp[0], lamp[1]);
    }

    std::vector<int> answers;
    answers.reserve(queries.size());
    for (const auto& query : queries) {
      const int row = query[0];
      const int col = query[1];
      answers.push_back(grid.is_illuminated(row, col) ? 1 : 0);
      grid.extinguish_around(row, col);
    }
    return answers;
  }
};

LC_PROBLEM(1001, "Grid Illumination", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    const std::vector<std::vector<int>> lamps = {{0, 0}, {4, 4}};
    const std::vector<std::vector<int>> queries = {{1, 1}, {1, 0}};
    const std::vector<int> expected = {1, 0};
    ctx.expect_eq(sol.gridIllumination(5, lamps, queries), expected);
  });

  ctx.test("example_2_remaining_diagonal_lamp", [&] {
    // Turning off [0,0] still leaves [4,4] illuminating [1,1] along main diag.
    const std::vector<std::vector<int>> lamps = {{0, 0}, {4, 4}};
    const std::vector<std::vector<int>> queries = {{1, 1}, {1, 1}};
    const std::vector<int> expected = {1, 1};
    ctx.expect_eq(sol.gridIllumination(5, lamps, queries), expected);
  });

  ctx.test("duplicate_lamps_deduplicated", [&] {
    // Repeated coordinates must count as a single lamp so one extinguish
    // completely removes its illumination.
    const std::vector<std::vector<int>> lamps = {{0, 0}, {0, 0}};
    const std::vector<std::vector<int>> queries = {{0, 0}, {0, 1}};
    const std::vector<int> expected = {1, 0};
    ctx.expect_eq(sol.gridIllumination(5, lamps, queries), expected);
  });

  ctx.test("extinguish_entire_3x3_cluster", [&] {
    // All three lamps lie within the 3x3 neighborhood of [2, 2].
    const std::vector<std::vector<int>> lamps = {{2, 2}, {2, 3}, {3, 3}};
    const std::vector<std::vector<int>> queries = {{2, 2}, {4, 4}};
    const std::vector<int> expected = {1, 0};
    ctx.expect_eq(sol.gridIllumination(5, lamps, queries), expected);
  });

  ctx.test("large_sparse_grid_1e9", [&] {
    constexpr int kLargeN = 1'000'000'000;
    const std::vector<std::vector<int>> lamps = {{0, kLargeN - 1},
                                                 {kLargeN - 1, 0}};
    // Anti-diagonal r + c == kLargeN - 1 illuminates (500M, 499999999) without
    // touching either corner lamp; (0, 0) stays lit via row 0 and col 0.
    const std::vector<std::vector<int>> queries = {
        {500'000'000, 499'999'999}, {0, 0}, {1, 1}};
    const std::vector<int> expected = {1, 1, 0};
    ctx.expect_eq(sol.gridIllumination(kLargeN, lamps, queries), expected);
  });

  ctx.test("empty_lamps", [&] {
    const std::vector<std::vector<int>> lamps = {};
    const std::vector<std::vector<int>> queries = {{0, 0}, {2, 3}};
    const std::vector<int> expected = {0, 0};
    ctx.expect_eq(sol.gridIllumination(5, lamps, queries), expected);
  });
}

}  // namespace
