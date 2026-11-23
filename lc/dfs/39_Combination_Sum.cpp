// LeetCode 39: Combination Sum
// https://leetcode.com/problems/combination-sum/
//
// Problem:
//   Given an array of distinct positive integers `candidates` and a positive
//   integer `target`, return all unique combinations of `candidates` whose
//   chosen numbers sum to `target`. The same candidate number may be chosen
//   an unlimited number of times. Two combinations are unique if the frequency
//   of at least one chosen number differs.
//   Constraints: 1 <= candidates.length <= 30, 2 <= candidates[i] <= 40,
//   all elements of `candidates` are distinct, 1 <= target <= 40.
//
// Examples:
//   Example 1:
//     Input:  candidates = [2, 3, 6, 7], target = 7
//     Output: [[2, 2, 3], [7]]
//     Explanation:
//       - Sorting candidates leaves [2, 3, 6, 7], and dfs(0, 7) begins with current
//         = [].
//       - Choosing index 0 (2) repeatedly reaches current = [2, 2, 2] with
//         remaining = 1, where candidates[0] = 2 > 1 breaks the loop and backtracks
//         to [2, 2] (remaining = 3).
//       - From [2, 2], choosing index 1 (3) hits remaining = 0 to record [2, 2, 3],
//         after which candidates[2] = 6 > 3 prunes the remaining candidates.
//       - Backtracking to [2] (remaining = 5) and trying 3 reaches remaining = 2 <
//         3 and prunes, while starting at 3 or 6 also fails to hit 0.
//       - Starting from current = [] at index 3 (7) reaches remaining = 0 and
//         records [7], producing [[2, 2, 3], [7]].
//
//   Example 2:
//     Input:  candidates = [2, 3, 5], target = 8
//     Output: [[2, 2, 2, 2], [2, 3, 3], [3, 5]]
//     Explanation:
//       - After sorting [2, 3, 5], dfs(0, 8) reuses index 0 (2) four times
//         (remaining: 8 -> 6 -> 4 -> 2 -> 0) to record [2, 2, 2, 2].
//       - Backtracking to [2, 2] (remaining = 4) tries 3 (leaving remaining = 1 <
//         3) and prunes 5 > 4, then backtracks to [2] (remaining = 6).
//       - From [2] (remaining = 6), choosing index 1 (3) twice reaches remaining =
//         0 and records [2, 3, 3], while [2, 5] leaves remaining = 1 and prunes.
//       - Backtracking to [] (remaining = 8) tries index 1 (3): [3, 3] leaves
//         remaining = 2 < 3 and prunes, while [3, 5] reaches remaining = 0 and
//         records [3, 5].
//       - Starting with index 2 (5) leaves remaining = 3 < 5 and prunes, returning
//         [[2, 2, 2, 2], [2, 3, 3], [3, 5]].
//
//   Example 3:
//     Input:  candidates = [2], target = 1
//     Output: []
//     Explanation:
//       - After sorting [2], dfs(0, 1) starts at index 0 with remaining = 1.
//       - Because candidates[0] = 2 exceeds remaining = 1, the loop breaks
//         immediately without recursing and returns [].
//
// Algorithm:
//   Sort `candidates` ascending, then run backtracking DFS:
//     - Maintain a non-decreasing index invariant: at recursion state
//       `(start, remaining)`, only consider candidates at index `i >= start`.
//       This prevents generating permutations of the same multiset (e.g.,
//       both [2, 2, 3] and [2, 3, 2]).
//     - Because each element may be reused unlimited times, recursing after
//       picking `candidates[i]` passes `i` (not `i + 1`) as the next `start`.
//     - Because `candidates` is sorted, as soon as `candidates[i] > remaining`
//       we can `break` out of the loop and prune the entire remaining suffix.
//     - When `remaining == 0`, snapshot the current path into `results`.
//   Time Complexity:  O(n^(T / M + 1)) worst case, where n = candidates.size(),
//                     T = target, and M = min(candidates).
//   Space Complexity: O(T / M) recursion depth and path buffer (excluding output).
//
// Data Structures:
//   - Sorted `std::vector<int>`: enables monotonic index ordering (eliminating
//     duplicate permutations) and early suffix pruning.
//   - Path stack (`std::vector<int>`): tracks the active combination via O(1)
//     push_back / pop_back during DFS.
//   - Result list (`std::vector<std::vector<int>>`): stores all valid
//     combinations.
//
// Real-World Applications:
//   - Currency / coin dispensing and ATM bill-pack enumeration under fixed
//     denominations.
//   - Cutting-stock and 1D bin-packing pattern generation (enumerating valid
//     ways to cut stock lengths to exact customer orders).
//   - Cloud resource sizing and VM instance-type combinations matching an
//     exact CPU/RAM target budget.
//   - Nutritional / recipe formulation and dosage combinations matching exact
//     active-ingredient targets.
//
// Related LeetCode Problems:
//   - #40 Combination Sum II [done] (each candidate used at most once with duplicates)
//   - #77 Combinations [done] (fixed-size k-subsets from [1, n] without reuse)
//   - #216 Combination Sum III [new] (k distinct digits from 1..9 summing to n)
//   - #377 Combination Sum IV [done] (counts ordered permutations via 1D DP)
//   - #518 Coin Change II [done] (counts unordered combinations via DP instead of enumerating)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all unique combinations of `candidates` (with unlimited reuse) that
  // sum to `target`. Takes `candidates` by value to sort in place.
  // Time: O(n^(T/M + 1)), Space: O(T/M) auxiliary.
  std::vector<std::vector<int>> combinationSum(std::vector<int> candidates,
                                               int target) {
    if (target < 0) {
      return {};
    }

    // Sorting lets us break out of the DFS loop as soon as candidates[i] > remaining.
    std::ranges::sort(candidates);

    std::vector<std::vector<int>> results;
    std::vector<int> current;

    // DFS invariant: `current` holds a non-decreasing sequence summing to
    // `target - remaining`; only indices >= `start` may be chosen next.
    auto dfs = [&](this auto&& self, std::size_t start, int remaining) -> void {
      if (remaining == 0) {
        results.push_back(current);
        return;
      }

      for (std::size_t i = start; i < candidates.size(); ++i) {
        if (candidates[i] > remaining) {
          // Since `candidates` is sorted ascending, every later value also
          // exceeds `remaining` and can be pruned immediately.
          break;
        }

        current.push_back(candidates[i]);
        // Pass `i` (not `i + 1`) because `candidates[i]` may be reused.
        self(i, remaining - candidates[i]);
        current.pop_back();
      }
    };

    dfs(0uz, target);
    return results;
  }
};

LC_PROBLEM(39, "Combination Sum", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_target_7", [&] {
    const std::vector<std::vector<int>> expected = {
        {2, 2, 3},
        {7},
    };
    ctx.expect_eq(sol.combinationSum({2, 3, 6, 7}, 7), expected);
  });

  ctx.test("example_2_target_8", [&] {
    const std::vector<std::vector<int>> expected = {
        {2, 2, 2, 2},
        {2, 3, 3},
        {3, 5},
    };
    ctx.expect_eq(sol.combinationSum({2, 3, 5}, 8), expected);
  });

  ctx.test("unsorted_candidates", [&] {
    const std::vector<std::vector<int>> expected = {
        {2, 2, 3},
        {7},
    };
    ctx.expect_eq(sol.combinationSum({7, 2, 6, 3}, 7), expected);
  });

  ctx.test("no_valid_combination", [&] {
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.combinationSum({2}, 1), expected);
  });

  ctx.test("single_candidate_multiple", [&] {
    const std::vector<std::vector<int>> expected = {{3, 3, 3}};
    ctx.expect_eq(sol.combinationSum({3}, 9), expected);
  });
}

}  // namespace
