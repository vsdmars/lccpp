// LeetCode 102: Binary Tree Level Order Traversal
// https://leetcode.com/problems/binary-tree-level-order-traversal/
//
// Problem:
//   Given the root of a binary tree, return the level order traversal of its
//   nodes' values grouped by depth (from left to right, level by level,
//   starting at the root). An empty tree returns an empty 2D vector.
//
// Algorithm:
//   Breadth-First Search (BFS) with level-boundary snapshotting. Seed a FIFO
//   queue with the root node. On each outer iteration, capture the current
//   queue size `level_size` — every one of those nodes belongs to the exact
//   same depth because children discovered during the loop are appended to the
//   back of the queue for the next level. Pop `level_size` nodes in order,
//   record their values in `level`, and enqueue any non-null left and right
//   children.
//   - Time Complexity: O(N), where N is the number of nodes (each node is
//     enqueued and dequeued exactly once).
//   - Space Complexity: O(W) for the BFS queue where W is the maximum width of
//     the tree (up to N/2 = O(N) on the bottom level of a full binary tree),
//     plus O(N) for the returned result.
//
// Data Structures:
//   - `std::queue<const TreeNode*>` as the FIFO BFS frontier preserving
//     strict left-to-right ordering within each depth layer.
//   - `std::vector<std::vector<int>>` to collect per-level node values.
//
// Real-World Applications:
//   - Breadth-first crawling and shortest-hop discovery in network topologies,
//     social graphs, and web crawlers.
//   - Rendering hierarchical UI trees, organization charts, or scene graphs
//     layer by layer.
//   - Parallel task scheduling across dependency DAGs where each level
//     represents a wave of independent tasks ready to execute concurrently.
//
// Related LeetCode Problems:
//   - #103 Binary Tree Zigzag Level Order Traversal [done] (alternates left-to-right and right-to-left per level)
//   - #107 Binary Tree Level Order Traversal II [done] (returns levels bottom-up from leaves to root)
//   - #199 Binary Tree Right Side View [done] (takes the last node of each BFS level)
//   - #637 Average of Levels in Binary Tree [done] (computes mean of values at each BFS level)
//   - #429 N-ary Tree Level Order Traversal [new] (generalizes level-order BFS to arbitrary branching)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode binary tree node representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Returns node values grouped level by level from top to bottom and left to
  // right using FIFO breadth-first search.
  // Time: O(N), Space: O(N).
  std::vector<std::vector<int>> levelOrder(const TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (root == nullptr) {
      return result;
    }

    std::queue<const TreeNode*> frontier;
    frontier.push(root);

    // Invariant: at the start of each outer iteration, `frontier` holds
    // exactly the nodes of the next depth level in left-to-right order.
    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      std::vector<int> level;
      level.reserve(level_size);

      for (std::size_t i = 0uz; i < level_size; ++i) {
        const TreeNode* node = frontier.front();
        frontier.pop();
        level.push_back(node->val);

        if (node->left != nullptr) {
          frontier.push(node->left);
        }
        if (node->right != nullptr) {
          frontier.push(node->right);
        }
      }

      result.push_back(std::move(level));
    }

    return result;
  }
};

// RAII node arena backed by std::deque for leak-free test tree construction.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

  TreeNode* from_level_order(const std::vector<std::optional<int>>& values) {
    if (values.empty() || !values[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*values[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);
    std::size_t idx = 1uz;

    while (!pending.empty() && idx < values.size()) {
      TreeNode* curr = pending.front();
      pending.pop();

      if (idx < values.size() && values[idx].has_value()) {
        curr->left = make(*values[idx]);
        pending.push(curr->left);
      }
      ++idx;

      if (idx < values.size() && values[idx].has_value()) {
        curr->right = make(*values[idx]);
        pending.push(curr->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(102, "Binary Tree Level Order Traversal", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_and_python_build", [&] {
    TreeArena arena;
    //     3
    //    / \
    //   9  20
    //     /  \
    //    15   7
    TreeNode* root =
        arena.from_level_order({3, 9, 20, std::nullopt, std::nullopt, 15, 7});
    const std::vector<std::vector<int>> expected = {{3}, {9, 20}, {15, 7}};
    ctx.expect_eq(sol.levelOrder(root), expected);
  });

  ctx.test("empty_tree", [&] {
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.levelOrder(nullptr), expected);
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);
    const std::vector<std::vector<int>> expected = {{1}};
    ctx.expect_eq(sol.levelOrder(root), expected);
  });

  ctx.test("left_skewed_tree", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.make(1, arena.make(2, arena.make(3), nullptr), nullptr);
    const std::vector<std::vector<int>> expected = {{1}, {2}, {3}};
    ctx.expect_eq(sol.levelOrder(root), expected);
  });

  ctx.test("complete_binary_tree", [&] {
    TreeArena arena;
    TreeNode* root = arena.from_level_order({1, 2, 3, 4, 5, 6, 7});
    const std::vector<std::vector<int>> expected = {
        {1}, {2, 3}, {4, 5, 6, 7}};
    ctx.expect_eq(sol.levelOrder(root), expected);
  });
}

}  // namespace
