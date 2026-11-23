// LeetCode 2025: Maximum Number Of Ways To Partition An Array
// https://leetcode.com/problems/maximum-number-of-ways-to-partition-an-array/
//
// Problem:
//   Given a 0-indexed integer array `nums` of length `n` and an integer `k`, a
//   pivot index `pivot` (1 <= pivot < n) is valid if the prefix sum
//   nums[0] + ... + nums[pivot - 1] equals the suffix sum
//   nums[pivot] + ... + nums[n - 1]. You may replace at most one element of
//   `nums` with `k` (or leave `nums` unchanged). Return the maximum possible
//   number of valid pivot indices achievable.
//   Constraints: 2 <= n <= 10^5; -10^5 <= nums[i], k <= 10^5.
//
// Algorithm:
//   Prefix/suffix difference decomposition with sliding left/right hash maps:
//   For each split point `p` in [0, n - 2] (corresponding to `pivot = p + 1`),
//   define `diff[p] = left_sum(p) - right_sum(p) = 2 * prefix[p] - total`.
//   All sums use 64-bit signed integers because `|total|` can reach 10^10.
//   - Leaving `nums` unchanged yields the count of splits with `diff[p] == 0`.
//   - Replacing `nums[i]` with `k` adds `delta = k - nums[i]` to `nums[i]`:
//     * For every split `p < i`, index `i` lies in the right half, so
//       `right_sum(p)` grows by `delta` and `new_diff[p] = diff[p] - delta`.
//       Thus split `p` becomes valid iff `diff[p] == delta`.
//     * For every split `p >= i`, index `i` lies in the left half, so
//       `left_sum(p)` grows by `delta` and `new_diff[p] = diff[p] + delta`.
//       Thus split `p` becomes valid iff `diff[p] == -delta`.
//   Sweep `i` from `0` to `n - 1`, maintaining two frequency maps:
//   `left_diffs` covering splits `p < i` and `right_diffs` covering splits
//   `p >= i`. At each `i`, the score for changing `nums[i]` to `k` is simply
//   `left_diffs[delta] + right_diffs[-delta]`. Then transfer `diff[i]` from
//   `right_diffs` to `left_diffs` before advancing `i`.
//   Time Complexity: O(n) average.
//   Space Complexity: O(n) for the `diffs` vector and frequency maps.
//
// Data Structures:
//   - std::vector<std::int64_t>: Stores `diff[p] = 2 * prefix[p] - total` for
//     each split point `p in [0, n - 2]`.
//   - Two std::unordered_map<std::int64_t, int> maps (`left_diffs` and
//     `right_diffs`): Track frequencies of split differences strictly left of
//     and at/right of the currently modified index `i` in O(1) average time.
//
// Real-World Applications:
//   - Distributed storage shard rebalancing where adjusting one hot-key weight
//     maximizes balanced split points across partitions.
//   - Financial portfolio or load-balancer sensitivity analysis ("what-if"
//     single-entry perturbation queries in O(1) per candidate).
//   - Streaming time-series change-point analysis with prefix-sum invariants.
//
// Related LeetCode Problems:
//   - #724 Find Pivot Index [done] (prefix vs. suffix sum equality without modification)
//   - #1991 Find the Middle Index in Array [new] (equivalent pivot sum partition check)
//   - #560 Subarray Sum Equals K [done] (prefix-sum frequency tracking with hash maps)
//   - #2270 Number of Ways to Split Array [new] (counting valid prefix/suffix splits)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum number of valid equal-sum partitions achievable after
  // changing at most one element of `nums` to `k`.
  // Time: O(n) average, Space: O(n).
  int waysToPartition(const std::vector<int>& nums, int k) {
    const std::size_t n = nums.size();
    if (n < 2uz) {
      return 0;
    }

    // Compute total sum in 64-bit integer arithmetic to prevent overflow
    // (n <= 10^5 and |nums[i]| <= 10^5 => |total| <= 10^10).
    std::int64_t total = 0;
    for (const int num : nums) {
      total += num;
    }

    // Phase 1: Precompute `diff[p] = left_sum(p) - right_sum(p)` for all
    // `n - 1` split points `p` (partitioning into [0..p] and [p+1..n-1]).
    // Initially all splits lie at or to the right of index 0 (`right_diffs`).
    const std::size_t split_count = n - 1uz;
    std::vector<std::int64_t> diffs;
    diffs.reserve(split_count);

    std::unordered_map<std::int64_t, int> left_diffs;
    std::unordered_map<std::int64_t, int> right_diffs;
    right_diffs.reserve(split_count);

    std::int64_t prefix = 0;
    for (std::size_t p = 0uz; p < split_count; ++p) {
      prefix += nums[p];
      const std::int64_t diff = (2 * prefix) - total;
      diffs.push_back(diff);
      ++right_diffs[diff];
    }

    // Baseline: leave `nums` unchanged (splits where `diff[p] == 0`).
    int best = lookup_count(right_diffs, 0);

    // Phase 2: Try replacing each `nums[i]` with `k`.
    // Invariant at start of iteration `i`:
    //   - `left_diffs` holds `diffs[0 .. i-1]` (splits where `i` is on right).
    //   - `right_diffs` holds `diffs[i .. n-2]` (splits where `i` is on left).
    for (std::size_t i = 0uz; i < n; ++i) {
      const std::int64_t delta =
          static_cast<std::int64_t>(k) - static_cast<std::int64_t>(nums[i]);
      const int ways =
          lookup_count(left_diffs, delta) + lookup_count(right_diffs, -delta);
      best = std::max(best, ways);

      // Advance boundary: split `i` moves from `right_diffs` to `left_diffs`
      // for all subsequent indices `> i`.
      if (i < split_count) {
        const std::int64_t d = diffs[i];
        --right_diffs[d];
        ++left_diffs[d];
      }
    }

    return best;
  }

 private:
  // Looks up `key` in `freq` without inserting zero-valued entries.
  [[nodiscard]] static int lookup_count(
      const std::unordered_map<std::int64_t, int>& freq, std::int64_t key) {
    const auto it = freq.find(key);
    return it != freq.end() ? it->second : 0;
  }
};

LC_PROBLEM(2025, "Maximum Number Of Ways To Partition An Array", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_change_first", [&] {
    ctx.expect_eq(sol.waysToPartition({2, -1, 2}, 3), 1);
  });

  ctx.test("example_2_leave_unchanged", [&] {
    ctx.expect_eq(sol.waysToPartition({0, 0, 0}, 1), 2);
  });

  ctx.test("example_3_complex", [&] {
    const std::vector<int> nums = {22,  4,  -25, -20, -15, 15, -16,
                                   7,   19, -10, 0,   -13, -14};
    ctx.expect_eq(sol.waysToPartition(nums, -33), 4);
  });

  ctx.test("two_elements", [&] {
    ctx.expect_eq(sol.waysToPartition({1, 5}, 5), 1);
    ctx.expect_eq(sol.waysToPartition({1, 5}, 9), 0);
  });

  ctx.test("large_values_no_overflow", [&] {
    // 25,000 * 100,000 = 2.5e9 > INT32_MAX, exercising 64-bit prefix sums.
    const std::vector<int> nums(25000uz, 100000);
    ctx.expect_eq(sol.waysToPartition(nums, 100000), 1);
  });
}

}  // namespace
