// LeetCode 533: Lonely Pixel II
// https://leetcode.com/problems/lonely-pixel-ii/
//
// Problem:
//   Given an M x N matrix `picture` consisting of black ('B') and white ('W')
//   pixels and a positive integer `target`, return the number of black pixels
//   located at some row R and column C that satisfy BOTH rules:
//     1. Row R and column C each contain exactly `target` black pixels.
//     2. Every row that has a black pixel at column C is identical to row R.
//
// Algorithm:
//   Key structural invariant:
//     Suppose pixel (R, C) is valid. By Rule 1, column C contains exactly
//     `target` black pixels; by Rule 2, all `target` rows with 'B' at column C
//     must be identical to row R. Moreover, no extra copy of row R can exist
//     (otherwise column C would contain more than `target` black pixels).
//     Conversely, if a row pattern S has exactly `target` black pixels,
//     appears exactly `target` times in `picture`, and column C has S[C] == 'B'
//     with total column black count == `target`, then those `target` identical
//     rows account for every 'B' in column C, making all `target` pixels at
//     column C valid.
//   Implementation:
//     1. Single pass over `picture`: count black pixels per column in
//        `col_black[c]`, and for each row containing exactly `target` black
//        pixels, serialize it as a string signature and increment its frequency
//        in `row_pattern_count`.
//     2. Iterate over each distinct signature in `row_pattern_count` whose
//        occurrence count equals `target`. For every column c where
//        signature[c] == 'B' and `col_black[c] == target`, add `target` to the
//        answer.
//   Time Complexity:  O(M * N) to scan the matrix and evaluate unique patterns.
//   Space Complexity: O(M * N) for storing qualifying row patterns in the hash
//                     map plus O(N) for column black-pixel counts.
//
// Data Structures:
//   - std::vector<int> col_black: stores the number of 'B' pixels per column.
//   - std::unordered_map<std::string, int> row_pattern_count: maps each row
//     signature (having exactly `target` black pixels) to its row multiplicity.
//
// Real-World Applications:
//   - Optical inspection and semiconductor wafer defect analysis identifying
//     repeating row/column anomaly signatures across sensor matrices.
//   - Database table integrity and schema mining detecting exact duplicate
//     attribute patterns constrained by column cardinality.
//   - Image compression and binary pattern matching grouping identical scan
//     lines via canonical row hashing.
//
// Related LeetCode Problems:
//   - #531 Lonely Pixel I [done] (simpler variant with target = 1 and no row-equality rule)
//   - #1582 Special Positions in a Binary Matrix [new] (unique 1s in both row and column)
//   - #1072 Flip Columns For Maximum Number of Equal Rows [new] (canonical row signature hashing)
//   - #49 Group Anagrams [done] (grouping items by canonical string keys in a hash map)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Counts black pixels (R, C) where row R and col C each have `target` black
  // pixels and all rows with 'B' at col C are identical to row R.
  // Time: O(M * N), Space: O(M * N).
  int findBlackPixel(const std::vector<std::vector<char>>& picture,
                     int target) {
    if (target <= 0 || picture.empty() || picture[0].empty()) {
      return 0;
    }

    const std::size_t cols = picture[0].size();
    std::vector<int> col_black(cols, 0);
    // Maps a row signature (only for rows with exactly `target` black pixels)
    // to how many times that exact row appears in the picture.
    std::unordered_map<std::string, int> row_pattern_count;

    // Phase 1: Count black pixels per column and record candidate row patterns.
    for (const auto& row : picture) {
      int row_black = 0;
      std::string signature;
      signature.reserve(cols);
      for (std::size_t c = 0uz; c < cols; ++c) {
        signature.push_back(row[c]);
        if (row[c] == 'B') {
          ++row_black;
          ++col_black[c];
        }
      }
      // Only rows with exactly `target` black pixels can satisfy Rule 1.
      if (row_black == target) {
        ++row_pattern_count[signature];
      }
    }

    // Phase 2: A distinct row pattern yields valid lonely pixels at column c
    // iff the pattern occurs `target` times, has 'B' at column c, and column c
    // has no other 'B' outside those `target` identical rows.
    int total = 0;
    for (const auto& [signature, multiplicity] : row_pattern_count) {
      if (multiplicity != target) {
        continue;
      }
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (signature[c] == 'B' && col_black[c] == target) {
          total += target;
        }
      }
    }

    return total;
  }
};

LC_PROBLEM(533, "Lonely Pixel II", "hash", ctx) {
  ctx.test("example_six_lonely_pixels", [&] {
    Solution sol;
    const std::vector<std::vector<char>> picture = {
        {'W', 'B', 'W', 'B', 'B', 'W'},
        {'W', 'B', 'W', 'B', 'B', 'W'},
        {'W', 'B', 'W', 'B', 'B', 'W'},
        {'W', 'W', 'B', 'W', 'B', 'W'},
    };
    ctx.expect_eq(sol.findBlackPixel(picture, 3), 6);
  });

  ctx.test("mismatched_rows_yield_zero", [&] {
    Solution sol;
    // Matches the first build() grid in the Python source: column 1 has 3 'B's
    // in rows 0, 1, 2, but row 0 differs from rows 1 and 2.
    const std::vector<std::vector<char>> picture = {
        {'W', 'B', 'W', 'B', 'B', 'W'},
        {'W', 'B', 'B', 'W', 'B', 'W'},
        {'W', 'B', 'B', 'W', 'B', 'W'},
        {'W', 'W', 'B', 'W', 'B', 'W'},
    };
    ctx.expect_eq(sol.findBlackPixel(picture, 3), 0);
  });

  ctx.test("target_one_reduces_to_lonely_pixel_i", [&] {
    Solution sol;
    const std::vector<std::vector<char>> picture = {
        {'W', 'W', 'B'},
        {'W', 'B', 'W'},
        {'B', 'W', 'W'},
    };
    ctx.expect_eq(sol.findBlackPixel(picture, 1), 3);
  });

  ctx.test("all_white_or_too_many_identical_rows", [&] {
    Solution sol;
    const std::vector<std::vector<char>> all_white = {
        {'W', 'W'},
        {'W', 'W'},
    };
    ctx.expect_eq(sol.findBlackPixel(all_white, 1), 0);

    // Three identical rows with 2 'B's each: column counts are 3 != 2.
    const std::vector<std::vector<char>> extra_copy = {
        {'B', 'B', 'W'},
        {'B', 'B', 'W'},
        {'B', 'B', 'W'},
    };
    ctx.expect_eq(sol.findBlackPixel(extra_copy, 2), 0);
  });

  ctx.test("empty_and_non_positive_target", [&] {
    Solution sol;
    const std::vector<std::vector<char>> empty;
    const std::vector<std::vector<char>> single = {{'B'}};
    ctx.expect_eq(sol.findBlackPixel(empty, 1), 0);
    ctx.expect_eq(sol.findBlackPixel(single, 0), 0);
  });
}

}  // namespace
