// LeetCode 1345: Jump Game IV
// https://leetcode.com/problems/jump-game-iv/
//
// Problem:
//   Given an integer array `arr` (1 <= arr.size() <= 5e4), you start at index
//   0. In one step from index i, you can jump to:
//     1) i + 1 (if i + 1 < n),
//     2) i - 1 (if i - 1 >= 0), or
//     3) any index j != i such that arr[i] == arr[j].
//   Return the minimum number of steps required to reach the last index n - 1.
//
// Algorithm:
//   Unweighted shortest-path BFS with value-group pruning:
//   - Group all indices sharing the same value into an adjacency hash map:
//     value -> list of indices.
//   - Run a standard BFS starting from index 0, marking visited indices so
//     each index enters the queue at most once.
//   - From index `idx`, enqueue unvisited neighbors `idx - 1`, `idx + 1`, and
//     all unvisited indices in `value_indices[arr[idx]]`.
//   - Crucial O(n) pruning step: immediately erase `arr[idx]` from
//     `value_indices` after expanding it once. Any future visit to another
//     index with the same value would only re-scan already-visited indices,
//     which would degrade worst-case time to O(n^2) on arrays full of
//     duplicates.
//   - Time Complexity: O(n), since every index is enqueued at most once and
//     every value's index list is iterated at most once.
//   - Space Complexity: O(n) for the hash map, visited array, and BFS queue.
//
// Data Structures:
//   - Hash map (`std::unordered_map<int, std::vector<std::size_t>>`) mapping
//     each array value to all indices holding that value (implicit clique edges).
//   - Visited byte array (`std::vector<std::uint8_t>`) for O(1) deduplication.
//   - FIFO queue (`std::queue<std::pair<std::size_t, int>>`) storing
//     (index, steps) pairs for level-order BFS.
//
// Real-World Applications:
//   - Routing in teleportation/overlay graphs where nodes sharing a tag or
//     subnet form a virtual clique (e.g., transit hubs or SDN virtual switches).
//   - Shortest-path search over state graphs with equivalence-class shortcuts
//     (avoiding explicit quadratic edge materialization).
//   - Network latency optimization with multicast/shortcut links.
//
// Related LeetCode Problems:
//   - #55 Jump Game [done] (reachability greedy scan on jump array)
//   - #45 Jump Game II [done] (minimum jumps with contiguous range transitions)
//   - #1306 Jump Game III [new] (BFS/DFS reachability with +/- arr[i] jumps)
//   - #1340 Jump Game V [done] (DAG longest jump path with DP)
//   - #127 Word Ladder [done] (unweighted BFS with bucketed implicit edges)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum number of jumps from index 0 to the last index.
  // Time: O(n). Space: O(n).
  int minJumps(const std::vector<int>& arr) {
    const std::size_t n = arr.size();
    if (n <= 1uz) {
      return 0;
    }

    // Build implicit shortcut edges: value -> all indices with that value.
    std::unordered_map<int, std::vector<std::size_t>> value_indices;
    value_indices.reserve(n);
    for (std::size_t i = 0uz; i < n; ++i) {
      value_indices[arr[i]].push_back(i);
    }

    const std::size_t last_idx = n - 1uz;
    std::vector<std::uint8_t> visited(n, 0);
    std::queue<std::pair<std::size_t, int>> queue;

    visited[0uz] = 1;
    queue.emplace(0uz, 0);

    while (!queue.empty()) {
      const auto [idx, steps] = queue.front();
      queue.pop();

      const int next_steps = steps + 1;

      // Neighbor 1 & 2: step left (idx - 1) and step right (idx + 1).
      if (idx > 0uz && try_enqueue(idx - 1uz, next_steps, last_idx, visited,
                                   queue)) {
        return next_steps;
      }
      if ((idx + 1uz) < n &&
          try_enqueue(idx + 1uz, next_steps, last_idx, visited, queue)) {
        return next_steps;
      }

      // Neighbor 3: jump to any index sharing the same value, then erase the
      // bucket so each value's index list is scanned at most once (O(n) total).
      if (expand_value_group(arr[idx], next_steps, last_idx, value_indices,
                             visited, queue)) {
        return next_steps;
      }
    }

    return -1;
  }

 private:
  // Marks `next_idx` visited and enqueues it; returns true if `last_idx` is
  // reached so the caller can short-circuit immediately.
  static bool try_enqueue(std::size_t next_idx, int next_steps,
                          std::size_t last_idx,
                          std::vector<std::uint8_t>& visited,
                          std::queue<std::pair<std::size_t, int>>& queue) {
    if (visited[next_idx] != 0) {
      return false;
    }
    if (next_idx == last_idx) {
      return true;
    }
    visited[next_idx] = 1;
    queue.emplace(next_idx, next_steps);
    return false;
  }

  // Enqueues all unvisited indices sharing `value`, then erases the bucket to
  // prevent quadratic re-scanning of duplicate values.
  static bool expand_value_group(
      int value, int next_steps, std::size_t last_idx,
      std::unordered_map<int, std::vector<std::size_t>>& value_indices,
      std::vector<std::uint8_t>& visited,
      std::queue<std::pair<std::size_t, int>>& queue) {
    const auto it = value_indices.find(value);
    if (it == value_indices.end()) {
      return false;
    }

    for (const std::size_t next_idx : it->second) {
      if (try_enqueue(next_idx, next_steps, last_idx, visited, queue)) {
        return true;
      }
    }
    value_indices.erase(it);
    return false;
  }
};

LC_PROBLEM(1345, "Jump Game IV", "bfs", ctx) {
  ctx.test("example_1_mixed_shortcuts", [&] {
    Solution sol;
    const std::vector<int> arr = {100, -23, -23, 404, 100,
                                  23,  23,  23,  3,   404};
    ctx.expect_eq(sol.minJumps(arr), 3);
  });

  ctx.test("example_2_single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.minJumps({7}), 0);
  });

  ctx.test("example_3_direct_jump_to_end", [&] {
    Solution sol;
    ctx.expect_eq(sol.minJumps({7, 6, 9, 6, 9, 6, 9, 7}), 1);
  });

  ctx.test("example_4_no_duplicates", [&] {
    Solution sol;
    ctx.expect_eq(sol.minJumps({6, 1, 9}), 2);
  });

  ctx.test("example_5_repeated_middle_block", [&] {
    Solution sol;
    const std::vector<int> arr = {11, 22, 7, 7, 7, 7, 7, 7, 7, 22, 13};
    ctx.expect_eq(sol.minJumps(arr), 3);
  });

  ctx.test("python_build_case", [&] {
    Solution sol;
    const std::vector<int> arr = {11, 22, 7, 7, 7, 6, 7, 7,
                                  7,  22, 7, 7, 7, 7, 13};
    // 0(11) -> 1(22) -> 2(7) -> 13(7) -> 14(13) = 4 steps.
    ctx.expect_eq(sol.minJumps(arr), 4);
  });

  ctx.test("large_all_equal_linear_time", [&] {
    Solution sol;
    const std::vector<int> arr(20000uz, 42);
    ctx.expect_eq(sol.minJumps(arr), 1);
  });
}

}  // namespace
