// LeetCode 1038: Binary Search Tree To Greater Sum Tree
// https://leetcode.com/problems/binary-search-tree-to-greater-sum-tree/
//
// Problem:
//   Given the root of a binary search tree (BST) with distinct integer keys,
//   convert it in-place into a Greater Sum Tree (GST) where every node's key is
//   replaced by its original key plus the sum of all keys in the BST that are
//   strictly greater than the original key. Return the root of the modified
//   tree. (Identical to LeetCode #538.)
//
// Examples:
//   Example 1:
//     Input:  root = [4, 1, 6, 0, 2, 5, 7, null, null, null, 3, null, null, null,
//             8]
//     Output: [30, 36, 21, 36, 35, 26, 15, null, null, null, 33, null, null, null,
//             8]
//     Explanation:
//       - Initialize running_sum = 0 and recurse right-first from root 4 down to
//         the largest key 8.
//       - Visit the right subtree in descending order (8 -> 7 -> 6 -> 5):
//         running_sum accumulates 8, 15, 21, and 26, overwriting nodes 8, 7, 6, and
//         5 with those sums.
//       - Return to root 4: update running_sum = 26 + 4 = 30 and set root->val = 30
//         before recursing into the left subtree.
//       - Visit the left subtree in reverse in-order (3 -> 2 -> 1 -> 0):
//         running_sum becomes 33, 35, 36, and 36, updating nodes 3, 2, 1, and 0.
//       - Level-order serialization of the in-place modified tree produces [30, 36,
//         21, 36, 35, 26, 15, null, null, null, 33, null, null, null, 8].
//
//   Example 2:
//     Input:  root = [3, 2, 4, 1]
//     Output: [7, 9, 4, 10]
//     Explanation:
//       - Start with running_sum = 0 and traverse right first to leaf 4, updating
//         running_sum = 4 and node 4 to 4.
//       - Back at root 3, add 3 to get running_sum = 7 and update root 3 to 7.
//       - Recurse left to node 2 (whose right child is nullptr): update running_sum
//         = 7 + 2 = 9 and node 2 to 9.
//       - Recurse left to leaf 1: update running_sum = 9 + 1 = 10 and node 1 to 10,
//         yielding [7, 9, 4, 10].
//
//   Example 3:
//     Input:  root = [0, null, 1]
//     Output: [1, null, 1]
//     Explanation:
//       - Start at root 0 with running_sum = 0 and recurse into its right child 1.
//       - At node 1, add 1 to get running_sum = 1 and update node 1 to 1.
//       - Return to root 0, update running_sum = 1 + 0 = 1 and root 0 to 1, and
//         return [1, null, 1] since root->left is nullptr.
//
// Algorithm:
//   Reverse In-Order Traversal (Right -> Node -> Left):
//   In a BST, visiting right subtree, then current node, then left subtree
//   enumerates all keys in strictly descending order. Maintain a running suffix
//   accumulator `running_sum`:
//     1. Recurse on `node->right` so all strictly greater keys are summed first.
//     2. Update `running_sum += node->val` and assign `node->val = running_sum`.
//     3. Recurse on `node->left`, passing along the updated `running_sum`.
//   Time Complexity: O(N) — each node is visited exactly once.
//   Space Complexity: O(H) for recursion stack, where H is tree height
//   (O(log N) balanced, O(N) skewed).
//
// Data Structures:
//   - Binary Search Tree (TreeNode): updated in-place during reverse in-order
//     traversal.
//   - Running Suffix Accumulator (int): holds the sum of all keys greater than
//     the current node.
//
// Real-World Applications:
//   - Financial order books computing cumulative depth ("total volume at or
//     above price P") across price-ordered levels.
//   - Percentile and tail-distribution analytics (complementary CDF / survival
//     functions) maintained over ordered bucket trees.
//   - Leaderboard and quota dashboards displaying cumulative score or revenue
//     held by all ranks at or above each entry.
//
// Related LeetCode Problems:
//   - #538 Convert BST to Greater Tree [done] (exact duplicate problem using the same reverse in-order sweep)
//   - #938 Range Sum Of BST [done] (accumulates BST key sums over an interval)
//   - #897 Increasing Order Search Tree [done] (in-order BST transformation)
//   - #230 Kth Smallest Element in a BST [done] (exploits sorted in-order traversal of a BST)

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
  // Converts `root` in-place to a Greater Sum Tree via reverse in-order DFS.
  // Time: O(N), Space: O(H) where H is the height of the BST.
  TreeNode* bstToGst(TreeNode* root) {
    int running_sum = 0;

    // Reverse in-order (right -> node -> left) visits keys from largest to
    // smallest, so `running_sum` always equals the sum of all strictly greater
    // keys before updating `node->val`.
    auto reverse_inorder = [&running_sum](this auto&& self,
                                          TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }

      self(node->right);

      running_sum += node->val;
      node->val = running_sum;

      self(node->left);
    };

    reverse_inorder(root);
    return root;
  }
};

// RAII arena owning test TreeNode instances to guarantee zero memory leaks.
class TreeArena {
 public:
  // Builds a binary tree from LeetCode's level-order vector (std::nullopt =
  // missing child).
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

// Serializes a binary tree into LeetCode's compact level-order format.
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

LC_PROBLEM(1038, "Binary Search Tree To Greater Sum Tree",
           "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_nine_node_bst", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({4, 1, 6, 0, 2, 5, 7, std::nullopt,
                                  std::nullopt, std::nullopt, 3, std::nullopt,
                                  std::nullopt, std::nullopt, 8});
    TreeNode* gst = sol.bstToGst(root);
    const std::vector<std::optional<int>> expected{
        30, 36, 21, 36, 35, 26, 15, std::nullopt, std::nullopt, std::nullopt,
        33, std::nullopt, std::nullopt, std::nullopt, 8};
    ctx.expect_eq(to_level_order(gst), expected);
  });

  ctx.test("example_2_two_nodes", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({0, std::nullopt, 1});
    const std::vector<std::optional<int>> expected{1, std::nullopt, 1};
    ctx.expect_eq(to_level_order(sol.bstToGst(root)), expected);
  });

  ctx.test("example_3_and_example_4", [&] {
    TreeArena arena;
    TreeNode* root3 = arena.build({1, 0, 2});
    ctx.expect_eq(to_level_order(sol.bstToGst(root3)),
                  std::vector<std::optional<int>>{3, 3, 2});

    TreeNode* root4 = arena.build({3, 2, 4, 1});
    ctx.expect_eq(to_level_order(sol.bstToGst(root4)),
                  std::vector<std::optional<int>>{7, 9, 4, 10});
  });

  ctx.test("python_build_and_edge_cases", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 2, 13});
    ctx.expect_eq(to_level_order(sol.bstToGst(root)),
                  std::vector<std::optional<int>>{18, 20, 13});

    TreeNode* single = arena.build({9});
    ctx.expect_eq(to_level_order(sol.bstToGst(single)),
                  std::vector<std::optional<int>>{9});

    ctx.expect_true(sol.bstToGst(nullptr) == nullptr);
  });
}

}  // namespace
