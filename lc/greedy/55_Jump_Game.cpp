// LeetCode 55: Jump Game
// https://leetcode.com/problems/jump-game/
//
// Problem:
//   Given an array of non-negative integers `nums`, you start at index 0. Each
//   element `nums[i]` represents the maximum jump length from position `i` (you
//   may jump anywhere from `0` to `nums[i]` steps forward). Determine whether
//   it is possible to reach the last index (`nums.size() - 1`).
//
// Algorithm:
//   Single-pass greedy farthest-reach tracking.
//   - Maintain `farthest`, the maximum index reachable from any position in the
//     prefix visited so far (`0..i-1`).
//   - Because every jump from `j` can land on ANY index in `[j, j + nums[j]]`,
//     the set of reachable indices forms a contiguous interval `[0, farthest]`.
//   - While scanning `i` from `0` to `nums.size() - 1`:
//       * If `i > farthest`, index `i` (and everything beyond it) is
//         unreachable -> return `false`.
//       * Otherwise expand `farthest = max(farthest, i + nums[i])`.
//       * Early exit: as soon as `farthest >= nums.size() - 1`, the last index
//         is guaranteed reachable -> return `true`.
//
//   Time Complexity:  O(n) single pass over `nums` (often terminating early).
//   Space Complexity: O(1) constant extra memory.
//
// Data Structures:
//   - Scalar reach boundary (`std::size_t farthest`): Collapses the entire
//     reachable prefix `[0, farthest]` into a single integer invariant.
//
// Real-World Applications:
//   - Interval coverage and range-reachability checks in wireless sensor /
//     cell-tower placement (verifying continuous signal coverage across a path).
//   - Fuel/battery-range feasibility checks along a linear route with fixed
//     recharge stations.
//   - Buffering and streaming window reachability in network packet / video
//     chunk prefetchers.
//
// Related LeetCode Problems:
//   - #45 Jump Game II [done] (minimum number of jumps to reach the last index via greedy BFS levels)
//   - #1306 Jump Game III [new] (bidirectional jumps modeled as graph BFS/DFS)
//   - #1326 Minimum Number of Taps to Open to Water a Garden [new] (interval covering reduction to Jump Game II)
//   - #1024 Video Stitching [done] (greedy farthest-reach interval coverage)
//   - #134 Gas Station [done] (single-pass greedy feasibility along an array)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff index `nums.size() - 1` is reachable starting from index 0.
  //
  // Invariant: Before inspecting index `i`, every index in `[0, min(farthest, n-1)]`
  // is reachable from index 0 using valid jumps.
  // Time: O(n), Space: O(1).
  bool canJump(const std::vector<int>& nums) {
    if (nums.size() <= 1) {
      return true;
    }

    const std::size_t target = nums.size() - 1;
    std::size_t farthest = 0;

    for (std::size_t i = 0; i < nums.size(); ++i) {
      // If we cannot even reach `i`, no later index is reachable either.
      if (i > farthest) {
        return false;
      }

      farthest = std::max(farthest, i + static_cast<std::size_t>(nums[i]));

      // Short-circuit as soon as the last index falls within our reach.
      if (farthest >= target) {
        return true;
      }
    }

    return true;
  }
};

LC_PROBLEM(55, "Jump Game", "greedy", ctx) {
  ctx.test("example_1_reachable", [&] {
    Solution sol;
    ctx.expect_true(sol.canJump({2, 3, 1, 1, 4}));
    ctx.expect_true(sol.canJump({1, 3, 1, 1, 4}));
  });

  ctx.test("example_2_trapped_at_zero", [&] {
    Solution sol;
    ctx.expect_false(sol.canJump({3, 2, 1, 0, 4}));
  });

  ctx.test("python_build_case_trailing_zeros", [&] {
    Solution sol;
    ctx.expect_false(sol.canJump({3, 1, 0, 0, 0}));
  });

  ctx.test("single_element_and_edge_cases", [&] {
    Solution sol;
    ctx.expect_true(sol.canJump({0}));
    ctx.expect_true(sol.canJump({5}));
    ctx.expect_false(sol.canJump({0, 1}));
    ctx.expect_true(sol.canJump({2, 0, 0}));
  });

  ctx.test("large_initial_jump_early_exit", [&] {
    Solution sol;
    ctx.expect_true(sol.canJump({10, 0, 0, 0, 0, 0}));
  });
}

}  // namespace
