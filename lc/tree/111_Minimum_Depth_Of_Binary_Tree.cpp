// LeetCode 111: Minimum Depth Of Binary Tree
// https://leetcode.com/problems/minimum-depth-of-binary-tree/
//
// Problem:
//   Given the root of a binary tree, find its minimum depth — the number of
//   nodes along the shortest path from the root node down to the nearest leaf
//   node (a node with no children). An empty tree has minimum depth 0.
//   Note: A node with only one child is NOT a leaf, so paths must continue
//   down the existing subtree.
//
// Algorithm:
//   Level-by-level Breadth-First Search (BFS) with early termination.
//   Unlike DFS (which must explore the entire tree in the worst case before
//   confirming the global minimum), BFS explores nodes in non-decreasing order
//   of depth. Increment `depth` at the start of each level; the very first
//   node encountered with `left == nullptr && right == nullptr` is guaranteed
//   to be a nearest leaf, so we immediately return `depth` without traversing
//   deeper subtrees.
//   - Time Complexity: O(N) worst case (complete tree where all leaves are at
//     the bottom), and O(2^d) early-exit time when the nearest leaf sits at
//     shallow depth d << H.
//   - Space Complexity: O(W) for the BFS queue where W is the width of the
//     frontier up to the shallowest leaf (worst case O(N)).
//
// Data Structures:
//   - `std::queue<const TreeNode*>` for FIFO breadth-first expansion by depth.
//
// Real-World Applications:
//   - Shortest-path and earliest-exit discovery in decision trees, rule
//     engines, and game-state search trees.
//   - Detecting shallow failure modes or minimal witness paths in formal
//     verification and model checking.
//   - Network reachability probes finding the minimum hop count from a root
//     router to the closest terminal endpoint.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth of Binary Tree [done] (longest root-to-leaf path; requires full traversal)
//   - #102 Binary Tree Level Order Traversal [done] (same level-by-level BFS frontier skeleton)
//   - #110 Balanced Binary Tree [done] (compares subtree heights at every internal node)
//   - #112 Path Sum [done] (also distinguishes true leaf nodes from single-child internal nodes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
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
  // Computes the minimum root-to-leaf depth using level-order BFS so we stop
  // immediately upon reaching the shallowest leaf.
  // Time: O(N) worst case (early exit at shallowest leaf), Space: O(N).
  int minDepth(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    std::queue<const TreeNode*> frontier;
    frontier.push(root);
    int depth = 0;

    while (!frontier.empty()) {
      ++depth;
      const std::size_t level_size = frontier.size();

      for (std::size_t i = 0uz; i < level_size; ++i) {
        const TreeNode* node = frontier.front();
        frontier.pop();

        // First leaf reached in BFS order defines the exact minimum depth.
        if (node->left == nullptr && node->right == nullptr) {
          return depth;
        }
        if (node->left != nullptr) {
          frontier.push(node->left);
        }
        if (node->right != nullptr) {
          frontier.push(node->right);
        }
      }
    }

    return depth;
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

LC_PROBLEM(111, "Minimum Depth Of Binary Tree", "tree", ctx) {
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
    ctx.expect_eq(sol.minDepth(root), 2);
  });

  ctx.test("example_2_right_skewed_not_leaf_at_root", [&] {
    TreeArena arena;
    // 2 -> 3 -> 4 -> 5 -> 6 (root has one child, so minimum depth is 5, not 1)
    TreeNode* root = arena.make(
        2, nullptr,
        arena.make(3, nullptr,
                   arena.make(4, nullptr,
                              arena.make(5, nullptr, arena.make(6)))));
    ctx.expect_eq(sol.minDepth(root), 5);
  });

  ctx.test("empty_tree", [&] { ctx.expect_eq(sol.minDepth(nullptr), 0); });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);
    ctx.expect_eq(sol.minDepth(root), 1);
  });

  ctx.test("left_skewed_chain", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.make(1, arena.make(2, arena.make(3), nullptr), nullptr);
    ctx.expect_eq(sol.minDepth(root), 3);
  });

  ctx.test("complete_binary_tree", [&] {
    TreeArena arena;
    TreeNode* root = arena.from_level_order({1, 2, 3, 4, 5, 6, 7});
    ctx.expect_eq(sol.minDepth(root), 3);
  });
}

}  // namespace
