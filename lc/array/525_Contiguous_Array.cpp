// LeetCode 525: Contiguous Array
// https://leetcode.com/problems/contiguous-array/
//
// Problem:
//   Given a binary array `nums` containing only `0`s and `1`s, return the
//   maximum length of a contiguous subarray with an equal number of `0`s
//   and `1`s.
//
// Examples:
//   Example 1:
//     Input:  nums = [0, 1, 1, 1, 0, 0, 1, 1]
//     Output: 6
//     Explanation:
//       - Seed `first_seen = {0: -1}` and set `balance = 0`, `max_len = 0`.
//       - At `i = 0` (`0`), `balance` becomes `-1` and `first_seen[-1] = 0` is
//         stored; at `i = 1` (`1`), `balance` returns to `0` (already at `-1`),
//         updating `max_len = 1 - (-1) = 2`.
//       - At `i = 2` (`1`) and `i = 3` (`1`), `balance` rises to `1` and `2`,
//         recording `first_seen[1] = 2` and `first_seen[2] = 3`.
//       - At `i = 4` (`0`), `balance` drops to `1` (already at `2`, span `4 - 2 =
//         2`); at `i = 5` (`0`), `balance` drops to `0` (already at `-1`), updating
//         `max_len = 5 - (-1) = 6`.
//       - At `i = 6` (`1`) and `i = 7` (`1`), `balance` becomes `1` and `2`
//         (already at `2` and `3`), giving spans of `4` and leaving `max_len = 6`.
//
//   Example 2:
//     Input:  nums = [0, 1, 0]
//     Output: 2
//     Explanation:
//       - Seed `first_seen = {0: -1}` with `balance = 0` and `max_len = 0`.
//       - At `i = 0` (`0`), update `balance = -1` and insert `first_seen[-1] = 0`.
//       - At `i = 1` (`1`), update `balance = 0`; since `0` is already in
//         `first_seen` at `-1`, set `max_len = max(0, 1 - (-1)) = 2`.
//       - At `i = 2` (`0`), update `balance = -1`; since `-1` is already at `0`,
//         check span `2 - 0 = 2` and return `max_len = 2`.
//
//   Example 3:
//     Input:  nums = [0, 0, 0, 0]
//     Output: 0
//     Explanation:
//       - Seed `first_seen = {0: -1}` with `balance = 0` and `max_len = 0`.
//       - Each `0` at indices `0` through `3` decrements `balance` by `1`,
//         producing strictly new prefix balances `-1`, `-2`, `-3`, and `-4`.
//       - Because no prefix balance repeats in `first_seen`, `max_len` is never
//         updated and `0` is returned.
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
