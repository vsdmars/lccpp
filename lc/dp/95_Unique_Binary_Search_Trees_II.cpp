// LeetCode 95: Unique Binary Search Trees II
// https://leetcode.com/problems/unique-binary-search-trees-ii/
//
// Problem:
//   Given an integer n (1 <= n <= 8), generate and return all structurally
//   unique binary search trees (BSTs) that store the exact set of values
//   1, 2, ..., n. The trees may be returned in any order.
//
// Algorithm:
//   Top-down memoized interval DP / divide-and-conquer. Every BST over the
//   inclusive value range [left, right] is uniquely determined by choosing a
//   root value r in [left, right], recursively constructing all unique left
//   subtrees over [left, r - 1], and recursively constructing all unique right
//   subtrees over [r + 1, right]. Taking the Cartesian product of left and
//   right subtree lists yields all BSTs rooted at r. An empty range
//   (left > right) yields a single {nullptr} placeholder so skewed trees with
//   missing children are formed naturally. Memoizing each sub-range [left,
//   right] avoids recomputing overlapping subproblems and safely shares
//   immutable subtree nodes owned by a `std::deque<TreeNode>` arena.
//   The number of distinct trees equals the n-th Catalan number C_n.
//   Time Complexity:  O(n * C_n) to construct the root combinations (with
//                     memoized subtree sharing).
//   Space Complexity: O(n * C_n) for the arena nodes and memoized lists.
//
// Data Structures:
//   - TreeNode struct: binary tree node storing val, left, and right pointers.
//   - std::deque<TreeNode> arena_: pointer-stable RAII storage owning every
//     allocated TreeNode so shared subtrees never leak or double-free.
//   - 2D memo table (`memo[left][right]`): caches all generated TreeNode* roots
//     for each inclusive value sub-range [left, right].
//
// Real-World Applications:
//   - Relational database query optimizers enumerating join-order expression
//     trees (Catalan-many parenthesizations / binary join trees).
//   - Compiler AST and expression-tree fuzzing / exhaustive test generation.
//   - Optimal binary search tree (OBST) search-space analysis and phylogenetic
//     tree topology enumeration.
//
// Related LeetCode Problems:
//   - #96 Unique Binary Search Trees [done] (counts C_n without building trees)
//   - #98 Validate Binary Search Tree [done] (verifying the BST invariant)
//   - #894 All Possible Full Binary Trees [new] (same memoized Cartesian DP)
//   - #241 Different Ways to Add Parentheses [done] (Catalan interval DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <queue>
#include <set>
#include <string>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Generates all structurally unique BSTs containing values 1..n.
  // Returned TreeNode* pointers remain valid for the lifetime of this Solution
  // instance (until the next generateTrees call or Solution destruction).
  // Time: O(n * C_n), Space: O(n * C_n).
  std::vector<TreeNode*> generateTrees(int n) {
    arena_.clear();
    if (n <= 0) {
      return {};
    }

    const auto size = static_cast<std::size_t>(n) + 1uz;
    // memo[left][right] stores all unique BST roots for values in [left, right].
    std::vector<std::vector<std::vector<TreeNode*>>> memo(
        size, std::vector<std::vector<TreeNode*>>(size));

    // Sentinel list returned for empty sub-ranges (left > right) so the
    // Cartesian-product loops naturally attach nullptr for absent children.
    const std::vector<TreeNode*> empty_subtree = {nullptr};

    const auto generate = [&](this auto&& self, int left,
                              int right) -> const std::vector<TreeNode*>& {
      if (left > right) {
        return empty_subtree;
      }

      auto& cached = memo[static_cast<std::size_t>(left)]
                         [static_cast<std::size_t>(right)];
      if (!cached.empty()) {
        return cached;
      }

      // Every value in [left, right] (inclusive) can serve as the subtree root,
      // which also covers left-skewed and right-skewed shapes.
      for (int root_val = left; root_val <= right; ++root_val) {
        const auto& left_trees = self(left, root_val - 1);
        const auto& right_trees = self(root_val + 1, right);

        for (TreeNode* left_node : left_trees) {
          for (TreeNode* right_node : right_trees) {
            // std::deque guarantees pointer stability on push_back.
            arena_.push_back(TreeNode{
                .val = root_val, .left = left_node, .right = right_node});
            cached.push_back(&arena_.back());
          }
        }
      }

      return cached;
    };

    return generate(1, n);
  }

 private:
  // Pointer-stable RAII arena owning all nodes created by generateTrees().
  std::deque<TreeNode> arena_;
};

// Serializes a binary tree in LeetCode's trimmed level-order format, e.g.,
// "1,null,2,null,3", so tree structures can be compared deterministically.
std::string serialize_tree(const TreeNode* root) {
  if (root == nullptr) {
    return "";
  }

  std::vector<std::string> tokens;
  std::queue<const TreeNode*> q;
  q.push(root);

  while (!q.empty()) {
    const TreeNode* node = q.front();
    q.pop();
    if (node == nullptr) {
      tokens.emplace_back("null");
    } else {
      tokens.push_back(std::to_string(node->val));
      q.push(node->left);
      q.push(node->right);
    }
  }

  while (!tokens.empty() && tokens.back() == "null") {
    tokens.pop_back();
  }

  std::string out;
  for (std::size_t i = 0uz; i < tokens.size(); ++i) {
    if (i > 0uz) {
      out.push_back(',');
    }
    out += tokens[i];
  }
  return out;
}

// Performs iterative in-order traversal (mirroring Python's pp helper) to
// verify the BST ordering invariant: every valid tree yields [1, 2, ..., n].
std::vector<int> inorder_traversal(const TreeNode* root) {
  std::vector<int> result;
  std::vector<const TreeNode*> stack;
  const TreeNode* cur = root;

  while (cur != nullptr || !stack.empty()) {
    while (cur != nullptr) {
      stack.push_back(cur);
      cur = cur->left;
    }
    cur = stack.back();
    stack.pop_back();
    result.push_back(cur->val);
    cur = cur->right;
  }

  return result;
}

LC_PROBLEM(95, "Unique Binary Search Trees II", "dp", ctx) {
  ctx.test("example_1_n_equals_3", [&] {
    Solution sol;
    const auto trees = sol.generateTrees(3);
    ctx.expect_eq(trees.size(), 5uz);

    std::vector<std::string> actual;
    actual.reserve(trees.size());
    const std::vector<int> expected_inorder = {1, 2, 3};

    for (const TreeNode* root : trees) {
      ctx.expect_eq(inorder_traversal(root), expected_inorder);
      actual.push_back(serialize_tree(root));
    }
    std::ranges::sort(actual);

    std::vector<std::string> expected = {
        "1,null,2,null,3", "1,null,3,2", "2,1,3",
        "3,1,null,null,2", "3,2,null,1"};
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_2_n_equals_1", [&] {
    Solution sol;
    const auto trees = sol.generateTrees(1);
    ctx.expect_eq(trees.size(), 1uz);
    ctx.expect_eq(serialize_tree(trees[0]), std::string("1"));
  });

  ctx.test("n_equals_2", [&] {
    Solution sol;
    const auto trees = sol.generateTrees(2);
    ctx.expect_eq(trees.size(), 2uz);

    std::vector<std::string> actual = {serialize_tree(trees[0]),
                                       serialize_tree(trees[1])};
    std::ranges::sort(actual);
    const std::vector<std::string> expected = {"1,null,2", "2,1"};
    ctx.expect_eq(actual, expected);
  });

  ctx.test("catalan_counts_and_uniqueness_for_n_4_and_5", [&] {
    Solution sol;

    // C_4 = 14 unique trees, all strictly distinct and valid BSTs.
    const auto trees4 = sol.generateTrees(4);
    ctx.expect_eq(trees4.size(), 14uz);
    std::set<std::string> unique4;
    const std::vector<int> expected4 = {1, 2, 3, 4};
    for (const TreeNode* root : trees4) {
      ctx.expect_eq(inorder_traversal(root), expected4);
      unique4.insert(serialize_tree(root));
    }
    ctx.expect_eq(unique4.size(), 14uz);

    // C_5 = 42 unique trees.
    const auto trees5 = sol.generateTrees(5);
    ctx.expect_eq(trees5.size(), 42uz);
  });

  ctx.test("max_constraint_n_equals_8", [&] {
    Solution sol;
    // C_8 = 1430 unique trees.
    const auto trees8 = sol.generateTrees(8);
    ctx.expect_eq(trees8.size(), 1430uz);
  });
}

}  // namespace
