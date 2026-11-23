// LeetCode 1074: Number Of Submatrices That Sum To Target
// https://leetcode.com/problems/number-of-submatrices-that-sum-to-target/
//
// Problem:
//   Given an `m x n` integer matrix `matrix` and an integer `target`, return
//   the number of non-empty axis-aligned submatrices whose element sum equals
//   `target`. Two submatrices are distinct if any of their four boundary
//   coordinates `(r1, c1, r2, c2)` differ.
//
// Algorithm:
//   2D-to-1D reduction via row prefix sums + 1D prefix-sum hash map:
//     1. Precompute 1-indexed horizontal prefix sums for every row so that
//        `row_prefix[r][c + 1] = sum(matrix[r][0..c])`. Then the sum of row `r`
//        across any column slice `[left_col, right_col]` is computed in O(1) as
//        `row_prefix[r][right_col + 1] - row_prefix[r][left_col]`.
//     2. Enumerate every pair of column boundaries `0 <= left_col <= right_col
//        < cols`. Fixing the column slice collapses the 2D problem into the 1D
//        "Subarray Sum Equals K" problem (LeetCode #560) along the rows.
//     3. Sweep `r = 0..rows-1`, accumulating `running_sum` and counting prior
//        prefix sums equal to `running_sum - target` in a hash map initialized
//        with `{0: 1}`.
//   Time Complexity: O(rows * cols^2).
//   Space Complexity: O(rows * cols) for the row-prefix table plus O(rows) for
//     the per-slice frequency map.
//
// Data Structures:
//   - 2D `std::vector<std::vector<int>>` storing 1-indexed per-row prefix sums
//     (avoids mutating the input matrix and eliminates left-boundary branches).
//   - `std::unordered_map<int, int>` mapping 1D vertical prefix sums to their
//     occurrence frequencies for O(1) complement lookups.
//
// Real-World Applications:
//   - Computer vision and integral images: counting rectangular image regions
//     whose summed pixel intensity or feature response matches a target energy.
//   - Geospatial and heatmap analytics: locating rectangular map tiles whose
//     aggregate demand, rainfall, or sensor load equals a threshold.
//   - Database OLAP range queries: fast 2D aggregation over precomputed
//     prefix-sum data cubes.
//
// Related LeetCode Problems:
//   - #560 Subarray Sum Equals K [done] (1D core subroutine solved inside every fixed column pair)
//   - #363 Max Sum of Rectangle No Larger Than K [done] (same 2D column-pair reduction using ordered set instead of hash map)
//   - #304 Range Sum Query 2D [done] - Immutable (2D prefix sum fundamentals)
//   - #523 Continuous Subarray Sum [done] (1D prefix sum with hash map)
//   - #1292 Maximum Side Length of a Square with Sum Less than or Equal to Threshold [new] (2D prefix sums with monotonic search)

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Counts all non-empty submatrices summing to `target` by reducing each
  // column slice `[left_col, right_col]` to 1D Subarray Sum Equals K.
  // Time: O(rows * cols^2), Space: O(rows * cols).
  int numSubmatrixSumTarget(const std::vector<std::vector<int>>& matrix,
                            int target) {
    if (matrix.empty() || matrix[0].empty()) {
      return 0;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0].size();

    // Build 1-indexed per-row prefix sums:
    // row_prefix[r][c + 1] = matrix[r][0] + ... + matrix[r][c].
    std::vector<std::vector<int>> row_prefix(
        rows, std::vector<int>(cols + 1uz, 0));
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        row_prefix[r][c + 1uz] = row_prefix[r][c] + matrix[r][c];
      }
    }

    // Enumerate every column slice [left_col, right_col] and count vertical
    // row intervals whose compressed sum equals `target`.
    int total_count = 0;
    for (std::size_t left_col = 0uz; left_col < cols; ++left_col) {
      for (std::size_t right_col = left_col; right_col < cols; ++right_col) {
        total_count +=
            countRowSubarrays(row_prefix, left_col, right_col, target);
      }
    }

    return total_count;
  }

 private:
  // Given a fixed column span [left_col, right_col], treats each row's slice
  // sum as a 1D array element and counts subarrays summing to `target`.
  static int countRowSubarrays(const std::vector<std::vector<int>>& row_prefix,
                               std::size_t left_col, std::size_t right_col,
                               int target) {
    std::unordered_map<int, int> prefix_counts;
    prefix_counts.reserve(row_prefix.size() + 1uz);
    prefix_counts[0] = 1;  // Empty vertical prefix before row 0.

    int running_sum = 0;
    int matches = 0;

    for (const auto& row : row_prefix) {
      running_sum += row[right_col + 1uz] - row[left_col];
      if (const auto it = prefix_counts.find(running_sum - target);
          it != prefix_counts.end()) {
        matches += it->second;
      }
      ++prefix_counts[running_sum];
    }

    return matches;
  }
};

LC_PROBLEM(1074, "Number Of Submatrices That Sum To Target", "sliding_window",
           ctx) {
  ctx.test("leetcode example 1", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.numSubmatrixSumTarget({{0, 1, 0}, {1, 1, 1}, {0, 1, 0}}, 0), 4);
  });

  ctx.test("leetcode example 2", [&] {
    Solution sol;
    ctx.expect_eq(sol.numSubmatrixSumTarget({{1, -1}, {-1, 1}}, 0), 5);
  });

  ctx.test("leetcode example 3 single cell miss", [&] {
    Solution sol;
    ctx.expect_eq(sol.numSubmatrixSumTarget({{904}}, 0), 0);
  });

  ctx.test("python build case", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.numSubmatrixSumTarget({{1, -1, 3}, {-1, 1, 4}, {0, 9, 3}}, 3), 3);
  });

  ctx.test("single cell hit", [&] {
    Solution sol;
    ctx.expect_eq(sol.numSubmatrixSumTarget({{5}}, 5), 1);
  });
}

}  // namespace
