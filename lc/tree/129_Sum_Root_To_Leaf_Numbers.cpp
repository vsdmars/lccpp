// LeetCode 129: Sum Root To Leaf Numbers
// https://leetcode.com/problems/sum-root-to-leaf-numbers/
//
// Problem:
//   Given the root of a binary tree where every node contains a single digit
//   from 0 to 9, each root-to-leaf path represents a base-10 integer formed by
//   concatenating the digits along the path (e.g., 1 -> 2 -> 3 represents 123).
//   Return the total sum of all root-to-leaf numbers. The test cases guarantee
//   the answer fits within a 32-bit signed integer.
//
// Algorithm:
//   Use a top-down depth-first search (DFS) that carries the accumulated
//   base-10 prefix value down the recursion:
//     - On entering a node with prefix `current_num`, shift left by one decimal
//       digit and append the current digit: `next_num = (current_num * 10) +
//       node->val`.
//     - If `node` is a leaf (both children are nullptr), return `next_num`.
//     - Otherwise, recursively sum the results from any non-null left and right
//       children.
//   Updating the numeric value incrementally during descent avoids maintaining
//   a digit stack and re-multiplying digits at every leaf, reducing time from
//   O(N * H) to strict O(N).
//   Time Complexity:  O(N), where N is the number of nodes in the tree.
//   Space Complexity: O(H), where H is the height of the tree for the recursion
//                     call stack (O(log N) balanced, O(N) skewed).
//
// Data Structures:
//   - Binary tree (`TreeNode`) storing single-digit node values.
//   - Implicit call stack passing the running base-10 integer prefix.
//
// Real-World Applications:
//   - Prefix-tree (trie) numeric key decoding and aggregation (e.g., telephone
//     routing prefixes or hierarchical account/code trees).
//   - Base-radix serialization and checksum aggregation over decision trees.
//   - Path-encoded identifier evaluation in compact binary/decimal tries.
//
// Related LeetCode Problems:
//   - #112 Path Sum [done] (root-to-leaf DFS checking additive path sum)
//   - #113 Path Sum II [done] (root-to-leaf DFS collecting all paths matching a sum)
//   - #1022 Sum of Root To Leaf Binary Numbers [new] (identical DFS accumulating base-2 integers)
//   - #988 Smallest String Starting From Leaf [new] (root-to-leaf symbol path construction)
//   - #124 Binary Tree Maximum Path Sum [done] (maximizes sum across arbitrary tree paths)

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
  // Computes the sum of all base-10 numbers formed by root-to-leaf paths.
  //
  // Invariant: on entry to dfs(node, current_num), `current_num` equals the
  // base-10 integer formed by all strict ancestors of `node` from the root.
  // Time: O(N), Space: O(H).
  int sumNumbers(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    auto dfs = [](this auto&& self, const TreeNode* node,
                  int current_num) -> int {
      const int next_num = (current_num * 10) + node->val;

      // A leaf completes one full root-to-leaf integer.
      if (node->left == nullptr && node->right == nullptr) {
        return next_num;
      }

      int total = 0;
      if (node->left != nullptr) {
        total += self(node->left, next_num);
      }
      if (node->right != nullptr) {
        total += self(node->right, next_num);
      }
      return total;
    };

    return dfs(root, 0);
  }
};

LC_PROBLEM(129, "Sum Root To Leaf Numbers", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 ([1,2,3] -> 25)", [&] {
    //    1
    //   / \
    //  2   3
    // 12 + 13 = 25
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), arena.make(3));
    ctx.expect_eq(sol.sumNumbers(root), 25);
  });

  ctx.test("leetcode example 2 and python build() ([4,9,0,5,1] -> 1026)", [&] {
    //      4
    //     / \
    //    9   0
    //   / \
    //  5   1
    // 495 + 491 + 40 = 1026
    TreeArena arena;
    TreeNode* root =
        arena.make(4, arena.make(9, arena.make(5), arena.make(1)),
                   arena.make(0));
    ctx.expect_eq(sol.sumNumbers(root), 1026);
  });

  ctx.test("empty tree returns 0", [&] {
    ctx.expect_eq(sol.sumNumbers(nullptr), 0);
  });

  ctx.test("single node trees", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.sumNumbers(arena.make(7)), 7);
    ctx.expect_eq(sol.sumNumbers(arena.make(0)), 0);
  });

  ctx.test("leading zero at root", [&] {
    //     0
    //    / \
    //   1   3
    //  /
    // 2
    // 012 + 03 = 15
    TreeArena arena;
    TreeNode* root =
        arena.make(0, arena.make(1, arena.make(2)), arena.make(3));
    ctx.expect_eq(sol.sumNumbers(root), 15);
  });

  ctx.test("single skewed chain", [&] {
    // 1 -> 2 -> 3 -> 4 => 1234
    TreeArena arena;
    TreeNode* root = arena.make(
        1, nullptr,
        arena.make(2, nullptr, arena.make(3, nullptr, arena.make(4))));
    ctx.expect_eq(sol.sumNumbers(root), 1234);
  });
}

}  // namespace
