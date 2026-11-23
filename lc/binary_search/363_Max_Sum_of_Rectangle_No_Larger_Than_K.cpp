// LeetCode 363: Max Sum of Rectangle No Larger Than K
// https://leetcode.com/problems/max-sum-of-rectangle-no-larger-than-k/
//
// Problem:
//   Given an m x n integer matrix and an integer k, return the maximum sum of
//   a non-empty axis-aligned submatrix (rectangle) whose sum is <= k. It is
//   guaranteed that at least one rectangle with sum <= k exists.
//
// Algorithm:
//   2D Strip Compression + 1D Prefix-Sum Balanced BST (with Kadane Fast Path):
//   1. Choose the smaller dimension (`min(m, n)`) as the pair-enumeration axis
//      and the larger dimension (`max(m, n)`) as the 1D strip axis. This
//      directly addresses the follow-up ("What if rows >> columns?") and
//      keeps the quadratic factor on the smaller side.
//   2. For every starting boundary `start` and ending boundary `end >= start`
//      along the smaller axis, accumulate the 1D strip sums across the larger
//      axis in O(max(m, n)) time.
//   3. Find the maximum 1D subarray sum <= k in `strip`:
//        - Fast path (Kadane's algorithm, O(L)): compute the unconstrained
//          maximum subarray sum. If it is already <= k, return it immediately.
//        - Exact path (Ordered Prefix Sums, O(L log L)): maintain a sorted
//          `std::set<std::int64_t>` of prefix sums seen so far (seeded with 0).
//          For each running prefix sum `P`, we want the smallest previous
//          prefix `Q` such that `P - Q <= k`, i.e., `Q >= P - k`. Query
//          `prefix_sums.lower_bound(P - k)`; if found, `P - *it` is the best
//          valid subarray ending here.
//   4. Early-exit immediately if the global maximum reaches `k`.
//   Time: O(min(m, n)^2 * max(m, n) * log(max(m, n))), Space: O(max(m, n)).
//
// Data Structures:
//   - `std::vector<int> strip`: compressed 1D sums across the fixed pair of
//     rows (or columns).
//   - `std::set<std::int64_t> prefix_sums`: balanced BST (red-black tree)
//     supporting O(log L) insertion and `lower_bound` predecessor search.
//
// Real-World Applications:
//   - Budget-constrained 2D region selection in geospatial analytics (finding
//     the highest-value rectangular land parcel or ad-grid placement under a
//     cost cap k).
//   - Image processing and computer vision integral-region matching under
//     energy or brightness budgets.
//   - Risk-capped portfolio or resource allocation across a 2D time-by-sector
//     return matrix.
//
// Related LeetCode Problems:
//   - #53 Maximum Subarray [done] (unconstrained 1D Kadane's algorithm used as our fast path)
//   - #304 Range Sum Query 2D [done] - Immutable (2D prefix sums over submatrices)
//   - #560 Subarray Sum Equals K [done] (1D prefix-sum lookup for exact target k)
//   - #1074 Number of Submatrices That Sum to Target [done] (same 2D strip compression + 1D prefix map)
//   - #327 Count of Range Sum [new] (prefix-sum range queries with ordered structures / merge sort)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <set>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the maximum sum of any non-empty submatrix with sum <= k.
  // Automatically compresses along the smaller dimension to handle tall or
  // wide matrices efficiently.
  // Time: O(min(m, n)^2 * max(m, n) * log(max(m, n))), Space: O(max(m, n)).
  int maxSumSubmatrix(const std::vector<std::vector<int>>& matrix, int k) {
    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix.front().size();

    // Enumerate pairs along the smaller dimension so the O(d^2) loop stays
    // as small as possible (handles rows >> cols and cols >> rows equally well).
    const bool compress_rows = rows <= cols;
    const std::size_t outer = compress_rows ? rows : cols;
    const std::size_t inner = compress_rows ? cols : rows;

    std::int64_t best = std::numeric_limits<std::int64_t>::min();
    std::vector<int> strip(inner, 0);

    for (std::size_t start = 0uz; start < outer; ++start) {
      std::ranges::fill(strip, 0);

      for (std::size_t end = start; end < outer; ++end) {
        // Extend the 1D compressed strip by one row (or one column).
        for (std::size_t i = 0uz; i < inner; ++i) {
          strip[i] += compress_rows ? matrix[end][i] : matrix[i][end];
        }

        best = std::max(best, maxSubarrayNoLargerThanK(strip, k));
        if (best == k) {
          // Cannot exceed k, so exit immediately.
          return k;
        }
      }
    }

    return static_cast<int>(best);
  }

 private:
  // Returns the maximum non-empty subarray sum of `vals` that is <= k, or
  // INT64_MIN if every subarray of `vals` exceeds k.
  // Time: O(L) on Kadane fast path, O(L log L) worst case; Space: O(L).
  static std::int64_t maxSubarrayNoLargerThanK(const std::vector<int>& vals,
                                               int k) {
    // Fast path: if the unconstrained maximum subarray sum (Kadane) is already
    // <= k, it is guaranteed to be the optimal answer for this strip.
    std::int64_t current = 0;
    std::int64_t kadane_best = std::numeric_limits<std::int64_t>::min();
    for (const int val : vals) {
      current = std::max<std::int64_t>(val, current + val);
      kadane_best = std::max(kadane_best, current);
    }
    if (kadane_best <= k) {
      return kadane_best;
    }

    // General path: find the smallest prior prefix sum Q >= prefix - k so that
    // prefix - Q is maximized while remaining <= k.
    std::set<std::int64_t> prefix_sums{0};
    std::int64_t prefix = 0;
    std::int64_t best = std::numeric_limits<std::int64_t>::min();

    for (const int val : vals) {
      prefix += val;
      const auto it = prefix_sums.lower_bound(prefix - k);
      if (it != prefix_sums.end()) {
        best = std::max(best, prefix - *it);
        if (best == k) {
          return k;
        }
      }
      prefix_sums.insert(prefix);
    }

    return best;
  }
};

LC_PROBLEM(363, "Max Sum of Rectangle No Larger Than K", "binary_search", ctx) {
  ctx.test("docstring_example", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {1, 0, 1},
        {0, -2, 3},
    };
    ctx.expect_eq(sol.maxSumSubmatrix(matrix, 2), 2);
  });

  ctx.test("negative_result_and_single_row", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {{2, 2, -1}};
    ctx.expect_eq(sol.maxSumSubmatrix(matrix, 3), 3);
    ctx.expect_eq(sol.maxSumSubmatrix(matrix, 0), -1);
  });

  ctx.test("tall_matrix_rows_greater_than_cols", [&] {
    // Exercises the column-compression branch (rows > cols).
    Solution sol;
    const std::vector<std::vector<int>> tall = {
        {5, -4},
        {-3, -4},
        {5, 1},
        {2, -3},
        {1, 4},
    };
    ctx.expect_eq(sol.maxSumSubmatrix(tall, 3), 3);
    ctx.expect_eq(sol.maxSumSubmatrix(tall, 8), 8);
  });

  ctx.test("complex_mixed_signs", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {5, -4, -3, 4},
        {-3, -4, 4, 5},
        {5, 1, 5, -4},
    };
    ctx.expect_eq(sol.maxSumSubmatrix(matrix, 8), 8);
    ctx.expect_eq(sol.maxSumSubmatrix(matrix, 3), 2);
  });
}

}  // namespace
