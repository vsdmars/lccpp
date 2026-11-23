// LeetCode 312: Burst Balloons
// https://leetcode.com/problems/burst-balloons/
//
// Problem:
//   Given an integer array `nums` of length n representing balloons (each with
//   a non-negative value 0 <= nums[i] <= 100), burst all balloons to maximize
//   total coins collected. Bursting balloon i yields
//   `nums[left] * nums[i] * nums[right]` coins, where `left` and `right` are
//   the currently adjacent surviving balloons (with virtual boundary balloons
//   `nums[-1] = nums[n] = 1` that cannot be burst).
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 1, 5, 8]
//     Output: 167
//     Explanation:
//       - Pad the array with sentinel 1s at both ends to form padded = [1, 3, 1, 5,
//         8, 1] of length m = 6.
//       - For span len = 2 single-balloon intervals, compute dp[0][2] = 3, dp[1][3]
//         = 15, dp[2][4] = 40, and dp[3][5] = 40.
//       - For span len = 3, burst k = 1 last in (0, 3) for dp[0][3] = 1*3*5 + 0 +
//         15 = 30, burst k = 3 last in (1, 4) for dp[1][4] = 3*5*8 + 15 + 0 = 135,
//         and burst k = 4 last in (2, 5) for dp[2][5] = 48.
//       - For span len = 4, choosing k = 1 last in (0, 4) gives dp[0][4] = 1*3*8 +
//         0 + 135 = 159, and choosing k = 4 last in (1, 5) gives dp[1][5] = 3*8*1 +
//         135 + 0 = 159.
//       - For the full interval (0, 5), choosing k = 4 (balloon 8) as the last
//         burst maximizes 1*8*1 + dp[0][4] + dp[4][5] = 8 + 159 + 0 = 167.
//
//   Example 2:
//     Input:  nums = [1, 5]
//     Output: 10
//     Explanation:
//       - Pad nums to padded = [1, 1, 5, 1] with virtual boundary balloons at
//         indices 0 and 3.
//       - Compute span len = 2 base intervals: dp[0][2] = 1 * 1 * 5 = 5 and
//         dp[1][3] = 1 * 5 * 1 = 5.
//       - For interval (0, 3), trying k = 1 gives 1*1*1 + 0 + 5 = 6 while trying k
//         = 2 gives 1*5*1 + 5 + 0 = 10, so dp[0][3] = 10.
//
//   Example 3:
//     Input:  nums = [3, 0, 5]
//     Output: 20
//     Explanation:
//       - Pad nums to padded = [1, 3, 0, 5, 1]; all span len = 2 intervals score 0
//         because each multiplies by the zero-value balloon at index 2.
//       - For len = 3, dp[0][3] = 1*3*5 + 0 + 0 = 15 (bursting 0 then 3) and
//         dp[1][4] = 3*5*1 + 0 + 0 = 15 (bursting 0 then 5).
//       - For interval (0, 4), choosing k = 3 (value 5) last yields 1*5*1 +
//         dp[0][3] + dp[3][4] = 5 + 15 + 0 = 20, beating k = 1 (18) and k = 2 (0).
//
// Algorithm:
//   Interval Dynamic Programming (reverse-time framing). Thinking about which
//   balloon to burst *first* couples the left and right subproblems because
//   adjacent neighbors change dynamically. Instead, pad `nums` with virtual
//   1s at both ends (`padded[0] = padded[n + 1] = 1`) and choose which balloon
//   `k` is burst *last* inside an open interval `(left, right)`. Because `k`
//   is the last survivor in `(left, right)`, its neighbors at the moment it
//   bursts are guaranteed to be `padded[left]` and `padded[right]`, splitting
//   the interval cleanly into two independent subproblems `(left, k)` and
//   `(k, right)`:
//     dp[left][right] = max_{left < k < right} (
//         padded[left] * padded[k] * padded[right] + dp[left][k] + dp[k][right]
//     )
//   Both a top-down memoized recursion (using `-1` as the uncomputed sentinel
//   so zero-value balloons do not cause exponential recomputation) and a
//   bottom-up length-ordered table fill are provided.
//   Time Complexity:  O(n^3) — O(n^2) intervals, each iterating O(n) split
//                     points.
//   Space Complexity: O(n^2) for the DP/memoization table.
//
// Data Structures:
//   - Padded `std::vector<int>` of size n + 2 storing sentinel 1s at indices 0
//     and n + 1 so boundary multiplications require no special casing.
//   - 2D DP table `std::vector<std::vector<int>>` of dimensions (n + 2) x
//     (n + 2) caching optimal coins for every open interval `(left, right)`.
//
// Real-World Applications:
//   - Matrix chain multiplication optimization in linear-algebra libraries and
//     SQL/tensor query planners to minimize scalar operations.
//   - Optimal polygon triangulation in computational geometry and 3D mesh
//     generation.
//   - Optimal binary search tree construction and parser parenthesization in
//     compilers.
//
// Related LeetCode Problems:
//   - #1039 Minimum Score Triangulation of Polygon [new] (isomorphic interval DP choosing the last triangle apex)
//   - #1547 Minimum Cost to Cut a Stick [done] (interval DP over padded cut positions)
//   - #1000 Minimum Cost to Merge Stones [new] (generalized interval DP with k-way merges)
//   - #375 Guess Number Higher Or Lower II [done] (minimax interval DP over contiguous ranges)
//   - #664 Strange Printer [new] (interval DP decomposing range-paint operations)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Top-down memoized interval DP: returns the maximum coins obtainable by
  // bursting all balloons in `nums`.
  // Invariant: `solve(left, right)` computes the optimal score for bursting
  // all balloons strictly inside the open interval `(left, right)` in `padded`,
  // while `padded[left]` and `padded[right]` remain intact.
  // Time: O(n^3), Space: O(n^2).
  int maxCoins(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    // Pad both ends with virtual boundary balloons of value 1 so every real
    // balloon always has valid left and right boundary multipliers.
    std::vector<int> padded;
    padded.reserve(nums.size() + 2uz);
    padded.push_back(1);
    padded.insert(padded.end(), nums.begin(), nums.end());
    padded.push_back(1);

    const std::size_t m = padded.size();
    // Use -1 (not 0) as the uncomputed sentinel because balloons can have
    // value 0, making 0 a legitimate optimal score for an interval.
    std::vector<std::vector<int>> memo(m, std::vector<int>(m, -1));

    auto solve = [&](this auto&& self, std::size_t left,
                     std::size_t right) -> int {
      // Base case: no balloons strictly between `left` and `right`.
      if (left + 1uz >= right) {
        return 0;
      }
      if (memo[left][right] != -1) {
        return memo[left][right];
      }

      int best = 0;
      // Try every balloon `k` strictly inside `(left, right)` as the LAST
      // balloon to be burst in this interval.
      for (std::size_t k = left + 1uz; k < right; ++k) {
        const int burst_coins = padded[left] * padded[k] * padded[right];
        const int total = burst_coins + self(left, k) + self(k, right);
        best = std::max(best, total);
      }

      memo[left][right] = best;
      return best;
    };

    return solve(0uz, m - 1uz);
  }

  // Bottom-up iterative interval DP: fills intervals in strictly increasing
  // span `len = right - left` so subproblems `(left, k)` and `(k, right)` are
  // always computed before `(left, right)`.
  // Time: O(n^3), Space: O(n^2).
  int maxCoinsBottomUp(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    std::vector<int> padded;
    padded.reserve(nums.size() + 2uz);
    padded.push_back(1);
    padded.insert(padded.end(), nums.begin(), nums.end());
    padded.push_back(1);

    const std::size_t m = padded.size();
    std::vector<std::vector<int>> dp(m, std::vector<int>(m, 0));

    // Span `len` starts at 2 because open interval `(left, left + 2)` is the
    // smallest interval containing at least one interior balloon.
    for (std::size_t len = 2uz; len < m; ++len) {
      for (std::size_t left = 0uz; left + len < m; ++left) {
        const std::size_t right = left + len;
        int best = 0;
        for (std::size_t k = left + 1uz; k < right; ++k) {
          const int burst_coins = padded[left] * padded[k] * padded[right];
          best = std::max(best, burst_coins + dp[left][k] + dp[k][right]);
        }
        dp[left][right] = best;
      }
    }

    return dp[0uz][m - 1uz];
  }
};

LC_PROBLEM(312, "Burst Balloons", "dp", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<int> nums = {3, 1, 5, 8};
    ctx.expect_eq(sol.maxCoins(nums), 167);
    ctx.expect_eq(sol.maxCoinsBottomUp(nums), 167);
  });

  ctx.test("two_balloons", [&] {
    Solution sol;
    const std::vector<int> nums = {1, 5};
    ctx.expect_eq(sol.maxCoins(nums), 10);
    ctx.expect_eq(sol.maxCoinsBottomUp(nums), 10);
  });

  ctx.test("single_balloon", [&] {
    Solution sol;
    const std::vector<int> nums = {7};
    ctx.expect_eq(sol.maxCoins(nums), 7);
    ctx.expect_eq(sol.maxCoinsBottomUp(nums), 7);
  });

  ctx.test("empty_input", [&] {
    Solution sol;
    const std::vector<int> nums = {};
    ctx.expect_eq(sol.maxCoins(nums), 0);
    ctx.expect_eq(sol.maxCoinsBottomUp(nums), 0);
  });

  ctx.test("balloons_with_zeros", [&] {
    Solution sol;
    const std::vector<int> all_zeros = {0, 0, 0, 0};
    ctx.expect_eq(sol.maxCoins(all_zeros), 0);
    ctx.expect_eq(sol.maxCoinsBottomUp(all_zeros), 0);

    // Burst 0 first (0 coins), then 3 (1*3*5=15), then 5 (1*5*1=5) => 20.
    const std::vector<int> mixed = {3, 0, 5};
    ctx.expect_eq(sol.maxCoins(mixed), 20);
    ctx.expect_eq(sol.maxCoinsBottomUp(mixed), 20);
  });
}

}  // namespace
