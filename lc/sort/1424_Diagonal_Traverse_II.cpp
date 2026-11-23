// LeetCode 1424: Diagonal Traverse II
// https://leetcode.com/problems/diagonal-traverse-ii/
//
// Problem:
//   Given a 2D ragged integer array `nums` (rows may have different lengths),
//   return all elements of `nums` in anti-diagonal order: diagonals are visited
//   in increasing order of `(row + col)`, and within each diagonal elements are
//   emitted from bottom-left to top-right (decreasing `row`).
//   Constraints: 1 <= nums.length <= 10^5, 1 <= nums[i].length <= 10^5,
//   total number of elements N <= 10^5, 1 <= nums[i][j] <= 10^9.
//
// Algorithm:
//   All cells `(r, c)` on the same anti-diagonal share the invariant sum
//   `d = r + c`. Because every row is non-empty, the occupied diagonal indices
//   form a contiguous range `0, 1, ..., max_diag`.
//   1. Scan rows top-to-bottom (`r = 0 .. rows - 1`) and columns left-to-right
//      (`c = 0 .. nums[r].size() - 1`), appending `nums[r][c]` to bucket
//      `diagonals[r + c]`. Within each bucket, elements are naturally ordered
//      by increasing `r` (top-to-bottom).
//   2. Iterate through `diagonals` in ascending order of `d`, appending each
//      bucket in reverse order (bottom-to-top, i.e., decreasing `r`) to the
//      flat output vector.
//   Using a flat `std::vector<std::vector<int>>` instead of a hash map exploits
//   the dense key range for cache-friendly O(1) bucket indexing without hashing.
//   Time Complexity:  O(N), where N is the total number of integers in `nums`.
//   Space Complexity: O(N) to group elements into diagonal buckets.
//
// Data Structures:
//   - `std::vector<std::vector<int>>`: direct-addressed bucket array indexed by
//     anti-diagonal ID `r + c`.
//   - `std::vector<int>`: pre-reserved result buffer holding the flattened
//     diagonal traversal.
//
// Real-World Applications:
//   - JPEG / MPEG entropy coding (zig-zag and diagonal scan ordering of 2D DCT
//     frequency coefficient blocks).
//   - Wavefront (hyperplane) parallel scheduling of 2D dynamic programming
//     tables (e.g., Smith-Waterman sequence alignment) where all cells with
//     equal `r + c` have no mutual data dependencies and can execute in parallel.
//   - Processing sparse or ragged time-series matrices along constant-lag
//     diagonals.
//
// Related LeetCode Problems:
//   - #498 Diagonal Traverse [done] (alternating-direction diagonal traversal of a rectangular matrix)
//   - #1329 Sort The Matrix Diagonally [done] (groups elements by main-diagonal invariant `r - c`)
//   - #54 Spiral Matrix [done] (coordinate-based 2D matrix traversal)
//   - #766 Toeplitz Matrix [done] (verifies constant values along every `r - c` diagonal)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all elements of the ragged 2D array `nums` in bottom-left to
  // top-right anti-diagonal order.
  // Time: O(N), Space: O(N), where N is the total number of elements.
  std::vector<int> findDiagonalOrder(const std::vector<std::vector<int>>& nums) {
    std::vector<std::vector<int>> diagonals;
    std::size_t total_elements = 0uz;

    // Group elements by their anti-diagonal key (r + c). Scanning r from 0
    // upward stores each diagonal top-to-bottom.
    for (std::size_t r = 0uz; r < nums.size(); ++r) {
      total_elements += nums[r].size();
      for (std::size_t c = 0uz; c < nums[r].size(); ++c) {
        const std::size_t diag_id = r + c;
        if (diag_id >= diagonals.size()) {
          diagonals.resize(diag_id + 1uz);
        }
        diagonals[diag_id].push_back(nums[r][c]);
      }
    }

    // Flatten diagonals in ascending diag_id order, reversing each bucket so
    // elements appear bottom-to-top (decreasing row index).
    std::vector<int> result;
    result.reserve(total_elements);
    for (const auto& diag : diagonals) {
      result.insert(result.end(), diag.rbegin(), diag.rend());
    }
    return result;
  }
};

LC_PROBLEM(1424, "Diagonal Traverse II", "sort", ctx) {
  Solution sol;

  ctx.test("example1_square_matrix", [&] {
    const std::vector<std::vector<int>> nums = {
        {1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    const std::vector<int> expected = {1, 4, 2, 7, 5, 3, 8, 6, 9};
    ctx.expect_eq(sol.findDiagonalOrder(nums), expected);
  });

  ctx.test("example2_ragged_matrix", [&] {
    const std::vector<std::vector<int>> nums = {{1, 2, 3, 4, 5},
                                                {6, 7},
                                                {8},
                                                {9, 10, 11},
                                                {12, 13, 14, 15, 16}};
    const std::vector<int> expected = {1, 6,  2,  8,  7,  3,  9,  4,
                                       12, 10, 5,  13, 11, 14, 15, 16};
    ctx.expect_eq(sol.findDiagonalOrder(nums), expected);
  });

  ctx.test("example3_alternating_row_lengths", [&] {
    const std::vector<std::vector<int>> nums = {
        {1, 2, 3}, {4}, {5, 6, 7}, {8}, {9, 10, 11}};
    const std::vector<int> expected = {1, 4, 2, 5, 3, 8, 6, 9, 7, 10, 11};
    ctx.expect_eq(sol.findDiagonalOrder(nums), expected);
  });

  ctx.test("example4_single_row", [&] {
    const std::vector<std::vector<int>> nums = {{1, 2, 3, 4, 5, 6}};
    const std::vector<int> expected = {1, 2, 3, 4, 5, 6};
    ctx.expect_eq(sol.findDiagonalOrder(nums), expected);
  });

  ctx.test("single_column_and_single_cell", [&] {
    ctx.expect_eq(sol.findDiagonalOrder({{1}, {2}, {3}, {4}}),
                  std::vector<int>{1, 2, 3, 4});
    ctx.expect_eq(sol.findDiagonalOrder({{7}}), std::vector<int>{7});
  });
}

}  // namespace
