// https://leetcode.com/problems/redundant-connection-ii/description/
//
// A rooted tree with N nodes (labeled 1..N) has had one extra directed edge
// added. Return the edge `[u, v]` that can be removed so the remaining graph is
// a valid rooted tree. If multiple answers exist, return the one occurring last
// in `edges`.
//
// Three failure modes can occur after adding one directed edge:
//   1. A node has two parents, with no cycle -> remove the second incoming edge.
//   2. Every node has at most one parent, with a cycle -> remove the last edge
//      that closes the cycle.
//   3. A node has two parents AND there is a cycle -> remove whichever of the
//      two candidate incoming edges participates in the cycle.

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Identifies candidate 2-parent edges first, then uses Union-Find (skipping
  // the second candidate) to distinguish all three failure modes in O(N alpha(N)).
  // Time: O(N * alpha(N)), Space: O(N).
  std::vector<int> findRedundantDirectedConnection(
      const std::vector<std::vector<int>>& edges) {
    const std::size_t n = edges.size();
    std::vector<int> direct_parent(n + 1uz, 0);

    // If a node gets two parents, record both candidate edges (`cand1` earlier,
    // `cand2` later) and remember `cand2_idx` so Union-Find skips it.
    std::vector<int> cand1;
    std::vector<int> cand2;
    std::size_t cand2_idx = n;

    for (std::size_t i = 0uz; i < n; ++i) {
      const int u = edges[i][0];
      const int v = edges[i][1];
      const auto uv = static_cast<std::size_t>(v);
      if (direct_parent[uv] == 0) {
        direct_parent[uv] = u;
      } else {
        cand1 = {direct_parent[uv], v};
        cand2 = {u, v};
        cand2_idx = i;
      }
    }

    // Union-Find over remaining edges (ignoring `cand2` if present).
    std::vector<int> dsu(n + 1uz);
    for (std::size_t i = 0uz; i <= n; ++i) {
      dsu[i] = static_cast<int>(i);
    }

    auto find = [&](this auto&& self, int x) -> int {
      const auto idx = static_cast<std::size_t>(x);
      if (dsu[idx] != x) {
        dsu[idx] = self(dsu[idx]);
      }
      return dsu[idx];
    };

    for (std::size_t i = 0uz; i < n; ++i) {
      if (i == cand2_idx) {
        continue;
      }
      const int rx = find(edges[i][0]);
      const int ry = find(edges[i][1]);
      if (rx == ry) {
        // Skipping `cand2` still left a cycle:
        // - If a 2-parent node exists, `cand1` must be the culprit in the cycle.
        // - Otherwise (pure cycle case), this edge closed the cycle last.
        return cand1.empty() ? edges[i] : cand1;
      }
      dsu[static_cast<std::size_t>(ry)] = rx;
    }

    // No cycle remained after skipping `cand2`, so `cand2` is the answer.
    return cand2;
  }
};

LC_PROBLEM(685, "Redundant Connection II", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: two parents without cycle", [&] {
    ctx.expect_eq(
        sol.findRedundantDirectedConnection({{1, 2}, {1, 3}, {2, 3}}),
        std::vector<int>{2, 3});
  });

  ctx.test("example 2: pure cycle without two parents", [&] {
    ctx.expect_eq(sol.findRedundantDirectedConnection(
                      {{1, 2}, {2, 3}, {3, 4}, {4, 1}, {1, 5}}),
                  std::vector<int>{4, 1});
  });

  ctx.test("both two parents and cycle", [&] {
    ctx.expect_eq(
        sol.findRedundantDirectedConnection({{2, 1}, {3, 1}, {4, 2}, {1, 4}}),
        std::vector<int>{2, 1});
  });
}

}  // namespace
