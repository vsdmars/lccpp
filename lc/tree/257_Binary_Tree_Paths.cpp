// LeetCode 257: Binary Tree Paths
// https://leetcode.com/problems/binary-tree-paths/
//
// Problem:
//   Given the root of a binary tree, return all root-to-leaf paths in any
//   order. Each path is formatted as a string of node values separated by
//   `"->"` (e.g., `"1->2->5"`). A leaf is a node with no left or right child.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 2, 3, null, 5]
//     Output: ["1->2->5", "1->3"]
//     Explanation:
//       - Enter root 1 (path = [1]), recurse left to 2 (path = [1, 2]), skip its
//         null left child, and recurse right to leaf 5 (path = [1, 2, 5]).
//       - At leaf 5, format_path([1, 2, 5]) appends "1->2->5" to result, then pops
//         5 and 2 while returning to 1 (path = [1]).
//       - Recurse right to leaf 3 (path = [1, 3]), append "1->3" to result, and pop
//         3 and 1 to return ["1->2->5", "1->3"].
//
//   Example 2:
//     Input:  root = [-10, 20, -30, 4]
//     Output: ["-10->20->4", "-10->-30"]
//     Explanation:
//       - Push -10 onto path, recurse left to 20 (path = [-10, 20]), and then left
//         to leaf 4 (path = [-10, 20, 4]).
//       - Format and record "-10->20->4" at leaf 4, then backtrack by popping 4 and
//         20 back to path = [-10].
//       - Recurse right into leaf -30 (path = [-10, -30]), record "-10->-30", and
//         pop -30 and -10 to finish.
//
//   Example 3:
//     Input:  root = [7]
//     Output: ["7"]
//     Explanation:
//       - Enter root 7 and push its value onto path (path = [7]).
//       - Both left and right are nullptr, so format_path([7]) appends "7" to
//         result without adding any "->" separator.
//       - Pop 7 from path and return ["7"].
//
// Algorithm:
//   Preorder DFS with backtracking:
//     - Maintain a single mutable `path` stack storing node values from `root`
//       down to the current node.
//     - Upon entering `node`, push `node->val` onto `path`.
//     - If `node` is a leaf (`left == nullptr && right == nullptr`), format the
//       entire `path` joined by `"->"` and append it to `result`.
//     - Otherwise recurse into existing left and right children.
//     - Before returning, pop `node->val` from `path` so sibling subtrees see
//       the exact ancestor prefix.
//   Time Complexity:  O(n * h) worst-case (at most O(n) leaves, each formatting
//                     a path of length up to h; O(n log n) for balanced trees,
//                     O(n^2) for degenerate comb trees).
//   Space Complexity: O(h) auxiliary recursion and path stack (excluding the
//                     output strings).
//
// Data Structures:
//   - `std::vector<int>` backtracking stack holding the active root-to-node
//     value sequence.
//   - `std::vector<std::string>` accumulator for formatted root-to-leaf paths.
//
// Real-World Applications:
//   - Enumerating full hierarchical paths in file systems, XML/JSON schemas,
//     or URL routing tries.
//   - Extracting all decision rules (root-to-leaf conjunctions) from trained
//     decision trees for model explainability and audit logs.
//   - Generating execution-trace summaries across call trees and fault trees.
//
// Related LeetCode Problems:
//   - #112 Path Sum [done] (checking whether any root-to-leaf path hits a target sum)
//   - #113 Path Sum II [done] (collecting all root-to-leaf value lists matching a sum)
//   - #129 Sum Root to Leaf Numbers [done] (aggregating root-to-leaf paths as integers)
//   - #988 Smallest String Starting From Leaf [new] (lexicographic comparison across root-to-leaf paths)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <string>
#include <vector>

namespace {

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Returns all root-to-leaf paths formatted as "v1->v2->...->vk".
  // Time: O(n * h) worst case, Space: O(h) auxiliary stack.
  std::vector<std::string> binaryTreePaths(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    std::vector<std::string> result;
    std::vector<int> path;

    auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
      path.push_back(node->val);

      // Leaf reached: materialize the current root-to-leaf path string.
      if (node->left == nullptr && node->right == nullptr) {
        result.push_back(format_path(path));
        path.pop_back();
        return;
      }

      if (node->left != nullptr) {
        self(node->left);
      }
      if (node->right != nullptr) {
        self(node->right);
      }

      // Backtrack: remove current node before returning to parent.
      path.pop_back();
    };

    dfs(root);
    return result;
  }

 private:
  // Joins a non-empty sequence of node values with "->".
  static std::string format_path(const std::vector<int>& path) {
    std::string formatted;
    for (std::size_t i = 0uz; i < path.size(); ++i) {
      if (i > 0uz) {
        formatted += "->";
      }
      formatted += std::to_string(path[i]);
    }
    return formatted;
  }
};

LC_PROBLEM(257, "Binary Tree Paths", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example", [&] {
    //     1
    //    / \
    //   2   3
    //    \
    //     5
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->right = make(3);
    root->left->right = make(5);

    const std::vector<std::string> expected = {"1->2->5", "1->3"};
    ctx.expect_eq(sol.binaryTreePaths(root), expected);
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(sol.binaryTreePaths(nullptr), std::vector<std::string>{});

    TreeNode single(7);
    ctx.expect_eq(sol.binaryTreePaths(&single), std::vector<std::string>{"7"});
  });

  ctx.test("negative_and_multidigit_values", [&] {
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(-10);
    root->left = make(20);
    root->right = make(-30);
    root->left->left = make(4);

    const std::vector<std::string> expected = {"-10->20->4", "-10->-30"};
    ctx.expect_eq(sol.binaryTreePaths(root), expected);
  });

  ctx.test("full_binary_tree", [&] {
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->right = make(3);
    root->left->left = make(4);
    root->left->right = make(5);
    root->right->left = make(6);
    root->right->right = make(7);

    const std::vector<std::string> expected = {
        "1->2->4", "1->2->5", "1->3->6", "1->3->7"};
    ctx.expect_eq(sol.binaryTreePaths(root), expected);
  });

  ctx.test("right_skewed_chain", [&] {
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->right = make(2);
    root->right->right = make(3);

    const std::vector<std::string> expected = {"1->2->3"};
    ctx.expect_eq(sol.binaryTreePaths(root), expected);
  });
}

}  // namespace
