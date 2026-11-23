// LeetCode 685: Redundant Connection II
// https://leetcode.com/problems/redundant-connection-ii/
//
// Problem:
//   A rooted directed tree with `N` nodes labeled `1..N` has had one extra
//   directed edge added between two existing nodes, giving `N` edges in total.
//   Return an edge `[u, v]` that can be removed so the remaining `N - 1` edges
//   form a valid rooted tree (one root with in-degree 0, every other node with
//   in-degree 1, and no cycles). If multiple edges qualify, return the one
//   that appears last in `edges`.
//
// Examples:
//   Example 1:
//     Input:  edges = [[1, 2], [1, 3], [2, 3]]
//     Output: [2, 3]
//     Explanation:
//       - Pass 1 records direct_parent[2] = 1 and direct_parent[3] = 1, then sees
//         node 3 already has parent 1 on edge [2, 3], saving cand1 = [1, 3], cand2
//         = [2, 3], and cand2_idx = 2.
//       - Pass 2 initializes dsu = [0, 1, 2, 3] and unions [1, 2] (dsu[2] = 1) and
//         [1, 3] (dsu[3] = 1), skipping cand2 at index 2.
//       - No cycle is found among the remaining edges, so the algorithm returns
//         cand2 = [2, 3].
//
//   Example 2:
//     Input:  edges = [[1, 2], [2, 3], [3, 4], [4, 1], [1, 5]]
//     Output: [4, 1]
//     Explanation:
//       - Pass 1 sets direct_parent for nodes 2, 3, 4, 1, and 5 once each, so no
//         node has two parents and cand1 remains empty (cand2_idx = 5).
//       - Pass 2 unions [1, 2], [2, 3], and [3, 4], merging nodes 1, 2, 3, and 4
//         under root 1 (dsu[2] = dsu[3] = dsu[4] = 1).
//       - On edge [4, 1] at index 3, find(4) == 1 and find(1) == 1 detect a cycle,
//         and because cand1 is empty, return edges[3] = [4, 1].
//
//   Example 3:
//     Input:  edges = [[2, 1], [3, 1], [4, 2], [1, 4]]
//     Output: [2, 1]
//     Explanation:
//       - Pass 1 sets direct_parent[1] = 2 on [2, 1] and detects a second parent on
//         [3, 1], saving cand1 = [2, 1], cand2 = [3, 1], and cand2_idx = 1, then
//         sets direct_parent[2] = 4 and direct_parent[4] = 1.
//       - Pass 2 skips index 1 (cand2) and unions [2, 1] (dsu[1] = 2) and [4, 2]
//         (dsu[2] = 4).
//       - On edge [1, 4], find(1) and find(4) both evaluate to 4, so a cycle still
//         exists without cand2 and the algorithm returns cand1 = [2, 1].
//
// Algorithm:
//   Two-pass in-degree classification paired with Union-Find (DSU). Adding one
//   directed edge to a rooted tree produces exactly one of three failure modes:
//     1. A node has two parents, with no cycle -> remove the later incoming
//        edge (`cand2`).
//     2. Every node has at most one parent, with a cycle -> remove the last
//        edge that closes the cycle.
//     3. A node has two parents AND there is a cycle -> remove whichever of
//        the two incoming edges (`cand1` or `cand2`) lies on the cycle.
//   Pass 1 scans `edges` to detect any node with in-degree 2, saving its first
//   incoming edge (`cand1`), second incoming edge (`cand2`), and index
//   (`cand2_idx`). Pass 2 runs Union-Find with path compression over all edges
//   except `cand2_idx`. If a cycle still occurs, `cand1` is returned when a
//   two-parent node exists, or the cycle-closing edge otherwise; if no cycle
//   remains, `cand2` is returned.
//   Time: O(N * alpha(N)) amortized in practice (O(N log N) worst case without
//   union by rank), Space: O(N).
//
// Data Structures:
//   - `std::vector<int> direct_parent`: Tracks the first seen parent of each
//     node `1..N` to identify any node with in-degree 2.
//   - `std::vector<int> cand1`, `cand2` and `cand2_idx`: Store the two
//     conflicting incoming edges and allow skipping `cand2` in Pass 2.
//   - `std::vector<int> dsu`: Disjoint-set parent array with recursive path
//     compression for cycle detection.
//
// Real-World Applications:
//   - Organizational and RBAC hierarchy validation: repairing broken reporting
//     trees where a node accidentally gains two managers or forms a cycle.
//   - File system and container mount verification: detecting and breaking
//     invalid hard-link/mount loops or duplicate parent links in a directory
//     hierarchy.
//   - Package/configuration inheritance debugging: locating the exact override
//     edge that broke a single-inheritance tree.
//
// Related LeetCode Problems:
//   - #684 Redundant Connection [done] (undirected counterpart finding the last edge
//     forming a cycle)
//   - #261 Graph Valid Tree [done] (verifying that an undirected graph is acyclic and
//     connected)
//   - #1361 Validate Binary Tree Nodes [done] (checking single-parent, single-root,
//     and acyclicity invariants in a directed graph)
//   - #1579 Remove Max Number of Edges to Keep Graph Fully Traversable [new]
//     (Union-Find edge selection preserving connectivity)

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
