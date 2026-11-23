// LeetCode 41: First Missing Positive
// https://leetcode.com/problems/first-missing-positive/
//
// Problem:
//   Given an unsorted integer array `nums` (which may contain negative
//   numbers, zeros, duplicates, and values larger than `nums.size()`), return
//   the smallest positive integer (>= 1) that does not appear in `nums`.
//   The algorithm must run in O(n) time and use O(1) auxiliary space.
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 4, -1, 1]
//     Output: 2
//     Explanation:
//       - With n = 4, at idx = 0 (nums[0] = 3), target = 2 holds -1 != 3, so swap
//         them to get [-1, 4, 3, 1] and stop since -1 < 1.
//       - At idx = 1 (nums[1] = 4), swap with target = 3 (1) to get [-1, 1, 3, 4],
//         then swap nums[1] = 1 with target = 0 (-1) to get [1, -1, 3, 4].
//       - At idx = 2 (3) and idx = 3 (4), each value already equals nums[target],
//         so Phase 1 ends with [1, -1, 3, 4].
//       - In Phase 2, idx = 0 holds 1 == 1 while idx = 1 holds -1 != 2, returning
//         2.
//
//   Example 2:
//     Input:  nums = [7, 8, 9, 11, 12]
//     Output: 1
//     Explanation:
//       - With n = 5, every element exceeds n, so the Phase 1 while condition
//         nums[idx] <= 5 is never satisfied and no swaps occur.
//       - In Phase 2, idx = 0 holds nums[0] = 7 != 1, immediately returning 1.
//
//   Example 3:
//     Input:  nums = [2, 2, 1, 1, 3, 3]
//     Output: 4
//     Explanation:
//       - At idx = 0 and idx = 1, nums[idx] = 2 matches nums[1] = 2, so the
//         duplicate check breaks without swapping.
//       - At idx = 2 (1), swap with nums[0] (2) to form [1, 2, 2, 1, 3, 3], then
//         break because nums[2] == nums[1] == 2.
//       - At idx = 4 (3), swap with nums[2] (2) to form [1, 2, 3, 1, 2, 3], while
//         remaining duplicate indices break cleanly.
//       - In Phase 2, indices 0, 1, and 2 match 1, 2, and 3, and idx = 3 holds 1 !=
//         4, returning 4.
//
// Algorithm:
//   In-place cyclic sort (index hashing). For an array of length n, the
//   smallest missing positive integer is guaranteed to lie in [1, n + 1] by
//   the pigeonhole principle. We therefore treat index `i` as the canonical
//   home of the value `i + 1`.
//   1. Partition / place pass: iterate through each index `idx`. While the
//      current value `v = nums[idx]` is in [1, n] and is not already at its
//      home slot `nums[v - 1]`, swap `nums[idx]` with `nums[v - 1]`. Every
//      swap places at least one value into its final home, so the inner
//      while-loop performs at most n swaps in total across the entire pass.
//   2. Scan pass: walk indices 0..n-1; the first index where `nums[idx] !=
//      idx + 1` reveals the smallest missing positive `idx + 1`. If every
//      slot matches, all integers 1..n are present and the answer is n + 1.
//   Time complexity: O(n).
//   Space complexity: O(1) auxiliary space (in-place mutation of `nums`).
//
// Data Structures:
//   - Input array `nums` (`std::vector<int>`): repurposed in-place as a
//     direct-addressed presence table mapping value `v` in [1, n] to slot
//     `v - 1`, avoiding any external hash set or boolean bitmap.
//
// Real-World Applications:
//   - Operating-system file descriptor (FD) and process ID (PID) allocators
//     finding the lowest unused non-negative handle from an active table.
//   - Database primary-key / sequence gap detection and compact ID recycling
//     in memory-constrained embedded systems.
//   - Network packet sequence-number tracking to identify the earliest
//     missing frame in a sliding receive window.
//
// Related LeetCode Problems:
//   - #268 Missing Number [done] (simpler variant with unique values in [0, n])
//   - #287 Find the Duplicate Number [done] (pigeonhole / index-as-pointer reasoning)
//   - #442 Find All Duplicates in an Array [done] (same in-place index marking technique)
//   - #448 Find All Numbers Disappeared in an Array [done] (same cyclic sort / sign marking on [1, n])
//   - #1539 Kth Missing Positive Number [done] (generalizes to the k-th missing positive on sorted input)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the smallest missing positive integer (>= 1) in `nums` by placing
  // every valid value v in [1, n] into index (v - 1) in-place.
  //
  // Invariant after phase 1: every value v in [1, n] that exists in `nums`
  // satisfies `nums[v - 1] == v`.
  //
  // Time: O(n) — each swap puts at least one element into its final position.
  // Space: O(1) auxiliary space.
  int firstMissingPositive(std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n == 0uz) {
      return 1;
    }

    // Phase 1: Cyclic placement. Repeatedly move `nums[idx]` to its home index
    // `nums[idx] - 1` until `nums[idx]` is out of [1, n] or the home slot
    // already holds the same value (which also prevents infinite loops on
    // duplicate entries).
    for (std::size_t idx = 0uz; idx < n; ++idx) {
      while (nums[idx] >= 1 && std::cmp_less_equal(nums[idx], n)) {
        // Safe: nums[idx] >= 1 guarantees no underflow, and <= n guarantees
        // `target` is strictly within [0, n - 1].
        const auto target = static_cast<std::size_t>(nums[idx] - 1);
        if (nums[idx] == nums[target]) {
          break;
        }
        std::swap(nums[idx], nums[target]);
      }
    }

    // Phase 2: The first slot whose value does not equal (idx + 1) exposes the
    // smallest missing positive integer.
    for (std::size_t idx = 0uz; idx < n; ++idx) {
      const int expected = static_cast<int>(idx + 1uz);
      if (nums[idx] != expected) {
        return expected;
      }
    }

    // Every integer in [1, n] is present, so the answer is n + 1.
    return static_cast<int>(n + 1uz);
  }
};

LC_PROBLEM(41, "First Missing Positive", "array", ctx) {
  Solution sol;

  ctx.test("example_1_contains_zero", [&] {
    std::vector<int> nums{1, 2, 0};
    ctx.expect_eq(sol.firstMissingPositive(nums), 3);
  });

  ctx.test("example_2_negative_and_unsorted", [&] {
    std::vector<int> nums{3, 4, -1, 1};
    ctx.expect_eq(sol.firstMissingPositive(nums), 2);
  });

  ctx.test("example_3_all_larger_than_n", [&] {
    std::vector<int> nums{7, 8, 9, 11, 12};
    ctx.expect_eq(sol.firstMissingPositive(nums), 1);
  });

  ctx.test("empty_and_single_element", [&] {
    std::vector<int> empty;
    ctx.expect_eq(sol.firstMissingPositive(empty), 1);

    std::vector<int> one{1};
    ctx.expect_eq(sol.firstMissingPositive(one), 2);

    std::vector<int> two{2};
    ctx.expect_eq(sol.firstMissingPositive(two), 1);
  });

  ctx.test("duplicates_and_complete_range", [&] {
    std::vector<int> dup{1, 1};
    ctx.expect_eq(sol.firstMissingPositive(dup), 2);

    std::vector<int> dup_mixed{2, 2, 1, 1, 3, 3};
    ctx.expect_eq(sol.firstMissingPositive(dup_mixed), 4);

    std::vector<int> complete{4, 2, 1, 3};
    ctx.expect_eq(sol.firstMissingPositive(complete), 5);
  });

  ctx.test("extreme_int_values", [&] {
    std::vector<int> extremes{std::numeric_limits<int>::min(), -1, 0,
                              std::numeric_limits<int>::max()};
    ctx.expect_eq(sol.firstMissingPositive(extremes), 1);

    std::vector<int> with_one{std::numeric_limits<int>::min(), 1,
                              std::numeric_limits<int>::max()};
    ctx.expect_eq(sol.firstMissingPositive(with_one), 2);
  });
}

}  // namespace
