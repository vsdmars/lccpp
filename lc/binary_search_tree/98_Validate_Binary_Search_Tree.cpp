// LeetCode 98: Validate Binary Search Tree
// https://leetcode.com/problems/validate-binary-search-tree/
//
// Problem:
//   Given the root of a binary tree, determine whether it is a valid binary
//   search tree (BST). A valid BST requires that for every node:
//   - All keys in its left subtree are strictly less than the node's key.
//   - All keys in its right subtree are strictly greater than the node's key.
//   - Both left and right subtrees are themselves valid BSTs.
//   Node values span the full 32-bit signed integer range [INT_MIN, INT_MAX].
//
// Algorithm:
//   Depth-First Search (DFS) with top-down strict value bounds (low, high).
//   Starting from (-infinity, +infinity) represented in 64-bit signed integers
//   so 32-bit boundary values (INT_MIN, INT_MAX) never overflow, each recursive
//   call verifies that `low < node->val < high`. Descending into the left
//   subtree tightens the upper bound to `node->val`, while descending into the
//   right subtree tightens the lower bound to `node->val`. An empty subtree is
//   vacuously valid.
//   - Time Complexity:  O(n), where n is the number of nodes (each node is
//     visited at most once).
//   - Space Complexity: O(h) call-stack space, where h is the tree height
//     (O(log n) for balanced trees, O(n) worst case for skewed trees).
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`) storing an integer value and left/right
//     child pointers.
//   - Implicit recursion stack carrying the valid open interval `(low, high)`
//     for each ancestor path.
//
// Real-World Applications:
//   - Database index integrity checkers (verifying B-tree / B+tree key order
//     invariants during crash recovery or compaction diagnostics).
//   - In-memory ordered map/set (`std::map`, red-black tree, AVL tree) debug
//     assertions and fuzz-test invariant validators.
//   - Interval and spatial partitioning tree verification in computational
//     geometry and physics engines.
//
// Related LeetCode Problems:
//   - #99 Recover Binary Search Tree [done] (repair a BST with two swapped nodes)
//   - #94 Binary Tree Inorder Traversal [done] (inorder sequence of a BST is strictly increasing)
//   - #255 Verify Preorder Scequence In Binary Search Tree [done] (validate BST property from preorder array)
//   - #285 Inorder Successor In BST [done] (exploits the same BST ordering invariant)
//   - #530 Minimum Absolute Difference in BST [done] (adjacent inorder difference property)

#include "framework/lc_framework.h"

#include <cstdint>
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
  // Validates whether `root` satisfies the strict BST ordering property for
  // every node against all of its ancestors.
  // Time: O(n), Space: O(h).
  bool isValidBST(const TreeNode* root) {
    // Recursive top-down DFS maintaining the exclusive valid range (low, high).
    // 64-bit bounds ensure INT_MIN and INT_MAX compare cleanly without wrap.
    auto dfs = [](this auto&& self, const TreeNode* node, std::int64_t low,
                  std::int64_t high) -> bool {
      // Base case: an empty subtree trivially satisfies the BST invariant.
      if (node == nullptr) {
        return true;
      }

      const auto value = static_cast<std::int64_t>(node->val);
      // Every node must fall strictly inside the bounds inherited from all
      // ancestors (not merely its immediate parent).
      if (value <= low || value >= high) {
        return false;
      }

      // Left children must be < value; right children must be > value.
      return self(node->left, low, value) && self(node->right, value, high);
    };

    return dfs(root, std::numeric_limits<std::int64_t>::min(),
               std::numeric_limits<std::int64_t>::max());
  }
};

LC_PROBLEM(98, "Validate Binary Search Tree", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_valid_small_bst", [&] {
    TreeArena arena;
    //     2
    //    / \
    //   1   3
    TreeNode* root = arena.make(2, arena.make(1), arena.make(3));
    ctx.expect_true(sol.isValidBST(root));
  });

  ctx.test("example_2_invalid_left_greater", [&] {
    TreeArena arena;
    //     1
    //    / \
    //   2   3
    TreeNode* root = arena.make(1, arena.make(2), arena.make(3));
    ctx.expect_false(sol.isValidBST(root));
  });

  ctx.test("python_build_valid_tree", [&] {
    TreeArena arena;
    //       5
    //      / \
    //     3   7
    //    / \
    //   2   4
    TreeNode* left = arena.make(3, arena.make(2), arena.make(4));
    TreeNode* root = arena.make(5, left, arena.make(7));
    ctx.expect_true(sol.isValidBST(root));
  });

  ctx.test("ancestor_bound_violation_in_right_subtree", [&] {
    TreeArena arena;
    //       5
    //      / \
    //     4   6
    //        / \
    //       3   7   (3 < 5 violates root's lower bound for right subtree)
    TreeNode* right = arena.make(6, arena.make(3), arena.make(7));
    TreeNode* root = arena.make(5, arena.make(4), right);
    ctx.expect_false(sol.isValidBST(root));
  });

  ctx.test("duplicate_values_are_invalid", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(2, arena.make(2), arena.make(2));
    ctx.expect_false(sol.isValidBST(root));
  });

  ctx.test("int32_min_and_max_boundaries", [&] {
    TreeArena arena;
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();

    ctx.expect_true(sol.isValidBST(arena.make(kMin)));
    ctx.expect_true(sol.isValidBST(arena.make(kMax)));

    TreeNode* valid_pair = arena.make(kMax, arena.make(kMin), nullptr);
    ctx.expect_true(sol.isValidBST(valid_pair));

    TreeNode* dup_min = arena.make(kMin, arena.make(kMin), nullptr);
    ctx.expect_false(sol.isValidBST(dup_min));
  });

  ctx.test("empty_tree_is_valid",
           [&] { ctx.expect_true(sol.isValidBST(nullptr)); });
}

}  // namespace
