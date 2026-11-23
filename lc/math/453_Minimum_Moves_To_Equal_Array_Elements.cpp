// LeetCode 453: Minimum Moves To Equal Array Elements
// https://leetcode.com/problems/minimum-moves-to-equal-array-elements/
//
// Problem:
//   Given an integer array `nums` of length n, return the minimum number of
//   moves required to make all elements in the array equal. In a single move,
//   you may increment any n - 1 elements of the array by 1.
//   Constraints: 1 <= n <= 10^5, -10^9 <= nums[i] <= 10^9, and the answer is
//   guaranteed to fit in a 32-bit signed integer.
//
// Algorithm:
//   Incrementing n - 1 elements by 1 changes relative differences identically
//   to decrementing the single remaining element by 1. Under this equivalent
//   view, equalizing the array requires reducing every element down to the
//   minimum value in `nums`. Each element x therefore needs (x - min_val)
//   decrements, so the minimum total moves is sum(x - min_val) over all x in
//   `nums`.
//   Time Complexity:  O(n) — one pass to find the minimum and one pass to sum
//                     offsets.
//   Space Complexity: O(1) — only scalar accumulators are used.
//
// Data Structures:
//   - Input `std::vector<int>` scanned sequentially in read-only fashion.
//   - 64-bit integer accumulator (`std::int64_t`) to prevent intermediate
//     signed overflow when element differences approach 2 * 10^9.
//
// Real-World Applications:
//   - Distributed clock synchronization and drift compensation where all nodes
//     except one advance together and total relative lag must be quantified.
//   - Load balancing and quota leveling across servers when global headroom
//     adjustments shift relative resource deficits.
//   - Inventory and supply-chain rebalancing to compute minimum adjustment
//     steps required to equalize stock levels across warehouses.
//
// Related LeetCode Problems:
//   - #462 Minimum Moves to Equal Array Elements II [new] (increment or decrement one element; optimal target is median)
//   - #1551 Minimum Operations to Make Array Equal [new] (paired increment/decrement operations on odd arithmetic progression)
//   - #2448 Minimum Cost to Make Array Equal [new] (weighted equalization solved via weighted median)
//   - #2141 Maximum Running Time of N Computers [new] (equalizing battery drain rates across devices)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum number of (n - 1)-element increments needed to make
  // every entry of `nums` equal.
  //
  // Invariant: Incrementing n - 1 elements by 1 preserves all pairwise
  // differences except relative to the unchosen element, which effectively
  // decreases by 1. Thus every element must drop to `min_val`.
  // Time: O(n), Space: O(1).
  int minMoves(const std::vector<int>& nums) {
    // Edge case: an empty or single-element array is already equalized.
    if (nums.size() <= 1uz) {
      return 0;
    }

    // Identify the global minimum; every other element must be reduced to it.
    const std::int64_t min_val = std::ranges::min(nums);

    // Accumulate total decrements in 64-bit arithmetic to avoid signed overflow
    // on inputs spanning [-10^9, 10^9].
    std::int64_t total_moves = 0;
    for (const int num : nums) {
      total_moves += static_cast<std::int64_t>(num) - min_val;
    }

    return static_cast<int>(total_moves);
  }
};

LC_PROBLEM(453, "Minimum Moves To Equal Array Elements", "math", ctx) {
  Solution sol;

  ctx.test("example_1_increasing", [&] {
    ctx.expect_eq(sol.minMoves({1, 2, 3}), 3);
  });

  ctx.test("example_2_already_equal", [&] {
    ctx.expect_eq(sol.minMoves({1, 1, 1}), 0);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.minMoves({42}), 0);
  });

  ctx.test("negative_values", [&] {
    ctx.expect_eq(sol.minMoves({-100, 0, 100}), 300);
  });

  ctx.test("large_spread_no_overflow", [&] {
    ctx.expect_eq(sol.minMoves({-1'000'000'000, 1'000'000'000}), 2'000'000'000);
  });

  ctx.test("unsorted_with_duplicates", [&] {
    ctx.expect_eq(sol.minMoves({5, 6, 8, 8, 5}), 7);
  });
}

}  // namespace
