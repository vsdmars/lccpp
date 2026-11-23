// LeetCode 1696: Jump Game VI
// https://leetcode.com/problems/jump-game-vi/
//
// Problem:
//   Given a 0-indexed integer array `nums` (1 <= N <= 10^5, -10^4 <= nums[i]
//   <= 10^4) and a maximum jump length `k` (1 <= k <= 10^5), start at index 0
//   and jump forward by 1 to `k` steps per move until reaching index `N - 1`.
//   Your score is the sum of `nums[j]` across all visited indices `j`. Return
//   the maximum achievable score.
//
// Algorithm:
//   Dynamic Programming + Lazy-Deletion Max-Heap (Backward Sweep):
//   Let `dp[i]` be the maximum score obtainable starting at index `i` and
//   ending at `N - 1`. Then:
//     dp[N - 1] = nums[N - 1]
//     dp[i]     = nums[i] + max(dp[j] for j in [i + 1, min(N - 1, i + k)])
//   Iterate `i` from `N - 1` down to `0` while maintaining a max-heap of
//   `(dp[j], j)` pairs:
//   1. Lazily pop entries from the top of the heap whose index satisfies
//      `j - i > k` (out of the jump window `[i + 1, i + k]`).
//   2. Compute `dp[i] = nums[i] + (heap.empty() ? 0 : heap.top().score)`.
//   3. Push `(dp[i], i)` into the max-heap.
//   After `i = 0` is processed, `dp[0]` is the answer.
//   (Note: a monotonic decreasing deque can also maintain the sliding-window
//   maximum in O(N) time; this max-heap approach preserves the Python source
//   algorithm cleanly in O(N log N) time.)
//   Time Complexity:  O(N log N) — each index is pushed once and lazily popped
//                     at most once.
//   Space Complexity: O(N) for the max-heap in the worst case.
//
// Data Structures:
//   - Max-heap (`std::priority_queue<ScoreEntry>`): maintains `(dp[j], j)` so
//     the highest reachable DP score within `[i + 1, i + k]` is retrieved in
//     O(1) peek after O(log N) amortized lazy eviction of expired indices.
//
// Real-World Applications:
//   - Sliding-window dynamic programming for optimal checkpoint / keyframe
//     placement in video encoding and distributed stream recovery.
//   - Portfolio / energy-storage dispatch planning with bounded look-ahead or
//     recharge step limits.
//   - Sequence alignment and Viterbi-style decoding with bounded skip windows.
//
// Related LeetCode Problems:
//   - #239 Sliding Window Maximum [done] (core sliding-window max subproblem via heap or deque)
//   - #1425 Constrained Subsequence Sum [new] (same DP + sliding-window maximum recurrence)
//   - #45 Jump Game II [done] (minimum jumps to reach end of array)
//   - #55 Jump Game [done] (reachability variant without score weights)
//   - #1340 Jump Game V [done] (DAG longest-path DP over bounded jump windows)

#include "framework/lc_framework.h"

#include <compare>
#include <cstddef>
#include <queue>
#include <vector>

namespace {

// Stores the optimal suffix DP score starting at `index`. Max-heap orders by
// `score` descending.
struct ScoreEntry {
  int score = 0;
  std::size_t index = 0uz;

  auto operator<=>(const ScoreEntry&) const = default;
};

class Solution {
 public:
  // Computes the maximum score to jump from index 0 to index `nums.size() - 1`
  // with step size in `[1, k]` using backward DP accelerated by a max-heap.
  //
  // Time Complexity:  O(N log N)
  // Space Complexity: O(N)
  [[nodiscard]] int maxResult(const std::vector<int>& nums, int k) {
    if (nums.empty()) {
      return 0;
    }

    const auto max_step = static_cast<std::size_t>(k);
    std::priority_queue<ScoreEntry> max_heap;
    int best_from_i = 0;

    // Sweep backwards from `nums.size() - 1` down to 0 so `max_heap` holds
    // already-solved suffix states `j > i`.
    for (std::size_t rev = nums.size(); rev > 0uz; --rev) {
      const std::size_t i = rev - 1uz;

      // Lazily evict top entries that lie beyond the reachable window `i + k`.
      while (!max_heap.empty() && (max_heap.top().index - i) > max_step) {
        max_heap.pop();
      }

      // Base case (`i == n - 1`): heap is empty, so score is just `nums[i]`.
      const int best_next = max_heap.empty() ? 0 : max_heap.top().score;
      best_from_i = nums[i] + best_next;
      max_heap.push(ScoreEntry{.score = best_from_i, .index = i});
    }

    return best_from_i;
  }
};

LC_PROBLEM(1696, "Jump Game VI", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxResult({1, -1, -2, 4, -7, 3}, 2), 7);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxResult({10, -5, -2, 4, 0, 3}, 3), 17);
  });

  ctx.test("example_3_mixed_negatives", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxResult({1, -5, -20, 4, -1, 3, -6, -3}, 2), 0);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxResult({-42}, 1), -42);
    ctx.expect_eq(sol.maxResult({99}, 5), 99);
  });

  ctx.test("all_negatives_forces_best_stepping_stones", [&] {
    Solution sol;
    // Optimal path: -5 -> -2 -> -1 = -8 (skips -10).
    ctx.expect_eq(sol.maxResult({-5, -2, -10, -1}, 2), -8);
  });

  ctx.test("k_equals_1_visits_every_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxResult({2, -3, 4, -1}, 1), 2);
  });
}

}  // namespace
