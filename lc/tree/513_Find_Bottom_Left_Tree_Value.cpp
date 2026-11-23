// LeetCode 513: Find Bottom Left Tree Value
// https://leetcode.com/problems/find-bottom-left-tree-value/
//
// Problem:
//   Given the root of a non-empty binary tree, return the leftmost value in
//   the last (deepest) row of the tree.
//
// Examples:
//   Example 1:
//     Input:  root = [2, 1, 3]
//     Output: 1
//     Explanation:
//       - Initialize frontier = [2] and bottom_left = 2.
//       - On level 0 (level_size = 1), record bottom_left = frontier.front()->val =
//         2, dequeue 2, and enqueue children 1 and 3 so frontier = [1, 3].
//       - On level 1 (level_size = 2), record bottom_left = frontier.front()->val =
//         1, then dequeue 1 and 3 (neither has children).
//       - Because frontier is now empty, exit the loop and return bottom_left = 1.
//
//   Example 2:
//     Input:  root = [1, 2, 3, 4, null, 5, 6, null, null, 7]
//     Output: 7
//     Explanation:
//       - Initialize frontier = [1] and bottom_left = 1.
//       - On level 0 (level_size = 1), set bottom_left = 1, dequeue 1, and enqueue
//         2 and 3 so frontier = [2, 3].
//       - On level 1 (level_size = 2), set bottom_left = 2, dequeue 2 (enqueueing
//         4) and 3 (enqueueing 5 and 6), leaving frontier = [4, 5, 6].
//       - On level 2 (level_size = 3), set bottom_left = 4, dequeue 4 (no
//         children), 5 (enqueueing 7), and 6 (no children), leaving frontier = [7].
//       - On level 3 (level_size = 1), set bottom_left = 7 and dequeue 7 (no
//         children), emptying frontier and returning 7.
//
//   Example 3:
//     Input:  root = [1, null, 2, null, 3]
//     Output: 3
//     Explanation:
//       - Initialize frontier = [1] and bottom_left = 1.
//       - On level 0 (level_size = 1), set bottom_left = 1, dequeue 1, and enqueue
//         its right child 2 so frontier = [2].
//       - On level 1 (level_size = 1), set bottom_left = 2, dequeue 2, and enqueue
//         its right child 3 so frontier = [3].
//       - On level 2 (level_size = 1), set bottom_left = 3 and dequeue 3 (no
//         children), emptying frontier and returning 3.
//
// Algorithm:
//   Perform a standard left-to-right level-order Breadth-First Search (BFS)
//   using a FIFO queue. At the start of each level, the node at the front of
//   the queue is guaranteed to be the leftmost node of that depth, so record
//   its value as `bottom_left`. Then dequeue every node in the current level
//   and enqueue its left child followed by its right child. When the queue
//   becomes empty, `bottom_left` holds the leftmost value of the final level.
//   Time Complexity: O(n), visiting every node once.
//   Space Complexity: O(w), where w <= n/2 is the maximum width of the tree.
//
// Data Structures:
//   - Binary tree (`TreeNode`).
//   - FIFO queue (`std::queue<const TreeNode*>`) for level-by-level BFS.
//
// Real-World Applications:
//   - Locating the first visual element in the deepest nested layer of a UI
//     layout or DOM hierarchy.
//   - Identifying the earliest-scheduled leaf task at the deepest level of a
//     dependency or execution tree.
//   - Level-oriented tree rendering and pagination boundaries in hierarchical
//     document viewers.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (core level-by-level BFS template)
//   - #199 Binary Tree Right Side View [done] (extracting one extreme node per tree level)
//   - #515 Find Largest Value in Each Tree Row [done] (per-level aggregation via BFS)
//   - #104 Maximum Depth of Binary Tree [done] (determining deepest tree row)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <queue>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Finds the leftmost value in the deepest row using level-order BFS.
  // Returns 0 defensively if `root` is null.
  // Time: O(n), Space: O(w) where w is the maximum width of the tree.
  int findBottomLeftValue(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    std::queue<const TreeNode*> frontier;
    frontier.push(root);

    int bottom_left = root->val;

    // Process one full depth level per outer iteration. Because children are
    // enqueued left-before-right, `frontier.front()` at the beginning of each
    // iteration is always the leftmost node of the current row.
    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      bottom_left = frontier.front()->val;

      for (std::size_t i = 0uz; i < level_size; ++i) {
        const TreeNode* node = frontier.front();
        frontier.pop();

        if (node->left != nullptr) {
          frontier.push(node->left);
        }
        if (node->right != nullptr) {
          frontier.push(node->right);
        }
      }
    }

    return bottom_left;
  }
};

// RAII arena that owns test TreeNode instances without raw heap allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.push_back(TreeNode{.val = val, .left = left, .right = right});
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(513, "Find Bottom Left Tree Value", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_balanced_three_nodes", [&] {
    //     2
    //    / \
    //   1   3
    TreeArena arena;
    TreeNode* root = arena.make(2, arena.make(1), arena.make(3));
    ctx.expect_eq(sol.findBottomLeftValue(root), 1);
  });

  ctx.test("example_2_deep_node_in_right_subtree", [&] {
    //         1
    //        / \
    //       2   3
    //      /   / \
    //     4   5   6
    //        /
    //       7
    TreeArena arena;
    TreeNode* n2 = arena.make(2, arena.make(4), nullptr);
    TreeNode* n5 = arena.make(5, arena.make(7), nullptr);
    TreeNode* n3 = arena.make(3, n5, arena.make(6));
    TreeNode* root = arena.make(1, n2, n3);
    ctx.expect_eq(sol.findBottomLeftValue(root), 7);
  });

  ctx.test("python_build_right_child_of_left_subtree", [&] {
    //         1
    //        / \
    //       2   2
    //        \ / \
    //        7 4  3
    TreeArena arena;
    TreeNode* left = arena.make(2, nullptr, arena.make(7));
    TreeNode* right = arena.make(2, arena.make(4), arena.make(3));
    TreeNode* root = arena.make(1, left, right);
    ctx.expect_eq(sol.findBottomLeftValue(root), 7);
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(99);
    ctx.expect_eq(sol.findBottomLeftValue(root), 99);
  });

  ctx.test("right_skewed_tree", [&] {
    //   1 -> 2 -> 3 (all right children)
    TreeArena arena;
    TreeNode* root =
        arena.make(1, nullptr, arena.make(2, nullptr, arena.make(3)));
    ctx.expect_eq(sol.findBottomLeftValue(root), 3);
  });
}

}  // namespace
