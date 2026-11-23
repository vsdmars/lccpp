// LeetCode 368: Largest Divisible Subset
// https://leetcode.com/problems/largest-divisible-subset/
//
// Problem:
//   Given a set of distinct positive integers `nums`, return the largest
//   subset `answer` such that every pair `(answer[i], answer[j])` satisfies
//   either `answer[i] % answer[j] == 0` or `answer[j] % answer[i] == 0`. If
//   multiple subsets achieve the maximum size, return any one of them.
//
// Algorithm:
//   Sorting + Longest-Increasing-Subsequence-style DP with Parent Pointers.
//   Divisibility is transitive on positive integers: if `a < b < c`, `b % a == 0`,
//   and `c % b == 0`, then `c % a == 0`. Therefore, once `nums` is sorted in
//   ascending order, a subset is pairwise divisible if and only if every
//   element is divisible by its immediate predecessor in the sorted subset
//   (a chain in the divisibility poset).
//   Define `dp[i]` as the length of the longest divisible chain ending at
//   `sorted_nums[i]`, and `parent[i]` as the index of the predecessor element
//   (or `kNoParent` if `sorted_nums[i]` starts the chain):
//     dp[i] = 1 + max({0} U {dp[j] : j < i and sorted_nums[i] % sorted_nums[j] == 0})
//   Track `best_idx` achieving the maximum `dp[i]`, walk `parent` backward to
//   reconstruct the subset, and reverse it into ascending order.
//   Time Complexity:  O(n^2) for the DP transitions (dominates O(n log n) sort).
//   Space Complexity: O(n) for the sorted copy, `dp`, and `parent` arrays.
//
// Data Structures:
//   - `std::vector<int> sorted_nums`: sorted copy of input preserving caller
//     immutability.
//   - `std::vector<int> dp`: chain lengths ending at each index.
//   - `std::vector<int> parent`: backpointers for O(n) subset reconstruction.
//
// Real-World Applications:
//   - Finding longest chains in partially ordered sets (posets) such as
//     harmonic frequency scheduling and clock-divider hierarchies in embedded
//     hardware (where every faster clock must be an integer multiple of slower
//     clocks).
//   - Package / schema version compatibility chain selection where newer
//     revisions strictly subsume older ones.
//   - Periodic real-time task scheduling (harmonic task sets in Rate-Monotonic
//     Scheduling allow 100% CPU utilization).
//
// Related LeetCode Problems:
//   - #300 Longest Increasing Subsequence [done] (canonical chain DP on `<` instead of divisibility)
//   - #354 Russian Doll Envelopes [done] (2D poset longest chain via sorting + LIS)
//   - #646 Maximum Length of Pair Chain [new] (longest chain in interval dominance poset)
//   - #1048 Longest String Chain [done] (longest chain under single-character insertion order)
//   - #673 Number of Longest Increasing Subsequence [done] (counting optimal chains in the same DP framework)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_set>
#include <vector>

namespace {

constexpr int kNoParent = -1;

class Solution {
 public:
  // Returns a largest pairwise-divisible subset of `nums` in ascending order.
  // Invariant: `dp[i]` is the maximum size of a divisible subset whose largest
  // element is `sorted_nums[i]`, and following `parent` from `i` yields a
  // valid chain of length `dp[i]`.
  // Time: O(n^2), Space: O(n).
  std::vector<int> largestDivisibleSubset(const std::vector<int>& nums) {
    if (nums.empty()) {
      return {};
    }

    // Sorting ensures any divisor of `sorted_nums[i]` must appear at index
    // `j < i`, turning pairwise divisibility into a single-predecessor check.
    std::vector<int> sorted_nums = nums;
    std::ranges::sort(sorted_nums);

    const std::size_t n = sorted_nums.size();
    std::vector<int> dp(n, 1);
    std::vector<int> parent(n, kNoParent);

    std::size_t best_idx = 0uz;
    for (std::size_t i = 1uz; i < n; ++i) {
      for (std::size_t j = 0uz; j < i; ++j) {
        if ((sorted_nums[i] % sorted_nums[j] == 0) && (dp[j] + 1 > dp[i])) {
          dp[i] = dp[j] + 1;
          parent[i] = static_cast<int>(j);
        }
      }
      if (dp[i] > dp[best_idx]) {
        best_idx = i;
      }
    }

    // Reconstruct the optimal subset by walking backpointers from `best_idx`.
    std::vector<int> result;
    result.reserve(static_cast<std::size_t>(dp[best_idx]));
    int cur = static_cast<int>(best_idx);
    while (cur != kNoParent) {
      const auto u_cur = static_cast<std::size_t>(cur);
      result.push_back(sorted_nums[u_cur]);
      cur = parent[u_cur];
    }

    std::ranges::reverse(result);
    return result;
  }
};

// Verifies that `subset` is a valid pairwise-divisible subset of `input`.
bool isValidDivisibleSubset(const std::vector<int>& subset,
                            const std::vector<int>& input) {
  const std::unordered_set<int> allowed(input.begin(), input.end());
  std::unordered_set<int> seen;
  for (const int val : subset) {
    if (!allowed.contains(val) || !seen.insert(val).second) {
      return false;
    }
  }
  for (std::size_t i = 0uz; i < subset.size(); ++i) {
    for (std::size_t j = i + 1uz; j < subset.size(); ++j) {
      if ((subset[i] % subset[j] != 0) && (subset[j] % subset[i] != 0)) {
        return false;
      }
    }
  }
  return true;
}

LC_PROBLEM(368, "Largest Divisible Subset", "dp", ctx) {
  ctx.test("example_1_multiple_valid", [&] {
    Solution sol;
    const std::vector<int> nums = {1, 2, 3};
    const auto ans = sol.largestDivisibleSubset(nums);
    ctx.expect_eq(ans.size(), 2uz);
    ctx.expect_true(isValidDivisibleSubset(ans, nums));
  });

  ctx.test("example_2_powers_of_two", [&] {
    Solution sol;
    const std::vector<int> nums = {1, 2, 4, 8};
    ctx.expect_eq(sol.largestDivisibleSubset(nums),
                  std::vector<int>{1, 2, 4, 8});
  });

  ctx.test("unsorted_inputs_from_source", [&] {
    Solution sol;
    ctx.expect_eq(sol.largestDivisibleSubset({24, 12, 30, 1}),
                  std::vector<int>{1, 12, 24});
    ctx.expect_eq(sol.largestDivisibleSubset({4, 8, 10, 240}),
                  std::vector<int>{4, 8, 240});

    const std::vector<int> multi = {2, 4, 6, 12};
    const auto ans = sol.largestDivisibleSubset(multi);
    ctx.expect_eq(ans.size(), 3uz);
    ctx.expect_true(isValidDivisibleSubset(ans, multi));
  });

  ctx.test("edge_cases_and_large_values", [&] {
    Solution sol;
    ctx.expect_eq(sol.largestDivisibleSubset({}), std::vector<int>{});
    ctx.expect_eq(sol.largestDivisibleSubset({7}), std::vector<int>{7});
    ctx.expect_eq(sol.largestDivisibleSubset({1, 2000000000}),
                  std::vector<int>{1, 2000000000});

    const std::vector<int> primes = {3, 5, 7, 11};
    const auto ans = sol.largestDivisibleSubset(primes);
    ctx.expect_eq(ans.size(), 1uz);
    ctx.expect_true(isValidDivisibleSubset(ans, primes));
  });
}

}  // namespace
