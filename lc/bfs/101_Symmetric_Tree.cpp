// LeetCode 101: Symmetric Tree
// https://leetcode.com/problems/symmetric-tree/
//
// Problem:
//   Given the root of a binary tree, determine whether the tree is a mirror of
//   itself (i.e., symmetric around its vertical center axis). An empty tree or
//   a single-node tree is trivially symmetric. Node counts range from 0 to
//   1,000, with node values in [-100, 100].
//
// Examples:
//   Example 1:
//     Input:  root = [1, 2, 2, 3, 4, 4, 3]
//     Output: true
//     Explanation:
//       - Verify root node 1 is non-null and seed the BFS queue with the mirrored
//         pair (left=2, right=2).
//       - Pop (2, 2), confirm their values match, and enqueue the outer pair (3, 3)
//         and inner pair (4, 4).
//       - Pop (3, 3) and (4, 4), confirm equal values for both pairs, and enqueue
//         their four (nullptr, nullptr) leaf-child pairs.
//       - Pop the four (nullptr, nullptr) pairs via continue until the queue
//         empties, then return true.
//
//   Example 2:
//     Input:  root = [1, 2, 2, null, 3, null, 3]
//     Output: false
//     Explanation:
//       - Seed the BFS queue with (root->left=2, root->right=2) below root node 1.
//       - Pop (2, 2), see matching values, and enqueue the outer pair (nullptr, 3)
//         and inner pair (3, nullptr).
//       - Pop (nullptr, 3): left is null while right has value 3, so mirror
//         symmetry fails and the function immediately returns false.
//
//   Example 3:
//     Input:  root = [1, 2, 2, null, 3, 3, null, 4, 5, 5, 4]
//     Output: true
//     Explanation:
//       - Seed the queue with (2, 2), pop it, and enqueue the outer pair (nullptr,
//         nullptr) and inner pair (3, 3).
//       - Skip (nullptr, nullptr) via continue, then pop (3, 3) and enqueue outer
//         (4, 4) and inner (5, 5).
//       - Pop (4, 4) and (5, 5), confirm equal values, and skip their four
//         (nullptr, nullptr) child pairs until the queue drains and returns true.
//
// Algorithm:
//   We provide two BFS approaches:
//   1. Mirrored-pair BFS (`isSymmetric`): Seed a FIFO queue with the pair
//      `(root->left, root->right)`. Repeatedly dequeue `(left, right)`: if both
//      are null, continue; if only one is null or their values differ, return
//      false immediately. Otherwise enqueue the outer mirror pair
//      `(left->left, right->right)` and the inner mirror pair
//      `(left->right, right->left)`. If the queue drains without mismatch, the
//      tree is symmetric.
//   2. Level-snapshot BFS (`isSymmetricLevelOrder`): Process one full tree
//      level at a time, preserving null placeholders for children of non-null
//      nodes, and verify each level is a palindrome (`level[i]` mirrors
//      `level[n - 1 - i]`) before expanding the next level.
//   Time Complexity:  O(N), where N is the number of nodes in the tree.
//   Space Complexity: O(W) <= O(N), where W is the maximum width of the tree.
//
// Data Structures:
//   - `std::queue<std::pair<const TreeNode*, const TreeNode*>>`: Stores
//     corresponding mirrored node pairs awaiting structural and value checks.
//   - `std::vector<const TreeNode*>`: Holds a full level snapshot (including
//     null children) for palindrome verification in the level-order variant.
//
// Real-World Applications:
//   - Structural equivalence and canonicalization checks in compiler ASTs and
//     algebraic expression trees (e.g., detecting symmetric subexpressions).
//   - Molecular and crystal-lattice symmetry verification in computational
//     chemistry when represented as rooted hierarchical graphs.
//   - Automated layout and UI hierarchy validation where mirrored panes must
//     preserve identical component structure.
//
// Related LeetCode Problems:
//   - #100 Same Tree [done] (identical pairwise comparison without mirroring)
//   - #226 Invert Binary Tree [done] (mirrors a binary tree in place)
//   - #104 Maximum Depth of Binary Tree [done] (level-order tree traversal)
//   - #572 Subtree of Another Tree [done] (structural tree matching)
//   - #951 Flip Equivalent Binary Trees [done] (generalizes mirror symmetry)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// RAII arena that owns all TreeNode instances for a test case without leaks.
// `std::deque` guarantees pointer stability across `emplace_back`.
TreeNode* build_tree(std::deque<TreeNode>& arena,
                     const std::vector<std::optional<int>>& level_order) {
  if (level_order.empty() || !level_order[0].has_value()) {
    return nullptr;
  }

  arena.emplace_back(*level_order[0]);
  TreeNode* root = &arena.back();
  std::queue<TreeNode*> q;
  q.push(root);

  std::size_t idx = 1uz;
  while (!q.empty() && idx < level_order.size()) {
    TreeNode* curr = q.front();
    q.pop();

    if (level_order[idx].has_value()) {
      arena.emplace_back(*level_order[idx]);
      curr->left = &arena.back();
      q.push(curr->left);
    }
    ++idx;

    if (idx < level_order.size() && level_order[idx].has_value()) {
      arena.emplace_back(*level_order[idx]);
      curr->right = &arena.back();
      q.push(curr->right);
    }
    ++idx;
  }
  return root;
}

class Solution {
 public:
  // Checks whether `root` is mirror-symmetric using mirrored-pair BFS.
  // Invariant: Every pair `(a, b)` in the queue occupies mirrored positions
  // across the tree's center axis.
  // Time: O(N), Space: O(N).
  bool isSymmetric(const TreeNode* root) {
    // An empty tree is vacuously symmetric around its center.
    if (root == nullptr) {
      return true;
    }

    std::queue<std::pair<const TreeNode*, const TreeNode*>> q;
    q.emplace(root->left, root->right);

    while (!q.empty()) {
      const auto [left, right] = q.front();
      q.pop();

      // Two matching null leaves are symmetric; no children to enqueue.
      if (left == nullptr && right == nullptr) {
        continue;
      }
      // Structural mismatch (one null) or differing values breaks symmetry.
      if (left == nullptr || right == nullptr || left->val != right->val) {
        return false;
      }

      // Outer children mirror each other, and inner children mirror each other.
      q.emplace(left->left, right->right);
      q.emplace(left->right, right->left);
    }

    return true;
  }

  // Checks symmetry level-by-level by verifying each BFS level (including null
  // placeholders) is a palindrome. Mirrors the Python level-array solution.
  // Time: O(N), Space: O(N).
  bool isSymmetricLevelOrder(const TreeNode* root) {
    if (root == nullptr) {
      return true;
    }

    std::vector<const TreeNode*> level = {root};
    while (!level.empty()) {
      if (!is_level_palindrome(level)) {
        return false;
      }

      // Expand non-null nodes in left-to-right order, keeping null children so
      // missing branches remain aligned with their mirror counterparts.
      std::vector<const TreeNode*> next_level;
      next_level.reserve(level.size() * 2uz);
      for (const TreeNode* node : level) {
        if (node != nullptr) {
          next_level.push_back(node->left);
          next_level.push_back(node->right);
        }
      }
      level = std::move(next_level);
    }

    return true;
  }

 private:
  // Returns true iff the nodes in `level` are pairwise mirror-equal from both
  // ends inward.
  static bool is_level_palindrome(const std::vector<const TreeNode*>& level) {
    const std::size_t n = level.size();
    for (std::size_t i = 0uz; i < n / 2uz; ++i) {
      const TreeNode* left = level[i];
      const TreeNode* right = level[n - 1uz - i];
      if (left == nullptr && right == nullptr) {
        continue;
      }
      if (left == nullptr || right == nullptr || left->val != right->val) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(101, "Symmetric Tree", "bfs", ctx) {
  Solution sol;

  ctx.test("example_1_symmetric", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_tree(arena, {1, 2, 2, 3, 4, 4, 3});
    ctx.expect_true(sol.isSymmetric(root));
    ctx.expect_true(sol.isSymmetricLevelOrder(root));
  });

  ctx.test("example_2_asymmetric_structure", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root =
        build_tree(arena, {1, 2, 2, std::nullopt, 3, std::nullopt, 3});
    ctx.expect_false(sol.isSymmetric(root));
    ctx.expect_false(sol.isSymmetricLevelOrder(root));
  });

  ctx.test("empty_tree", [&] {
    ctx.expect_true(sol.isSymmetric(nullptr));
    ctx.expect_true(sol.isSymmetricLevelOrder(nullptr));
  });

  ctx.test("single_node", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_tree(arena, {42});
    ctx.expect_true(sol.isSymmetric(root));
    ctx.expect_true(sol.isSymmetricLevelOrder(root));
  });

  ctx.test("asymmetric_values", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_tree(arena, {1, 2, 3});
    ctx.expect_false(sol.isSymmetric(root));
    ctx.expect_false(sol.isSymmetricLevelOrder(root));
  });

  ctx.test("deep_symmetric_with_null_gaps", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_tree(
        arena, {1, 2, 2, std::nullopt, 3, 3, std::nullopt, 4, 5, 5, 4});
    ctx.expect_true(sol.isSymmetric(root));
    ctx.expect_true(sol.isSymmetricLevelOrder(root));
  });
}

}  // namespace
