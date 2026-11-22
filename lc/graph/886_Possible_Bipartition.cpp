// https://leetcode.com/problems/possible-bipartition/
//
// Given `n` people numbered 1..n and pairs `[a, b]` indicating that `a` and `b`
// dislike each other, determine whether everyone can be partitioned into two
// groups such that no two people who dislike each other share a group.

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // DFS 2-coloring over the undirected dislike graph.
  // Time: O(V + E), Space: O(V + E).
  bool possibleBipartition(int n,
                           const std::vector<std::vector<int>>& dislikes) {
    const auto size = static_cast<std::size_t>(n) + 1uz;
    std::vector<std::vector<int>> adj(size);
    for (const auto& edge : dislikes) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      adj[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    // -1 = uncolored, 0 = group A, 1 = group B.
    std::vector<std::int8_t> color(size, -1);

    auto dfs = [&](this auto&& self, int person, std::int8_t expected) -> bool {
      const auto idx = static_cast<std::size_t>(person);
      if (color[idx] != -1) {
        return color[idx] == expected;
      }
      color[idx] = expected;
      const auto opposite = static_cast<std::int8_t>(expected ^ 1);
      for (const int other : adj[idx]) {
        if (!self(other, opposite)) {
          return false;
        }
      }
      return true;
    };

    for (int i = 1; i <= n; ++i) {
      if (color[static_cast<std::size_t>(i)] == -1 && !dfs(i, 0)) {
        return false;
      }
    }
    return true;
  }

  // BFS 2-coloring (mirrors rewrite_bfs() in 886_Possible_Bipartition.py).
  // Time: O(V + E), Space: O(V + E).
  bool rewriteBfs(int n, const std::vector<std::vector<int>>& dislikes) {
    const auto size = static_cast<std::size_t>(n) + 1uz;
    std::vector<std::vector<int>> adj(size);
    for (const auto& edge : dislikes) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      adj[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    // 0 = uncolored, +1 / -1 = the two groups.
    std::vector<std::int8_t> color(size, 0);
    std::queue<int> q;

    for (int i = 1; i <= n; ++i) {
      if (color[static_cast<std::size_t>(i)] != 0) {
        continue;
      }
      color[static_cast<std::size_t>(i)] = 1;
      q.push(i);

      while (!q.empty()) {
        const int cur = q.front();
        q.pop();
        const std::int8_t cur_color = color[static_cast<std::size_t>(cur)];

        for (const int nbr : adj[static_cast<std::size_t>(cur)]) {
          const auto unbr = static_cast<std::size_t>(nbr);
          if (color[unbr] == cur_color) {
            return false;
          }
          if (color[unbr] == 0) {
            color[unbr] = static_cast<std::int8_t>(-cur_color);
            q.push(nbr);
          }
        }
      }
    }

    return true;
  }
};

LC_PROBLEM(886, "Possible Bipartition", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: bipartite dislike graph", [&] {
    const std::vector<std::vector<int>> dislikes = {{1, 2}, {1, 3}, {2, 4}};
    ctx.expect_true(sol.possibleBipartition(4, dislikes));
    ctx.expect_true(sol.rewriteBfs(4, dislikes));
  });

  ctx.test("example 2: triangle cannot be 2-colored", [&] {
    const std::vector<std::vector<int>> dislikes = {{1, 2}, {1, 3}, {2, 3}};
    ctx.expect_false(sol.possibleBipartition(3, dislikes));
    ctx.expect_false(sol.rewriteBfs(3, dislikes));
  });

  ctx.test("example 3: 5-cycle cannot be 2-colored", [&] {
    const std::vector<std::vector<int>> dislikes = {
        {1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}};
    ctx.expect_false(sol.possibleBipartition(5, dislikes));
    ctx.expect_false(sol.rewriteBfs(5, dislikes));
  });

  ctx.test("disconnected components with odd cycle in second component", [&] {
    const std::vector<std::vector<int>> dislikes = {
        {1, 2}, {3, 4}, {4, 5}, {3, 5}};
    ctx.expect_false(sol.possibleBipartition(5, dislikes));
    ctx.expect_false(sol.rewriteBfs(5, dislikes));
  });
}

}  // namespace
