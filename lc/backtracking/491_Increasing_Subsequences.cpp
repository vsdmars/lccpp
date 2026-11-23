// LeetCode 491: Increasing Subsequences
// https://leetcode.com/problems/non-decreasing-subsequences/
//
// Problem:
//   Given an integer array `nums` (1 <= nums.size() <= 15, -100 <= nums[i] <= 100),
//   return all distinct non-decreasing subsequences of `nums` that contain at
//   least two elements. Elements must preserve their original relative order in
//   `nums`, and equal consecutive elements count as non-decreasing. The output
//   subsequences may be returned in any order.
//
// Algorithm:
//   Depth-first backtracking with per-level deduplication. Starting from index
//   `start`, we try extending the current non-decreasing prefix `path` with
//   each element `nums[i]` (i >= start) satisfying `path.empty() || nums[i] >= path.back()`.
//   Because the input array cannot be sorted without destroying subsequence
//   order, duplicate subsequences are avoided by maintaining a local hash set
//   `used` at each recursion frame: if a value `v` has already been chosen to
//   fill the current slot of `path`, any later occurrence of `v` at the same
//   depth can only produce a subset of already-explored suffixes and is skipped.
//   Whenever `path.size() >= 2`, we record a copy of `path` in the result.
//
//   Time Complexity:  O(n * 2^n) in the worst case (when `nums` is strictly
//                     increasing, there are 2^n - n - 1 valid subsequences,
//                     each taking up to O(n) time to copy).
//   Space Complexity: O(n) auxiliary recursion stack and working path (excluding
//                     the O(n * 2^n) output storage), plus O(n) per-frame sets
//                     summing to O(n^2) along a single root-to-leaf path.
//
// Data Structures:
//   - std::vector<int> (`path`): Working stack holding the current candidate
//     non-decreasing subsequence along the DFS branch.
//   - std::unordered_set<int> (`used`): Per-depth set recording values already
//     branched on at the current recursion level to prevent duplicate outputs.
//   - std::vector<std::vector<int>> (`result`): Accumulator for all unique
//     valid subsequences of length at least 2.
//
// Real-World Applications:
//   - Enumerating monotonic event chains in telemetry, audit logs, or version
//     histories without reordering timestamps.
//   - Identifying distinct non-decreasing trend patterns in financial time
//     series or sensor streams with repeated readings.
//   - Generating valid ordered subsets of tasks or pipeline stages subject to
//     priority/precedence constraints.
//
// Related LeetCode Problems:
//   - #78 Subsets [done] (power-set backtracking without ordering constraint)
//   - #90 Subsets II [done] (deduplicated subset generation via sorting)
//   - #300 Longest Increasing Subsequence [done] (finding maximum length instead of enumerating all)
//   - #673 Number of Longest Increasing Subsequence [done] (counting longest strictly increasing subsequences)
//   - #1964 Find the Longest Valid Obstacle Course at Each Position [new] (non-decreasing subsequence length per prefix)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all unique non-decreasing subsequences of `nums` with length >= 2.
  //
  // Invariant: At every call to `dfs(start)`, `path` is non-decreasing and
  // consists strictly of elements chosen from `nums[0..start-1]` in order.
  // Time: O(n * 2^n), Space: O(n^2) auxiliary (excluding output).
  std::vector<std::vector<int>> findSubsequences(const std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    path.reserve(nums.size());

    auto dfs = [&](this auto&& self, std::size_t start) -> void {
      // Every non-decreasing prefix of length >= 2 is a valid answer.
      if (path.size() >= 2) {
        result.push_back(path);
      }

      // Deduplicate choices at THIS recursion depth only. Unlike #90 Subsets II,
      // we cannot sort `nums` because subsequence order must be preserved.
      std::unordered_set<int> used;
      for (std::size_t i = start; i < nums.size(); ++i) {
        const int val = nums[i];

        // Skip values that would break the non-decreasing invariant.
        if (!path.empty() && val < path.back()) {
          continue;
        }

        // Skip values already branched on at this depth to prevent duplicates.
        if (!used.insert(val).second) {
          continue;
        }

        path.push_back(val);
        self(i + 1);
        path.pop_back();
      }
    };

    dfs(0uz);
    return result;
  }
};

// Sorts a collection of subsequences lexicographically for order-independent
// comparison in unit tests.
std::vector<std::vector<int>> normalize(std::vector<std::vector<int>> seqs) {
  std::ranges::sort(seqs);
  return seqs;
}

LC_PROBLEM(491, "Increasing Subsequences", "backtracking", ctx) {
  ctx.test("example_with_duplicates", [&] {
    Solution sol;
    const std::vector<int> nums = {4, 6, 7, 7};
    const std::vector<std::vector<int>> expected = {
        {4, 6}, {4, 6, 7}, {4, 6, 7, 7}, {4, 7},
        {4, 7, 7}, {6, 7}, {6, 7, 7}, {7, 7},
    };
    ctx.expect_eq(normalize(sol.findSubsequences(nums)), normalize(expected));
  });

  ctx.test("mostly_decreasing_with_equal_pair", [&] {
    Solution sol;
    const std::vector<int> nums = {4, 4, 3, 2, 1};
    const std::vector<std::vector<int>> expected = {{4, 4}};
    ctx.expect_eq(normalize(sol.findSubsequences(nums)), normalize(expected));
  });

  ctx.test("python_build_case_mixed_order", [&] {
    Solution sol;
    const std::vector<int> nums = {7, 1, 8, 8};
    const std::vector<std::vector<int>> expected = {
        {1, 8}, {1, 8, 8}, {7, 8}, {7, 8, 8}, {8, 8},
    };
    ctx.expect_eq(normalize(sol.findSubsequences(nums)), normalize(expected));
  });

  ctx.test("python_build_case_decreasing_prefix", [&] {
    Solution sol;
    const std::vector<int> nums = {7, 3, 1, 9, 10};
    const std::vector<std::vector<int>> expected = {
        {1, 9}, {1, 9, 10}, {1, 10}, {3, 9}, {3, 9, 10},
        {3, 10}, {7, 9}, {7, 9, 10}, {7, 10}, {9, 10},
    };
    ctx.expect_eq(normalize(sol.findSubsequences(nums)), normalize(expected));
  });

  ctx.test("all_equal_elements", [&] {
    Solution sol;
    const std::vector<int> nums = {2, 2, 2};
    const std::vector<std::vector<int>> expected = {{2, 2}, {2, 2, 2}};
    ctx.expect_eq(normalize(sol.findSubsequences(nums)), normalize(expected));
  });

  ctx.test("strictly_decreasing_and_single_element", [&] {
    Solution sol;
    const std::vector<std::vector<int>> empty;
    ctx.expect_eq(sol.findSubsequences({5, 4, 3, 2, 1}), empty);
    ctx.expect_eq(sol.findSubsequences({42}), empty);
    ctx.expect_eq(sol.findSubsequences({}), empty);
  });

  ctx.test("negative_values", [&] {
    Solution sol;
    const std::vector<int> nums = {-100, -100, 0, 100};
    const std::vector<std::vector<int>> expected = {
        {-100, -100},        {-100, -100, 0}, {-100, -100, 0, 100},
        {-100, -100, 100},   {-100, 0},       {-100, 0, 100},
        {-100, 100},         {0, 100},
    };
    ctx.expect_eq(normalize(sol.findSubsequences(nums)), normalize(expected));
  });
}

}  // namespace
