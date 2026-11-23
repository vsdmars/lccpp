// LeetCode 951: Flip Equivalent Binary Trees
// https://leetcode.com/problems/flip-equivalent-binary-trees/
//
// Problem:
//   A flip operation on a binary tree chooses any node and swaps its left and
//   right child subtrees. Two binary trees `root1` and `root2` are flip
//   equivalent if one can be transformed into the other after zero or more
//   flip operations. Return true if `root1` and `root2` are flip equivalent.
//   Constraints: Each tree has at most 100 nodes, and all node values within
//   each tree are unique integers in [0, 99].
//
// Algorithm:
//   Run simultaneous recursive DFS on `(root1, root2)`:
//     1. If both pointers are null, the empty subtrees match -> return true.
//     2. If exactly one is null, or `root1->val != root2->val`, they cannot be
//        flip equivalent -> return false.
//     3. Otherwise, the two subtrees are flip equivalent iff either:
//        - No flip at this node: `(left1 ~ left2) && (right1 ~ right2)`, OR
//        - Flip at this node: `(left1 ~ right2) && (right1 ~ left2)`.
//   Because every value in each tree is unique, at most one of the two child
//   pairings can pass the root-value check at each level, preventing
//   exponential branching.
//   Time Complexity: O(min(N1, N2)), where `N1` and `N2` are the node counts
//     of the two trees (unique values ensure each node pair is matched O(1)
//     times).
//   Space Complexity: O(min(H1, H2)) for the recursion stack, where `H1` and
//     `H2` are the heights of the two trees.
//
// Data Structures:
//   - Binary trees (`TreeNode` with `val`, `left`, `right`).
//   - Call stack for paired structural recursion.
//   - std::deque<TreeNode> arena in unit tests for leak-free RAII node storage.
//
// Real-World Applications:
//   - Compiler expression equivalence checking for commutative binary operators
//     (e.g., verifying `(a + b) * c` and `c * (b + a)` have equivalent ASTs).
//   - Rooted tree isomorphism testing in chemistry (molecular graphs) and
//     hierarchical XML/JSON structural diffing with unordered child pairs.
//   - Phylogenetic tree comparison in computational biology.
//
// Related LeetCode Problems:
//   - #100 Same Tree [done] (strict structural and value equality without flips)
//   - #101 Symmetric Tree [done] (checking whether left and right subtrees are mirrors)
//   - #226 Invert Binary Tree [done] (flipping every node in a binary tree)
//   - #572 Subtree of Another Tree [done] (recursive structural matching across trees)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  TreeNode() = default;
  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

// RAII arena that owns all test TreeNodes with stable pointer addresses.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(val, left, right);
  }

 private:
  std::deque<TreeNode> nodes_;
};

// Builds a binary tree from LeetCode's level-order array representation.
TreeNode* build_tree(TreeArena& arena,
                     const std::vector<std::optional<int>>& values) {
  if (values.empty() || !values[0].has_value()) {
    return nullptr;
  }

  auto* root = arena.make(*values[0]);
  std::queue<TreeNode*> q;
  q.push(root);

  std::size_t idx = 1uz;
  while (!q.empty() && idx < values.size()) {
    auto* curr = q.front();
    q.pop();

    if (values[idx].has_value()) {
      curr->left = arena.make(*values[idx]);
      q.push(curr->left);
    }
    ++idx;

    if (idx < values.size() && values[idx].has_value()) {
      curr->right = arena.make(*values[idx]);
      q.push(curr->right);
    }
    ++idx;
  }
  return root;
}

class Solution {
 public:
  // Returns true if `root1` and `root2` are flip equivalent via recursive DFS.
  // Time: O(min(N1, N2)) with unique values; Space: O(min(H1, H2)).
  bool flipEquiv(const TreeNode* root1, const TreeNode* root2) {
    if (root1 == nullptr && root2 == nullptr) {
      return true;
    }
    if (root1 == nullptr || root2 == nullptr || root1->val != root2->val) {
      return false;
    }

    // Either keep children in their current order or flip them at this node.
    const bool no_flip = flipEquiv(root1->left, root2->left) &&
                         flipEquiv(root1->right, root2->right);
    if (no_flip) {
      return true;
    }
    return flipEquiv(root1->left, root2->right) &&
           flipEquiv(root1->right, root2->left);
  }
};

LC_PROBLEM(951, "Flip Equivalent Binary Trees", "dfs", ctx) {
  ctx.test("example_1_multiple_flips", [&] {
    TreeArena arena;
    Solution sol;
    auto* r1 = build_tree(
        arena, {1, 2, 3, 4, 5, 6, std::nullopt, std::nullopt, std::nullopt, 7,
                8});
    auto* r2 = build_tree(
        arena, {1, 3, 2, std::nullopt, 6, 4, 5, std::nullopt, std::nullopt,
                std::nullopt, std::nullopt, 8, 7});
    ctx.expect_true(sol.flipEquiv(r1, r2));
  });

  ctx.test("example_2_both_empty", [&] {
    Solution sol;
    ctx.expect_true(sol.flipEquiv(nullptr, nullptr));
  });

  ctx.test("example_3_empty_and_nonempty", [&] {
    TreeArena arena;
    Solution sol;
    auto* r2 = arena.make(1);
    ctx.expect_false(sol.flipEquiv(nullptr, r2));
    ctx.expect_false(sol.flipEquiv(r2, nullptr));
  });

  ctx.test("single_child_flip", [&] {
    TreeArena arena;
    Solution sol;
    auto* r1 = build_tree(arena, {1, 2});
    auto* r2 = build_tree(arena, {1, std::nullopt, 2});
    ctx.expect_true(sol.flipEquiv(r1, r2));
  });

  ctx.test("identical_trees_zero_flips", [&] {
    TreeArena arena;
    Solution sol;
    auto* r1 = build_tree(arena, {1, 2, 3, 4, 5});
    auto* r2 = build_tree(arena, {1, 2, 3, 4, 5});
    ctx.expect_true(sol.flipEquiv(r1, r2));
  });

  ctx.test("non_equivalent_parent_child_mismatch", [&] {
    TreeArena arena;
    Solution sol;
    // Node 4 is under 2 in r1, but under 3 in r2.
    auto* r1 = build_tree(arena, {1, 2, 3, 4});
    auto* r2 = build_tree(arena, {1, 3, 2, 4});
    ctx.expect_false(sol.flipEquiv(r1, r2));
  });
}

}  // namespace
