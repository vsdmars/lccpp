// https://leetcode.com/problems/largest-color-value-in-a-directed-graph/
//
// Given a directed graph of `n` nodes where `colors[i]` is a lowercase letter
// assigned to node `i`, the color value of a directed path is the maximum
// frequency of any single color along that path. Return the largest color value
// over all valid paths, or -1 if the graph contains a cycle.

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
