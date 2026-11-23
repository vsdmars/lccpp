// LeetCode 1053: Previous Permutation With One Swap
// https://leetcode.com/problems/previous-permutation-with-one-swap/
//
// Problem:
//   Given an array of positive integers `arr` (not necessarily distinct),
//   return the lexicographically largest permutation that is strictly smaller
//   than `arr` and can be obtained with exactly one swap of two elements
//   `arr[i]` and `arr[j]`. If no smaller permutation exists (the array is
//   already sorted in non-decreasing order), return `arr` unchanged.
//   Constraints: 1 <= arr.length <= 10^4, 1 <= arr[i] <= 10^4.
//
// Algorithm:
//   Greedy Right-to-Left Pivot + Leftmost Best Replacement:
//   1. Scan from right to left to find the rightmost index `pivot_idx` where
//      `arr[pivot_idx] > arr[pivot_idx + 1]`. Every element strictly to the
//      right of `pivot_idx` forms a non-decreasing suffix, so no swap solely
//      within that suffix can make the array lexicographically smaller.
//      Modifying `pivot_idx` is the rightmost possible change, maximizing the
//      shared prefix with `arr`.
//   2. If no such `pivot_idx` exists, `arr` is already the smallest possible
//      permutation; return it unchanged.
//   3. Within the suffix `(pivot_idx, n)`, find the largest value strictly
//      smaller than `arr[pivot_idx]`. When that maximum value appears multiple
//      times, pick its **leftmost** occurrence so the larger displaced value
//      `arr[pivot_idx]` lands as far left as possible, yielding the
//      lexicographically largest result (e.g., [3,1,1,3] -> [1,3,1,3], not
//      [1,1,3,3]). Using strict `<` when updating `swap_idx` naturally keeps
//      the leftmost occurrence.
//   4. Swap `arr[pivot_idx]` and `arr[swap_idx]` and return `arr`.
//   Time Complexity:  O(n) — at most two linear passes over `arr`.
//   Space Complexity: O(1) auxiliary space (mutates the returned copy).
//
// Data Structures:
//   - In-place `std::vector<int>` array with two index cursors (`pivot_idx`,
//     `swap_idx`).
//
// Real-World Applications:
//   - Combinatorial search and backtracking steppers that enumerate
//     lexicographic neighborhoods under a single-transposition budget.
//   - Job scheduling and priority queue tie-breaking adjustments where one
//     pairwise swap yields the closest preceding feasible order.
//   - Ranking and leaderboard perturbation tools that compute the nearest
//     strictly lower ranking reachable in a single transposition.
//
// Related LeetCode Problems:
//   - #31 Next Permutation [done] (lexicographically next permutation with suffix reversal)
//   - #670 Maximum Swap [done] (largest number achievable with at most one swap)
//   - #556 Next Greater Element III [done] (next permutation on decimal digits)
//   - #46 Permutations [done] (generating all permutations)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the lexicographically largest permutation strictly smaller than
  // `arr` reachable in one swap, or returns `arr` unchanged if none exists.
  // Time: O(n), Space: O(1) auxiliary.
  std::vector<int> prevPermOpt1(std::vector<int> arr) {
    const std::size_t n = arr.size();
    if (n < 2uz) {
      return arr;
    }

    // Phase 1: find the rightmost drop `arr[i - 1] > arr[i]`.
    // Everything to the right of `pivot_idx` is non-decreasing.
    std::size_t pivot_idx = n;
    for (std::size_t i = n - 1uz; i > 0uz; --i) {
      if (arr[i - 1uz] > arr[i]) {
        pivot_idx = i - 1uz;
        break;
      }
    }

    // Already sorted in non-decreasing order: no smaller permutation exists.
    if (pivot_idx == n) {
      return arr;
    }

    // Phase 2: find the leftmost occurrence of the largest value strictly less
    // than `arr[pivot_idx]` in the suffix `(pivot_idx, n)`.
    // Because `arr[pivot_idx + 1] < arr[pivot_idx]`, `pivot_idx + 1` is always
    // a valid initial candidate. Using strict `<` preserves the leftmost tie.
    std::size_t swap_idx = pivot_idx + 1uz;
    for (std::size_t i = swap_idx + 1uz; i < n; ++i) {
      if (arr[i] < arr[pivot_idx] && arr[i] > arr[swap_idx]) {
        swap_idx = i;
      }
    }

    std::swap(arr[pivot_idx], arr[swap_idx]);
    return arr;
  }
};

LC_PROBLEM(1053, "Previous Permutation With One Swap", "array", ctx) {
  Solution sol;

  ctx.test("example_1_descending", [&] {
    ctx.expect_eq(sol.prevPermOpt1({3, 2, 1}), std::vector<int>{3, 1, 2});
  });

  ctx.test("example_2_already_smallest", [&] {
    ctx.expect_eq(sol.prevPermOpt1({1, 1, 5}), std::vector<int>{1, 1, 5});
  });

  ctx.test("example_3_non_adjacent_swap", [&] {
    ctx.expect_eq(sol.prevPermOpt1({1, 9, 4, 6, 7}),
                  std::vector<int>{1, 7, 4, 6, 9});
  });

  ctx.test("example_4_duplicate_candidates_choose_leftmost", [&] {
    ctx.expect_eq(sol.prevPermOpt1({3, 1, 1, 3}),
                  std::vector<int>{1, 3, 1, 3});
  });

  ctx.test("build_case_trailing_duplicates", [&] {
    ctx.expect_eq(sol.prevPermOpt1({7, 9, 6, 6, 2, 1, 1, 1}),
                  std::vector<int>{7, 9, 6, 6, 1, 2, 1, 1});
  });

  ctx.test("edge_cases_single_and_all_equal", [&] {
    ctx.expect_eq(sol.prevPermOpt1({5}), std::vector<int>{5});
    ctx.expect_eq(sol.prevPermOpt1({4, 4, 4}), std::vector<int>{4, 4, 4});
  });
}

}  // namespace
