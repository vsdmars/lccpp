// LeetCode 310: Minimum Height Trees
// https://leetcode.com/problems/minimum-height-trees/
//
// Problem:
//   Given an undirected tree of `n` nodes labeled `0` through `n - 1` and a
//   list of `n - 1` undirected edges, choosing any node as the root yields a
//   rooted tree with some height (the longest root-to-leaf distance). Return
//   all root labels that produce a Minimum Height Tree (MHT). The answer may
//   be returned in any order.
//
// Examples:
//   Example 1:
//     Input:  n = 4, edges = [[1, 0], [1, 2], [1, 3]]
//     Output: [1]
//     Explanation:
//       - Build initial degrees [1, 3, 1, 1], seeding leaves = [0, 2, 3] with
//         remaining = 4.
//       - Since remaining > 2, peel leaves [0, 2, 3] (remaining becomes 4 - 3 = 1)
//         and decrement neighbor 1's degree three times: 3 -> 2 -> 1 -> 0.
//       - Node 1 is pushed to next_leaves exactly when degree[1] hits 1, leaving
//         leaves = [1] and remaining = 1 <= 2 to return [1].
//
//   Example 2:
//     Input:  n = 6, edges = [[0, 3], [1, 3], [2, 3], [4, 3], [5, 4]]
//     Output: [3, 4]
//     Explanation:
//       - Build initial degrees [1, 1, 1, 4, 2, 1], seeding leaves = [0, 1, 2, 5]
//         with remaining = 6.
//       - Peel the 4 leaves (remaining becomes 6 - 4 = 2): decrementing degrees
//         drops degree[3] from 4 down to 1 and degree[4] from 2 down to 1.
//       - Both 3 and 4 enter next_leaves, so leaves = [3, 4] with remaining = 2
//         terminates the loop and returns [3, 4].
//
//   Example 3:
//     Input:  n = 1, edges = []
//     Output: [0]
//     Explanation:
//       - Detect n <= 2 at the top of findMinHeightTrees and skip building the
//         adjacency list.
//       - Populate and return all node indices from 0 to n - 1, yielding [0].
//
// Algorithm:
//   Any tree has at most two centers (midpoints of every longest path / tree
//   diameter), and those centers are precisely the roots of its MHTs. We find
//   them via topological leaf-peeling (multi-source BFS from the boundary):
//     1. Handle trivial graphs (`n <= 2`: every node is already a center).
//     2. Build an undirected adjacency list and track each vertex's degree.
//     3. Initialize the current frontier `leaves` with all degree-1 nodes.
//     4. While more than 2 nodes remain unpeeled, remove all current leaves
//        simultaneously and decrement the degrees of their neighbors. Any
//        neighbor whose degree drops to exactly 1 becomes a leaf for the next
//        layer.
//     5. Once `remaining <= 2`, the surviving frontier contains the 1 or 2
//        MHT root labels.
//   Time Complexity: O(N), since each node is pushed to a leaf frontier once
//   and each undirected edge is traversed at most twice.
//   Space Complexity: O(N) for the adjacency list, degree array, and frontier
//   vectors.
//
// Data Structures:
//   - Adjacency list (`std::vector<std::vector<int>>`): compact representation
//     of the undirected tree.
//   - Degree vector (`std::vector<int>`): tracks remaining incident edges per
//     node to identify new leaves in O(1) time.
//   - Frontier vectors (`leaves`, `next_leaves`): drive synchronous layer-by-
//     layer leaf trimming.
//
// Real-World Applications:
//   - Network topology design: selecting optimal hub/broker/controller
//     placement in a tree-structured LAN or overlay network to minimize
//     worst-case propagation latency (eccentricity).
//   - Distributed consensus and broadcast trees: picking a root coordinator to
//     minimize synchronization depth across cluster nodes.
//   - Facility location on road/rail trees (1-center problem) minimizing
//     maximum emergency response distance.
//
// Related LeetCode Problems:
//   - #207 Course Schedule [done] (Kahn's algorithm topological peeling using vertex degrees)
//   - #210 Course Schedule II [done] (BFS topological sort order via degree decrementing)
//   - #543 Diameter of Binary Tree [done] (longest path in a tree; MHT roots are diameter midpoints)
//   - #1245 Tree Diameter [done] (computing diameter of an undirected tree)
//   - #2603 Collect Coins in a Tree [new] (iterative leaf pruning on an undirected tree)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all roots that yield a minimum height tree by peeling leaves
  // layer-by-layer until at most two center nodes remain.
  // Time: O(N), Space: O(N).
  std::vector<int> findMinHeightTrees(
      int n, const std::vector<std::vector<int>>& edges) {
    if (n <= 0) {
      return {};
    }
    if (n <= 2) {
      std::vector<int> roots;
      roots.reserve(static_cast<std::size_t>(n));
      for (int i = 0; i < n; ++i) {
        roots.push_back(i);
      }
      return roots;
    }

    const auto node_count = static_cast<std::size_t>(n);
    std::vector<std::vector<int>> adj(node_count);
    std::vector<int> degree(node_count, 0);

    // Populate undirected adjacency list and initial vertex degrees.
    for (const auto& edge : edges) {
      const int u = edge[0];
      const int v = edge[1];
      adj[static_cast<std::size_t>(u)].push_back(v);
      adj[static_cast<std::size_t>(v)].push_back(u);
      ++degree[static_cast<std::size_t>(u)];
      ++degree[static_cast<std::size_t>(v)];
    }

    // Seed the first layer with all degree-1 leaf nodes.
    std::vector<int> leaves;
    for (int i = 0; i < n; ++i) {
      if (degree[static_cast<std::size_t>(i)] == 1) {
        leaves.push_back(i);
      }
    }

    // Trim leaves symmetrically from the outside in. Because a tree with >= 3
    // nodes has no adjacent leaves, every surviving center drops to degree 1
    // at most once (even when multiple leaves peel off a star's center in the
    // final layer, its degree passes through 1 exactly once).
    int remaining = n;
    while (remaining > 2) {
      remaining -= static_cast<int>(leaves.size());
      std::vector<int> next_leaves;

      for (const int leaf : leaves) {
        for (const int neighbor : adj[static_cast<std::size_t>(leaf)]) {
          const auto nb = static_cast<std::size_t>(neighbor);
          --degree[nb];
          if (degree[nb] == 1) {
            next_leaves.push_back(neighbor);
          }
        }
      }

      leaves = std::move(next_leaves);
    }

    return leaves;
  }
};

// Sorts a copy of `v` so order-independent MHT root lists can be compared.
std::vector<int> sorted_copy(std::vector<int> v) {
  std::ranges::sort(v);
  return v;
}

LC_PROBLEM(310, "Minimum Height Trees", "tree", ctx) {
  ctx.test("example_1_star_4_nodes", [&] {
    Solution sol;
    const std::vector<std::vector<int>> edges = {{1, 0}, {1, 2}, {1, 3}};
    ctx.expect_eq(sorted_copy(sol.findMinHeightTrees(4, edges)),
                  std::vector<int>{1});
  });

  ctx.test("example_2_two_centers", [&] {
    Solution sol;
    const std::vector<std::vector<int>> edges = {
        {0, 3}, {1, 3}, {2, 3}, {4, 3}, {5, 4}};
    ctx.expect_eq(sorted_copy(sol.findMinHeightTrees(6, edges)),
                  std::vector<int>{3, 4});
  });

  ctx.test("single_and_two_nodes", [&] {
    Solution sol;
    ctx.expect_eq(sorted_copy(sol.findMinHeightTrees(1, {})),
                  std::vector<int>{0});
    ctx.expect_eq(sorted_copy(sol.findMinHeightTrees(2, {{0, 1}})),
                  std::vector<int>{0, 1});
  });

  ctx.test("odd_and_even_path_graphs", [&] {
    Solution sol;
    // Path 0-1-2-3-4 has unique center 2.
    const std::vector<std::vector<int>> odd_path = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}};
    ctx.expect_eq(sorted_copy(sol.findMinHeightTrees(5, odd_path)),
                  std::vector<int>{2});

    // Path 0-1-2-3-4-5 has two centers: 2 and 3.
    const std::vector<std::vector<int>> even_path = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}};
    ctx.expect_eq(sorted_copy(sol.findMinHeightTrees(6, even_path)),
                  std::vector<int>{2, 3});
  });

  ctx.test("larger_star_5_nodes", [&] {
    Solution sol;
    const std::vector<std::vector<int>> edges = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}};
    ctx.expect_eq(sorted_copy(sol.findMinHeightTrees(5, edges)),
                  std::vector<int>{0});
  });
}

}  // namespace
