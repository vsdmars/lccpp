// LeetCode 47: Permutations II
// https://leetcode.com/problems/permutations-ii/
//
// Problem:
//   Given a collection of integers `nums` that might contain duplicates,
//   return all possible unique permutations in any order.
//   Constraints: 1 <= nums.length <= 8, -10 <= nums[i] <= 10.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 1, 2]
//     Output: [[1, 1, 2], [1, 2, 1], [2, 1, 1]]
//     Explanation:
//       - Sort nums to [1, 1, 2] and initialize used = [0, 0, 0] and path = [].
//       - At depth 0, pick index 0 (1), then explore indices 1 and 2 in remaining
//         slots to emit [1, 1, 2] and [1, 2, 1].
//       - Back at depth 0, skip index 1 (1) because nums[1] == nums[0] and used[0]
//         == 0, preventing duplicate branches starting with 1.
//       - Pick index 2 (2) at depth 0, then pick index 0 (1) and index 1 (1) to
//         emit [2, 1, 1].
//
//   Example 2:
//     Input:  nums = [1, 2, 3]
//     Output: [[1, 2, 3], [1, 3, 2], [2, 1, 3], [2, 3, 1], [3, 1, 2], [3, 2, 1]]
//     Explanation:
//       - Nums is already sorted as [1, 2, 3] with all distinct elements, so the
//         adjacent-duplicate guard never triggers.
//       - At depth 0, selecting i = 0 (1) recurses over [2, 3] and [3, 2] to emit
//         [1, 2, 3] and [1, 3, 2].
//       - Backtracking to try i = 1 (2) and i = 2 (3) at depth 0 enumerates the
//         remaining 4 branches, producing all 3! = 6 permutations in
//         lexicographical order.
//
//   Example 3:
//     Input:  nums = [2, 2, 2]
//     Output: [[2, 2, 2]]
//     Explanation:
//       - Sort nums = [2, 2, 2] and initialize used = [0, 0, 0].
//       - At depth 0, pick index 0 (used[0] = 1) and skip indices 1 and 2 because
//         nums[i] == nums[i - 1] with used[i - 1] == 0.
//       - At depth 1 pick index 1 (allowed since used[0] == 1) while skipping index
//         2, then at depth 2 pick index 2 to emit the single permutation [2, 2, 2].
//
// Algorithm:
//   1. Sorted Backtracking with Same-Level Deduplication (`permuteUnique`):
//      First sort `nums` so identical values are adjacent. Maintain a `used`
//      flag array and a partial `path`. At each recursion depth, iterate over
//      every index `i`:
//        - Skip if `used[i]` is already true.
//        - Skip if `i > 0 && nums[i] == nums[i - 1] && !used[i - 1]`.
//          Invariant: among identical values, we always pick them in strictly
//          left-to-right index order. If the earlier duplicate `i - 1` is not
//          currently on the recursion path, picking `i` would generate a
//          subtree identical to the one already explored when `i - 1` was
//          picked at this same depth.
//      When `path.size() == nums.size()`, emit a completed permutation.
//   2. Iterative Insertion with Duplicate Cut (`permuteUniqueIterative`):
//      Start with `[[]]`. For each number `val` in `nums`, insert `val` at
//      every slot `0..perm.size()` of each existing partial permutation, but
//      break as soon as `perm[slot] == val` so duplicate values never swap
//      relative order to create duplicate sequences.
//   Time Complexity:  O(n * n!) worst-case (specifically O(n * U), where U is
//                     the number of unique permutations <= n!).
//   Space Complexity: O(n) auxiliary space for recursion stack, `used`, and
//                     `path` (excluding O(n * U) output storage).
//
// Data Structures:
//   - Sorted input array (`std::vector<int>`) to group duplicates contiguously.
//   - Byte-flag array (`std::vector<std::uint8_t>`) tracking active elements on
//     the current DFS path.
//   - Mutable path buffer (`std::vector<int>`) pushed/popped in place.
//
// Real-World Applications:
//   - Combinatorial test-case generation for multi-stage pipelines where some
//     stages or inputs are indistinguishable (avoiding redundant test runs).
//   - Anagram generation and crossword/Scrabble solvers handling repeated
//     letter tiles without emitting duplicate words.
//   - Scheduling and task-ordering search when identical resource jobs share
//     symmetry that must be broken during branch-and-bound.
//
// Related LeetCode Problems:
//   - #46 Permutations [done] (simpler variant with all distinct elements)
//   - #31 Next Permutation [done] (in-place lexicographic permutation stepper)
//   - #78 Subsets [done] (power-set generation without duplicates)
//   - #90 Subsets II [done] (same sort + adjacent-skip deduplication pattern)
//   - #39 Combination Sum [done] (backtracking with element reuse)
//   - #40 Combination Sum II [done] (same-level duplicate skipping on sorted input)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Generates all unique permutations of `nums` in lexicographical order using
  // sorted backtracking with same-level duplicate pruning.
  // Time: O(n * U), Space: O(n) auxiliary (U = number of unique permutations).
  std::vector<std::vector<int>> permuteUnique(std::vector<int> nums) {
    // Sorting groups equal values so adjacent duplicate checks work in O(1).
    std::ranges::sort(nums);

    const std::size_t n = nums.size();
    std::vector<std::vector<int>> results;
    std::vector<int> path;
    path.reserve(n);
    std::vector<std::uint8_t> used(n, 0);

    auto dfs = [&](this auto&& self) -> void {
      if (path.size() == n) {
        results.push_back(path);
        return;
      }

      for (std::size_t i = 0; i < n; ++i) {
        if (used[i] != 0) {
          continue;
        }
        // Symmetry breaking: if the previous identical element was NOT used
        // in the current branch, choosing `nums[i]` here repeats a branch
        // already explored at this depth.
        if (i > 0 && nums[i] == nums[i - 1] && used[i - 1] == 0) {
          continue;
        }

        used[i] = 1;
        path.push_back(nums[i]);
        self();
        path.pop_back();
        used[i] = 0;
      }
    };

    dfs();
    return results;
  }

  // Iterative insertion approach: inserts each element into every slot of the
  // existing partial permutations, stopping after inserting immediately before
  // an equal element to avoid duplicate results.
  // Time: O(n * U), Space: O(n * U) for intermediate layers.
  std::vector<std::vector<int>> permuteUniqueIterative(
      const std::vector<int>& nums) {
    std::vector<std::vector<int>> perms = {{}};

    for (const int value : nums) {
      std::vector<std::vector<int>> next_perms;
      for (const auto& perm : perms) {
        for (std::size_t pos = 0; pos <= perm.size(); ++pos) {
          std::vector<int> candidate;
          candidate.reserve(perm.size() + 1);
          candidate.insert(candidate.end(), perm.begin(),
                           perm.begin() + static_cast<std::ptrdiff_t>(pos));
          candidate.push_back(value);
          candidate.insert(candidate.end(),
                           perm.begin() + static_cast<std::ptrdiff_t>(pos),
                           perm.end());
          next_perms.push_back(std::move(candidate));

          // Stop once we hit an equal element so identical values never cross
          // over each other and generate duplicate permutations.
          if (pos < perm.size() && perm[pos] == value) {
            break;
          }
        }
      }
      perms = std::move(next_perms);
    }

    return perms;
  }
};

LC_PROBLEM(47, "Permutations", "backtracking", ctx) {
  Solution sol;

  ctx.test("example_with_duplicates", [&] {
    const std::vector<int> input = {1, 1, 2};
    const std::vector<std::vector<int>> expected = {
        {1, 1, 2},
        {1, 2, 1},
        {2, 1, 1},
    };
    ctx.expect_eq(sol.permuteUnique(input), expected);

    auto iterative = sol.permuteUniqueIterative(input);
    std::ranges::sort(iterative);
    ctx.expect_eq(iterative, expected);
  });

  ctx.test("all_distinct", [&] {
    const std::vector<int> input = {1, 2, 3};
    const std::vector<std::vector<int>> expected = {
        {1, 2, 3}, {1, 3, 2}, {2, 1, 3},
        {2, 3, 1}, {3, 1, 2}, {3, 2, 1},
    };
    ctx.expect_eq(sol.permuteUnique(input), expected);

    auto iterative = sol.permuteUniqueIterative(input);
    std::ranges::sort(iterative);
    ctx.expect_eq(iterative, expected);
  });

  ctx.test("all_identical_and_single", [&] {
    const std::vector<std::vector<int>> expected_all_same = {{2, 2, 2}};
    ctx.expect_eq(sol.permuteUnique({2, 2, 2}), expected_all_same);
    ctx.expect_eq(sol.permuteUniqueIterative({2, 2, 2}), expected_all_same);

    const std::vector<std::vector<int>> expected_single = {{7}};
    ctx.expect_eq(sol.permuteUnique({7}), expected_single);
    ctx.expect_eq(sol.permuteUniqueIterative({7}), expected_single);
  });

  ctx.test("unsorted_negatives_with_duplicates", [&] {
    const std::vector<int> input = {0, -1, 0, -1};
    auto backtrack = sol.permuteUnique(input);
    auto iterative = sol.permuteUniqueIterative(input);
    std::ranges::sort(iterative);

    // 4! / (2! * 2!) = 6 unique permutations.
    ctx.expect_eq(backtrack.size(), 6uz);
    ctx.expect_eq(iterative, backtrack);
  });
}

}  // namespace
