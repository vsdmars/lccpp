// LeetCode 40: Combination Sum II
// https://leetcode.com/problems/combination-sum-ii/
//
// Problem:
//   Given a collection of positive integer `candidates` (which may contain
//   duplicate values) and a positive integer `target`, find all unique
//   combinations in `candidates` whose numbers sum to `target`. Each element
//   in `candidates` may be used at most once in a combination, and the solution
//   set must not contain duplicate combinations.
//   Constraints: 1 <= candidates.length <= 100, 1 <= candidates[i] <= 50,
//   1 <= target <= 30.
//
// Algorithm:
//   Sort `candidates` ascending, then run backtracking DFS with level-based
//   deduplication and suffix pruning:
//     - At recursion state `(start, remaining)`, iterate `i` from `start` to
//       `candidates.size() - 1`.
//     - Duplicate suppression: if `i > start && candidates[i] == candidates[i - 1]`,
//       skip `i`. This ensures equal values can still be chosen together at
//       *deeper* recursion levels (e.g., [1, 1, 6]), while never spawning
//       identical sibling branches at the *same* recursion level (avoiding
//       duplicate [1, 2, 5] combinations).
//     - Early pruning: since `candidates` is sorted, break immediately once
//       `candidates[i] > remaining`.
//     - Advance `start` to `i + 1` in the recursive call so each index is used
//       at most once.
//   Time Complexity:  O(2^n * n) worst case (at most 2^n subsets, O(n) to copy
//                     each valid combination), heavily pruned in practice.
//   Space Complexity: O(n) recursion depth and path buffer (excluding output).
//
// Data Structures:
//   - Sorted `std::vector<int>`: clusters duplicate values adjacently for O(1)
//     sibling deduplication and enables monotonic early exit.
//   - Path stack (`std::vector<int>`): maintains the active subset via O(1)
//     push_back / pop_back without copying intermediate prefixes.
//   - Result list (`std::vector<std::vector<int>>`): stores all unique valid
//     combinations.
//
// Real-World Applications:
//   - Invoice-to-payment reconciliation where duplicate line-item amounts exist
//     in an accounts-receivable ledger and each invoice may be applied once.
//   - Knapsack / cargo-loading enumeration with finite inventory multiplicities
//     (matching exact weight or volume targets without redundant manifests).
//   - Financial order-book fill matching against discrete resting lots.
//   - Test-case and feature-subset selection under strict cost budgets.
//
// Related LeetCode Problems:
//   - #39 Combination Sum [done] (unlimited reuse of distinct candidates)
//   - #77 Combinations [done] (fixed-size k-subsets without duplicates)
//   - #78 Subsets [done] (all subsets of distinct elements)
//   - #90 Subsets II [done] (same sort + same-level duplicate skipping pattern)
//   - #216 Combination Sum III [new] (fixed count k from digits 1..9)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all unique combinations of `candidates` (each element used at most
  // once) that sum to `target`. Takes `candidates` by value to sort in place.
  // Time: O(2^n * n) worst case, Space: O(n) auxiliary.
  std::vector<std::vector<int>> combinationSum2(std::vector<int> candidates,
                                                int target) {
    if (target < 0) {
      return {};
    }

    // Sorting groups identical values adjacently and enables early suffix breaks.
    std::ranges::sort(candidates);

    std::vector<std::vector<int>> results;
    std::vector<int> current;

    // DFS invariant: `current` contains elements chosen from indices `< start`
    // summing to `target - remaining`.
    auto dfs = [&](this auto&& self, std::size_t start, int remaining) -> void {
      if (remaining == 0) {
        results.push_back(current);
        return;
      }

      for (std::size_t i = start; i < candidates.size(); ++i) {
        // Skip duplicate values at the same tree depth while still allowing
        // duplicates along a root-to-leaf path (when `i == start`).
        if (i > start && candidates[i] == candidates[i - 1]) {
          continue;
        }
        if (candidates[i] > remaining) {
          // Remaining candidates are >= candidates[i] > remaining.
          break;
        }

        current.push_back(candidates[i]);
        // Pass `i + 1` so each element in `candidates` is used at most once.
        self(i + 1, remaining - candidates[i]);
        current.pop_back();
      }
    };

    dfs(0uz, target);
    return results;
  }
};

LC_PROBLEM(40, "Combination Sum II", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_with_duplicates", [&] {
    const std::vector<std::vector<int>> expected = {
        {1, 1, 6},
        {1, 2, 5},
        {1, 7},
        {2, 6},
    };
    ctx.expect_eq(sol.combinationSum2({10, 1, 2, 7, 6, 1, 5}, 8), expected);
  });

  ctx.test("example_2_repeated_twos", [&] {
    const std::vector<std::vector<int>> expected = {
        {1, 2, 2},
        {5},
    };
    ctx.expect_eq(sol.combinationSum2({2, 5, 2, 1, 2}, 5), expected);
  });

  ctx.test("all_identical_elements", [&] {
    const std::vector<std::vector<int>> expected = {{1, 1}};
    ctx.expect_eq(sol.combinationSum2({1, 1, 1, 1}, 2), expected);
  });

  ctx.test("no_valid_combination", [&] {
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.combinationSum2({3, 4, 5}, 2), expected);
  });

  ctx.test("exact_full_array_sum", [&] {
    const std::vector<std::vector<int>> expected = {{1, 2, 3}};
    ctx.expect_eq(sol.combinationSum2({3, 1, 2}, 6), expected);
  });
}

}  // namespace
