// LeetCode 1740: Find Distance In A Binary Tree
// https://leetcode.com/problems/find-distance-in-a-binary-tree/
//
// Problem:
//   Given the root of a binary tree with unique node values and two integers
//   p and q guaranteed to exist in the tree, return the distance between the
//   nodes with values p and q (the number of edges on the unique simple path
//   connecting them).
//
// Examples:
//   Example 1:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 0
//     Output: 3
//     Explanation:
//       - build_graph runs DFS over the tree to populate bidirectional parent-child
//         edges in adj, including 5 <-> 3, 3 <-> 1, and 1 <-> 0.
//       - BFS starts with frontier = [5], visited = {5}, and distance = 0; since 5
//         != 0, it enqueues 5's neighbors 3, 6, and 2 and increments distance to 1.
//       - At distance = 1, popping 3 enqueues unvisited neighbor 1 while 6 and 2
//         enqueue 7 and 4, incrementing distance to 2.
//       - At distance = 2, popping 1 enqueues unvisited neighbors 0 and 8,
//         incrementing distance to 3.
//       - At distance = 3, popping 0 matches curr == q and returns 3 along the path
//         5 -> 3 -> 1 -> 0.
//
//   Example 2:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 7
//     Output: 2
//     Explanation:
//       - build_graph records undirected edges in adj, including 5 <-> 2 and 2 <->
//         7.
//       - BFS pops 5 at distance = 0, enqueues neighbors 3, 6, and 2, and advances
//         distance to 1.
//       - At distance = 1, popping 2 enqueues unvisited children 7 and 4 (alongside
//         3 enqueuing 1), advancing distance to 2.
//       - At distance = 2, popping 7 satisfies curr == q and returns 2 along the
//         path 5 -> 2 -> 7.
//
//   Example 3:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 5
//     Output: 0
//     Explanation:
//       - findDistance verifies root != nullptr and evaluates if (p == q).
//       - Because p and q are both 5, it immediately returns 0 without constructing
//         adj or launching BFS.
//
// Algorithm:
//   Tree-to-Graph Conversion + Level-Order BFS:
//   - Because every node value in the tree is unique, perform a DFS traversal
//     over the binary tree to build an undirected adjacency list mapping each
//     node value to its parent and children.
//   - If p == q, immediately return 0.
//   - Run an unweighted BFS starting at value p, expanding level by level and
//     tracking visited values to avoid cycling back along undirected edges.
//     The BFS level at which value q is reached equals the exact edge distance.
//   - Time Complexity: O(n), where n is the number of nodes in the tree (one
//     DFS pass to construct the graph and one BFS pass visiting each node at
//     most once).
//   - Space Complexity: O(n) for the adjacency list, visited set, and BFS
//     frontier queue.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`) managed in tests via an RAII arena
//     (`std::deque<TreeNode>`) to guarantee zero memory leaks.
//   - Undirected adjacency list (`std::unordered_map<int, std::vector<int>>`)
//     encoding parent-child links bidirectionally.
//   - Visited hash set (`std::unordered_set<int>`) and FIFO queue
//     (`std::queue<int>`) for shortest-path BFS.
//
// Real-World Applications:
//   - Computing hop distance between entities in hierarchical networks (e.g.,
//     organizational charts, DNS/filesystem directory trees, XML/DOM trees).
//   - Phylogenetic tree distance queries between species in bioinformatics.
//   - Routing packet hops in spanning-tree network topologies.
//
// Related LeetCode Problems:
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (LCA enables O(1)-extra-graph-free distance computation)
//   - #863 All Nodes Distance K in Binary Tree [done] (converting a binary tree to an undirected graph for BFS)
//   - #543 Diameter of Binary Tree [done] (maximum pairwise edge distance in a binary tree)
//   - #2096 Step-By-Step Directions From a Binary Tree Node to Another [new] (explicit path reconstruction between two tree nodes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Standard LeetCode binary tree node definition.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
};

// RAII test helper that owns all allocated TreeNodes in a contiguous-address
// deque (push_back never invalidates pointers to existing elements).
class TreeArena {
 public:
  // Builds a binary tree from LeetCode level-order representation.
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create_node(*level_order[0]);
    std::queue<TreeNode*> queue;
    queue.push(root);

    std::size_t idx = 1uz;
    while (!queue.empty() && idx < level_order.size()) {
      TreeNode* curr = queue.front();
      queue.pop();

      if (level_order[idx].has_value()) {
        curr->left = create_node(*level_order[idx]);
        queue.push(curr->left);
      }
      ++idx;

      if (idx < level_order.size() && level_order[idx].has_value()) {
        curr->right = create_node(*level_order[idx]);
        queue.push(curr->right);
      }
      ++idx;
    }

    return root;
  }

 private:
  TreeNode* create_node(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns the number of edges between nodes with values `p` and `q`,
  // or -1 if either node is not present in the tree.
  // Time: O(n). Space: O(n).
  int findDistance(const TreeNode* root, int p, int q) {
    if (root == nullptr) {
      return -1;
    }
    if (p == q) {
      return 0;
    }

    // Build an undirected adjacency list so BFS can traverse both up to
    // parents and down to children.
    std::unordered_map<int, std::vector<int>> adj;
    build_graph(root, adj);

    // Run level-by-level BFS from `p` until `q` is encountered.
    std::queue<int> frontier;
    std::unordered_set<int> visited;
    frontier.push(p);
    visited.insert(p);

    int distance = 0;
    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      for (std::size_t i = 0uz; i < level_size; ++i) {
        const int curr = frontier.front();
        frontier.pop();

        if (curr == q) {
          return distance;
        }

        const auto it = adj.find(curr);
        if (it == adj.end()) {
          continue;
        }
        for (const int neighbor : it->second) {
          if (visited.insert(neighbor).second) {
            frontier.push(neighbor);
          }
        }
      }
      ++distance;
    }

    return -1;
  }

 private:
  // Recursively populates bidirectional parent-child edges into `adj`.
  static void build_graph(const TreeNode* node,
                          std::unordered_map<int, std::vector<int>>& adj) {
    if (node == nullptr) {
      return;
    }
    if (node->left != nullptr) {
      adj[node->val].push_back(node->left->val);
      adj[node->left->val].push_back(node->val);
      build_graph(node->left, adj);
    }
    if (node->right != nullptr) {
      adj[node->val].push_back(node->right->val);
      adj[node->right->val].push_back(node->val);
      build_graph(node->right, adj);
    }
  }
};

LC_PROBLEM(1740, "Find Distance In A Binary Tree", "bfs", ctx) {
  const std::vector<std::optional<int>> kSampleTree = {
      3, 5, 1, 6, 2, 0, 8, std::nullopt, std::nullopt, 7, 4,
  };

  ctx.test("example_1_across_root", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build(kSampleTree);
    Solution sol;
    // Path: 5 -> 3 -> 1 -> 0 (3 edges).
    ctx.expect_eq(sol.findDistance(root, 5, 0), 3);
  });

  ctx.test("example_2_ancestor_to_descendant", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build(kSampleTree);
    Solution sol;
    // Path: 5 -> 2 -> 7 (2 edges).
    ctx.expect_eq(sol.findDistance(root, 5, 7), 2);
  });

  ctx.test("example_3_same_node_zero_distance", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build(kSampleTree);
    Solution sol;
    ctx.expect_eq(sol.findDistance(root, 5, 5), 0);
  });

  ctx.test("leaves_in_same_subtree", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build(kSampleTree);
    Solution sol;
    // Path: 6 -> 5 -> 2 -> 4 (3 edges).
    ctx.expect_eq(sol.findDistance(root, 6, 4), 3);
  });

  ctx.test("single_node_tree", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({42});
    Solution sol;
    ctx.expect_eq(sol.findDistance(root, 42, 42), 0);
  });

  ctx.test("skewed_chain_tree", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.build({1, 2, std::nullopt, 3, std::nullopt, 4});
    Solution sol;
    ctx.expect_eq(sol.findDistance(root, 1, 4), 3);
    ctx.expect_eq(sol.findDistance(root, 4, 2), 2);
  });
}

}  // namespace
