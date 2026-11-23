// LeetCode 270: Closest Binary Search Tree Value
// https://leetcode.com/problems/closest-binary-search-tree-value/
//
// Problem:
//   Given the root of a binary search tree (BST) and a floating-point `target`,
//   return the integer value in the BST that is closest to `target`. If two
//   values in the BST are equally close to `target`, return the smaller value.
//   Returns 0 defensively if `root` is null.
//
// Algorithm:
//   Binary search along a single root-to-leaf path in the BST:
//   - At each node, compute `diff = |node->val - target|`. Update `best_val`
//     if `diff` is strictly smaller than `best_diff`, or if `diff == best_diff`
//     and `node->val < best_val` (note: any real number equidistant from two
//     distinct integers is an integer or half-integer, both of which are exact
//     in IEEE-754 double precision for all 32-bit integers).
//   - If `target == node->val`, we found an exact match and terminate early.
//   - Otherwise, move right when `target > node->val` or left when
//     `target < node->val`, because the discarded subtree cannot contain a
//     closer value.
//   Both the O(1)-space iterative traversal (`closestValue`) and the recursive
//   counterpart (`closestValueRecursive`) are included.
//   - Time Complexity:  O(h), where h is the tree height (O(log n) balanced,
//     O(n) worst case).
//   - Space Complexity: O(1) for iterative `closestValue`, O(h) for recursive.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`).
//   - Constant scalar state (`best_val`, `best_diff`) tracking the closest
//     candidate seen along the search path.
//
// Real-World Applications:
//   - Nearest-neighbor lookup in 1D sorted indexes (matching a sensor reading
//     or timestamp to the nearest indexed event or calibration bucket).
//   - Consistent hashing rings (locating the closest server hash slot).
//   - Quantitative finance tick-rounding and strike-price matching against an
//     ordered tree of available option contracts.
//
// Related LeetCode Problems:
//   - #272 Closest Binary Search Tree Value II [done] (find the k closest values to target in a BST)
//   - #700 Search in a Binary Search Tree [new] (exact-match version of the same BST descent)
//   - #285 Inorder Successor In BST [done] (tracking the best ancestor candidate during BST search)
//   - #658 Find K Closest Elements [done] (closest-value search in a sorted array)

#include "framework/lc_framework.h"

#include <cmath>
#include <deque>
#include <limits>

namespace {

// Binary tree node definition matching LeetCode's standard representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

// RAII arena for test trees; std::deque guarantees pointer stability on growth.
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
  // Finds the BST node value closest to `target` using iterative O(h) descent.
  // Breaks distance ties in favor of the smaller integer value.
  // Time: O(h), Space: O(1).
  int closestValue(const TreeNode* root, double target) {
    if (root == nullptr) {
      return 0;
    }

    int best_val = root->val;
    double best_diff = std::fabs(static_cast<double>(root->val) - target);

    const TreeNode* cur = root;
    while (cur != nullptr) {
      const auto node_val = static_cast<double>(cur->val);
      const double diff = std::fabs(node_val - target);

      // Update on strictly closer distance, or on exact tie with a smaller key.
      if (diff < best_diff || (diff == best_diff && cur->val < best_val)) {
        best_diff = diff;
        best_val = cur->val;
      }

      // Exact match cannot be beaten; stop descending immediately.
      if (diff == 0.0) {
        break;
      }

      // Prune the half of the tree that moves strictly farther from `target`.
      cur = (target > node_val) ? cur->right : cur->left;
    }

    return best_val;
  }

  // Recursive DFS counterpart mirroring the iterative BST search.
  // Time: O(h), Space: O(h).
  int closestValueRecursive(const TreeNode* root, double target) {
    if (root == nullptr) {
      return 0;
    }

    int best_val = root->val;
    double best_diff = std::numeric_limits<double>::infinity();

    auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }

      const auto node_val = static_cast<double>(node->val);
      const double diff = std::fabs(node_val - target);

      if (diff < best_diff || (diff == best_diff && node->val < best_val)) {
        best_diff = diff;
        best_val = node->val;
      }

      if (diff == 0.0) {
        return;
      }

      if (target > node_val) {
        self(node->right);
      } else {
        self(node->left);
      }
    };

    dfs(root);
    return best_val;
  }
};

LC_PROBLEM(270, "Closest Binary Search Tree Value", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("python_build_midpoint_tie_breaks_smaller", [&] {
    TreeArena arena;
    //       4
    //      / \
    //     2   5
    //    / \
    //   1   3
    TreeNode* left = arena.make(2, arena.make(1), arena.make(3));
    TreeNode* root = arena.make(4, left, arena.make(5));

    // Distance to 4 and 5 is both 0.5 -> must return smaller value 4.
    ctx.expect_eq(sol.closestValue(root, 4.5), 4);
    ctx.expect_eq(sol.closestValueRecursive(root, 4.5), 4);

    // Classic LeetCode example: target = 3.714286 -> 4.
    ctx.expect_eq(sol.closestValue(root, 3.714286), 4);
    ctx.expect_eq(sol.closestValueRecursive(root, 3.714286), 4);

    // Exact match.
    ctx.expect_eq(sol.closestValue(root, 3.0), 3);
    ctx.expect_eq(sol.closestValueRecursive(root, 3.0), 3);

    // Target outside the tree's min/max range.
    ctx.expect_eq(sol.closestValue(root, -100.5), 1);
    ctx.expect_eq(sol.closestValueRecursive(root, -100.5), 1);
    ctx.expect_eq(sol.closestValue(root, 100.5), 5);
    ctx.expect_eq(sol.closestValueRecursive(root, 100.5), 5);
  });

  ctx.test("negative_values_and_tie_break", [&] {
    TreeArena arena;
    //      -5
    //      / \
    //   -10  -2
    TreeNode* root = arena.make(-5, arena.make(-10), arena.make(-2));

    // Midpoint between -5 and -2 is -3.5 -> tie breaks to -5.
    ctx.expect_eq(sol.closestValue(root, -3.5), -5);
    ctx.expect_eq(sol.closestValueRecursive(root, -3.5), -5);

    ctx.expect_eq(sol.closestValue(root, -3.4), -2);
    ctx.expect_eq(sol.closestValueRecursive(root, -3.4), -2);
  });

  ctx.test("single_node_and_null_root", [&] {
    TreeArena arena;
    TreeNode* single = arena.make(1);
    ctx.expect_eq(sol.closestValue(single, 4.428571), 1);
    ctx.expect_eq(sol.closestValueRecursive(single, 4.428571), 1);

    ctx.expect_eq(sol.closestValue(nullptr, 2.5), 0);
    ctx.expect_eq(sol.closestValueRecursive(nullptr, 2.5), 0);
  });
}

}  // namespace
