// LeetCode 90: Subsets II
// https://leetcode.com/problems/subsets-ii/
//
// Problem:
//   Given an integer array `nums` that may contain duplicates, return all
//   possible subsets (the power set). The solution set must not contain
//   duplicate subsets and may be returned in any order.
//   Constraints: 1 <= nums.length <= 10, -10 <= nums[i] <= 10.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 2]
//     Output: [[], [1], [1, 2], [1, 2, 2], [2], [2, 2]]
//     Explanation:
//       - Sort `nums` to `[1, 2, 2]` and call `dfs(0)` with `path = []`, pushing
//         `[]`.
//       - At `i = 0`, push `1` and call `dfs(1)`, which pushes `[1]`, recurses on
//         `i = 1` to push `[1, 2]` and `[1, 2, 2]`, and skips `i = 2` since `i > 1
//         && nums[2] == nums[1]`.
//       - Back at `dfs(0)`, `i = 1` pushes `2` and `dfs(2)` records `[2]` and `[2,
//         2]` because `i == start == 2` allows the second `2` at deeper depth.
//       - At `dfs(0)`, `i = 2` is skipped (`i > 0 && nums[2] == nums[1]`),
//         returning all 6 unique subsets.
//
//   Example 2:
//     Input:  nums = [4, 4, 4, 1, 4]
//     Output: [[], [1], [1, 4], [1, 4, 4], [1, 4, 4, 4], [1, 4, 4, 4, 4], [4], [4,
//             4], [4, 4, 4], [4, 4, 4, 4]]
//     Explanation:
//       - Sort `nums` into `[1, 4, 4, 4, 4]` so all four `4`s are contiguous and
//         `dfs(0)` records `[]`.
//       - Choosing `1` at `i = 0` lets deeper calls append 0 to 4 copies of `4`
//         (recording `[1]` through `[1, 4, 4, 4, 4]`) while skipping duplicate `4`
//         branches at the same frame.
//       - Back at `dfs(0)`, `i = 1` picks the first `4` to generate `[4]` through
//         `[4, 4, 4, 4]`, and `i = 2..4` are skipped as same-level duplicates,
//         yielding `2 * 5 = 10` subsets.
//
//   Example 3:
//     Input:  nums = []
//     Output: [[]]
//     Explanation:
//       - Sort the empty `nums` vector (`n = 0`) and invoke `dfs(0)` with `path =
//         []`.
//       - Push `[]` to `results` and skip the `i < 0` loop, returning `[[]]`.
//
// Algorithm:
//   1. Sorted Backtracking with Same-Level Duplicate Skipping (`subsetsWithDup`):
//      Sort `nums` so duplicate values are contiguous. At each DFS frame
//      starting at `start`, record `path` and iterate `i` from `start` to
//      `n - 1`. Whenever `i > start && nums[i] == nums[i - 1]`, skip `i`.
//      Invariant: the first copy of a repeated value is allowed at depth `d`
//      (and subsequent copies can still be chosen in deeper recursive calls),
//      preventing duplicate subsets while preserving multiplicity up to the
//      actual frequency of that value.
//   2. Iterative Incremental-Range Expansion (`subsetsWithDupIterative`):
//      Sort `nums` and seed `results` with `[[]]`. Track `prev_round_start`,
//      the starting index of the subsets added in the previous step. When
//      `nums[i] == nums[i - 1]`, append `nums[i]` ONLY to subsets in
//      `[prev_round_start, current_size)`; otherwise append to all subsets in
//      `[0, current_size)`.
//   Time Complexity:  O(n * 2^n) worst-case (all elements distinct).
//   Space Complexity: O(n) auxiliary space for DFS recursion and `path`
//                     (excluding O(n * 2^n) output storage).
//
// Data Structures:
//   - Sorted input vector (`std::vector<int>`) grouping equal values.
//   - Mutable backtracking buffer (`std::vector<int>`) pushed/popped in place.
//   - Output power set (`std::vector<std::vector<int>>`).
//
// Real-World Applications:
//   - Shopping-cart / bundle-pricing engines enumerating unique item subsets
//     when multiple identical SKUs exist in a basket.
//   - Chemistry and molecular formula solvers enumerating distinct multisets
//     of atoms/functional groups without redundant isomers.
//   - Security policy and permission-set deduplication when roles grant
//     overlapping capabilities.
//
// Related LeetCode Problems:
//   - #78 Subsets [done] (power set for distinct elements)
//   - #47 Permutations II [done] (same sort + adjacent-duplicate pruning)
//   - #40 Combination Sum II [done] (same duplicate-skipping pattern with target sum)
//   - #491 Non-decreasing Subsequences [done] (deduplicating subsequences without sorting)
//   - #77 Combinations [done] (fixed-size subset backtracking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Generates all unique subsets of `nums` (which may contain duplicates) via
  // sorted DFS backtracking.
  // Time: O(n * 2^n), Space: O(n) auxiliary (excluding output).
  std::vector<std::vector<int>> subsetsWithDup(std::vector<int> nums) {
    // Sorting places identical values next to each other for O(1) pruning.
    std::ranges::sort(nums);

    const std::size_t n = nums.size();
    std::vector<std::vector<int>> results;
    std::vector<int> path;
    path.reserve(n);

    auto dfs = [&](this auto&& self, std::size_t start) -> void {
      results.push_back(path);

      for (std::size_t i = start; i < n; ++i) {
        // Skip duplicate values at the same recursion depth; duplicates are
        // still chosen in deeper calls (where `i == start`), so multisets
        // like `[2, 2]` are preserved without generating `[2]` twice.
        if (i > start && nums[i] == nums[i - 1]) {
          continue;
        }
        path.push_back(nums[i]);
        self(i + 1);
        path.pop_back();
      }
    };

    dfs(0uz);
    return results;
  }

  // Iterative expansion: when encountering a duplicate value, extend only the
  // slice of subsets created during the immediately preceding iteration.
  // Time: O(n * 2^n), Space: O(n * 2^n) for output.
  std::vector<std::vector<int>> subsetsWithDupIterative(std::vector<int> nums) {
    std::ranges::sort(nums);

    std::vector<std::vector<int>> results;
    results.emplace_back();  // Seed with the empty subset.

    std::size_t prev_round_start = 0;
    for (std::size_t i = 0; i < nums.size(); ++i) {
      // If `nums[i]` repeats the previous element, only extend subsets that
      // already received the previous copy to avoid duplicate combinations.
      const std::size_t begin_idx =
          (i > 0 && nums[i] == nums[i - 1]) ? prev_round_start : 0uz;
      const std::size_t current_size = results.size();
      prev_round_start = current_size;

      for (std::size_t j = begin_idx; j < current_size; ++j) {
        std::vector<int> extended = results[j];
        extended.push_back(nums[i]);
        results.push_back(std::move(extended));
      }
    }

    return results;
  }
};

// Canonicalizes a list of subsets for order-independent comparison.
std::vector<std::vector<int>> canonicalize(
    std::vector<std::vector<int>> power_set) {
  for (auto& subset : power_set) {
    std::ranges::sort(subset);
  }
  std::ranges::sort(power_set);
  return power_set;
}

LC_PROBLEM(90, "Subsets II", "backtracking", ctx) {
  Solution sol;

  ctx.test("example_with_duplicates", [&] {
    const std::vector<int> input = {1, 2, 2};
    const std::vector<std::vector<int>> expected = {
        {}, {1}, {1, 2}, {1, 2, 2}, {2}, {2, 2},
    };
    ctx.expect_eq(canonicalize(sol.subsetsWithDup(input)), expected);
    ctx.expect_eq(canonicalize(sol.subsetsWithDupIterative(input)), expected);
  });

  ctx.test("all_distinct_python_build_input", [&] {
    const std::vector<int> input = {1, 2, 3};
    const std::vector<std::vector<int>> expected = {
        {}, {1}, {1, 2}, {1, 2, 3}, {1, 3}, {2}, {2, 3}, {3},
    };
    ctx.expect_eq(canonicalize(sol.subsetsWithDup(input)), expected);
    ctx.expect_eq(canonicalize(sol.subsetsWithDupIterative(input)), expected);
  });

  ctx.test("unsorted_repeated_elements", [&] {
    const std::vector<int> input = {4, 4, 4, 1, 4};
    const auto backtrack = canonicalize(sol.subsetsWithDup(input));
    const auto iterative = canonicalize(sol.subsetsWithDupIterative(input));
    // Choices: 0..1 copies of `1` (2 options) * 0..4 copies of `4` (5 options) = 10.
    ctx.expect_eq(backtrack.size(), 10uz);
    ctx.expect_eq(backtrack, iterative);
  });

  ctx.test("single_and_empty", [&] {
    const std::vector<std::vector<int>> expected_zero = {{}, {0}};
    ctx.expect_eq(canonicalize(sol.subsetsWithDup({0})), expected_zero);
    ctx.expect_eq(canonicalize(sol.subsetsWithDupIterative({0})),
                  expected_zero);

    const std::vector<std::vector<int>> expected_empty = {{}};
    ctx.expect_eq(sol.subsetsWithDup({}), expected_empty);
    ctx.expect_eq(sol.subsetsWithDupIterative({}), expected_empty);
  });
}

}  // namespace
