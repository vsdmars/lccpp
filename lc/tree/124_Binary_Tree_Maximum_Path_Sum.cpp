// LeetCode 124: Binary Tree Maximum Path Sum
// https://leetcode.com/problems/binary-tree-maximum-path-sum/
//
// Problem:
//   Given the root of a non-empty binary tree, find the maximum path sum of any
//   non-empty path. A path is a sequence of nodes where each pair of adjacent
//   nodes has an edge between them, and no node appears more than once. The
//   path does not need to pass through the root and does not need to end at
//   leaf nodes. Node values may be negative.
//
// Algorithm:
//   Solve with bottom-up postorder tree dynamic programming (DFS):
//     - Every valid path in a tree has a unique highest node (lowest common
//       ancestor of its endpoints) where the path "turns" from the left subtree
//       to the right subtree.
//     - For each node, recursively compute the maximum single-branch downward
//       gain from its left child (`left_gain`) and right child (`right_gain`),
//       clamping negative gains to 0 (choosing not to extend into a negative
//       subtree).
//     - Update the global maximum `best_sum` with the best path turning at this
//       node: `node->val + left_gain + right_gain`.
//     - Return `node->val + max(left_gain, right_gain)` to the parent, because
//       any path continuing upward through the parent may choose at most one
//       child branch to remain a simple path.
//   Initialize `best_sum` to `std::numeric_limits<int>::min()` so trees with
//   all-negative values still return the largest (least negative) single node.
//   Time Complexity:  O(N), visiting each node once in postorder.
//   Space Complexity: O(H) for the recursion stack, where H is the tree height
//                     (O(log N) balanced, O(N) skewed).
//
// Data Structures:
//   - Binary tree (`TreeNode`) traversed bottom-up via the recursion stack.
//   - Scalar state (`best_sum`) tracking the global optimum across all candidate
//     turning nodes.
//
// Real-World Applications:
//   - VLSI circuit and network-topology optimization (finding maximum-weight
//     routing corridors or critical timing paths in tree-structured networks).
//   - Phylogenetic and evolutionary tree analysis (identifying the highest-
//     scoring connected lineage segment across weighted branches).
//   - Hierarchical financial/portfolio attribution (locating the highest
//     net-return connected segment in a tree of correlated entities).
//
// Related LeetCode Problems:
//   - #543 Diameter of Binary Tree [done] (same postorder DP pattern maximizing turning path length)
//   - #687 Longest Univalue Path [done] (postorder DFS combining left and right equal-value chains)
//   - #112 Path Sum [done] (checks root-to-leaf path sum equality)
//   - #129 Sum Root to Leaf Numbers [done] (aggregates root-to-leaf paths into a numeric sum)
//   - #1372 Longest ZigZag Path in a Binary Tree [new] (tree DP tracking directional downward gains)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>
#include <limits>

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
  // Returns the maximum path sum over all non-empty simple paths in the tree.
  //
  // Invariant: maxGain(node) returns the maximum sum of a downward path
  // starting at `node` (inclusive) and going into at most one child subtree,
  // while updating `best_sum` with the best path whose highest node is `node`.
  // Time: O(N), Space: O(H).
  int maxPathSum(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    int best_sum = std::numeric_limits<int>::min();

    auto max_gain = [&](this auto&& self, const TreeNode* node) -> int {
      if (node == nullptr) {
        return 0;
      }

      // Discard negative child contributions since extending into them lowers
      // the path sum.
      const int left_gain = std::max(0, self(node->left));
      const int right_gain = std::max(0, self(node->right));

      // Candidate path with `node` as its highest (turning) vertex.
      const int path_through_node = node->val + left_gain + right_gain;
      best_sum = std::max(best_sum, path_through_node);

      // Only one branch can be extended upward to `node`'s parent.
      return node->val + std::max(left_gain, right_gain);
    };

    max_gain(root);
    return best_sum;
  }
};

LC_PROBLEM(124, "Binary Tree Maximum Path Sum", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 ([1,2,3])", [&] {
    //    1
    //   / \
    //  2   3
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), arena.make(3));
    ctx.expect_eq(sol.maxPathSum(root), 6);
  });

  ctx.test("leetcode example 2 (best path skips negative root)", [&] {
    //    -10
    //    / \
    //   9  20
    //     /  \
    //    15   7
    TreeArena arena;
    TreeNode* root =
        arena.make(-10, arena.make(9),
                   arena.make(20, arena.make(15), arena.make(7)));
    ctx.expect_eq(sol.maxPathSum(root), 42);
  });

  ctx.test("single negative node from python build()", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(-3);
    ctx.expect_eq(sol.maxPathSum(root), -3);
  });

  ctx.test("all negative nodes picks least negative single node", [&] {
    //     -10
    //     / \
    //   -20  -5
    TreeArena arena;
    TreeNode* root = arena.make(-10, arena.make(-20), arena.make(-5));
    ctx.expect_eq(sol.maxPathSum(root), -5);
  });

  ctx.test("root alone beats extending into negative children", [&] {
    //     2
    //    / \
    //  -1  -2
    TreeArena arena;
    TreeNode* root = arena.make(2, arena.make(-1), arena.make(-2));
    ctx.expect_eq(sol.maxPathSum(root), 2);
  });

  ctx.test("deep path combining left and right subtrees", [&] {
    //        5
    //       / \
    //      4   8
    //     /   / \
    //    11  13  4
    //   /  \      \
    //  7    2      1
    // Best path: 7 -> 11 -> 4 -> 5 -> 8 -> 13 = 48.
    TreeArena arena;
    TreeNode* root = arena.make(
        5, arena.make(4, arena.make(11, arena.make(7), arena.make(2))),
        arena.make(8, arena.make(13), arena.make(4, nullptr, arena.make(1))));
    ctx.expect_eq(sol.maxPathSum(root), 48);
  });
}

}  // namespace
