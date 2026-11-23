// LeetCode 536: Construct Binary Tree From String
// https://leetcode.com/problems/construct-binary-tree-from-string/
//
// Problem:
//   Construct a binary tree from a string consisting of integers (including
//   negative values) and parentheses. A subtree is formatted as an integer
//   followed by zero, one, or two parenthesized child subtrees:
//   `val(left)(right)`. The left child is always constructed before the right
//   child when present, and an empty string represents an empty tree.
//
// Algorithm:
//   Scan the string left-to-right using an explicit ancestor stack:
//     - When a signed integer token is encountered, parse its value, allocate
//       a new `TreeNode`, and push it onto the stack.
//     - When `'('` is encountered, advance past it (the upcoming subtree will
//       become a child of the current stack top).
//     - When `')'` is encountered, the node on top of the stack has finished
//       constructing its own subtree: pop it and attach it to the new stack
//       top's `left` pointer if empty, otherwise to `right`.
//   After scanning the entire string, the single remaining node on the stack
//   is the root of the binary tree.
//   Time Complexity:  O(n), where n is the length of `s` (single linear pass).
//   Space Complexity: O(h) for the explicit stack (where h is the tree height,
//                     at most n) plus O(n) for storing the constructed nodes.
//
// Data Structures:
//   - Ancestor stack (`std::vector<TreeNode*>`): tracks currently open parent
//     nodes whose parenthesized child subtrees have not yet closed.
//   - Node arena (`std::deque<TreeNode>`): RAII owner providing pointer-stable
//     allocation for all constructed `TreeNode` objects with zero leaks.
//
// Real-World Applications:
//   - Parsing Newick phylogenetic tree strings in bioinformatics pipelines.
//   - Deserializing S-expressions, Lisp ASTs, and parenthesized parse trees in
//     compilers and NLP constituency parsers.
//   - Reconstructing hierarchical UI / DOM or scene-graph snapshots from
//     compact text representations.
//
// Related LeetCode Problems:
//   - #606 Construct String from Binary Tree [done] (inverse serialization task)
//   - #297 Serialize and Deserialize Binary Tree [done] (general tree codec)
//   - #1028 Recover a Tree From Preorder Traversal [new] (stack-based parsing)
//   - #449 Serialize and Deserialize BST [done] (compact tree encoding)
//   - #385 Mini Parser [new] (stack-based nested structure parsing)

#include "framework/lc_framework.h"

#include <charconv>
#include <cstddef>
#include <deque>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
};

class Solution {
 public:
  // Parses a parenthesized binary-tree encoding into a linked `TreeNode` tree.
  // Nodes are owned by `arena_` for the lifetime of this `Solution` instance.
  // Time: O(n), Space: O(n).
  TreeNode* str2tree(std::string_view s) {
    if (s.empty()) {
      return nullptr;
    }

    // Stack invariant: `stack.back()` is always the most recently opened node
    // whose parenthesized subtree has not yet been closed by `')'`.
    std::vector<TreeNode*> stack;
    std::size_t idx = 0uz;
    const std::size_t n = s.size();

    while (idx < n) {
      if (s[idx] == ')') {
        // Complete the child subtree at stack top and wire it to its parent.
        if (stack.size() >= 2uz) {
          TreeNode* child = stack.back();
          stack.pop_back();
          TreeNode* parent = stack.back();
          if (parent->left == nullptr) {
            parent->left = child;
          } else {
            parent->right = child;
          }
        }
        ++idx;
      } else if (s[idx] == '(') {
        // Opening parenthesis simply introduces the next child node.
        ++idx;
      } else {
        // Signed integer token: construct a node in the pointer-stable arena.
        const int value = parse_int(s, idx);
        arena_.emplace_back(value);
        stack.push_back(&arena_.back());
      }
    }

    return stack.empty() ? nullptr : stack.front();
  }

 private:
  // Parses a signed decimal integer starting at `s[idx]` and advances `idx`
  // past the last digit consumed.
  static int parse_int(std::string_view s, std::size_t& idx) {
    const std::size_t start = idx;
    if (idx < s.size() && s[idx] == '-') {
      ++idx;
    }
    while (idx < s.size() && s[idx] >= '0' && s[idx] <= '9') {
      ++idx;
    }

    int value = 0;
    const auto res = std::from_chars(s.data() + start, s.data() + idx, value);
    if (res.ec != std::errc{}) {
      return 0;
    }
    return value;
  }

  // `std::deque` guarantees references/pointers remain valid across pushes.
  std::deque<TreeNode> arena_;
};

// Serializes a binary tree back into canonical parenthesized form (always
// distinguishing left vs. right children) for exact structural test assertions.
std::string serialize_tree(const TreeNode* root) {
  if (root == nullptr) {
    return "";
  }
  std::string out = std::to_string(root->val);
  if (root->left != nullptr || root->right != nullptr) {
    out += '(';
    out += serialize_tree(root->left);
    out += ')';
  }
  if (root->right != nullptr) {
    out += '(';
    out += serialize_tree(root->right);
    out += ')';
  }
  return out;
}

LC_PROBLEM(536, "Construct Binary Tree From String", "stack", ctx) {
  ctx.test("example_balanced_tree", [&] {
    Solution sol;
    const TreeNode* root = sol.str2tree("4(2(3)(1))(6(5))");
    ctx.expect_eq(serialize_tree(root), std::string("4(2(3)(1))(6(5))"));
    ctx.expect_true(root != nullptr);
    if (root != nullptr) {
      ctx.expect_eq(root->val, 4);
      ctx.expect_true(root->left != nullptr);
      ctx.expect_true(root->right != nullptr);
      if (root->left != nullptr) {
        ctx.expect_eq(root->left->val, 2);
      }
      if (root->right != nullptr) {
        ctx.expect_eq(root->right->val, 6);
        ctx.expect_true(root->right->left != nullptr);
        if (root->right->left != nullptr) {
          ctx.expect_eq(root->right->left->val, 5);
        }
        ctx.expect_true(root->right->right == nullptr);
      }
    }
  });

  ctx.test("negative_root_full_tree", [&] {
    Solution sol;
    const TreeNode* root = sol.str2tree("-4(2(3)(1))(6(5)(7))");
    ctx.expect_eq(serialize_tree(root), std::string("-4(2(3)(1))(6(5)(7))"));
  });

  ctx.test("multi_digit_and_negative_children", [&] {
    Solution sol;
    const TreeNode* root = sol.str2tree("51(232)(-434)");
    ctx.expect_eq(serialize_tree(root), std::string("51(232)(-434)"));
  });

  ctx.test("single_node_without_parens", [&] {
    Solution sol;
    const TreeNode* root = sol.str2tree("4");
    ctx.expect_true(root != nullptr);
    if (root != nullptr) {
      ctx.expect_eq(root->val, 4);
      ctx.expect_true(root->left == nullptr);
      ctx.expect_true(root->right == nullptr);
    }
  });

  ctx.test("empty_string_returns_null", [&] {
    Solution sol;
    ctx.expect_true(sol.str2tree("") == nullptr);
  });

  ctx.test("left_skewed_chain", [&] {
    Solution sol;
    const TreeNode* root = sol.str2tree("1(2(3(4)))");
    ctx.expect_eq(serialize_tree(root), std::string("1(2(3(4)))"));
  });
}

}  // namespace
