// LeetCode 545: Boundary Of Binary Tree
// https://leetcode.com/problems/boundary-of-binary-tree/
//
// Problem:
//   Given the root of a binary tree, return the values of its boundary in
//   anti-clockwise order starting from the root without duplicates. The
//   boundary consists of:
//   1. The root node.
//   2. The left boundary: top-down path starting from `root->left`, always
//      stepping left when available and right otherwise, excluding leaf nodes
//      (empty if `root->left == nullptr`).
//   3. All leaf nodes from left to right (excluding the root if it is a leaf,
//      since the root is already included first).
//   4. The right boundary: bottom-up (reversed) path starting from
//      `root->right`, always stepping right when available and left otherwise,
//      excluding leaf nodes (empty if `root->right == nullptr`).
//
// Examples:
//   Example 1:
//     Input:  root = [1, null, 2, 3, 4]
//     Output: [1, 3, 4, 2]
//     Explanation:
//       - Initialize boundary = [1] with root->val.
//       - Phase 1 (addLeftBoundary(root->left)) returns immediately because
//         root->left is nullptr.
//       - Phase 2 (addLeaves) runs a left-to-right DFS and appends non-root leaf
//         nodes 3 and 4, giving boundary = [1, 3, 4].
//       - Phase 3 (addRightBoundary(2)) steps right to leaf 4 (stopping at
//         isLeaf(4)) and appends 2 on post-order unwind, returning [1, 3, 4, 2].
//
//   Example 2:
//     Input:  root = [1, 2, 3, 4, 5, 6, null, null, null, 7, 8, 9, 10]
//     Output: [1, 2, 4, 7, 8, 9, 10, 6, 3]
//     Explanation:
//       - Initialize boundary = [1] with root->val.
//       - Phase 1 (addLeftBoundary(2)) appends internal node 2, follows 2->left to
//         4, and stops because 4 is a leaf, leaving boundary = [1, 2].
//       - Phase 2 (addLeaves) visits all leaves from left to right and appends 4,
//         7, 8, 9, and 10, giving boundary = [1, 2, 4, 7, 8, 9, 10].
//       - Phase 3 (addRightBoundary(3)) falls back to 3->left = 6 (since 3->right
//         is nullptr), stops at leaf 10 (6->right), and appends 6 then 3 on unwind,
//         returning [1, 2, 4, 7, 8, 9, 10, 6, 3].
//
//   Example 3:
//     Input:  root = [1, 2, null, null, 3, 4]
//     Output: [1, 2, 3, 4]
//     Explanation:
//       - Initialize boundary = [1] with root->val.
//       - Phase 1 (addLeftBoundary(2)) appends 2, falls back to 2->right = 3
//         because 2->left is nullptr, appends 3, and stops at leaf 4 (3->left),
//         leaving boundary = [1, 2, 3].
//       - Phase 2 (addLeaves) collects the single leaf 4, giving boundary = [1, 2,
//         3, 4].
//       - Phase 3 (addRightBoundary(nullptr)) adds nothing since root->right is
//         nullptr, returning [1, 2, 3, 4].
//
// Algorithm:
//   Decompose the anti-clockwise perimeter into three disjoint traversals
//   after recording `root->val`:
//   1. Left boundary (`add_left_boundary`): pre-order descent from
//      `root->left`, recording each internal (non-leaf) node before recursing
//      to `node->left` if non-null, else `node->right`. Stopping at leaves
//      prevents duplicating the leftmost leaf in phase 2.
//   2. Leaves (`add_leaves`): standard left-to-right DFS over the entire tree,
//      appending every leaf node that is not the root itself.
//   3. Right boundary (`add_right_boundary`): post-order descent from
//      `root->right`, recursing to `node->right` if non-null, else
//      `node->left`, and appending each internal node on unwind so the right
//      boundary appears in bottom-up anti-clockwise order without reversing.
//   Time Complexity: O(n), visiting left/right spines once plus one full DFS
//   over all n nodes for leaves.
//   Space Complexity: O(h) recursion stack space (plus O(n) for the output
//   vector).
//
// Data Structures:
//   - Binary tree (`TreeNode`).
//   - Dynamic array (`std::vector<int>`) accumulating boundary node values in
//     anti-clockwise order.
//
// Real-World Applications:
//   - Computing the outer silhouette or convex hull contour of hierarchical
//     bounding-volume trees (BVH / quadtrees) in 2D/3D graphics rendering.
//   - Visualizing outer perimeter nodes of organizational charts, call graphs,
//     or phylogenetic trees in graph-layout engines.
//   - Summarizing boundary constraints in spatial partition trees.
//
// Related LeetCode Problems:
//   - #199 Binary Tree Right Side View [done] (extracting the rightmost visible nodes per level)
//   - #987 Vertical Order Traversal of a Binary Tree [done] (positional tree traversal)
//   - #102 Binary Tree Level Order Traversal [done] (structured tree traversal)
//   - #366 Find Leaves of Binary Tree [done] (identifying and collecting tree leaves)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Returns the anti-clockwise boundary of the binary tree starting at `root`
  // with no duplicate nodes.
  // Time: O(n), Space: O(h) auxiliary stack space.
  std::vector<int> boundaryOfBinaryTree(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    std::vector<int> boundary;
    boundary.push_back(root->val);

    // Phase 1: Left boundary (top-down, excluding root and all leaves).
    addLeftBoundary(root->left, boundary);

    // Phase 2: All leaf nodes from left to right (excluding root if it is a
    // single-node tree, since root was already added).
    addLeaves(root, root, boundary);

    // Phase 3: Right boundary (bottom-up via post-order, excluding root and
    // all leaves).
    addRightBoundary(root->right, boundary);

    return boundary;
  }

 private:
  // Returns true iff `node` has neither a left nor a right child.
  static bool isLeaf(const TreeNode* node) {
    return node->left == nullptr && node->right == nullptr;
  }

  // Walks down the left boundary in pre-order, stopping before any leaf node
  // so leaves are collected exactly once in `addLeaves`.
  static void addLeftBoundary(const TreeNode* node,
                              std::vector<int>& boundary) {
    if (node == nullptr || isLeaf(node)) {
      return;
    }
    boundary.push_back(node->val);
    // Prefer going left; only fall back to right if no left child exists.
    if (node->left != nullptr) {
      addLeftBoundary(node->left, boundary);
    } else {
      addLeftBoundary(node->right, boundary);
    }
  }

  // Collects every leaf node in left-to-right DFS order, skipping `root` to
  // avoid duplicating a single-node tree.
  static void addLeaves(const TreeNode* node, const TreeNode* root,
                        std::vector<int>& boundary) {
    if (node == nullptr) {
      return;
    }
    if (node != root && isLeaf(node)) {
      boundary.push_back(node->val);
      return;
    }
    addLeaves(node->left, root, boundary);
    addLeaves(node->right, root, boundary);
  }

  // Walks down the right boundary and appends internal nodes on the way back
  // up (post-order) so they appear in anti-clockwise bottom-to-top order.
  static void addRightBoundary(const TreeNode* node,
                               std::vector<int>& boundary) {
    if (node == nullptr || isLeaf(node)) {
      return;
    }
    // Prefer going right; only fall back to left if no right child exists.
    if (node->right != nullptr) {
      addRightBoundary(node->right, boundary);
    } else {
      addRightBoundary(node->left, boundary);
    }
    boundary.push_back(node->val);
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

LC_PROBLEM(545, "Boundary Of Binary Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_no_left_subtree", [&] {
    //   1
    //    \
    //     2
    //    / \
    //   3   4
    TreeArena arena;
    TreeNode* n2 = arena.make(2, arena.make(3), arena.make(4));
    TreeNode* root = arena.make(1, nullptr, n2);
    ctx.expect_eq(sol.boundaryOfBinaryTree(root), std::vector<int>{1, 3, 4, 2});
  });

  ctx.test("example_2_full_perimeter", [&] {
    //        1
    //      /   \
    //     2     3
    //    / \   /
    //   4   5 6
    //      /\ /\
    //     7 8 9 10
    TreeArena arena;
    TreeNode* n5 = arena.make(5, arena.make(7), arena.make(8));
    TreeNode* n2 = arena.make(2, arena.make(4), n5);
    TreeNode* n6 = arena.make(6, arena.make(9), arena.make(10));
    TreeNode* n3 = arena.make(3, n6, nullptr);
    TreeNode* root = arena.make(1, n2, n3);
    ctx.expect_eq(sol.boundaryOfBinaryTree(root),
                  std::vector<int>{1, 2, 4, 7, 8, 9, 10, 6, 3});
  });

  ctx.test("python_build_tree", [&] {
    //      1
    //     / \
    //    2   4
    //   /
    //  3
    TreeArena arena;
    TreeNode* root =
        arena.make(1, arena.make(2, arena.make(3), nullptr), arena.make(4));
    ctx.expect_eq(sol.boundaryOfBinaryTree(root), std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    TreeNode* solo = arena.make(1);
    ctx.expect_eq(sol.boundaryOfBinaryTree(solo), std::vector<int>{1});
    ctx.expect_eq(sol.boundaryOfBinaryTree(nullptr), std::vector<int>{});
  });

  ctx.test("zigzag_left_boundary_no_right_subtree", [&] {
    //    1
    //   /
    //  2
    //   \
    //    3
    //   /
    //  4
    TreeArena arena;
    TreeNode* n3 = arena.make(3, arena.make(4), nullptr);
    TreeNode* n2 = arena.make(2, nullptr, n3);
    TreeNode* root = arena.make(1, n2, nullptr);
    ctx.expect_eq(sol.boundaryOfBinaryTree(root), std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("right_skewed_chain", [&] {
    //   1 -> 2 -> 3 (right children only)
    TreeArena arena;
    TreeNode* root =
        arena.make(1, nullptr, arena.make(2, nullptr, arena.make(3)));
    ctx.expect_eq(sol.boundaryOfBinaryTree(root), std::vector<int>{1, 3, 2});
  });
}

}  // namespace
