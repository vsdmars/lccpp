// LeetCode 213: House Robber II
// https://leetcode.com/problems/house-robber-ii/
//
// Problem:
//   Given an integer array `nums` representing the non-negative amount of money
//   at each house arranged in a circle (so house 0 and house n-1 are adjacent),
//   return the maximum amount of money that can be robbed without robbing any
//   two adjacent houses.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 3, 1]
//     Output: 4
//     Explanation:
//       - Because nums.size() == 4 > 1, set sub_len = 3 and split the circular
//         street into view.first(3) = [1, 2, 3] and view.last(3) = [2, 3, 1].
//       - robLinear([1, 2, 3]) steps (prev, curr) through (0, 1), (1, 2), and (2,
//         max(2, 1 + 3) = 4), returning exclude_last = 4.
//       - robLinear([2, 3, 1]) steps (prev, curr) through (0, 2), (2, 3), and (3,
//         max(3, 2 + 1) = 3), returning exclude_first = 3.
//       - Returns std::max(4, 3) = 4.
//
//   Example 2:
//     Input:  nums = [10, 1, 1, 10]
//     Output: 11
//     Explanation:
//       - With sub_len = 3, split the cycle into exclude_last = robLinear([10, 1,
//         1]) and exclude_first = robLinear([1, 1, 10]).
//       - robLinear([10, 1, 1]) updates (prev, curr) through (0, 10), (10, 10), and
//         (10, max(10, 10 + 1) = 11), yielding 11.
//       - robLinear([1, 1, 10]) updates (prev, curr) through (0, 1), (1, 1), and
//         (1, max(1, 1 + 10) = 11), yielding 11.
//       - Returns std::max(11, 11) = 11, avoiding taking both circularly adjacent
//         endpoints nums[0] and nums[3].
//
//   Example 3:
//     Input:  nums = [1]
//     Output: 1
//     Explanation:
//       - The input is non-empty and satisfies nums.size() == 1uz, triggering the
//         single-house base case.
//       - Returns nums[0] = 1 directly because there are no adjacent houses in the
//         cycle.
//
// Algorithm:
//   Reduce the circular constraint to two independent linear House Robber
//   (#198) subproblems. Because house 0 and house n-1 cannot both be robbed,
//   any valid selection must omit at least one of them:
//     1. Consider only the prefix slice nums[0 .. n-2] (excluding the last).
//     2. Consider only the suffix slice nums[1 .. n-1] (excluding the first).
//   Each linear subproblem is solved in O(n) time and O(1) space using rolling
//   two-state DP (`prev` and `curr`), and the answer is the maximum of the two.
//   Single-house and empty inputs are handled directly as base cases.
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - `std::span<const int>` zero-copy subrange views over the input vector.
//   - Two rolling scalar DP variables (`prev` and `curr`) per linear pass.
//
// Real-World Applications:
//   - Maximum-weight independent set on cycle graphs (e.g., ring-network node
//     activation where adjacent nodes cannot transmit simultaneously).
//   - Cyclic shift or periodic maintenance scheduling across a repeating week
//     or day boundary.
//   - Circular seating or perimeter sensor placement with non-adjacency rules.
//
// Related LeetCode Problems:
//   - #198 House Robber [done] (the linear subproblem solved twice here)
//   - #337 House Robber III [new] (extends non-adjacent selection to binary trees)
//   - #2560 House Robber IV [new] (binary search + greedy for min capability over k houses)
//   - #740 Delete and Earn [done] (value-aggregated House Robber reduction)
//   - #256 Paint House [done] (linear DP with adjacent-house constraints)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum money obtainable from houses arranged in a circle.
  //
  // Time: O(n), Space: O(1).
  int rob(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }
    if (nums.size() == 1uz) {
      // With a single house there are no circular neighbor conflicts.
      return nums[0];
    }

    const std::span<const int> view(nums);
    const std::size_t sub_len = nums.size() - 1uz;

    // Split the cycle into two linear streets:
    //   - Excluding the last house:  nums[0 .. n-2]
    //   - Excluding the first house: nums[1 .. n-1]
    const int exclude_last = robLinear(view.first(sub_len));
    const int exclude_first = robLinear(view.last(sub_len));
    return std::max(exclude_last, exclude_first);
  }

 private:
  // Solves the linear House Robber subproblem over a contiguous view of houses.
  //
  // Invariant: `prev` = optimal loot up to 2 houses ago; `curr` = optimal loot
  // up to 1 house ago.
  // Time: O(k), Space: O(1), where k = houses.size().
  static int robLinear(std::span<const int> houses) {
    int prev = 0;
    int curr = 0;

    for (const int amount : houses) {
      const int next = std::max(curr, prev + amount);
      prev = curr;
      curr = next;
    }

    return curr;
  }
};

LC_PROBLEM(213, "House Robber II", "dp", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.rob({2, 3, 2}), 3);
    ctx.expect_eq(sol.rob({1, 2, 3, 1}), 4);
    ctx.expect_eq(sol.rob({1, 2, 3}), 3);
  });

  ctx.test("source_build_cases", [&] {
    ctx.expect_eq(sol.rob({1}), 1);
    ctx.expect_eq(sol.rob({1, 3, 1, 3, 100}), 103);
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.rob({}), 0);
    ctx.expect_eq(sol.rob({0}), 0);
    ctx.expect_eq(sol.rob({5, 9}), 9);
    ctx.expect_eq(sol.rob({9, 5}), 9);
    ctx.expect_eq(sol.rob({0, 0, 0}), 0);
  });

  ctx.test("cycle_forces_dropping_one_endpoint", [&] {
    // Linearly [10, 1, 1, 10] gives 20, but in a circle endpoints conflict.
    ctx.expect_eq(sol.rob({10, 1, 1, 10}), 11);
    ctx.expect_eq(sol.rob({200, 3, 140, 20, 10}), 340);
  });
}

}  // namespace
