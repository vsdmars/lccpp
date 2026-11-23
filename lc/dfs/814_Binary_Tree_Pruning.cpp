// LeetCode 814: Binary Tree Pruning
// https://leetcode.com/problems/binary-tree-pruning/
//
// Problem:
//   Given the `root` of a binary tree where every node's value is either `0`
//   or `1`, return the same tree with every subtree that does not contain a
//   `1` removed. A node's subtree consists of the node itself plus all of its
//   descendants.
//   Constraints: 1 <= number of nodes <= 200, node->val in {0, 1}.
//
// Examples:
//   Example 1:
//     Input:  root = [1, null, 0, 0, 1]
//     Output: [1, null, 0, null, 1]
//     Explanation:
//       - Post-order dfs recurses into right child 0 and visits its left leaf 0 and
//         right leaf 1 first.
//       - The left leaf 0 has val == 0 and null children so pruneTree returns
//         nullptr to prune it, while the right leaf 1 is kept.
//       - Returning to the right child 0, its right pointer is non-null, so it
//         stays and root 1 returns [1, null, 0, null, 1].
//
//   Example 2:
//     Input:  root = [1, 0, 1, 0, 0, 0, 1]
//     Output: [1, null, 1, null, 1]
//     Explanation:
//       - In the left subtree rooted at 0, both 0 leaves return nullptr, leaving
//         the left child 0 childless so it also returns nullptr.
//       - In the right subtree rooted at 1, its left leaf 0 returns nullptr while
//         its right leaf 1 is kept.
//       - Returning to root 1, root->left is now nullptr and root->right is [1,
//         null, 1], producing [1, null, 1, null, 1].
//
//   Example 3:
//     Input:  root = [0, 0, 0, 0]
//     Output: nullptr
//     Explanation:
//       - Post-order dfs reaches the deepest left leaf 0 and returns nullptr
//         because val == 0 with null children.
//       - Returning to the left child 0 and visiting the right leaf 0 both yield
//         nullptr since neither has a 1 or remaining children.
//       - At root 0, both left and right are now nullptr, so pruneTree returns
//         nullptr.
//
// Algorithm:
//   Use post-order DFS (`pruneTree`):
//     1. Base case: if `root == nullptr`, return `nullptr`.
//     2. Recursively prune both children first (`root->left`, `root->right`).
//        After these calls return, a child pointer is non-null if and only if
//        that child's subtree contains at least one `1`.
//     3. If `root->val == 0` and both `root->left` and `root->right` are now
//        `nullptr`, the entire subtree rooted at `root` contains no `1`s, so
//        return `nullptr` to prune it; otherwise return `root`.
//   Evaluating children before the parent naturally cascades pruning bottom-up
//   in a single pass.
//   Time Complexity: O(n), visiting each node once.
//   Space Complexity: O(h) for the recursion stack, where `h` is tree height
//     (O(n) worst case for a skewed tree, O(log n) for a balanced tree).
//
// Data Structures:
//   - Binary tree (`TreeNode` with `val`, `left`, `right`).
//   - Call stack for post-order traversal.
//   - std::deque<TreeNode> arena in unit tests for leak-free RAII node storage.
//
// Real-World Applications:
//   - Compiler dead-code elimination and AST pruning (removing empty blocks or
//     unused sub-expressions bottom-up).
//   - Directory tree cleanup (recursively deleting empty folders that contain
//     no matching files).
//   - Decision-tree and radix-trie compaction when leaf entries are removed.
//
// Related LeetCode Problems:
//   - #1325 Delete Leaves With a Given Value [new] (identical bottom-up post-order pruning of target leaves)
//   - #669 Trim a Binary Search Tree [done] (recursive BST pruning by value range)
//   - #1110 Delete Nodes And Return Forest [done] (post-order node deletion returning remaining roots)
//   - #1080 Insufficient Nodes in Root to Leaf Paths [new] (pruning nodes based on root-to-leaf path sums)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <string>
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

// Serializes a binary tree into trimmed LeetCode level-order string format
// (e.g. "1,null,0,null,1") for deterministic assertions.
std::string serialize_tree(const TreeNode* root) {
  if (root == nullptr) {
    return "";
  }

  std::vector<std::optional<int>> tokens;
  std::queue<const TreeNode*> q;
  q.push(root);

  while (!q.empty()) {
    const auto* node = q.front();
    q.pop();
    if (node != nullptr) {
      tokens.emplace_back(node->val);
      q.push(node->left);
      q.push(node->right);
    } else {
      tokens.emplace_back(std::nullopt);
    }
  }

  // Strip trailing nulls to match canonical LeetCode representation.
  while (!tokens.empty() && !tokens.back().has_value()) {
    tokens.pop_back();
  }

  std::string out;
  for (std::size_t i = 0uz; i < tokens.size(); ++i) {
    if (i > 0uz) {
      out.push_back(',');
    }
    if (tokens[i].has_value()) {
      out += std::to_string(*tokens[i]);
    } else {
      out += "null";
    }
  }
  return out;
}

class Solution {
 public:
  // Prunes every subtree that does not contain a `1` using post-order DFS.
  // Note: ownership of nodes stays with the caller (no `delete` performed).
  // Time: O(n); Space: O(h).
  TreeNode* pruneTree(TreeNode* root) {
    if (root == nullptr) {
      return nullptr;
    }

    // Post-order: prune children first so we know whether any descendant has 1.
    root->left = pruneTree(root->left);
    root->right = pruneTree(root->right);

    // A node with value 0 and no remaining children represents a 0-only
    // subtree and should itself be removed.
    if (root->val == 0 && root->left == nullptr && root->right == nullptr) {
      return nullptr;
    }
    return root;
  }
};

LC_PROBLEM(814, "Binary Tree Pruning", "dfs", ctx) {
  ctx.test("example_1_single_zero_leaf_pruned", [&] {
    TreeArena arena;
    Solution sol;
    auto* root = build_tree(arena, {1, std::nullopt, 0, 0, 1});
    ctx.expect_eq(serialize_tree(sol.pruneTree(root)), "1,null,0,null,1");
  });

  ctx.test("example_2_multiple_zero_subtrees", [&] {
    TreeArena arena;
    Solution sol;
    auto* root = build_tree(arena, {1, 0, 1, 0, 0, 0, 1});
    ctx.expect_eq(serialize_tree(sol.pruneTree(root)), "1,null,1,null,1");
  });

  ctx.test("example_3_mixed_tree", [&] {
    TreeArena arena;
    Solution sol;
    auto* root = build_tree(arena, {1, 1, 0, 1, 1, 0, 1, 0});
    ctx.expect_eq(serialize_tree(sol.pruneTree(root)), "1,1,0,1,1,null,1");
  });

  ctx.test("all_zeros_pruned_to_null", [&] {
    TreeArena arena;
    Solution sol;
    auto* root = build_tree(arena, {0, 0, 0, 0});
    ctx.expect_eq(sol.pruneTree(root), nullptr);
  });

  ctx.test("single_node_cases_and_null", [&] {
    TreeArena arena;
    Solution sol;
    ctx.expect_eq(sol.pruneTree(nullptr), nullptr);
    ctx.expect_eq(sol.pruneTree(arena.make(0)), nullptr);
    ctx.expect_eq(serialize_tree(sol.pruneTree(arena.make(1))), "1");
  });
}

}  // namespace
