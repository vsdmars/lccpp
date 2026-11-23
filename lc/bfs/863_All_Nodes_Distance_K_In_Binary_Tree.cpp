// LeetCode 863: All Nodes Distance K In Binary Tree
// https://leetcode.com/problems/all-nodes-distance-k-in-binary-tree/
//
// Problem:
//   Given the `root` of a binary tree with unique node values, a `target` node
//   in the tree, and a non-negative integer `k`, return a list of the values of
//   all nodes that are at an exact edge distance `k` from `target`. The answer
//   may be returned in any order.
//   Constraints: 1 <= number of nodes <= 500, 0 <= node.val <= 500 (unique),
//   0 <= k <= 1000.
//
// Algorithm:
//   Convert the directed parent->child tree into an undirected graph, then run
//   level-by-level BFS from `target`:
//   1. Graph Construction (DFS): Traverse the tree recursively and add
//      undirected edges between every parent and its non-null left/right
//      children in an adjacency list keyed by unique node value.
//   2. Distance-K Wave BFS: Start a frontier containing only `target->val` and
//      mark it visited. Repeat `k` times (stopping early if the frontier empties
//      before reaching distance `k`): expand every node in the current frontier
//      to all unvisited neighbors to form the next distance layer.
//   3. After `k` expansions, the current frontier contains every node at exact
//      shortest-path distance `k` from `target`.
//   Time Complexity:  O(N) — each node and tree edge is visited once during
//                     graph construction and at most once during BFS.
//   Space Complexity: O(N) — adjacency list, visited set, recursion stack, and
//                     BFS frontier.
//
// Data Structures:
//   - Undirected adjacency list (`std::unordered_map<int, std::vector<int>>`):
//     enables upward (child -> parent) as well as downward traversal.
//   - Visited hash set (`std::unordered_set<int>`): prevents bouncing back
//     across undirected edges during BFS.
//   - Frontier vectors (`std::vector<int>`): store the exact set of nodes at
//     the current BFS distance layer.
//
// Real-World Applications:
//   - Network topology & fault blast-radius analysis: identifying all hosts or
//     switches exactly `k` hops away from a failing node in a spanning tree.
//   - File system / organizational hierarchy queries: finding all entities at a
//     given degree of separation from a specific employee or directory.
//   - Phylogenetic & lineage trees: locating all species or commits within a
//     fixed evolutionary or branching distance of a reference node.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (layer-by-layer BFS traversal of a binary tree)
//   - #2385 Amount of Time for Binary Tree to Be Infected [new] (undirected tree conversion + BFS from a start node)
//   - #742 Closest Leaf in a Binary Tree [done] (building parent links and running BFS from a target node)
//   - #1161 Maximum Level Sum of a Binary Tree [done] (level-indexed BFS aggregation on a binary tree)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int x) : val(x) {}
};

// RAII arena for test tree nodes; std::deque guarantees pointer stability on
// emplace_back and frees all nodes automatically when the arena leaves scope.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val);
    auto* node = &nodes_.back();
    node->left = left;
    node->right = right;
    return node;
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns all node values in the binary tree `root` that are at exact edge
  // distance `k` from `target`.
  // Time: O(N), Space: O(N).
  std::vector<int> distanceK(const TreeNode* root, const TreeNode* target,
                             int k) {
    if (root == nullptr || target == nullptr || k < 0) {
      return {};
    }

    // Phase 1: Build an undirected adjacency list so BFS can move both down to
    // children and up to ancestors. Node values are guaranteed unique.
    std::unordered_map<int, std::vector<int>> adj;
    buildGraph(root, adj);

    // Phase 2: Expand outward from `target->val` one hop per iteration.
    std::vector<int> frontier = {target->val};
    std::unordered_set<int> visited = {target->val};

    for (int step = 0; step < k && !frontier.empty(); ++step) {
      std::vector<int> next_frontier;
      for (const int u : frontier) {
        const auto it = adj.find(u);
        if (it == adj.end()) {
          continue;
        }
        for (const int v : it->second) {
          if (visited.insert(v).second) {
            next_frontier.push_back(v);
          }
        }
      }
      frontier = std::move(next_frontier);
    }

    return frontier;
  }

 private:
  // Recursively populates `adj` with bidirectional edges for every parent-child
  // pair in the subtree rooted at `node`.
  static void buildGraph(const TreeNode* node,
                         std::unordered_map<int, std::vector<int>>& adj) {
    if (node == nullptr) {
      return;
    }

    if (node->left != nullptr) {
      adj[node->val].push_back(node->left->val);
      adj[node->left->val].push_back(node->val);
      buildGraph(node->left, adj);
    }

    if (node->right != nullptr) {
      adj[node->val].push_back(node->right->val);
      adj[node->right->val].push_back(node->val);
      buildGraph(node->right, adj);
    }
  }
};

// Sorts a result vector so order-independent outputs can be compared cleanly.
std::vector<int> sortedCopy(std::vector<int> vals) {
  std::ranges::sort(vals);
  return vals;
}

LC_PROBLEM(863, "All Nodes Distance K In Binary Tree", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_distance_2_from_5", [&] {
    //       3
    //      / \
    //     5   1
    //    / \ / \
    //   6  2 0  8
    //     / \
    //    7   4
    TreeArena arena;
    auto* n7 = arena.make(7);
    auto* n4 = arena.make(4);
    auto* n2 = arena.make(2, n7, n4);
    auto* n6 = arena.make(6);
    auto* n5 = arena.make(5, n6, n2);
    auto* n0 = arena.make(0);
    auto* n8 = arena.make(8);
    auto* n1 = arena.make(1, n0, n8);
    auto* n3 = arena.make(3, n5, n1);

    ctx.expect_eq(sortedCopy(sol.distanceK(n3, n5, 2)),
                  std::vector<int>{1, 4, 7});
  });

  ctx.test("distance_zero_returns_target", [&] {
    TreeArena arena;
    auto* n2 = arena.make(2);
    auto* n1 = arena.make(1, n2, nullptr);
    ctx.expect_eq(sol.distanceK(n1, n2, 0), std::vector<int>{2});
  });

  ctx.test("distance_exceeds_tree_diameter", [&] {
    TreeArena arena;
    auto* n1 = arena.make(1);
    ctx.expect_eq(sol.distanceK(n1, n1, 3), std::vector<int>{});
  });

  ctx.test("target_at_deep_leaf_reaches_other_subtree", [&] {
    TreeArena arena;
    auto* n7 = arena.make(7);
    auto* n4 = arena.make(4);
    auto* n2 = arena.make(2, n7, n4);
    auto* n6 = arena.make(6);
    auto* n5 = arena.make(5, n6, n2);
    auto* n0 = arena.make(0);
    auto* n8 = arena.make(8);
    auto* n1 = arena.make(1, n0, n8);
    auto* n3 = arena.make(3, n5, n1);

    // Distance 4 from node 7: 7 -> 2 -> 5 -> 3 -> 1 (1 is at distance 4).
    ctx.expect_eq(sortedCopy(sol.distanceK(n3, n7, 4)), std::vector<int>{1});
    // Distance 5 from node 7 reaches 0 and 8 via 1.
    ctx.expect_eq(sortedCopy(sol.distanceK(n3, n7, 5)), std::vector<int>{0, 8});
  });
}

}  // namespace
