// LeetCode 112: Path Sum
// https://leetcode.com/problems/path-sum/
//
// Problem:
//   Given the root of a binary tree and an integer target_sum, determine
//   whether the tree has a root-to-leaf path such that adding up all node
//   values along the path equals target_sum. A leaf is defined as a node with
//   no left or right children. An empty tree has no root-to-leaf paths, so it
//   always returns false (even when target_sum is 0).
//
// Algorithm:
//   Perform a top-down depth-first search (DFS) from the root, tracking the
//   remaining sum required to reach target_sum. At each visited node, subtract
//   its value from the remaining target:
//     - Base case (null node): return false because no path can end at null.
//     - Leaf check: if both left and right children are nullptr, return whether
//       the remaining target equals node->val.
//     - Recursive step: return true if either the left subtree or the right
//       subtree contains a valid path for (remaining - node->val).
//   Short-circuit evaluation stops exploration immediately once a valid path
//   is found.
//   Time Complexity:  O(N), where N is the number of nodes in the tree (each
//                     node is visited at most once).
//   Space Complexity: O(H), where H is the height of the tree for the call
//                     stack (O(log N) for balanced trees, O(N) worst case).
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`) storing an integer value and left/right
//     child pointers.
//   - Implicit recursion call stack storing active root-to-current path frames.
//
// Real-World Applications:
//   - Decision-tree and rule-engine feasibility checks (verifying whether any
//     complete branch meets a required score or cost budget).
//   - Hierarchical budget/quota verification in organizational or cloud
//     resource trees (checking whether an end-to-end allocation path matches a
//     target expenditure).
//   - Game tree and state-space reachability checks for exact terminal scores.
//
// Related LeetCode Problems:
//   - #113 Path Sum II [done] (collects all matching root-to-leaf paths instead of existence)
//   - #437 Path Sum III [done] (counts downward paths starting and ending at any node)
//   - #129 Sum Root to Leaf Numbers [done] (root-to-leaf DFS accumulating base-10 path values)
//   - #124 Binary Tree Maximum Path Sum [done] (maximizes sum across arbitrary tree paths)
//   - #111 Minimum Depth of Binary Tree [done] (root-to-leaf traversal with leaf termination guard)

#include "framework/lc_framework.h"

#include <deque>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int value) : val(value) {}
  TreeNode(int value, TreeNode* left_child, TreeNode* right_child)
      : val(value), left(left_child), right(right_child) {}
};

// Arena owning TreeNode instances for leak-free test tree construction.
// std::deque guarantees pointer stability across emplace_back() calls.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns true if there exists a root-to-leaf path whose node values sum to
  // target_sum.
  //
  // Invariant: at each recursive call dfs(node, remaining), `remaining` equals
  // target_sum minus the sum of all strict ancestors of `node`.
  // Time: O(N), Space: O(H).
  bool hasPathSum(const TreeNode* root, int target_sum) {
    auto dfs = [](this auto&& self, const TreeNode* node,
                  int remaining) -> bool {
      // Null branches cannot form a root-to-leaf path.
      if (node == nullptr) {
        return false;
      }

      const int next_remaining = remaining - node->val;

      // Only terminate successfully at a true leaf (no children).
      if (node->left == nullptr && node->right == nullptr) {
        return next_remaining == 0;
      }

      // Short-circuit as soon as either subtree yields a valid path.
      return self(node->left, next_remaining) ||
             self(node->right, next_remaining);
    };

    return dfs(root, target_sum);
  }
};

LC_PROBLEM(112, "Path Sum", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 (sum 22 exists)", [&] {
    //       5
    //      / \
    //     4   8
    //    /   / \
    //   11  13  4
    //  /  \      \
    // 7    2      1
    TreeArena arena;
    TreeNode* root = arena.make(
        5, arena.make(4, arena.make(11, arena.make(7), arena.make(2))),
        arena.make(8, arena.make(13), arena.make(4, nullptr, arena.make(1))));

    ctx.expect_true(sol.hasPathSum(root, 22));
    ctx.expect_true(sol.hasPathSum(root, 27));
    ctx.expect_true(sol.hasPathSum(root, 26));
    ctx.expect_true(sol.hasPathSum(root, 18));
    ctx.expect_false(sol.hasPathSum(root, 20));
  });

  ctx.test("symmetric tree from python build()", [&] {
    //       1
    //      / \
    //     2   2
    //    / \ / \
    //   3  4 4  3
    TreeArena arena;
    TreeNode* root =
        arena.make(1, arena.make(2, arena.make(3), arena.make(4)),
                   arena.make(2, arena.make(4), arena.make(3)));

    ctx.expect_true(sol.hasPathSum(root, 7));
    ctx.expect_true(sol.hasPathSum(root, 6));
    ctx.expect_false(sol.hasPathSum(root, 5));
  });

  ctx.test("empty tree returns false even for target 0", [&] {
    ctx.expect_false(sol.hasPathSum(nullptr, 0));
    ctx.expect_false(sol.hasPathSum(nullptr, 5));
  });

  ctx.test("single node tree", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(5);
    ctx.expect_true(sol.hasPathSum(root, 5));
    ctx.expect_false(sol.hasPathSum(root, 0));
  });

  ctx.test("internal node matching target is not a leaf", [&] {
    //   1
    //  /
    // 2
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2));
    ctx.expect_false(sol.hasPathSum(root, 1));
    ctx.expect_true(sol.hasPathSum(root, 3));
  });

  ctx.test("negative node values and negative target", [&] {
    //   -2
    //     \
    //     -3
    TreeArena arena;
    TreeNode* root = arena.make(-2, nullptr, arena.make(-3));
    ctx.expect_true(sol.hasPathSum(root, -5));
    ctx.expect_false(sol.hasPathSum(root, -2));
  });
}

}  // namespace
