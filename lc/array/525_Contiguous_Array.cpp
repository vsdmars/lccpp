// LeetCode 525: Contiguous Array
// https://leetcode.com/problems/contiguous-array/
//
// Problem:
//   Given a binary array `nums` containing only `0`s and `1`s, return the
//   maximum length of a contiguous subarray with an equal number of `0`s
//   and `1`s.
//
// Algorithm:
//   Transform each `0` into `-1` and keep each `1` as `+1`. Under this
//   mapping, a contiguous subarray `nums[l..r]` has equal counts of `0`s and
//   `1`s if and only if its transformed sum is `0`, which holds precisely when
//   the running prefix balance at index `r` equals the prefix balance at
//   index `l - 1`:
//       balance(r) - balance(l - 1) == 0  <=>  balance(r) == balance(l - 1).
//
//   Maintain a hash map `first_seen` storing the earliest index where each
//   prefix balance occurred, seeded with `first_seen[0] = -1` to account for
//   balanced subarrays starting at index `0`. During a single linear scan:
//     - Update `balance += (nums[i] == 1 ? 1 : -1)`.
//     - If `balance` is already in `first_seen`, the subarray from
//       `first_seen[balance] + 1` to `i` has length `i - first_seen[balance]`;
//       update `max_len`.
//     - Otherwise, record `first_seen[balance] = i` (never overwriting later
//       so the earliest index maximizes subarray length).
//
//   Time Complexity:  O(n) average — single pass with O(1) hash map lookups.
//   Space Complexity: O(n) — at most `n + 1` distinct prefix balances stored.
//
// Data Structures:
//   - `std::unordered_map<int, int>` (`first_seen`): maps each encountered
//     prefix balance (`count(1) - count(0)`) to the earliest array index at
//     which that balance occurred.
//
// Real-World Applications:
//   - Finding the longest net-zero drift window in financial order-flow or
//     buy/sell imbalance streams.
//   - Detecting the longest load-balanced interval in A/B experiment traffic
//     splits or alternating producer/consumer event logs.
//   - Identifying GC-neutral or base-balanced windows in genomic binary
//     sequence analysis.
//
// Related LeetCode Problems:
//   - #560 Subarray Sum Equals K [done] (prefix sum + hash map counting subarrays matching a target sum)
//   - #325 Maximum Size Subarray Sum Equals k [done] (earliest-index prefix-sum hash map for arbitrary k)
//   - #523 Continuous Subarray Sum [done] (earliest-index prefix-sum modulo k in a hash map)
//   - #485 Max Consecutive Ones [done] (linear scan over a binary array)
//   - #1124 Longest Well-Performing Interval [new] (prefix balance where +1s must strictly exceed -1s)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum length of a contiguous subarray of `nums` with an
  // equal number of 0s and 1s.
  //
  // Invariant: `first_seen[b]` stores the smallest index `idx` (with `-1`
  // representing the empty prefix) such that `sum(nums[0..idx]` with 0 -> -1)
  // equals `b`.
  //
  // Time: O(n) average, Space: O(n).
  int findMaxLength(const std::vector<int>& nums) {
    // Seed balance 0 at virtual index -1 so balanced prefixes from index 0 are
    // measured correctly as `i - (-1) == i + 1`.
    std::unordered_map<int, int> first_seen;
    first_seen.emplace(0, -1);

    int balance = 0;
    int max_len = 0;

    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      // Map 1 -> +1 and 0 -> -1 so equal counts correspond to zero net change.
      balance += (nums[i] == 1) ? 1 : -1;
      const int idx = static_cast<int>(i);

      // Keep only the earliest index for each balance to maximize span length.
      if (const auto [it, inserted] = first_seen.try_emplace(balance, idx);
          !inserted) {
        max_len = std::max(max_len, idx - it->second);
      }
    }

    return max_len;
  }
};

LC_PROBLEM(525, "Contiguous Array", "array", ctx) {
  ctx.test("example_1_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxLength({0, 1, 1, 1, 0, 0, 1, 1}), 6);
  });

  ctx.test("example_2_three_elements", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxLength({0, 1, 0}), 2);
  });

  ctx.test("additional_python_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxLength({0, 1, 1}), 2);
    ctx.expect_eq(sol.findMaxLength({1, 1, 0}), 2);
    ctx.expect_eq(sol.findMaxLength({0, 1}), 2);
  });

  ctx.test("entire_array_balanced", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxLength({0, 0, 1, 0, 1, 1}), 6);
  });

  ctx.test("no_balanced_subarray", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxLength({0, 0, 0, 0}), 0);
    ctx.expect_eq(sol.findMaxLength({1, 1, 1}), 0);
  });

  ctx.test("empty_and_single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxLength({}), 0);
    ctx.expect_eq(sol.findMaxLength({0}), 0);
    ctx.expect_eq(sol.findMaxLength({1}), 0);
  });
}

}  // namespace
