// LeetCode 378: Kth Smallest Element In A Sorted Matrix
// https://leetcode.com/problems/kth-smallest-element-in-a-sorted-matrix/
//
// Problem:
//   Given an n x n matrix where each row and each column is sorted in
//   non-decreasing order, return the k-th smallest element in the matrix
//   (1-indexed in sorted order, counting duplicates rather than distinct
//   values).
//
// Examples:
//   Example 1:
//     Input:  matrix = [[1, 5, 9], [10, 11, 13], [12, 13, 15]], k = 8
//     Output: 13
//     Explanation:
//       - Initialize the value-space binary search bounds to lo = 1 and hi = 15
//         with target = 8.
//       - At mid = 8, count_less_or_equal sums upper_bound per row to get 2 + 0 + 0
//         = 2 < 8, so advance lo = 9.
//       - At mid = 12, count_less_or_equal sums 3 + 2 + 1 = 6 < 8, so advance lo =
//         13.
//       - At mid = 14 (count 3 + 3 + 2 = 8 >= 8) and then mid = 13 (count 3 + 3 + 2
//         = 8 >= 8), shrink hi to 14 and then 13.
//       - The loop terminates with lo == hi == 13 and returns 13.
//
//   Example 2:
//     Input:  matrix = [[1, 5, 9], [10, 11, 11], [11, 13, 15]], k = 6
//     Output: 11
//     Explanation:
//       - Initialize value-space bounds to lo = 1 and hi = 15 with target = 6.
//       - At mid = 8, row counts sum to 2 + 0 + 0 = 2 < 6, advancing lo = 9; at mid
//         = 12, row counts sum to 3 + 3 + 1 = 7 >= 6, shrinking hi = 12.
//       - At mid = 10, row counts sum to 3 + 1 + 0 = 4 < 6, advancing lo = 11.
//       - At mid = 11, row counts sum to 3 + 3 + 1 = 7 >= 6, shrinking hi = 11 so
//         lo == hi == 11 and returning 11.
//
//   Example 3:
//     Input:  matrix = [[-5]], k = 1
//     Output: -5
//     Explanation:
//       - Initialize lo = matrix[0][0] = -5 and hi = matrix[0][0] = -5.
//       - Because lo < hi is false, the binary search loop executes zero iterations
//         and returns lo = -5.
//
// Algorithm:
//   1. Binary Search on Value Space (primary):
//      The answer lies in the closed value interval [matrix[0][0],
//      matrix[n - 1][n - 1]]. For a candidate midpoint value `mid`, we count
//      how many entries in the matrix are <= mid by running `upper_bound` on
//      each sorted row. If fewer than k entries are <= mid, the k-th smallest
//      value must be strictly greater than mid (`lo = mid + 1`); otherwise it
//      is at most mid (`hi = mid`). When the interval collapses (`lo == hi`),
//      `lo` is guaranteed to be an element present in the matrix and equal to
//      the k-th smallest value.
//      Time:  O(n * log(n) * log(max - min))
//      Space: O(1)
//
//   2. K-Way Min-Heap Merge (heap alternative):
//      Treat the first min(n, k) rows as sorted streams. Seed a min-heap with
//      the first element of each row, then pop the smallest element k times;
//      each pop advances that row's pointer by one column and pushes the next
//      element if one exists.
//      Time:  O(min(n, k) + k * log(min(n, k)))
//      Space: O(min(n, k))
//
// Data Structures:
//   - Binary search bounds (64-bit signed integers to avoid overflow when
//     computing midpoints over wide signed ranges).
//   - Min-heap (`std::priority_queue` with `std::greater<>`) of (value, row,
//     col) entries for k-way sorted stream merging.
//
// Real-World Applications:
//   - Distributed database query engines computing exact quantiles/percentiles
//     across sorted partitions or shards without full materialization.
//   - Multi-way external merge sort (LSM-tree compaction, MapReduce shuffle)
//     selecting the top-k smallest keys from sorted runs.
//   - Image and scientific grid processing where monotonic 2D lookup tables
//     require fast rank queries.
//
// Related LeetCode Problems:
//   - #23 Merge k Sorted Lists [done] (k-way min-heap merge over linked lists)
//   - #373 Find K Pairs with Smallest Sums [done] (implicit sorted matrix top-k)
//   - #668 Kth Smallest Number in Multiplication Table [new] (binary search on value space)
//   - #719 Find K-th Smallest Pair Distance [new] (value-space binary search + counting)
//   - #215 Kth Largest Element in an Array [done] (order statistics / selection)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <queue>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the k-th smallest element via binary search over the matrix's
  // value range [min, max], counting elements <= mid per sorted row.
  // Time: O(n * log(n) * log(max - min)), Space: O(1).
  int kthSmallest(const std::vector<std::vector<int>>& matrix, int k) {
    if (matrix.empty() || matrix[0].empty() || k <= 0) {
      return -1;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0].size();
    // Use 64-bit bounds so `hi - lo` cannot overflow even across full int32
    // range and `lo + ((hi - lo) / 2)` always rounds toward -infinity.
    std::int64_t lo = matrix[0][0];
    std::int64_t hi = matrix[rows - 1][cols - 1];
    const auto target = static_cast<std::int64_t>(k);

    while (lo < hi) {
      const std::int64_t mid = lo + ((hi - lo) / 2);
      const std::int64_t count = count_less_or_equal(matrix, mid);

      // Invariant: the k-th smallest element always lies in [lo, hi].
      if (count < target) {
        lo = mid + 1;
      } else {
        hi = mid;
      }
    }

    return static_cast<int>(lo);
  }

  // Finds the k-th smallest element using a k-way min-heap merge across the
  // sorted rows of the matrix.
  // Time: O(min(n, k) + k * log(min(n, k))), Space: O(min(n, k)).
  [[nodiscard]] int kthSmallestHeap(
      const std::vector<std::vector<int>>& matrix, int k) const {
    if (matrix.empty() || matrix[0].empty() || k <= 0) {
      return -1;
    }

    struct HeapEntry {
      int value = 0;
      std::size_t row = 0;
      std::size_t col = 0;

      [[nodiscard]] bool operator>(const HeapEntry& other) const noexcept {
        return value > other.value;
      }
    };

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0].size();
    const auto steps = static_cast<std::size_t>(k);

    // At most min(rows, k) rows can contribute to the first k elements because
    // column 0 is itself sorted in non-decreasing order.
    const std::size_t seed_rows = std::min(rows, steps);
    std::vector<HeapEntry> initial;
    initial.reserve(seed_rows);
    for (std::size_t r = 0uz; r < seed_rows; ++r) {
      initial.push_back(HeapEntry{.value = matrix[r][0], .row = r, .col = 0});
    }

    // Build the initial min-heap in O(seed_rows) time.
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<>>
        min_heap(std::greater<>{}, std::move(initial));

    int answer = -1;
    for (std::size_t popped = 0uz; popped < steps && !min_heap.empty();
         ++popped) {
      const HeapEntry curr = min_heap.top();
      min_heap.pop();
      answer = curr.value;

      // Advance to the next element in the same sorted row if available.
      const std::size_t next_col = curr.col + 1;
      if (next_col < cols) {
        min_heap.push(HeapEntry{
            .value = matrix[curr.row][next_col],
            .row = curr.row,
            .col = next_col,
        });
      }
    }

    return answer;
  }

 private:
  // Counts how many matrix entries are <= `limit` using binary search on each
  // sorted row (`std::ranges::upper_bound` finds the first element > limit).
  [[nodiscard]] static std::int64_t count_less_or_equal(
      const std::vector<std::vector<int>>& matrix, std::int64_t limit) {
    std::int64_t count = 0;
    const auto key = static_cast<int>(limit);
    for (const auto& row : matrix) {
      const auto it = std::ranges::upper_bound(row, key);
      count += static_cast<std::int64_t>(std::distance(row.begin(), it));
    }
    return count;
  }
};

LC_PROBLEM(378, "Kth Smallest Element In A Sorted Matrix", "heap", ctx) {
  ctx.test("example_1_docstring", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {1, 5, 9},
        {10, 11, 13},
        {12, 13, 15},
    };
    ctx.expect_eq(sol.kthSmallest(matrix, 8), 13);
    ctx.expect_eq(sol.kthSmallestHeap(matrix, 8), 13);
  });

  ctx.test("python_build_with_duplicates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {1, 5, 9},
        {10, 11, 11},
        {11, 13, 15},
    };
    ctx.expect_eq(sol.kthSmallest(matrix, 8), 13);
    ctx.expect_eq(sol.kthSmallestHeap(matrix, 8), 13);
    ctx.expect_eq(sol.kthSmallest(matrix, 6), 11);
    ctx.expect_eq(sol.kthSmallestHeap(matrix, 6), 11);
  });

  ctx.test("single_element_negative", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {{-5}};
    ctx.expect_eq(sol.kthSmallest(matrix, 1), -5);
    ctx.expect_eq(sol.kthSmallestHeap(matrix, 1), -5);
  });

  ctx.test("extremes_and_negative_values", [&] {
    Solution sol;
    const std::vector<std::vector<int>> matrix = {
        {-10, -4, 0},
        {-5, -1, 3},
        {-2, 2, 7},
    };
    ctx.expect_eq(sol.kthSmallest(matrix, 1), -10);
    ctx.expect_eq(sol.kthSmallestHeap(matrix, 1), -10);
    ctx.expect_eq(sol.kthSmallest(matrix, 5), -1);
    ctx.expect_eq(sol.kthSmallestHeap(matrix, 5), -1);
    ctx.expect_eq(sol.kthSmallest(matrix, 9), 7);
    ctx.expect_eq(sol.kthSmallestHeap(matrix, 9), 7);
  });
}

}  // namespace
