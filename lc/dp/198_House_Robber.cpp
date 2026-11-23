// LeetCode 198: House Robber
// https://leetcode.com/problems/house-robber/
//
// Problem:
//   Given an integer array `nums` representing the non-negative amount of money
//   stashed at each house along a street, determine the maximum total amount of
//   money that can be robbed in one night without robbing two adjacent houses.
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 7, 9, 3, 1]
//     Output: 12
//     Explanation:
//       - Initialize rolling state variables prev = 0 and curr = 0 before scanning
//         nums.
//       - Process houses 0 and 1: amount = 2 gives next = max(0, 0 + 2) = 2 (prev =
//         0, curr = 2), and amount = 7 gives next = max(2, 0 + 7) = 7 (prev = 2,
//         curr = 7).
//       - At house 2 (amount = 9), next = max(7, 2 + 9) = 11 updates (prev, curr)
//         to (7, 11); at house 3 (amount = 3), next = max(11, 7 + 3) = 11 updates
//         (prev, curr) to (11, 11).
//       - At house 4 (amount = 1), next = max(11, 11 + 1) = 12 updates curr to 12
//         and returns 12.
//
//   Example 2:
//     Input:  nums = [2, 1, 1, 2]
//     Output: 4
//     Explanation:
//       - Start with prev = 0, curr = 0 and process index 0 (2) to reach prev = 0,
//         curr = 2.
//       - At index 1 (1), next = max(2, 0 + 1) = 2 keeps curr = 2 and sets prev =
//         2.
//       - At index 2 (1), next = max(2, 2 + 1) = 3 updates (prev, curr) to (2, 3).
//       - At index 3 (2), next = max(3, 2 + 2) = 4 updates curr to 4 (robbing
//         houses 0 and 3) and returns 4.
//
//   Example 3:
//     Input:  nums = []
//     Output: 0
//     Explanation:
//       - Initialize prev = 0 and curr = 0.
//       - The range-based loop over nums runs zero iterations and returns curr = 0.
//
// Algorithm:
//   Bottom-up dynamic programming with O(1) space optimization.
//   Let dp[i] denote the maximum money obtainable from the prefix nums[0..i].
//   At each house i with value v = nums[i], we either skip it (keeping dp[i-1])
//   or rob it (adding v to dp[i-2]), giving the recurrence:
//     dp[i] = max(dp[i-1], dp[i-2] + nums[i]).
//   Since each state depends only on the two immediately preceding states, we
//   roll two scalar variables (`prev` for dp[i-2] and `curr` for dp[i-1]) across
//   a single linear pass.
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Two rolling scalar DP state variables (`prev` and `curr`) replacing an
//   explicit 1D DP array.
//
// Real-World Applications:
//   - Weighted independent set on path graphs (e.g., non-overlapping job or
//     shift scheduling with mandatory rest gaps).
//   - Wireless channel / cell-tower frequency allocation along linear corridors
//     where adjacent transmitters interfere.
//   - Resource placement along pipelines or assembly lines with spacing rules.
//
// Related LeetCode Problems:
//   - #213 House Robber II [done] (circular street variant breaking the cycle into two linear passes)
//   - #337 House Robber III [new] (tree-structured houses solved via post-order tree DP)
//   - #740 Delete and Earn [done] (reduces directly to House Robber over value-bucket totals)
//   - #256 Paint House [done] (adjacent-constraint linear DP with multi-state transitions)
//   - #2140 Solving Questions With Brainpower [new] (generalized skip-distance linear DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum loot achievable from `nums` without robbing two
  // adjacent houses.
  //
  // Invariant: Before processing house `amount`, `prev` holds the optimal
  // total from all houses up to two steps ago, and `curr` holds the optimal
  // total from all houses up to the immediately preceding house.
  //
  // Time: O(n), Space: O(1).
  int rob(const std::vector<int>& nums) {
    // Base states before the first house: zero loot with 0 houses considered.
    int prev = 0;
    int curr = 0;

    for (const int amount : nums) {
      // Choose the better of skipping this house (`curr`) or robbing it
      // (`prev + amount`, which is guaranteed non-adjacent).
      const int next = std::max(curr, prev + amount);
      prev = curr;
      curr = next;
    }

    return curr;
  }
};

LC_PROBLEM(198, "House Robber", "dp", ctx) {
  Solution sol;

  ctx.test("source_build_case", [&] {
    // Robbing houses 0 and 3 yields 2 + 2 = 4 (skipping two middle houses).
    ctx.expect_eq(sol.rob({2, 1, 1, 2}), 4);
  });

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.rob({1, 2, 3, 1}), 4);
    ctx.expect_eq(sol.rob({2, 7, 9, 3, 1}), 12);
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.rob({}), 0);
    ctx.expect_eq(sol.rob({5}), 5);
    ctx.expect_eq(sol.rob({2, 7}), 7);
    ctx.expect_eq(sol.rob({7, 2}), 7);
    ctx.expect_eq(sol.rob({0, 0, 0, 0}), 0);
  });

  ctx.test("alternating_and_clustered_values", [&] {
    ctx.expect_eq(sol.rob({10, 1, 1, 10, 1, 1, 10}), 30);
    ctx.expect_eq(sol.rob({4, 1, 2, 7, 5, 3, 1}), 14);
  });
}

}  // namespace
