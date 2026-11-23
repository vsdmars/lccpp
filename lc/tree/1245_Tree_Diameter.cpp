// LeetCode 1245: Tree Diameter
// https://leetcode.com/problems/tree-diameter/
//
// Problem:
//   Given an undirected tree represented by an edge list `edges` where
//   `edges[i] = [u, v]` connects nodes u and v, and node labels lie in
//   {0, 1, ..., edges.size()}, return the diameter of the tree: the number of
//   edges on a longest simple path between any two nodes.
//   Constraints: 0 <= edges.size() < 10^4; the edges form a valid tree.
//
// Examples:
//   Example 1:
//     Input:  edges = [[0, 1], [1, 2], [2, 3], [1, 4], [4, 5]]
//     Output: 4
//     Explanation:
//       - Build the undirected adjacency list for 6 nodes (0:[1], 1:[0,2,4],
//         2:[1,3], 3:[2], 4:[1,5], 5:[4]) and call dfs(0, -1).
//       - Leaf 3 returns 0 so node 2 sees branch_len = 1 (diameter = max(0, 1 + 0)
//         = 1) and returns 1; similarly leaf 5 returns 0 so node 4 sees branch_len
//         = 1 and returns 1.
//       - At node 1, child 2 yields branch_len = 1 + 1 = 2 and child 4 yields
//         branch_len = 1 + 1 = 2, setting top1 = 2, top2 = 2, updating diameter =
//         max(1, 2 + 2) = 4, and returning top1 = 2.
//       - Root 0 receives branch_len = 2 + 1 = 3 from node 1 (top1 = 3, top2 = 0),
//         keeping diameter = 4 and returning 4.
//
//   Example 2:
//     Input:  edges = [[0, 1], [1, 2], [1, 3], [3, 4], [4, 5], [2, 6], [6, 7], [7,
//             8]]
//     Output: 7
//     Explanation:
//       - Build adj for 9 nodes and root the post-order DFS at node 0 via dfs(0,
//         -1).
//       - Down branch 1 -> 3 -> 4 -> 5, depths return 0 from 5, 1 from 4, and 2
//         from 3, giving node 1 a candidate branch_len = 2 + 1 = 3.
//       - Down branch 1 -> 2 -> 6 -> 7 -> 8, depths return 0 from 8, 1 from 7, 2
//         from 6, and 3 from 2, giving node 1 a candidate branch_len = 3 + 1 = 4.
//       - At node 1, top1 = 4 and top2 = 3 combine into diameter = max(diameter, 4
//         + 3) = 7 (path 5-4-3-1-2-6-7-8), returning top1 = 4 to root 0 whose
//         single child branch 4 + 1 = 5 leaves diameter = 7.
//
//   Example 3:
//     Input:  edges = [[0, 1]]
//     Output: 1
//     Explanation:
//       - Build adj for 2 nodes (0:[1], 1:[0]) and start dfs(0, -1).
//       - In dfs(1, 0), neighbor 0 equals parent and is skipped, so top1 = 0, top2
//         = 0, diameter = 0, and dfs(1, 0) returns 0.
//       - Back at dfs(0, -1), child 1 yields branch_len = 0 + 1 = 1 (top1 = 1, top2
//         = 0), updating diameter = max(0, 1 + 0) = 1 and returning 1.
//
// Algorithm:
//   Build an undirected adjacency list for the N = edges.size() + 1 nodes and
//   root the tree arbitrarily at node 0. Run a single post-order DFS that
//   passes the parent index to avoid revisiting edges (no visited hash set is
//   needed because a tree is acyclic). For each node, compute the longest
//   downward edge-path into every child subtree and retain only the top two
//   branch lengths `top1 >= top2`. The longest path whose highest point is the
//   current node has length `top1 + top2`; update the global `diameter` with
//   that candidate and return `top1` upward to the parent.
//   Time Complexity:  O(V), where V = edges.size() + 1 (each node and edge is
//                     traversed once, tracking top-2 in O(1) per child).
//   Space Complexity: O(V) for the adjacency list and DFS recursion stack.
//
// Data Structures:
//   - std::vector<std::vector<int>>: compact adjacency list indexed directly
//     by node ID [0, V).
//   - Two scalar accumulators (top1, top2) per recursion frame replacing a
//     heap for O(1) top-2 selection.
//
// Real-World Applications:
//   - Computing worst-case broadcast latency (network diameter) in spanning
//     trees, multicast overlays, and sensor networks.
//   - Finding critical-path lengths in hierarchical clock-distribution trees
//     and VLSI routing topologies.
//   - Phylogenetic tree analysis (measuring maximum evolutionary distance).
//
// Related LeetCode Problems:
//   - #543 Diameter of Binary Tree [done] (binary-tree special case)
//   - #1522 Diameter Of N Ary Tree [done] (rooted N-ary tree version)
//   - #310 Minimum Height Trees [done] (tree centers lie at the midpoint of the diameter)
//   - #2246 Longest Path With Different Adjacent Characters [done] (constrained tree diameter)
//   - #3203 Find Minimum Diameter After Merging Two Trees [new] (diameter of two joined trees)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the diameter (maximum edge count between any two vertices) of an
  // undirected tree given by `edges`.
  // Post-order tree DP rooted at node 0 tracks the two deepest child branches
  // at every node. Time: O(V), Space: O(V).
  int treeDiameter(const std::vector<std::vector<int>>& edges) {
    if (edges.empty()) {
      return 0;
    }

    // A tree with E edges always has V = E + 1 vertices labeled 0..E.
    const std::size_t num_nodes = edges.size() + 1uz;
    std::vector<std::vector<int>> adj(num_nodes);
    for (const auto& edge : edges) {
      const auto u = static_cast<std::size_t>(edge[0]);
      const auto v = static_cast<std::size_t>(edge[1]);
      adj[u].push_back(edge[1]);
      adj[v].push_back(edge[0]);
    }

    int diameter = 0;

    // Returns the longest downward path length (in edges) starting at `node`
    // without going back toward `parent`.
    auto dfs = [&](this auto&& self, int node, int parent) -> int {
      int top1 = 0;
      int top2 = 0;

      for (const int next : adj[static_cast<std::size_t>(node)]) {
        if (next == parent) {
          continue;
        }
        // Extend the child's deepest downward path by the edge (node, next).
        const int branch_len = self(next, node) + 1;
        if (branch_len > top1) {
          top2 = top1;
          top1 = branch_len;
        } else if (branch_len > top2) {
          top2 = branch_len;
        }
      }

      // Longest path turning at `node` combines its two deepest branches.
      // For a node with one child, top2 == 0 so this also covers single paths.
      diameter = std::max(diameter, top1 + top2);
      return top1;
    };

    dfs(0, -1);
    return diameter;
  }
};

LC_PROBLEM(1245, "Tree Diameter", "tree", ctx) {
  ctx.test("example_1_small_tree", [&] {
    Solution sol;
    ctx.expect_eq(sol.treeDiameter({{0, 1}, {0, 2}}), 2);
  });

  ctx.test("example_2_branched_tree", [&] {
    Solution sol;
    ctx.expect_eq(sol.treeDiameter({{0, 1}, {1, 2}, {2, 3}, {1, 4}, {4, 5}}),
                  4);
  });

  ctx.test("empty_and_single_edge", [&] {
    Solution sol;
    ctx.expect_eq(sol.treeDiameter({}), 0);
    ctx.expect_eq(sol.treeDiameter({{0, 1}}), 1);
  });

  ctx.test("star_topology", [&] {
    Solution sol;
    ctx.expect_eq(sol.treeDiameter({{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}}),
                  2);
  });

  ctx.test("linear_chain", [&] {
    Solution sol;
    ctx.expect_eq(sol.treeDiameter({{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}}),
                  5);
  });

  ctx.test("diameter_not_passing_through_node_0", [&] {
    // Node 0 connects only to 1; the longest path 5-4-3-1-2-6-7-8 has 7 edges
    // and never visits node 0.
    Solution sol;
    const std::vector<std::vector<int>> edges = {
        {0, 1}, {1, 2}, {1, 3}, {3, 4}, {4, 5}, {2, 6}, {6, 7}, {7, 8}};
    ctx.expect_eq(sol.treeDiameter(edges), 7);
  });
}

}  // namespace
