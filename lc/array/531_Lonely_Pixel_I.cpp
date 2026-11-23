// LeetCode 531: Lonely Pixel I
// https://leetcode.com/problems/lonely-pixel-i/
//
// Problem:
//   Given an `m x n` 2D grid `picture` consisting of black (`'B'`) and white
//   (`'W'`) pixels, return the number of black lonely pixels. A black lonely
//   pixel is a `'B'` at position `(r, c)` such that row `r` and column `c`
//   contain no other `'B'` pixels.
//
// Algorithm:
//   Use two passes with per-row and per-column frequency tables:
//     1. Counting pass: Scan the entire grid once and tally the number of `'B'`
//        pixels in each row (`row_black[r]`) and each column (`col_black[c]`).
//     2. Verification pass: For every row `r` with `row_black[r] == 1`, scan
//        its columns `c`; if `picture[r][c] == 'B'` and `col_black[c] == 1`,
//        increment the lonely pixel count (and break out of row `r` early
//        since row `r` has only one `'B'`).
//
//   This improves upon directional ray-casting (O(m * n * (m + n)) and
//   mutating the grid) to optimal O(m * n) time without modifying `picture`.
//
//   Time Complexity:  O(m * n) — at most two scans of the `m x n` grid.
//   Space Complexity: O(m + n) — row and column black-pixel count arrays.
//
// Data Structures:
//   - `std::vector<int> row_black`: length `m`, count of `'B'` pixels per row.
//   - `std::vector<int> col_black`: length `n`, count of `'B'` pixels per col.
//
// Real-World Applications:
//   - Detecting isolated defective pixels (hot/dead pixels) in camera sensor
//     calibration matrices and display-panel QA grids.
//   - Identifying non-conflicting singleton assignments in bipartite matching
//     or scheduling incidence matrices (unique row/column entries).
//   - Finding isolated non-zero pivots in sparse matrix Gaussian elimination.
//
// Related LeetCode Problems:
//   - #533 Lonely Pixel II [done] (adds exact row-equality constraint for N black pixels per row/col)
//   - #1582 Special Positions in a Binary Matrix [new] (identical problem formulated with 1s and 0s)
//   - #1267 Count Servers that Communicate [new] (complementary row/column degree counting on a grid)
//   - #73 Set Matrix Zeroes [done] (row and column summary tracking on a 2D matrix)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Counts black pixels `'B'` that are the sole `'B'` in both their row and
  // their column.
  //
  // Invariant: After the counting pass, `row_black[r]` equals the total number
  // of `'B'` cells in row `r`, and `col_black[c]` equals the total number of
  // `'B'` cells in column `c`.
  //
  // Time: O(m * n), Space: O(m + n).
  int findLonelyPixel(const std::vector<std::vector<char>>& picture) {
    if (picture.empty() || picture[0].empty()) {
      return 0;
    }

    const std::size_t rows = picture.size();
    const std::size_t cols = picture[0].size();

    std::vector<int> row_black(rows, 0);
    std::vector<int> col_black(cols, 0);

    // Pass 1: Tally black pixels per row and per column.
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (picture[r][c] == 'B') {
          ++row_black[r];
          ++col_black[c];
        }
      }
    }

    // Pass 2: A cell (r, c) is lonely iff it is 'B' and both its row and
    // column counts equal 1.
    int lonely = 0;
    for (std::size_t r = 0uz; r < rows; ++r) {
      if (row_black[r] != 1) {
        continue;
      }
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (picture[r][c] == 'B') {
          if (col_black[c] == 1) {
            ++lonely;
          }
          // Row `r` contains only one 'B', so no further columns need checking.
          break;
        }
      }
    }

    return lonely;
  }
};

LC_PROBLEM(531, "Lonely Pixel I", "array", ctx) {
  ctx.test("example_1_all_three_lonely", [&] {
    Solution sol;
    const std::vector<std::vector<char>> picture = {
        {'W', 'W', 'B'},
        {'W', 'B', 'W'},
        {'B', 'W', 'W'},
    };
    ctx.expect_eq(sol.findLonelyPixel(picture), 3);
  });

  ctx.test("python_docstring_variant_one_lonely", [&] {
    Solution sol;
    const std::vector<std::vector<char>> picture = {
        {'W', 'W', 'B'},
        {'W', 'B', 'B'},
        {'B', 'W', 'W'},
    };
    ctx.expect_eq(sol.findLonelyPixel(picture), 1);
  });

  ctx.test("python_build_none_lonely", [&] {
    Solution sol;
    const std::vector<std::vector<char>> picture = {
        {'W', 'W', 'B'},
        {'W', 'B', 'B'},
        {'B', 'W', 'B'},
    };
    ctx.expect_eq(sol.findLonelyPixel(picture), 0);
  });

  ctx.test("all_black_and_all_white", [&] {
    Solution sol;
    const std::vector<std::vector<char>> all_black = {
        {'B', 'B'},
        {'B', 'B'},
    };
    const std::vector<std::vector<char>> all_white = {
        {'W', 'W'},
        {'W', 'W'},
    };
    ctx.expect_eq(sol.findLonelyPixel(all_black), 0);
    ctx.expect_eq(sol.findLonelyPixel(all_white), 0);
  });

  ctx.test("single_cell_and_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.findLonelyPixel({{'B'}}), 1);
    ctx.expect_eq(sol.findLonelyPixel({{'W'}}), 0);
    ctx.expect_eq(sol.findLonelyPixel({}), 0);
  });
}

}  // namespace
