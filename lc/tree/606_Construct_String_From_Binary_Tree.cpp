// LeetCode 606: Construct String From Binary Tree
// https://leetcode.com/problems/construct-string-from-binary-tree/
//
// Problem:
//   Given the root of a binary tree, serialize it into a string of integers
//   and parentheses using preorder traversal. Each non-null child subtree is
//   wrapped in parentheses, and a null left child must be represented as an
//   empty pair "()" whenever a right child is present so the mapping between
//   tree structure and string remains unambiguous (one-to-one). All other
//   redundant empty parenthesis pairs must be omitted.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 2, 3, 4]
//     Output: "1(2(4))(3)"
//     Explanation:
//       - Start preorder DFS at node 1: append "1", see it is not a leaf, emit '(',
//         and recurse on left child 2.
//       - At node 2, append "2", emit '(', recurse on left child 4 (a leaf, which
//         appends "4" and returns immediately), then emit ')'.
//       - Because node 2 has right == nullptr, omit the right parenthesis group and
//         return to node 1, closing its left subtree with ')'.
//       - Since node 1 has right child 3, emit '(', visit leaf 3 to append "3", and
//         emit ')' to produce "1(2(4))(3)".
//
//   Example 2:
//     Input:  root = [1, 2, 3, null, 4]
//     Output: "1(2()(4))(3)"
//     Explanation:
//       - Start at root 1: append "1", emit '(', and recurse into left child 2.
//       - At node 2, append "2"; because 2 is not a leaf (right child 4 exists),
//         always emit '(' then recurse on left == nullptr (no output) then emit
//         ')', yielding the required "()" marker.
//       - Still at node 2, since right child 4 is non-null, emit '(', visit leaf 4
//         to append "4", and close with ')'.
//       - Return to node 1, close its left subtree with ')', then wrap right leaf 3
//         in "(3)" to obtain "1(2()(4))(3)".
//
//   Example 3:
//     Input:  root = [1, null, 2, null, 3]
//     Output: "1()(2()(3))"
//     Explanation:
//       - At node 1, append "1"; since right child 2 exists, emit '(' and recurse
//         on left == nullptr, closing immediately with ')' to form "1()".
//       - Still at node 1, emit '(', recurse into right child 2, append "2", and
//         emit "()" for node 2's null left child.
//       - From node 2, emit '(', visit leaf 3 (which appends "3" and returns), and
//         close both open right-subtree parentheses with "))" to form
//         "1()(2()(3))".
//
// Algorithm:
//   Perform a preorder DFS that appends directly into a single output buffer
//   `std::string`:
//   1. Append `node->val`.
//   2. If `node` is a leaf (`left == nullptr && right == nullptr`), return
//      immediately—both empty pairs are redundant and omitted.
//   3. Otherwise, at least one child exists. Always emit `'('`, recurse on
//      `node->left`, and emit `')'`. If `node->left == nullptr`, this produces
//      the required `"()"` placeholder that distinguishes the subsequent right
//      subtree from a left subtree.
//   4. Only if `node->right != nullptr`, emit `'('`, recurse on `node->right`,
//      and emit `')'`. Trailing empty right parentheses are always omitted.
//   Time Complexity: O(n), visiting each node once and appending its digits
//   and up to 2 parentheses amortized in O(1) per character.
//   Space Complexity: O(n) for the output string plus O(h) recursion stack
//   space (h = tree height).
//
// Data Structures:
//   - Binary tree (`TreeNode`).
//   - Mutable string buffer (`std::string`) built incrementally during
//     preorder DFS.
//
// Real-World Applications:
//   - Compact human-readable tree serialization formats (Newick phylogenetic
//     tree notation, S-expressions, and AST pretty-printers).
//   - Lossless tree fingerprinting for structural hashing and caching.
//   - Debugging and logging hierarchical expression trees in compilers.
//
// Related LeetCode Problems:
//   - #536 Construct Binary Tree from String [done] (inverse deserialization problem)
//   - #297 Serialize and Deserialize Binary Tree [done] (general tree codec)
//   - #449 Serialize and Deserialize BST [done] (compact BST-specific serialization)
//   - #652 Find Duplicate Subtrees [done] (uses canonical tree serialization as map keys)

#include "framework/lc_framework.h"

#include <deque>
#include <string>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Serializes `root` to its minimal unambiguous parenthesized preorder string.
  // Time: O(n), Space: O(n).
  std::string tree2str(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    std::string result;

    // Preorder DFS appending directly into `result` to avoid intermediate
    // string allocations and concatenations.
    auto serialize = [&](this auto&& self, const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }

      result += std::to_string(node->val);

      // Leaf node: omit both empty parenthesis pairs.
      if (node->left == nullptr && node->right == nullptr) {
        return;
      }

      // Emit left subtree parentheses whenever either child exists; if left is
      // null and right is non-null, this outputs the mandatory "()" marker.
      result.push_back('(');
      self(node->left);
      result.push_back(')');

      // Emit right subtree parentheses only when a right child actually exists.
      if (node->right != nullptr) {
        result.push_back('(');
        self(node->right);
        result.push_back(')');
      }
    };

    serialize(root);
    return result;
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

LC_PROBLEM(606, "Construct String From Binary Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_left_child_only_omits_right_parens", [&] {
    //        1
    //      /   \
    //     2     3
    //    /
    //   4
    TreeArena arena;
    TreeNode* n2 = arena.make(2, arena.make(4), nullptr);
    TreeNode* root = arena.make(1, n2, arena.make(3));
    ctx.expect_eq(sol.tree2str(root), std::string("1(2(4))(3)"));
  });

  ctx.test("example_2_right_child_keeps_empty_left_parens", [&] {
    //        1
    //      /   \
    //     2     3
    //      \
    //       4
    TreeArena arena;
    TreeNode* n2 = arena.make(2, nullptr, arena.make(4));
    TreeNode* root = arena.make(1, n2, arena.make(3));
    ctx.expect_eq(sol.tree2str(root), std::string("1(2()(4))(3)"));
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.tree2str(arena.make(42)), std::string("42"));
    ctx.expect_eq(sol.tree2str(nullptr), std::string{});
  });

  ctx.test("negative_values", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(-1, arena.make(-2), arena.make(-3));
    ctx.expect_eq(sol.tree2str(root), std::string("-1(-2)(-3)"));
  });

  ctx.test("right_skewed_and_left_skewed_chains", [&] {
    TreeArena arena;
    TreeNode* right_skewed =
        arena.make(1, nullptr, arena.make(2, nullptr, arena.make(3)));
    ctx.expect_eq(sol.tree2str(right_skewed), std::string("1()(2()(3))"));

    TreeNode* left_skewed =
        arena.make(1, arena.make(2, arena.make(3), nullptr), nullptr);
    ctx.expect_eq(sol.tree2str(left_skewed), std::string("1(2(3))"));
  });
}

}  // namespace
