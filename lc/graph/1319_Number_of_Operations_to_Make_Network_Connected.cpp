// https://leetcode.com/problems/number-of-operations-to-make-network-connected/
//
// Given `n` computers labeled 0..n-1 and undirected cable `connections`, you
// may relocate existing cables between any pair of disconnected computers.
// Return the minimum number of cable moves needed to connect all computers, or
// -1 if impossible.
//
// Key observation:
// Connecting `n` nodes requires at least `n - 1` cables total. When at least
// `n - 1` cables exist, any `C` connected components have enough redundant
// cables inside cycles to bridge them together in exactly `C - 1` operations.

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Verifies edge sufficiency and counts connected components via DFS.
  // Time: O(V + E), Space: O(V + E).
  int makeConnected(int n, const std::vector<std::vector<int>>& connections) {
    if (n <= 1) {
      return 0;
    }
    if (connections.size() < static_cast<std::size_t>(n - 1)) {
      return -1;
    }

    const auto count = static_cast<std::size_t>(n);
    std::vector<std::vector<int>> adj(count);
    for (const auto& edge : connections) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      adj[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    std::vector<std::uint8_t> visited(count, 0);
    auto dfs = [&](this auto&& self, int u) -> void {
      visited[static_cast<std::size_t>(u)] = 1;
      for (const int v : adj[static_cast<std::size_t>(u)]) {
        if (visited[static_cast<std::size_t>(v)] == 0) {
          self(v);
        }
      }
    };

    int components = 0;
    for (int node = 0; node < n; ++node) {
      if (visited[static_cast<std::size_t>(node)] == 0) {
        dfs(node);
        ++components;
      }
    }
    return components - 1;
  }
};

LC_PROBLEM(1319, "Number of Operations to Make Network Connected", "graph",
           ctx) {
  Solution sol;

  ctx.test("example 1: 4 computers, 1 move", [&] {
    ctx.expect_eq(sol.makeConnected(4, {{0, 1}, {0, 2}, {1, 2}}), 1);
  });

  ctx.test("example 2: 6 computers, 2 moves", [&] {
    ctx.expect_eq(
        sol.makeConnected(6, {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}}), 2);
  });

  ctx.test("example 3: not enough cables", [&] {
    ctx.expect_eq(sol.makeConnected(6, {{0, 1}, {0, 2}, {0, 3}, {1, 2}}), -1);
  });

  ctx.test("python sample: 5 computers with 1 isolated node", [&] {
    ctx.expect_eq(sol.makeConnected(5, {{0, 1}, {0, 2}, {0, 3}, {1, 2}}), 1);
  });
}

}  // namespace
