// LeetCode 78: Subsets
// https://leetcode.com/problems/subsets/
//
// Problem:
//   Given an integer array `nums` of unique elements, return all possible
//   subsets (the power set). The solution set must not contain duplicate
//   subsets and may be returned in any order.
//   Constraints: 1 <= nums.length <= 10, -10 <= nums[i] <= 10, all elements
//   are distinct.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 3]
//     Output: [[], [1], [1, 2], [1, 2, 3], [1, 3], [2], [2, 3], [3]]
//     Explanation:
//       - Reserve `2^3 = 8` entries in `results` and call `dfs(0)` with `path =
//         []`, immediately pushing `[]`.
//       - At `i = 0`, push `1` (`path = [1]`) and recurse through `dfs(1)` to
//         record `[1]`, `[1, 2]`, `[1, 2, 3]`, and after popping `2`, `[1, 3]`.
//       - Return to `dfs(0)` with `path = []`, then push `2` at `i = 1` so `dfs(2)`
//         records `[2]` and `[2, 3]`.
//       - Finally push `3` at `i = 2` so `dfs(3)` records `[3]`, completing all 8
//         subsets.
//
//   Example 2:
//     Input:  nums = [0]
//     Output: [[], [0]]
//     Explanation:
//       - Reserve `2^1 = 2` entries in `results` and call `dfs(0)` with `path =
//         []`, pushing `[]`.
//       - At `i = 0`, push `0` into `path`, call `dfs(1)` to push `[0]` (its loop
//         `i < 1` does not run), and pop `0` on return.
//
//   Example 3:
//     Input:  nums = []
//     Output: [[]]
//     Explanation:
//       - Compute `n = 0`, reserve `2^0 = 1` entry in `results`, and invoke
//         `dfs(0)` with an empty `path`.
//       - Push `[]` into `results` and exit immediately because `start < n` (`0 <
//         0`) is false.
//
// Algorithm:
//   Three classic approaches to enumerating all 2^n subsets are provided:
//   1. Backtracking DFS (`subsets`):
//      At index `start`, the current `path` is already a valid subset, so
//      record it immediately. Then iterate `i` from `start` to `n - 1`, push
//      `nums[i]`, recurse with `i + 1`, and pop `nums[i]` on backtrack.
//      Invariant: elements in `path` always have strictly increasing input
//      indices, guaranteeing every subset is generated exactly once.
//   2. Iterative Cascading (`subsetsIterative`):
//      Start with `[[]]`. For each `num` in `nums`, duplicate every subset
//      currently in `result` and append `num` to each copy.
//   3. Bitmask Enumeration (`subsetsBitmask`):
//      Iterate over every integer mask in `[0, 2^n)`. Use `std::countr_zero`
//      and Brian Kernighan's `mask &= (mask - 1)` trick to visit only set
//      bits directly in O(popcount(mask)) time per subset.
//   Time Complexity:  O(n * 2^n) to construct and store all 2^n subsets.
//   Space Complexity: O(n) auxiliary space for backtracking recursion/path
//                     (excluding O(n * 2^n) output storage).
//
// Data Structures:
//   - Output list of subsets (`std::vector<std::vector<int>>`) pre-reserved to
//     `1uz << nums.size()` to eliminate reallocations.
//   - Mutable backtracking buffer (`std::vector<int>`) reused across DFS calls.
//   - Unsigned bitmask (`std::uint32_t`) representing element membership.
//
// Real-World Applications:
//   - Feature-flag / A/B-test configuration matrix generation when testing all
//     combinations of independent boolean options.
//   - Query-optimizer join-order and index-subset enumeration in relational
//     databases (System R / Selinger DP over table subsets).
//   - Feature selection and ablation studies in machine learning pipelines.
//
// Related LeetCode Problems:
//   - #90 Subsets II [done] (power set when input contains duplicate values)
//   - #77 Combinations [done] (subsets restricted to a fixed size k)
//   - #39 Combination Sum [done] (backtracking subset selection with reuse)
//   - #47 Permutations II [done] (ordered arrangements instead of unordered sets)
//   - #784 Letter Case Permutation [done] (2^k branching over binary choices)

#include "framework/lc_framework.h"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Generates the power set of distinct elements `nums` via DFS backtracking.
  // Time: O(n * 2^n), Space: O(n) auxiliary (excluding output).
  std::vector<std::vector<int>> subsets(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    std::vector<std::vector<int>> results;
    results.reserve(1uz << n);

    std::vector<int> path;
    path.reserve(n);

    // Every recursion node corresponds to a unique subset whose elements
    // appear in strictly increasing index order.
    auto dfs = [&](this auto&& self, std::size_t start) -> void {
      results.push_back(path);
      for (std::size_t i = start; i < n; ++i) {
        path.push_back(nums[i]);
        self(i + 1);
        path.pop_back();
      }
    };

    dfs(0uz);
    return results;
  }

  // Iterative cascading construction: each new element doubles the subset
  // list by appending itself to a copy of every existing subset.
  // Time: O(n * 2^n), Space: O(n * 2^n) for output.
  std::vector<std::vector<int>> subsetsIterative(const std::vector<int>& nums) {
    std::vector<std::vector<int>> results;
    results.reserve(1uz << nums.size());
    results.emplace_back();  // Seed with the empty subset.

    for (const int value : nums) {
      // Snapshot the current count so we only extend pre-existing subsets.
      const std::size_t current_size = results.size();
      for (std::size_t i = 0; i < current_size; ++i) {
        std::vector<int> extended = results[i];
        extended.push_back(value);
        results.push_back(std::move(extended));
      }
    }

    return results;
  }

  // Bitmask enumeration using `std::countr_zero` to jump directly from one
  // set bit to the next without scanning zero bits.
  // Time: O(n * 2^n), Space: O(n * 2^n) for output.
  std::vector<std::vector<int>> subsetsBitmask(const std::vector<int>& nums) {
    const std::size_t total_masks = 1uz << nums.size();
    std::vector<std::vector<int>> results;
    results.reserve(total_masks);

    for (std::uint32_t mask = 0; mask < static_cast<std::uint32_t>(total_masks);
         ++mask) {
      std::vector<int> subset;
      std::uint32_t bits = mask;
      while (bits != 0) {
        const auto idx = static_cast<std::size_t>(std::countr_zero(bits));
        subset.push_back(nums[idx]);
        // Clear the lowest set bit (Brian Kernighan's trick).
        bits &= (bits - 1U);
      }
      results.push_back(std::move(subset));
    }

    return results;
  }
};

// Canonicalizes a power set for order-independent equality comparison.
std::vector<std::vector<int>> canonicalize(
    std::vector<std::vector<int>> power_set) {
  for (auto& subset : power_set) {
    std::ranges::sort(subset);
  }
  std::ranges::sort(power_set);
  return power_set;
}

LC_PROBLEM(78, "Subsets", "backtracking", ctx) {
  Solution sol;

  ctx.test("example_three_elements", [&] {
    const std::vector<int> input = {1, 2, 3};
    const std::vector<std::vector<int>> expected = {
        {}, {1}, {1, 2}, {1, 2, 3}, {1, 3}, {2}, {2, 3}, {3},
    };
    ctx.expect_eq(canonicalize(sol.subsets(input)), expected);
    ctx.expect_eq(canonicalize(sol.subsetsIterative(input)), expected);
    ctx.expect_eq(canonicalize(sol.subsetsBitmask(input)), expected);
  });

  ctx.test("single_element", [&] {
    const std::vector<int> input = {0};
    const std::vector<std::vector<int>> expected = {{}, {0}};
    ctx.expect_eq(canonicalize(sol.subsets(input)), expected);
    ctx.expect_eq(canonicalize(sol.subsetsIterative(input)), expected);
    ctx.expect_eq(canonicalize(sol.subsetsBitmask(input)), expected);
  });

  ctx.test("empty_input", [&] {
    const std::vector<int> input;
    const std::vector<std::vector<int>> expected = {{}};
    ctx.expect_eq(sol.subsets(input), expected);
    ctx.expect_eq(sol.subsetsIterative(input), expected);
    ctx.expect_eq(sol.subsetsBitmask(input), expected);
  });

  ctx.test("four_elements_with_negatives", [&] {
    const std::vector<int> input = {-3, 0, 2, 5};
    const auto a = canonicalize(sol.subsets(input));
    const auto b = canonicalize(sol.subsetsIterative(input));
    const auto c = canonicalize(sol.subsetsBitmask(input));

    ctx.expect_eq(a.size(), 16uz);
    ctx.expect_eq(a, b);
    ctx.expect_eq(a, c);
  });
}

}  // namespace
