// LeetCode 18: 4Sum
// https://leetcode.com/problems/4sum/
//
// Problem:
//   Given an integer array `nums` of length n and an integer `target`, return
//   all unique quadruplets [nums[a], nums[b], nums[c], nums[d]] such that
//   a, b, c, and d are pairwise distinct indices and
//   nums[a] + nums[b] + nums[c] + nums[d] == target.
//   The solution set must not contain duplicate quadruplets, and answers may
//   be returned in any order.
//   Constraints: 1 <= n <= 200, -10^9 <= nums[i], target <= 10^9.
//
// Algorithm:
//   Sort `nums` ascending, then reduce the problem recursively via generalized
//   k-Sum DFS down to a linear-time Two-Pointer 2-Sum base case:
//     1. Base case (k == 2): Scan `nums[start .. n-1]` from both ends with
//        left/right pointers. Because `nums` is sorted, adjust `left` upward
//        when the pair sum is too small and `right` downward when too large.
//        When a match is found, emit the quadruplet and advance both pointers
//        past duplicate values.
//     2. Recursive step (k > 2): Iterate `i` from `start` to `n - k`. Skip
//        adjacent duplicates (`i > start && nums[i] == nums[i - 1]`) so the
//        same value is never chosen twice at the same recursion depth.
//     3. Pruning & Overflow Safety: Before recursing, check whether the
//        smallest possible k-sum (`nums[start] * k`) exceeds `remaining` or
//        the largest possible k-sum (`nums.back() * k`) is below `remaining`.
//        All sum and bound arithmetic uses 64-bit `std::int64_t` to avoid
//        32-bit signed integer overflow when inputs approach +/-10^9.
//   Time Complexity:  O(n^3) — O(n log n) sort plus O(n^(k-1)) search for k=4.
//   Space Complexity: O(k) = O(1) auxiliary stack space (excluding output).
//
// Data Structures:
//   - Sorted `std::vector<int>`: enables monotonic two-pointer convergence,
//     O(1) duplicate skipping, and tight suffix min/max pruning.
//   - Backtracking prefix buffer (`std::vector<int>`): holds the currently
//     selected elements along the DFS path without per-call vector copies.
//   - Result list (`std::vector<std::vector<int>>`): collects all unique
//     quadruplets.
//
// Real-World Applications:
//   - Financial reconciliation and multi-leg transaction matching (finding
//     k-entry subsets of ledger items that net to a target balance).
//   - Portfolio rebalancing and fixed-income basket construction under exact
//     notional or risk-budget constraints.
//   - Cryptanalysis and meet-in-the-middle / k-sum collision search in hash
//     and lattice-based protocol analysis.
//   - Chemical mass spectrometry peak assignment (matching k fragment masses
//     to an observed precursor mass).
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (base 2-element sum problem solved via hash table)
//   - #15 3Sum [done] (3-element precursor using sort + two pointers)
//   - #16 3Sum Closest [done] (same sort + two-pointer scan minimizing distance)
//   - #167 Two Sum II [done] - Input Array Is Sorted (core two-pointer subproblem)
//   - #454 4Sum II [done] (four separate arrays solved in O(n^2) with hash map)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all unique quadruplets in `nums` that sum to `target`.
  // Takes `nums` by value because it is sorted in place.
  // Time: O(n^3), Space: O(1) auxiliary (excluding output).
  std::vector<std::vector<int>> fourSum(std::vector<int> nums, int target) {
    if (nums.size() < kQuadrupletSize) {
      return {};
    }

    // Sorting enables duplicate skipping, suffix bound pruning, and O(n) 2-Sum.
    std::ranges::sort(nums);

    std::vector<std::vector<int>> results;
    std::vector<int> current;
    current.reserve(kQuadrupletSize);

    kSum(nums, 0uz, kQuadrupletSize, static_cast<std::int64_t>(target), current,
         results);
    return results;
  }

 private:
  static constexpr std::size_t kQuadrupletSize = 4;

  // Two-pointer 2-Sum over sorted suffix `nums[start .. nums.size()-1]`.
  // Appends every unique pair summing to `remaining` onto `current` and
  // records the full combination in `results`.
  // Time: O(n), Space: O(1) auxiliary.
  static void twoSum(const std::vector<int>& nums, std::size_t start,
                     std::int64_t remaining, std::vector<int>& current,
                     std::vector<std::vector<int>>& results) {
    std::size_t left = start;
    std::size_t right = nums.size() - 1;

    while (left < right) {
      const std::int64_t pair_sum = static_cast<std::int64_t>(nums[left]) +
                                    static_cast<std::int64_t>(nums[right]);
      if (pair_sum < remaining) {
        ++left;
        continue;
      }
      if (pair_sum > remaining) {
        --right;
        continue;
      }

      // Record the matching quadruplet via push/pop to reuse `current`.
      current.push_back(nums[left]);
      current.push_back(nums[right]);
      results.push_back(current);
      current.pop_back();
      current.pop_back();

      // Advance both pointers past any duplicate values so the same pair
      // is never emitted twice for this prefix.
      const int left_val = nums[left];
      const int right_val = nums[right];
      while (left < right && nums[left] == left_val) {
        ++left;
      }
      while (left < right && nums[right] == right_val) {
        --right;
      }
    }
  }

  // Generalized recursive k-Sum DFS over sorted suffix `nums[start ..]`.
  // Invariant: `k >= 2` and `nums.size() - start >= k`.
  // Time: O(n^(k-1)), Space: O(k) recursion depth.
  static void kSum(const std::vector<int>& nums, std::size_t start,
                   std::size_t k, std::int64_t remaining,
                   std::vector<int>& current,
                   std::vector<std::vector<int>>& results) {
    // Early feasibility pruning using the smallest and largest available values
    // in the remaining sorted suffix.
    const auto count = static_cast<std::int64_t>(k);
    const std::int64_t min_possible =
        static_cast<std::int64_t>(nums[start]) * count;
    const std::int64_t max_possible =
        static_cast<std::int64_t>(nums.back()) * count;
    if (remaining < min_possible || remaining > max_possible) {
      return;
    }

    // Reduce directly to O(n) two-pointer search at the base case k == 2.
    if (k == 2) {
      twoSum(nums, start, remaining, current, results);
      return;
    }

    const std::size_t max_index = nums.size() - k;
    for (std::size_t i = start; i <= max_index; ++i) {
      // Skip duplicate choices at the same DFS level to guarantee unique tuples.
      if (i > start && nums[i] == nums[i - 1]) {
        continue;
      }

      current.push_back(nums[i]);
      kSum(nums, i + 1, k - 1, remaining - static_cast<std::int64_t>(nums[i]),
           current, results);
      current.pop_back();
    }
  }
};

LC_PROBLEM(18, "4Sum", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_mixed_signs", [&] {
    const std::vector<std::vector<int>> expected = {
        {-2, -1, 1, 2},
        {-2, 0, 0, 2},
        {-1, 0, 0, 1},
    };
    ctx.expect_eq(sol.fourSum({1, 0, -1, 0, -2, 2}, 0), expected);
  });

  ctx.test("example_2_all_twos", [&] {
    const std::vector<std::vector<int>> expected = {{2, 2, 2, 2}};
    ctx.expect_eq(sol.fourSum({2, 2, 2, 2, 2}, 8), expected);
  });

  ctx.test("all_zeros", [&] {
    const std::vector<std::vector<int>> expected = {{0, 0, 0, 0}};
    ctx.expect_eq(sol.fourSum({0, 0, 0, 0}, 0), expected);
  });

  ctx.test("rich_duplicates_deduplication", [&] {
    const std::vector<std::vector<int>> expected = {
        {-3, -2, 2, 3},  {-3, -1, 1, 3}, {-3, 0, 0, 3}, {-3, 0, 1, 2},
        {-2, -1, 0, 3},  {-2, -1, 1, 2}, {-2, 0, 0, 2}, {-1, 0, 0, 1},
    };
    ctx.expect_eq(sol.fourSum({-3, -2, -1, 0, 0, 1, 2, 3}, 0), expected);
  });

  ctx.test("fewer_than_four_elements", [&] {
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.fourSum({1, 2, 3}, 6), expected);
    ctx.expect_eq(sol.fourSum({}, 0), expected);
  });

  ctx.test("int32_overflow_guard", [&] {
    // 4 * 10^9 wraps to -294967296 in signed 32-bit int; 64-bit arithmetic
    // must reject this rather than producing a false positive.
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(
        sol.fourSum({1000000000, 1000000000, 1000000000, 1000000000},
                    -294967296),
        expected);
  });
}

}  // namespace
