// LeetCode 669: Trim A Binary Search Tree
// https://leetcode.com/problems/trim-a-binary-search-tree/
//
// Problem:
//   Given the root of a binary search tree (BST) and inclusive integer bounds
//   low and high (low <= high), trim the tree so that all remaining node values
//   lie within [low, high]. Trimming must preserve the relative structure of
//   the retained elements (any descendant of a node remains its descendant).
//   Return the root of the trimmed BST, which may change if the original root
//   falls outside [low, high].
//
// Examples:
//   Example 1:
//     Input:  root = [3, 0, 4, null, 2, null, null, 1], low = 1, high = 3
//     Output: [3, 2, null, 1]
//     Explanation:
//       - At root 3, 1 <= 3 <= 3 holds, so node 3 is retained and trimBST recurses
//         on both children.
//       - Left child 0 has 0 < 1, so 0 and its null left subtree are discarded and
//         trimBST recurses on 0->right (2).
//       - Node 2 and its left child 1 both lie in [1, 3], so 2 (with 1 attached on
//         its left) becomes 3->left.
//       - Right child 4 has 4 > 3, so 4 and its right subtree are pruned and
//         trimBST(4->left) returns null for 3->right.
//
//   Example 2:
//     Input:  root = [5, 2, 8, 1, 3, 7, 9], low = 6, high = 9
//     Output: [8, 7, 9]
//     Explanation:
//       - At root 5, 5 < 6, so node 5 and its entire left subtree [2, 1, 3] are
//         discarded and trimBST recurses on 5->right (8).
//       - At 8, 6 <= 8 <= 9 holds, so 8 becomes the returned root and both children
//         7 and 9 are trimmed.
//       - Both 7 and 9 fall inside [6, 9] with null children, leaving [8, 7, 9].
//
//   Example 3:
//     Input:  root = [4, 2, 6, 1, 3, 5, 7], low = 10, high = 20
//     Output: []
//     Explanation:
//       - At root 4, 4 < 10 prunes 4 and its left subtree, recursing on 4->right
//         (6).
//       - At 6, 6 < 10 prunes 6 and 5, recursing on 6->right (7); at 7, 7 < 10
//         recurses on 7->right (nullptr).
//       - The null base case returns nullptr, producing an empty trimmed tree [].
//
// Algorithm:
//   Recursive postorder-style BST pruning:
//   At each node, exploit the BST ordering invariant to prune entire invalid
//   subtrees in a single step:
//     - If node == nullptr, return nullptr.
//     - If node->val < low, node and its entire left subtree are strictly below
//       low, so discard them and return the trimmed right subtree.
//     - If node->val > high, node and its entire right subtree are strictly
//       above high, so discard them and return the trimmed left subtree.
//     - Otherwise, node->val is within [low, high], so keep node, recursively
//       trim both its left and right subtrees, and return node.
//   Time Complexity: O(N) in the worst case (every node visited at most once).
//   Space Complexity: O(H) for recursion stack, where H is tree height
//   (O(log N) balanced, O(N) skewed).
//
// Data Structures:
//   - Binary Search Tree (TreeNode): input/output ordered tree whose left/right
//     value ordering enables pruning entire subtrees without visiting them.
//   - Call Stack: tracks active ancestors along the current root-to-leaf path.
//
// Real-World Applications:
//   - Database B-tree / index range views that restrict an ordered index to a
//     specified key interval [low, high] before downstream query operators.
//   - Time-series retention windowing, discarding events older than a cutoff
//     or beyond a forecast horizon from an ordered event tree.
//   - Spatial / interval-tree viewport culling in graphics and GIS engines.
//
// Related LeetCode Problems:
//   - #776 Split BST [done] (partitions a BST around a threshold while preserving structure)
//   - #938 Range Sum Of BST [done] (uses the exact same BST range pruning to sum values in [low, high])
//   - #450 Delete Node in a BST [done] (removes a single target key while maintaining BST invariants)
//   - #108 Convert Sorted Array to Binary Search Tree [done] (builds a balanced BST from sorted keys)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

// Standard binary tree node for LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int x) : val(x) {}
};

class Solution {
 public:
  // Trims `root` in-place so only nodes with values in [low, high] remain,
  // preserving ancestor-descendant relationships.
  // Time: O(N), Space: O(H) where H is the height of the BST.
  TreeNode* trimBST(TreeNode* root, int low, int high) {
    // Base case: empty subtree requires no trimming.
    if (root == nullptr) {
      return nullptr;
    }

    // Because every node in root->right is strictly greater than root->val,
    // when root->val > high both root and its entire right subtree are invalid;
    // the only valid candidates live in root->left.
    if (root->val > high) {
      return trimBST(root->left, low, high);
    }

    // Symmetrically, when root->val < low both root and its entire left subtree
    // fall below the lower bound; discard them and keep trimming root->right.
    if (root->val < low) {
      return trimBST(root->right, low, high);
    }

    // Current node lies within [low, high]; keep it and recursively trim both
    // children so out-of-range descendants are excised cleanly.
    root->left = trimBST(root->left, low, high);
    root->right = trimBST(root->right, low, high);
    return root;
  }
};

// RAII arena that owns test TreeNode instances without raw new/delete leaks.
class TreeArena {
 public:
  // Constructs a binary tree from LeetCode's level-order vector where
  // std::nullopt denotes a missing child slot.
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*level_order[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);

    std::size_t idx = 1uz;
    while (!pending.empty() && idx < level_order.size()) {
      TreeNode* parent = pending.front();
      pending.pop();

      if (level_order[idx].has_value()) {
        parent->left = create(*level_order[idx]);
        pending.push(parent->left);
      }
      ++idx;

      if (idx < level_order.size() && level_order[idx].has_value()) {
        parent->right = create(*level_order[idx]);
        pending.push(parent->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  TreeNode* create(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  std::deque<TreeNode> nodes_;
};

// Serializes a binary tree into LeetCode's compact level-order format,
// stripping trailing std::nullopt entries for exact structural comparison.
std::vector<std::optional<int>> to_level_order(const TreeNode* root) {
  std::vector<std::optional<int>> result;
  if (root == nullptr) {
    return result;
  }

  std::queue<const TreeNode*> pending;
  pending.push(root);

  while (!pending.empty()) {
    const TreeNode* node = pending.front();
    pending.pop();

    if (node == nullptr) {
      result.emplace_back(std::nullopt);
      continue;
    }

    result.emplace_back(node->val);
    pending.push(node->left);
    pending.push(node->right);
  }

  while (!result.empty() && !result.back().has_value()) {
    result.pop_back();
  }
  return result;
}

LC_PROBLEM(669, "Trim A Binary Search Tree", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_trim_left_leaf", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({1, 0, 2});
    TreeNode* trimmed = sol.trimBST(root, 1, 2);
    const std::vector<std::optional<int>> expected{1, std::nullopt, 2};
    ctx.expect_eq(to_level_order(trimmed), expected);
  });

  ctx.test("example_2_promote_inner_subtree", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({3, 0, 4, std::nullopt, 2, std::nullopt,
                                  std::nullopt, 1});
    TreeNode* trimmed = sol.trimBST(root, 1, 3);
    const std::vector<std::optional<int>> expected{3, 2, std::nullopt, 1};
    ctx.expect_eq(to_level_order(trimmed), expected);
  });

  ctx.test("root_replaced_by_right_subtree", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 2, 8, 1, 3, 7, 9});
    TreeNode* trimmed = sol.trimBST(root, 6, 9);
    const std::vector<std::optional<int>> expected{8, 7, 9};
    ctx.expect_eq(to_level_order(trimmed), expected);
  });

  ctx.test("all_nodes_trimmed_out_of_range", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({4, 2, 6, 1, 3, 5, 7});
    ctx.expect_true(sol.trimBST(root, 10, 20) == nullptr);

    TreeNode* root2 = arena.build({4, 2, 6, 1, 3, 5, 7});
    ctx.expect_true(sol.trimBST(root2, -5, 0) == nullptr);
  });

  ctx.test("entire_tree_within_bounds", [&] {
    TreeArena arena;
    const std::vector<std::optional<int>> spec{4, 2, 6, 1, 3, 5, 7};
    TreeNode* root = arena.build(spec);
    TreeNode* trimmed = sol.trimBST(root, 1, 7);
    ctx.expect_eq(to_level_order(trimmed), spec);
  });

  ctx.test("single_point_range_and_empty_tree", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({4, 2, 6, 1, 3, 5, 7});
    TreeNode* trimmed = sol.trimBST(root, 3, 3);
    const std::vector<std::optional<int>> expected{3};
    ctx.expect_eq(to_level_order(trimmed), expected);

    ctx.expect_true(sol.trimBST(nullptr, 0, 10) == nullptr);
  });
}

}  // namespace
