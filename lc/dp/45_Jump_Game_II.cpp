// LeetCode 45: Jump Game II
// https://leetcode.com/problems/jump-game-ii/
//
// Problem:
//   Given an array of non-negative integers nums of length n (1 <= n <= 10^4),
//   where nums[i] is the maximum forward jump length from index i, you start at
//   index 0 and are guaranteed to be able to reach index n - 1. Return the
//   minimum number of jumps required to reach the last index.
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 3, 1, 1, 4]
//     Output: 2
//     Explanation:
//       - Initialize last = 4, jumps = 0, current_end = 0, and farthest = 0.
//       - At i = 0 (nums[0] = 2), update farthest = 2; since i == current_end (0),
//         increment jumps to 1 and set current_end = 2.
//       - At i = 1 (nums[1] = 3), reach = 1 + 3 = 4 updates farthest to 4.
//       - At i = 2 (nums[2] = 1), i == current_end (2) triggers jumps = 2 and
//         current_end = 4 >= last, breaking early to return 2.
//
//   Example 2:
//     Input:  nums = [2, 3, 0, 1, 4]
//     Output: 2
//     Explanation:
//       - Initialize last = 4, jumps = 0, current_end = 0, and farthest = 0.
//       - At i = 0 (nums[0] = 2), farthest becomes 2, and reaching current_end = 0
//         advances jumps to 1 with current_end = 2.
//       - At i = 1 (nums[1] = 3), farthest expands to 4, so scanning i = 2 (nums[2]
//         = 0) keeps farthest = 4 instead of getting stuck at the zero.
//       - At i = 2 == current_end, increment jumps to 2, set current_end = 4 >=
//         last, and break early to return 2.
//
//   Example 3:
//     Input:  nums = [0]
//     Output: 0
//     Explanation:
//       - Check n = nums.size() = 1 against the guard n <= 1uz.
//       - Because index 0 is already the last index, return 0 immediately without
//         executing the frontier loop.
//
// Algorithm:
//   1. Greedy implicit-BFS (`jump`): Treat each jump count as a BFS level over
//      array indices. Maintain the inclusive upper bound `current_end` of the
//      current jump level and `farthest`, the maximum index reachable from any
//      position visited so far. Scanning i from 0 to n - 2, update `farthest`;
//      whenever i reaches `current_end`, all positions at the current jump
//      depth have been explored, so increment `jumps` and advance `current_end`
//      to `farthest` (stopping early once `current_end >= n - 1`).
//      Time Complexity:  O(n) — each index is inspected at most once.
//      Space Complexity: O(1) — only scalar frontier trackers are maintained.
//   2. Bottom-up DP (`jumpDp`): Included as a secondary reference matching the
//      DP formulation in the original notes, where dp[i] is the minimum jumps
//      from index i to n - 1.
//      Time Complexity:  O(n * max(nums)), worst-case O(n^2).
//      Space Complexity: O(n).
//
// Data Structures:
//   - Scalar frontier bounds (`current_end`, `farthest`, `jumps`) modeling
//     implicit BFS layers without an explicit queue.
//   - std::vector<int> dp (in the reference DP method) storing optimal suffix
//     jump counts.
//
// Real-World Applications:
//   - Minimum-hop packet forwarding and wireless relay selection across
//     variable-range transmitters along a linear corridor.
//   - Fueling / EV charging stop minimization along a highway with fixed
//     station capacities.
//   - Interval covering problems (selecting the fewest intervals covering a
//     target span, e.g., video/log segment stitching).
//
// Related LeetCode Problems:
//   - #55 Jump Game [done] (feasibility check: can you reach the last index?)
//   - #1306 Jump Game III [new] (bidirectional BFS/DFS to reach a zero value)
//   - #1326 Minimum Number of Taps to Open to Water a Garden [new] (same greedy BFS)
//   - #1024 Video Stitching [done] (interval-covering equivalent of Jump Game II)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum number of jumps to reach the last index using an
  // implicit BFS / greedy level-frontier scan.
  // Time: O(n), Space: O(1).
  int jump(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 1uz) {
      return 0;
    }

    const std::size_t last = n - 1uz;
    int jumps = 0;
    std::size_t current_end = 0uz;  // Rightmost index reachable with `jumps`.
    std::size_t farthest = 0uz;     // Rightmost index reachable with `jumps + 1`.

    // Stop at last - 1: once i reaches `last`, we have already arrived.
    for (std::size_t i = 0uz; i < last; ++i) {
      const std::size_t reach =
          i + static_cast<std::size_t>(std::max(0, nums[i]));
      farthest = std::max(farthest, reach);

      // Finished scanning the current BFS layer; commit to the next jump.
      if (i == current_end) {
        ++jumps;
        current_end = farthest;
        if (current_end >= last) {
          break;
        }
      }
    }

    return jumps;
  }

  // Reference bottom-up DP solution computing minimum jumps from every index i
  // to the last index.
  // Time: O(n^2) worst case, Space: O(n).
  int jumpDp(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 1uz) {
      return 0;
    }

    constexpr int kInf = std::numeric_limits<int>::max() / 2;
    std::vector<int> dp(n, kInf);
    dp[n - 1uz] = 0;

    // Fill backwards so every reachable target `i + step` is already solved.
    for (std::size_t i = n - 1uz; i > 0uz; --i) {
      const std::size_t idx = i - 1uz;
      const std::size_t max_step =
          static_cast<std::size_t>(std::max(0, nums[idx]));
      const std::size_t limit = std::min(n - 1uz, idx + max_step);

      for (std::size_t next = idx + 1uz; next <= limit; ++next) {
        if (dp[next] != kInf) {
          dp[idx] = std::min(dp[idx], dp[next] + 1);
        }
      }
    }

    return dp[0];
  }
};

LC_PROBLEM(45, "Jump Game II", "dp", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> nums = {2, 3, 1, 1, 4};
    ctx.expect_eq(sol.jump(nums), 2);
    ctx.expect_eq(sol.jumpDp(nums), 2);
  });

  ctx.test("example_2_with_zero_trap", [&] {
    const std::vector<int> nums = {2, 3, 0, 1, 4};
    ctx.expect_eq(sol.jump(nums), 2);
    ctx.expect_eq(sol.jumpDp(nums), 2);
  });

  ctx.test("python_build_cases", [&] {
    // Both cases from the original Python build() helper.
    const std::vector<int> long_case = {2, 5, 1, 7, 1, 1, 1, 1, 1, 1, 1};
    ctx.expect_eq(sol.jump(long_case), 3);
    ctx.expect_eq(sol.jumpDp(long_case), 3);

    const std::vector<int> overshooting = {3, 2, 1};
    ctx.expect_eq(sol.jump(overshooting), 1);
    ctx.expect_eq(sol.jumpDp(overshooting), 1);
  });

  ctx.test("single_element_zero_jumps", [&] {
    const std::vector<int> single = {0};
    ctx.expect_eq(sol.jump(single), 0);
    ctx.expect_eq(sol.jumpDp(single), 0);
  });

  ctx.test("unit_steps_only", [&] {
    const std::vector<int> steps = {1, 1, 1, 1, 1};
    ctx.expect_eq(sol.jump(steps), 4);
    ctx.expect_eq(sol.jumpDp(steps), 4);
  });
}

}  // namespace
