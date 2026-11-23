// LeetCode 108: Convert Sorted Array To Binary Search Tree
// https://leetcode.com/problems/convert-sorted-array-to-binary-search-tree/
//
// Problem:
//   Given an integer array `nums` sorted in strictly ascending order, convert
//   it into a height-balanced Binary Search Tree (BST), where the depths of
//   the two subtrees of every node never differ by more than 1.
//
// Algorithm:
//   Divide-and-conquer midpoint selection over half-open index ranges
//   `[left, right)`. Because `nums` is sorted:
//   1. Selecting `mid = left + ((right - left) / 2)` as the root guarantees
//      all elements in `[left, mid)` are strictly smaller (left subtree) and
//      all elements in `[mid + 1, right)` are strictly larger (right subtree).
//   2. The two sub-ranges have sizes differing by at most 1, so by induction
//      every node's left and right subtree heights differ by at most 1.
//   3. Passing index boundaries instead of slicing sub-arrays avoids O(N log N)
//      copying overhead.
//   - Time Complexity: O(N), where N = `nums.size()` (each element becomes a
//     tree node in O(1) work).
//   - Space Complexity: O(log N) auxiliary recursion stack depth (guaranteed
//     logarithmic because the tree is balanced), plus O(N) for the returned
//     tree nodes.
//
// Data Structures:
//   - Half-open `std::size_t` slice bounds `[left, right)` indexing directly
//     into the input `std::vector<int>`.
//   - Dynamically allocated `TreeNode` BST owned in tests via RAII
//     `std::unique_ptr<TreeNode, TreeDeleter>`.
//
// Real-World Applications:
//   - Bulk-loading balanced search trees and static read-optimized B-tree /
//     BST indexes from sorted datasets (e.g., SSTable index construction in
//     LSM-tree databases).
//   - Periodic offline rebalancing of skewed binary search trees by flattening
//     via inorder traversal and rebuilding from the sorted array.
//   - Building balanced spatial partitioning trees (1D k-d trees / interval
//     trees) for O(log N) range queries.
//
// Related LeetCode Problems:
//   - #109 Convert Sorted List to Binary Search Tree [done] (same balanced BST construction from a singly linked list)
//   - #110 Balanced Binary Tree [done] (verifies the height-balance invariant produced here)
//   - #98 Validate Binary Search Tree [done] (verifies the BST ordering invariant produced here)
//   - #1382 Balance a Binary Search Tree [done] (rebalances an arbitrary BST via inorder flatten + #108 rebuild)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
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
  // Converts a strictly increasing array into a height-balanced BST by
  // recursively choosing the upper-middle element of each half-open range.
  // Caller takes ownership of the returned tree nodes.
  // Time: O(N), Space: O(log N) stack + O(N) output nodes.
  TreeNode* sortedArrayToBST(const std::vector<int>& nums) {
    auto build = [&](this auto&& self, std::size_t left,
                     std::size_t right) -> TreeNode* {
      if (left >= right) {
        return nullptr;
      }

      // Overflow-safe midpoint matching Python's `len // 2` split convention:
      // splits `[left, right)` into two halves differing in size by at most 1.
      const std::size_t mid = left + ((right - left) / 2uz);
      auto* root = new TreeNode(nums[mid]);
      root->left = self(left, mid);
      root->right = self(mid + 1uz, right);
      return root;
    };

    return build(0uz, nums.size());
  }
};

// Stateless RAII deleter for dynamically allocated TreeNode trees in tests.
struct TreeDeleter {
  void operator()(TreeNode* node) const noexcept { destroy(node); }

  static void destroy(TreeNode* node) noexcept {
    if (node == nullptr) {
      return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
  }
};

using OwnedTree = std::unique_ptr<TreeNode, TreeDeleter>;

// Collects inorder traversal; for a valid BST built from a strictly sorted
// array, this must exactly equal the original array.
std::vector<int> collect_inorder(const TreeNode* root) {
  std::vector<int> out;
  auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
    if (node == nullptr) {
      return;
    }
    self(node->left);
    out.push_back(node->val);
    self(node->right);
  };
  dfs(root);
  return out;
}

// Verifies the height-balanced invariant in O(N) bottom-up DFS.
bool is_height_balanced(const TreeNode* root) {
  auto height_or_unbalanced = [&](this auto&& self,
                                  const TreeNode* node) -> int {
    if (node == nullptr) {
      return 0;
    }
    const int left_h = self(node->left);
    if (left_h == -1) {
      return -1;
    }
    const int right_h = self(node->right);
    if (right_h == -1 || std::abs(left_h - right_h) > 1) {
      return -1;
    }
    return 1 + std::max(left_h, right_h);
  };
  return height_or_unbalanced(root) != -1;
}

LC_PROBLEM(108, "Convert Sorted Array To Binary Search Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_and_python_build", [&] {
    //       0
    //      / \
    //    -3   9
    //    /   /
    //  -10  5
    const std::vector<int> nums = {-10, -3, 0, 5, 9};
    const OwnedTree root(sol.sortedArrayToBST(nums));

    ctx.expect_true(root != nullptr);
    ctx.expect_true(is_height_balanced(root.get()));
    ctx.expect_eq(collect_inorder(root.get()), nums);
    ctx.expect_eq(root->val, 0);
    ctx.expect_true(root->left != nullptr && root->left->val == -3);
    ctx.expect_true(root->right != nullptr && root->right->val == 9);
  });

  ctx.test("two_elements", [&] {
    const std::vector<int> nums = {1, 3};
    const OwnedTree root(sol.sortedArrayToBST(nums));

    ctx.expect_true(root != nullptr);
    ctx.expect_true(is_height_balanced(root.get()));
    ctx.expect_eq(collect_inorder(root.get()), nums);
  });

  ctx.test("empty_array", [&] {
    const OwnedTree root(sol.sortedArrayToBST({}));
    ctx.expect_true(root == nullptr);
  });

  ctx.test("single_element", [&] {
    const std::vector<int> nums = {7};
    const OwnedTree root(sol.sortedArrayToBST(nums));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(root->val, 7);
    ctx.expect_true(root->left == nullptr);
    ctx.expect_true(root->right == nullptr);
    ctx.expect_true(is_height_balanced(root.get()));
  });

  ctx.test("larger_sorted_array", [&] {
    const std::vector<int> nums = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
    const OwnedTree root(sol.sortedArrayToBST(nums));

    ctx.expect_true(root != nullptr);
    ctx.expect_true(is_height_balanced(root.get()));
    ctx.expect_eq(collect_inorder(root.get()), nums);
  });
}

}  // namespace
