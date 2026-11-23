// LeetCode 1340: Jump Game V
// https://leetcode.com/problems/jump-game-v/
//
// Problem:
//   Given an integer array `arr` and a maximum jump radius `d`, you may start
//   at any index `i` and repeatedly jump to `j` within `1 <= |i - j| <= d`
//   (staying in bounds) provided that `arr[i] > arr[j]` and every intermediate
//   index `k` strictly between `i` and `j` also satisfies `arr[i] > arr[k]`.
//   Return the maximum number of indices you can visit.
//
// Examples:
//   Example 1:
//     Input:  arr = [6, 4, 14, 6, 8, 13, 9, 7, 10, 6, 12], d = 2
//     Output: 4
//     Explanation:
//       - dfs(6) at value 9 jumps right to index 7 (7 < 9, dfs(7) = 1) while left
//         neighbor index 5 (13 >= 9) blocks leftward jumps, yielding memo[6] = 2.
//       - dfs(8) at value 10 can jump left to index 6 (9 < 10) for 1 + memo[6] = 3
//         or to index 7 or 9 for 2, setting memo[8] = 3.
//       - dfs(10) at value 12 scans left up to d = 2 and jumps to index 8 (10 <
//         12), achieving 1 + memo[8] = 4 (path 10 -> 8 -> 6 -> 7).
//       - After computing dfs(i) for all indices, std::ranges::max(memo) returns 4.
//
//   Example 2:
//     Input:  arr = [3, 3, 3, 3, 3], d = 3
//     Output: 1
//     Explanation:
//       - For each index i in 0..4, dfs(i) initializes best = 1.
//       - Both the right scan (arr[i + 1] == 3 >= arr[i]) and left scan (arr[i - 1]
//         == 3 >= arr[i]) break immediately at step = 1 whenever in bounds.
//       - Every entry in memo becomes 1, so std::ranges::max(memo) returns 1.
//
//   Example 3:
//     Input:  arr = [10, 12, 1], d = 2
//     Output: 2
//     Explanation:
//       - At index 0 (10), the right scan stops at index 1 (12 >= 10), blocking a
//         jump of distance 2 over index 1 to index 2 (1), so memo[0] = 1.
//       - At index 2 (1), the left scan stops immediately at index 1 (12 >= 1), so
//         memo[2] = 1.
//       - At index 1 (12), jumping left to index 0 gives 1 + memo[0] = 2 and
//         jumping right to index 2 gives 1 + memo[2] = 2, so memo[1] = 2 and
//         max(memo) returns 2.
//
// Algorithm:
//   Top-down DFS with memoization over the implicit DAG of valid jumps. Because
//   every legal jump moves to a strictly smaller value (`arr[i] > arr[j]`), the
//   reachability graph is strictly acyclic (a DAG), so longest-path lengths are
//   well-defined and satisfy optimal substructure.
//   For each index `i`, `dfs(i)` scans outward to the right (`i + 1 .. i + d`)
//   and to the left (`i - 1 .. i - d`), stopping early in each direction as
//   soon as it encounters a bar with height `>= arr[i]` (since that bar blocks
//   all farther jumps in that direction). For every reachable landing index `j`,
//   we relax `best = max(best, 1 + dfs(j))`, starting from `best = 1` (visiting
//   `i` alone). Computing `dfs(i)` for every `i` and taking the global maximum
//   yields the answer.
//   Time Complexity:  O(n * d), where `n = arr.size()`.
//   Space Complexity: O(n) for the memoization table and recursion stack.
//
// Data Structures:
//   - 1D memoization vector (`std::vector<int>` of size `n`) caching the
//     longest valid jump sequence starting at each index (`0` = uncomputed).
//
// Real-World Applications:
//   - Longest-path computation in directed acyclic graphs (critical-path
//     scheduling in build pipelines and task DAGs).
//   - Line-of-sight / visibility-constrained routing across elevation profiles
//     (ski-slope descent planning, radio-relay placement).
//
// Related LeetCode Problems:
//   - #55 Jump Game [done] (greedy reachability across an array)
//   - #45 Jump Game II [done] (minimum jumps to reach array end)
//   - #1306 Jump Game III [new] (BFS/DFS reachability with bidirectional fixed jumps)
//   - #1345 Jump Game IV [done] (shortest-path BFS with value-grouped teleport jumps)
//   - #329 Longest Increasing Path in a Matrix [done] (memoized DFS longest path on a strictly monotonic DAG)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum number of indices reachable in a strictly descending
  // line-of-sight jump chain of step radius at most `d`.
  // Time: O(n * d), Space: O(n).
  int maxJumps(const std::vector<int>& arr, int d) {
    const std::size_t n = arr.size();
    if (n == 0uz) {
      return 0;
    }

    const auto max_step = static_cast<std::size_t>(d);
    // `memo[i]` stores the longest chain starting at `i` (0 means unvisited).
    std::vector<int> memo(n, 0);

    auto dfs = [&](this auto&& self, std::size_t idx) -> int {
      if (memo[idx] != 0) {
        return memo[idx];
      }

      // Visiting `idx` itself always counts as 1 index.
      int best = 1;

      // Scan right up to `d` steps; stop immediately once a taller or equal bar
      // blocks the line of sight.
      for (std::size_t step = 1uz; step <= max_step; ++step) {
        const std::size_t next = idx + step;
        if (next >= n || arr[next] >= arr[idx]) {
          break;
        }
        best = std::max(best, self(next) + 1);
      }

      // Scan left up to `d` steps with the same line-of-sight cutoff.
      for (std::size_t step = 1uz; step <= max_step; ++step) {
        if (step > idx) {
          break;
        }
        const std::size_t next = idx - step;
        if (arr[next] >= arr[idx]) {
          break;
        }
        best = std::max(best, self(next) + 1);
      }

      memo[idx] = best;
      return best;
    };

    for (std::size_t i = 0uz; i < n; ++i) {
      dfs(i);
    }

    return std::ranges::max(memo);
  }
};

LC_PROBLEM(1340, "Jump Game V", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_python_build", [&] {
    const std::vector<int> arr{6, 4, 14, 6, 8, 13, 9, 7, 10, 6, 12};
    ctx.expect_eq(sol.maxJumps(arr, 2), 4);
  });

  ctx.test("example_2_all_equal_heights", [&] {
    const std::vector<int> arr{3, 3, 3, 3, 3};
    ctx.expect_eq(sol.maxJumps(arr, 3), 1);
  });

  ctx.test("example_3_strictly_decreasing", [&] {
    const std::vector<int> arr{7, 6, 5, 4, 3, 2, 1};
    ctx.expect_eq(sol.maxJumps(arr, 1), 7);
  });

  ctx.test("example_4_alternating_peaks", [&] {
    const std::vector<int> arr{7, 1, 7, 1, 7, 1};
    ctx.expect_eq(sol.maxJumps(arr, 2), 2);
  });

  ctx.test("edge_cases_single_increasing_and_blocker", [&] {
    ctx.expect_eq(sol.maxJumps({66}, 1), 1);
    ctx.expect_eq(sol.maxJumps({1, 2, 3, 4, 5}, 2), 5);
    // Index 0 (10) cannot jump over index 1 (12) to reach index 2 (1).
    ctx.expect_eq(sol.maxJumps({10, 12, 1}, 2), 2);
  });
}

}  // namespace
