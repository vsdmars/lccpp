// LeetCode 742: Closest Leaf In A Binary Tree
// https://leetcode.com/problems/closest-leaf-in-a-binary-tree/
//
// Problem:
//   Given the root of a binary tree where every node has a unique integer
//   value, and a target integer k, return the value of the leaf node nearest
//   to the node with value k. Distance is measured by the number of tree edges
//   traversed (paths may go upward through ancestors as well as downward). A
//   leaf is a node with no children. If multiple leaves tie for minimum
//   distance, any of them may be returned.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 3, 2], k = 1
//     Output: 3
//     Explanation:
//       - build_graph links root 1 bidirectionally to left child 3 then right child
//         2 (adj[1] = [3, 2]) and records target = node 1.
//       - BFS starts at node 1 (not a leaf), pops it, and enqueues neighbors 3 and
//         2 at edge distance 1.
//       - BFS next pops leaf 3 (left == nullptr && right == nullptr) and returns 3
//         (2 is equally valid at distance 1).
//
//   Example 2:
//     Input:  root = [1, 2, 3, 4, null, null, null, 5, null, 6], k = 2
//     Output: 3
//     Explanation:
//       - build_graph records undirected edges (1-2, 1-3, 2-4, 4-5, 5-6) and
//         locates target = node 2.
//       - BFS pops node 2 at distance 0 (not a leaf) and enqueues its parent 1 and
//         child 4.
//       - At distance 1, BFS pops 1 (enqueues leaf 3) and 4 (enqueues 5), neither
//         of which is a leaf.
//       - At distance 2, BFS pops leaf 3 and returns 3 before ever reaching leaf 6
//         at distance 3.
//
//   Example 3:
//     Input:  root = [1], k = 99
//     Output: -1
//     Explanation:
//       - build_graph visits the single node 1, leaving target == nullptr because
//         no node matches k = 99.
//       - Seeing target == nullptr, findClosestLeaf immediately returns -1 without
//         running BFS.
//
// Algorithm:
//   Because the shortest path to a leaf may climb upward through parent links
//   before descending into another branch, we treat the tree as an undirected
//   graph:
//     1. Run a DFS from `root` to populate an undirected adjacency list mapping
//        each `const TreeNode*` to its parent and children, while locating the
//        target node pointer where `node->val == k`.
//     2. Run a Breadth-First Search (BFS) starting from the target node,
//        tracking visited nodes to prevent backtracking.
//     3. Since BFS explores nodes in strictly non-decreasing order of edge
//        distance, the very first popped node with `left == nullptr && right ==
//        nullptr` is guaranteed to be a closest leaf.
//   Time Complexity:  O(n) to build the adjacency list and run BFS.
//   Space Complexity: O(n) for the adjacency map, visited set, and BFS queue.
//
// Data Structures:
//   - Undirected adjacency list (`std::unordered_map<const TreeNode*,
//     std::vector<const TreeNode*>>`) enabling upward parent traversal.
//   - FIFO queue (`std::queue<const TreeNode*>`) for shortest-path BFS.
//   - Hash set (`std::unordered_set<const TreeNode*>`) tracking visited nodes.
//
// Real-World Applications:
//   - Distributed cache / CDN routing: locating the nearest edge/leaf server
//     from an arbitrary interior router in a tree-structured network.
//   - File-system and DOM navigation: finding the nearest terminal node
//     relative to a selected interior node via bidirectional parent/child hops.
//   - Emergency egress routing in hierarchical facility graphs.
//
// Related LeetCode Problems:
//   - #863 All Nodes Distance K in Binary Tree [done] (same tree-to-undirected-graph + BFS pattern)
//   - #2385 Amount of Time for Binary Tree to Be Infected [new] (undirected BFS from an interior target node)
//   - #111 Minimum Depth of Binary Tree [done] (BFS to nearest leaf starting from the root)
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (ancestor relationships in binary trees)

#include "framework/lc_framework.h"

#include <deque>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// RAII arena owning test TreeNode allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(val, left, right);
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Finds the value of a leaf node at minimum edge distance from node `k`.
  // Returns -1 if `root` is null or `k` does not exist in the tree.
  // Time: O(n), Space: O(n).
  int findClosestLeaf(TreeNode* root, int k) {
    if (root == nullptr) {
      return -1;
    }

    // Build bidirectional parent-child edges so BFS can walk both up and down.
    std::unordered_map<const TreeNode*, std::vector<const TreeNode*>> adj;
    const TreeNode* target = nullptr;

    auto build_graph = [&adj, &target, k](this auto&& self,
                                          const TreeNode* node,
                                          const TreeNode* parent) -> void {
      if (node == nullptr) {
        return;
      }
      if (node->val == k) {
        target = node;
      }
      if (parent != nullptr) {
        adj[node].push_back(parent);
        adj[parent].push_back(node);
      }
      self(node->left, node);
      self(node->right, node);
    };

    build_graph(root, nullptr);

    if (target == nullptr) {
      return -1;
    }

    // BFS expands in layers of equal edge distance; the first leaf reached is
    // guaranteed to be closest to `target`.
    std::queue<const TreeNode*> bfs;
    std::unordered_set<const TreeNode*> visited;
    bfs.push(target);
    visited.insert(target);

    while (!bfs.empty()) {
      const TreeNode* curr = bfs.front();
      bfs.pop();

      if (curr->left == nullptr && curr->right == nullptr) {
        return curr->val;
      }

      const auto it = adj.find(curr);
      if (it == adj.end()) {
        continue;
      }
      for (const TreeNode* neighbor : it->second) {
        if (visited.insert(neighbor).second) {
          bfs.push(neighbor);
        }
      }
    }

    return -1;
  }
};

LC_PROBLEM(742, "Closest Leaf In A Binary Tree", "tree", ctx) {
  ctx.test("example1_root_with_two_leaves", [&] {
    //     1
    //    / \
    //   3   2
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(3), arena.make(2));
    Solution sol;
    const int ans = sol.findClosestLeaf(root, 1);
    ctx.expect_true(ans == 2 || ans == 3);
  });

  ctx.test("example2_single_node_is_leaf", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);
    Solution sol;
    ctx.expect_eq(sol.findClosestLeaf(root, 1), 1);
  });

  ctx.test("example3_closest_leaf_via_parent", [&] {
    //         1
    //        / \
    //       2   3
    //      /
    //     4
    //    /
    //   5
    //  /
    // 6
    // From k = 2: leaf 3 is 2 edges away (2->1->3), leaf 6 is 3 edges away.
    TreeArena arena;
    TreeNode* n6 = arena.make(6);
    TreeNode* n5 = arena.make(5, n6, nullptr);
    TreeNode* n4 = arena.make(4, n5, nullptr);
    TreeNode* n2 = arena.make(2, n4, nullptr);
    TreeNode* root = arena.make(1, n2, arena.make(3));
    Solution sol;
    ctx.expect_eq(sol.findClosestLeaf(root, 2), 3);
  });

  ctx.test("target_is_already_leaf", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), arena.make(3));
    Solution sol;
    ctx.expect_eq(sol.findClosestLeaf(root, 3), 3);
  });

  ctx.test("closest_leaf_downward_beats_upward", [&] {
    //         1
    //        / \
    //       2   3
    //      /
    //     4
    //    /
    //   5
    // From k = 4: leaf 5 is 1 edge away vs leaf 3 which is 3 edges away.
    TreeArena arena;
    TreeNode* n5 = arena.make(5);
    TreeNode* n4 = arena.make(4, n5, nullptr);
    TreeNode* n2 = arena.make(2, n4, nullptr);
    TreeNode* root = arena.make(1, n2, arena.make(3));
    Solution sol;
    ctx.expect_eq(sol.findClosestLeaf(root, 4), 5);
  });

  ctx.test("empty_tree_or_missing_target", [&] {
    TreeArena arena;
    Solution sol;
    ctx.expect_eq(sol.findClosestLeaf(nullptr, 1), -1);
    ctx.expect_eq(sol.findClosestLeaf(arena.make(1), 99), -1);
  });
}

}  // namespace
