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
// Examples:
//   Example 1:
//     Input:  candidates = [10, 1, 2, 7, 6, 1, 5], target = 8
//     Output: [[1, 1, 6], [1, 2, 5], [1, 7], [2, 6]]
//     Explanation:
//       - Sorting candidates yields [1, 1, 2, 5, 6, 7, 10], and dfs(0, 8) begins at
//         start = 0.
//       - Picking index 0 (1) and index 1 (1, allowed since i == start = 1) leaves
//         remaining = 6, where index 4 (6) hits remaining = 0 and records [1, 1, 6]
//         before 7 > 6 breaks.
//       - Backtracking to [1] (remaining = 7), index 2 (2) pairs with index 3 (5)
//         to record [1, 2, 5], index 4 (6) leaves remaining = 1 and prunes, and
//         index 5 (7) hits remaining = 0 to record [1, 7].
//       - Back at start = 0, index 1 (1) is skipped because i > 0 and candidates[1]
//         == candidates[0], avoiding duplicate branches.
//       - Picking index 2 (2) pairs with index 4 (6) to record [2, 6], and larger
//         starting elements either fail to hit 0 or break at 10 > 8.
//
//   Example 2:
//     Input:  candidates = [2, 5, 2, 1, 2], target = 5
//     Output: [[1, 2, 2], [5]]
//     Explanation:
//       - Sorting produces [1, 2, 2, 2, 5], and dfs(0, 5) starts by picking index 0
//         (1) to recurse with remaining = 4.
//       - Under [1], picking index 1 (2) and index 2 (2, allowed at start = 2)
//         reaches remaining = 0 to record [1, 2, 2], while index 3 (2) at depth 2
//         is skipped as a duplicate sibling.
//       - Back at start = 0, picking index 1 (2) cannot sum to 3 with {2, 2, 5},
//         and indices 2 and 3 (both 2) are skipped by i > start && candidates[i] ==
//         candidates[i - 1].
//       - Finally, index 4 (5) hits remaining = 0 directly and records [5],
//         yielding [[1, 2, 2], [5]].
//
//   Example 3:
//     Input:  candidates = [1, 1, 1, 1], target = 2
//     Output: [[1, 1]]
//     Explanation:
//       - On the sorted array [1, 1, 1, 1], dfs(0, 2) chooses index 0 (1) and calls
//         dfs(1, 1).
//       - At dfs(1, 1), index 1 (1) is accepted because i == start = 1, hitting
//         remaining = 0 to record [1, 1], while indices 2 and 3 are skipped as
//         duplicates.
//       - Back at dfs(0, 2), indices 1, 2, and 3 all satisfy i > 0 && candidates[i]
//         == candidates[i - 1] and are skipped, leaving [[1, 1]].
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
