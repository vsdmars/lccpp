// LeetCode 1857: Largest Color Value in a Directed Graph
// https://leetcode.com/problems/largest-color-value-in-a-directed-graph/
//
// Problem:
//   Given a directed graph of `n` nodes (`0..n-1`), a string `colors` where
//   `colors[i]` is a lowercase English letter (`'a'..'z'`) assigned to node
//   `i`, and directed `edges[j] = [u, v]`, define the color value of a directed
//   path as the maximum frequency of any single color among the nodes on that
//   path. Return the largest color value over all valid directed paths, or `-1`
//   if the graph contains a cycle. Constraints: 1 <= n <= 10^5,
//   0 <= edges.length <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  colors = "abaca", edges = [[0, 1], [0, 2], [2, 3], [3, 4]]
//     Output: 3
//     Explanation:
//       - Initialize indegree = [0, 1, 1, 1, 1], set each node's own letter count
//         in dp to 1 (e.g., dp[0]['a'] = 1, dp[1]['b'] = 1), and enqueue node 0.
//       - Dequeue node 0 (best = 1) and relax edges 0 -> 1 and 0 -> 2, updating
//         dp[1]['a'] = 1, dp[2]['a'] = 2 and enqueuing nodes 1 and 2 as their
//         indegrees reach 0.
//       - Dequeue node 1 (no outgoing edges) and node 2 (best = 2), relaxing 2 -> 3
//         to set dp[3]['a'] = 2, dp[3]['c'] = 1 and enqueue node 3.
//       - Dequeue node 3 (best = 2), relax 3 -> 4 so dp[4]['a'] = 3, then dequeue
//         node 4 (best = 3); all 5 nodes are processed, returning 3.
//
//   Example 2:
//     Input:  colors = "a", edges = [[0, 0]]
//     Output: -1
//     Explanation:
//       - Build the adjacency list for edge 0 -> 0, setting indegree = [1].
//       - No node has indegree 0, so the BFS queue starts empty and processed
//         remains 0.
//       - Because processed (0) != n (1), a directed cycle is detected and -1 is
//         returned.
//
//   Example 3:
//     Input:  colors = "abaa", edges = [[0, 1], [0, 2], [1, 3], [2, 3]]
//     Output: 3
//     Explanation:
//       - Initialize indegree = [0, 1, 1, 2], seed each node's dp count with its
//         own color, and enqueue node 0.
//       - Dequeue node 0 (best = 1) and relax 0 -> 1 (dp[1]['a'] = 1, dp[1]['b'] =
//         1) and 0 -> 2 (dp[2]['a'] = 2), enqueuing nodes 1 and 2.
//       - Dequeue node 1 to set dp[3]['a'] = 2 (indegree[3] = 1), then dequeue node
//         2 (best = 2) to update dp[3]['a'] = max(2, 2 + 1) = 3 and enqueue node 3.
//       - Dequeue node 3 (best = 3); processed reaches 4 == n, so 3 is returned.
//
// Algorithm:
//   Kahn's topological sort (BFS) combined with Directed Acyclic Graph (DAG)
//   dynamic programming. Let `dp[u][c]` hold the maximum count of color `c`
//   (`0 <= c < 26`) along any path ending at node `u`. Each node starts with
//   `dp[i][color(i)] = 1`, and all zero-in-degree nodes enter the queue. When
//   `u` is dequeued, all incoming predecessors have already finalized `dp[u]`,
//   so `best` is updated with `max_c dp[u][c]` and each outgoing edge `u -> v`
//   relaxes `dp[v][c] = max(dp[v][c], dp[u][c] + (c == color(v)))` across all
//   26 colors. Decrementing `indegree[v]` enqueues `v` once it reaches 0. If
//   fewer than `n` nodes are processed, a directed cycle prevented remaining
//   nodes from reaching in-degree 0, so `-1` is returned.
//   Time Complexity: O(26 * (V + E)) = O(V + E), as alphabet size is constant.
//   Space Complexity: O(26 * V + E) = O(V + E) for the DP table, adjacency
//     list, in-degree array, and queue.
//
// Data Structures:
//   - `std::vector<std::vector<std::size_t>> adj`: Directed adjacency list
//     storing outgoing neighbors for each vertex.
//   - `std::vector<int> indegree`: Tracks remaining unprocessed incoming edges
//     per node for Kahn's topological ordering and cycle detection.
//   - `std::vector<std::array<int, 26>> dp`: Per-node fixed-size color-count DP
//     table tracking peak frequency of each letter on paths ending at node `u`.
//   - `std::queue<std::size_t> q`: FIFO queue of zero-in-degree frontier nodes.
//
// Real-World Applications:
//   - Build-system and CI/CD DAG critical-path analysis: bounding the worst-
//     case count of a specific job category (e.g., GPU or database-migration
//     steps) on any dependency chain while catching circular dependencies.
//   - Static program analysis over acyclic call graphs to find maximum
//     occurrences of categorized operations or taint tags along execution paths.
//   - Workflow orchestration engines (e.g., Airflow, Bazel) computing per-
//     resource bottleneck metrics across DAG execution pipelines.
//
// Related LeetCode Problems:
//   - #207 Course Schedule [done] (directed cycle detection via Kahn's BFS algorithm)
//   - #210 Course Schedule II [done] (producing a topological order via in-degrees)
//   - #2050 Parallel Courses III [new] (DAG dynamic programming propagated in Kahn's
//     topological order)
//   - #329 Longest Increasing Path in a Matrix [done] (longest-path DP on an implicit
//     DAG)
//   - #802 Find Eventual Safe States [done] (identifying cycle-free nodes via
//     topological peeling)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <queue>
#include <ranges>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Kahn's topological sort combined with DAG dynamic programming.
  // `dp[u][c]` stores the maximum number of nodes of color `c` along any path
  // ending at node `u`. Processing nodes in topological order guarantees all
  // incoming predecessors have already updated `dp[u]` before `u` propagates
  // to its outgoing neighbors.
  // Time: O(26 * (V + E)), Space: O(26 * V + E).
  int largestPathValue(std::string_view colors,
                       const std::vector<std::vector<int>>& edges) {
    const std::size_t n = colors.size();
    std::vector<std::vector<std::size_t>> adj(n);
    std::vector<int> indegree(n, 0);

    for (const auto& edge : edges) {
      const auto u = static_cast<std::size_t>(edge[0]);
      const auto v = static_cast<std::size_t>(edge[1]);
      adj[u].push_back(v);
      ++indegree[v];
    }

    // Initialize each node's DP vector with its own color count of 1.
    std::vector<std::array<int, kAlphabetSize>> dp(n);
    std::queue<std::size_t> q;
    for (std::size_t i = 0uz; i < n; ++i) {
      dp[i][static_cast<std::size_t>(colors[i] - 'a')] = 1;
      if (indegree[i] == 0) {
        q.push(i);
      }
    }

    int best = 0;
    std::size_t processed = 0uz;

    while (!q.empty()) {
      const std::size_t u = q.front();
      q.pop();
      ++processed;

      best = std::max(best, std::ranges::max(dp[u]));

      for (const std::size_t v : adj[u]) {
        const auto v_color = static_cast<std::size_t>(colors[v] - 'a');
        for (std::size_t c = 0uz; c < kAlphabetSize; ++c) {
          const int candidate = dp[u][c] + (c == v_color ? 1 : 0);
          dp[v][c] = std::max(dp[v][c], candidate);
        }
        if (--indegree[v] == 0) {
          q.push(v);
        }
      }
    }

    // Any unprocessed node belongs to (or depends on) a directed cycle.
    return processed == n ? best : -1;
  }
};

LC_PROBLEM(1857, "Largest Color Value in a Directed Graph", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: path 0->2->3->4 has three 'a' nodes", [&] {
    ctx.expect_eq(sol.largestPathValue("abaca", {{0, 1}, {0, 2}, {2, 3}, {3, 4}}),
                  3);
  });

  ctx.test("example 2: self-loop returns -1", [&] {
    ctx.expect_eq(sol.largestPathValue("a", {{0, 0}}), -1);
  });

  ctx.test("isolated single node returns 1", [&] {
    ctx.expect_eq(sol.largestPathValue("z", {}), 1);
  });

  ctx.test("diamond DAG picks highest color frequency across branches", [&] {
    // 0(a) -> 1(b) -> 3(a) has 2 'a's; 0(a) -> 2(a) -> 3(a) has 3 'a's.
    ctx.expect_eq(sol.largestPathValue("abaa", {{0, 1}, {0, 2}, {1, 3}, {2, 3}}),
                  3);
  });
}

}  // namespace
