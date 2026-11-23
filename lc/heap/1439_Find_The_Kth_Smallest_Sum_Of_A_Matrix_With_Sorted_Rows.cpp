// LeetCode 1439: Find The Kth Smallest Sum Of A Matrix With Sorted Rows
// https://leetcode.com/problems/find-the-kth-smallest-sum-of-a-matrix-with-sorted-rows/
//
// Problem:
//   Given an m x n matrix `mat` (1 <= m, n <= 40, 1 <= mat[i][j] <= 5000)
//   whose rows are sorted in non-decreasing order, and an integer `k`
//   (1 <= k <= min(200, n^m)), pick exactly one element from each row to form
//   a sum. Return the 1-indexed `k`-th smallest possible sum among all n^m
//   choices.
//
// Examples:
//   Example 1:
//     Input:  mat = [[1, 3, 11], [2, 4, 6]], k = 5
//     Output: 7
//     Explanation:
//       - Initialize curr from the first min(3, 5) = 3 elements of row 0: curr =
//         [1, 3, 11].
//       - Call merge_top_k(curr, [2, 4, 6], 5), seeding the min-heap with PairSum
//         entries (1+2=3, idx_a=0, idx_b=0), (3+2=5, 1, 0), and (11+2=13, 2, 0).
//       - Pop (3, 0, 0) to record 3 and push (1+4=5, 0, 1); then pop (5, 0, 1) and
//         (5, 1, 0), pushing (1+6=7, 0, 2) and (3+4=7, 1, 1).
//       - Pop the next two sums (7, 0, 2) and (7, 1, 1) to complete curr = [3, 5,
//         5, 7, 7], and return curr[k - 1] = curr[4] = 7.
//
//   Example 2:
//     Input:  mat = [[1, 10, 10], [1, 4, 5], [2, 3, 6]], k = 7
//     Output: 9
//     Explanation:
//       - Initialize curr = [1, 10, 10] from row 0 (min(3, 7) = 3 elements).
//       - Merge curr with row 1 [1, 4, 5] via merge_top_k(limit=7) to obtain the 7
//         smallest two-row sums: curr = [2, 5, 6, 11, 11, 14, 14].
//       - Merge curr with row 2 [2, 3, 6] via merge_top_k(limit=7), extracting sums
//         2+2=4, 2+3=5, 5+2=7, 2+6=8, 5+3=8, 6+2=8, and 6+3=9 into curr = [4, 5, 7,
//         8, 8, 8, 9].
//       - Return curr[k - 1] = curr[6] = 9.
//
//   Example 3:
//     Input:  mat = [[4], [5], [6]], k = 1
//     Output: 15
//     Explanation:
//       - Initialize curr = [4] from the first min(1, 1) = 1 element of row 0.
//       - Merge curr with row 1 [5] using merge_top_k(limit=1), popping the single
//         seeded pair (4+5=9, 0, 0) so curr = [9].
//       - Merge curr with row 2 [6] using merge_top_k(limit=1), popping (9+6=15, 0,
//         0) so curr = [15], and return curr[0] = 15.
//
// Algorithm:
//   Two complementary min-heap solutions are implemented:
//   1. Row-by-Row Top-K Min-Heap Merge (`kthSmallest`, primary):
//      Maintain the `k` smallest prefix sums `curr` formed from the first `r`
//      rows. To fold in `mat[r]`, merge the two sorted lists `curr` and
//      `mat[r]` using a min-heap (identical to LeetCode #373) to extract only
//      the `k` smallest pairwise sums in O(k log(min(k, n))) time. After all
//      `m` rows are folded, `curr[k - 1]` is the answer.
//      Time Complexity:  O(m * k * log(min(k, n))).
//      Space Complexity: O(k).
//   2. Canonical-Order Multi-Row Tuple Min-Heap (`kthSmallestTupleHeap`,
//      direct port of the Python `fast()` approach):
//      Start from `(sum of col 0 across all rows, cols={0..0}, cur_row=0)`. On
//      each of `k` pops, advance `cols[i]` by 1 for each row `i >= cur_row`
//      with `cols[i] + 1 < n` and push with `cur_row = i`. Restricting
//      increments to non-decreasing row indices guarantees every reachable
//      column tuple has a unique parent with sum <= child sum, avoiding a
//      visited hash set entirely.
//      Time Complexity:  O(k * m * (m + log(k * m))).
//      Space Complexity: O(k * m^2) worst-case for queued index vectors.
//
// Data Structures:
//   - Min-heap of `(sum, idx_a, idx_b)` pairs (`std::priority_queue` with
//     `std::greater<>`): merges two sorted arrays to produce their top-k sums.
//   - Sorted prefix-sum `std::vector<int>` (size <= k): carries the DP state
//     between consecutive matrix rows.
//   - Min-heap of `TupleState` (in `kthSmallestTupleHeap`): explores the
//     implicit m-dimensional grid in non-decreasing sum order.
//
// Real-World Applications:
//   - Database multi-way join top-k query execution (Fagin / NRA / rank-join
//     algorithms combining multiple sorted attribute indexes).
//   - Beam search and k-best decoding across sequential factor tables in NLP
//     and speech recognition.
//   - Multi-component cloud/hardware configuration selection minimizing total
//     cost or latency under sorted per-component catalogs.
//
// Related LeetCode Problems:
//   - #373 Find K Pairs with Smallest Sums [done] (2-row base case of row-by-row merge)
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (2D sorted matrix k-th element via heap/binary search)
//   - #786 K-th Smallest Prime Fraction [new] (k-th smallest over implicit sorted combinations)
//   - #23 Merge k Sorted Lists [done] (multi-way sorted merge with min-heap)
//   - #1057 Campus Bikes [done] (multi-source sorted frontier selection with min-heap)

#include "framework/lc_framework.h"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Candidate sum `a[idx_a] + b[idx_b]` in the two-array top-k merge min-heap.
struct PairSum {
  int sum = 0;
  std::size_t idx_a = 0uz;
  std::size_t idx_b = 0uz;

  auto operator<=>(const PairSum&) const = default;
};

// Implicit state in the canonical-order multi-row heap traversal.
struct TupleState {
  int sum = 0;
  std::size_t cur_row = 0uz;
  std::vector<std::size_t> cols;

  auto operator<=>(const TupleState&) const = default;
};

class Solution {
 public:
  // Finds the k-th smallest sum picking one element per row by iteratively
  // merging the running top-k prefix sums with each sorted row via a min-heap.
  //
  // Time Complexity:  O(m * k * log(min(k, n)))
  // Space Complexity: O(k)
  [[nodiscard]] int kthSmallest(const std::vector<std::vector<int>>& mat,
                                int k) {
    if (mat.empty() || mat[0].empty() || k <= 0) {
      return 0;
    }

    const auto limit = static_cast<std::size_t>(k);

    // Initialize running sums with up to the first `k` elements of row 0.
    const std::size_t init_count = std::min(mat[0].size(), limit);
    std::vector<int> curr(mat[0].begin(),
                          mat[0].begin() + static_cast<std::ptrdiff_t>(init_count));

    for (std::size_t r = 1uz; r < mat.size(); ++r) {
      curr = merge_top_k(curr, mat[r], limit);
    }

    return curr[limit - 1uz];
  }

  // Alternative approach (port of Python `fast()`): explores column-index
  // tuples directly with a min-heap, advancing rows in non-decreasing order
  // (`i >= cur_row`) so each tuple is generated at most once without a visited
  // set.
  //
  // Time Complexity:  O(k * m * (m + log(k * m)))
  // Space Complexity: O(k * m^2)
  [[nodiscard]] int kthSmallestTupleHeap(
      const std::vector<std::vector<int>>& mat, int k) {
    if (mat.empty() || mat[0].empty() || k <= 0) {
      return 0;
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();

    int base_sum = 0;
    for (const auto& row : mat) {
      base_sum += row[0];
    }

    std::priority_queue<TupleState, std::vector<TupleState>, std::greater<>>
        min_heap;
    min_heap.push(TupleState{
        .sum = base_sum,
        .cur_row = 0uz,
        .cols = std::vector<std::size_t>(rows, 0uz),
    });

    int current_sum = base_sum;
    for (int step = 0; step < k; ++step) {
      TupleState state = min_heap.top();
      min_heap.pop();
      current_sum = state.sum;

      // Only advance row indices >= `state.cur_row` to establish a unique
      // canonical derivation path for every column-index vector.
      for (std::size_t r = state.cur_row; r < rows; ++r) {
        if (state.cols[r] + 1uz == cols) {
          continue;
        }
        std::vector<std::size_t> next_cols = state.cols;
        const std::size_t prev_c = next_cols[r];
        const std::size_t next_c = prev_c + 1uz;
        next_cols[r] = next_c;

        const int next_sum = (state.sum - mat[r][prev_c]) + mat[r][next_c];
        min_heap.push(TupleState{
            .sum = next_sum,
            .cur_row = r,
            .cols = std::move(next_cols),
        });
      }
    }

    return current_sum;
  }

 private:
  // Merges two non-decreasing arrays `a` and `b` and returns up to `limit`
  // smallest pairwise sums `a[i] + b[j]` in sorted order.
  [[nodiscard]] static std::vector<int> merge_top_k(const std::vector<int>& a,
                                                    const std::vector<int>& b,
                                                    std::size_t limit) {
    const std::size_t seed_count = std::min(a.size(), limit);
    std::vector<PairSum> initial;
    initial.reserve(seed_count);
    for (std::size_t i = 0uz; i < seed_count; ++i) {
      initial.push_back(PairSum{
          .sum = a[i] + b[0],
          .idx_a = i,
          .idx_b = 0uz,
      });
    }

    std::priority_queue<PairSum, std::vector<PairSum>, std::greater<>> min_heap(
        std::greater<>{}, std::move(initial));

    std::vector<int> merged;
    merged.reserve(limit);

    while (!min_heap.empty() && merged.size() < limit) {
      const PairSum top = min_heap.top();
      min_heap.pop();
      merged.push_back(top.sum);

      // Advance along `b` for this `idx_a` row to reveal the next candidate sum.
      if (top.idx_b + 1uz < b.size()) {
        min_heap.push(PairSum{
            .sum = a[top.idx_a] + b[top.idx_b + 1uz],
            .idx_a = top.idx_a,
            .idx_b = top.idx_b + 1uz,
        });
      }
    }

    return merged;
  }
};

LC_PROBLEM(1439, "Find The Kth Smallest Sum Of A Matrix With Sorted Rows",
           "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {{1, 3, 11}, {2, 4, 6}};
    ctx.expect_eq(sol.kthSmallest(mat, 5), 7);
    ctx.expect_eq(sol.kthSmallestTupleHeap(mat, 5), 7);
  });

  ctx.test("example_2_max_k", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {{1, 3, 11}, {2, 4, 6}};
    ctx.expect_eq(sol.kthSmallest(mat, 9), 17);
    ctx.expect_eq(sol.kthSmallestTupleHeap(mat, 9), 17);
  });

  ctx.test("example_3_three_rows", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {
        {1, 10, 10}, {1, 4, 5}, {2, 3, 6}};
    ctx.expect_eq(sol.kthSmallest(mat, 7), 9);
    ctx.expect_eq(sol.kthSmallestTupleHeap(mat, 7), 9);
  });

  ctx.test("example_4_duplicates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat = {{1, 1, 10}, {2, 2, 9}};
    ctx.expect_eq(sol.kthSmallest(mat, 7), 12);
    ctx.expect_eq(sol.kthSmallestTupleHeap(mat, 7), 12);
  });

  ctx.test("single_row_and_k_equals_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> single_row = {{3, 5, 8, 12}};
    ctx.expect_eq(sol.kthSmallest(single_row, 3), 8);
    ctx.expect_eq(sol.kthSmallestTupleHeap(single_row, 3), 8);

    const std::vector<std::vector<int>> single_col = {{4}, {5}, {6}};
    ctx.expect_eq(sol.kthSmallest(single_col, 1), 15);
    ctx.expect_eq(sol.kthSmallestTupleHeap(single_col, 1), 15);
  });
}

}  // namespace
