// LeetCode 199: Binary Tree Right Side View
// https://leetcode.com/problems/binary-tree-right-side-view/
//
// Problem:
//   Given the root of a binary tree, imagine standing on the right side of the
//   tree and looking left. Return the values of the nodes visible at each depth
//   ordered from top (root) to bottom (deepest level). If the tree is empty,
//   return an empty list.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 2, 3, null, 5, null, 4]
//     Output: [1, 3, 4]
//     Explanation:
//       - BFS initializes level = [1], appends level.back()->val = 1 to result, and
//         collects next_level = [2, 3].
//       - At depth 1 with level = [2, 3], BFS appends level.back()->val = 3 to
//         result and collects children left-to-right into next_level = [5, 4].
//       - At depth 2 with level = [5, 4], BFS appends level.back()->val = 4 to
//         result, finds no further children, and returns [1, 3, 4].
//
//   Example 2:
//     Input:  root = [1, 2, 3, null, 5, null, 4, null, 7]
//     Output: [1, 3, 4, 7]
//     Explanation:
//       - BFS traverses levels [1], [2, 3], [5, 4], and [7], recording
//         level.back()->val at each tier to produce [1, 3, 4, 7].
//       - In rightSideViewDfs, visiting right children before left reaches 1 (depth
//         0), 3 (depth 1), and 4 (depth 2) first, appending [1, 3, 4] as depth ==
//         result.size().
//       - When DFS backtracks into the left branch 2 -> 5 -> 7, depths 1 and 2 are
//         already filled while node 7 at depth 3 == result.size() appends 7.
//
//   Example 3:
//     Input:  root = [] (nullptr)
//     Output: []
//     Explanation:
//       - In rightSideView, the initial guard root == nullptr immediately returns
//         an empty vector [].
//       - In rightSideViewDfs, dfs(nullptr, 0) hits the node == nullptr base case
//         on entry and leaves result empty.
//
// Algorithm:
//   1. Level-order BFS (primary):
//      Traverse the tree one level at a time from left to right. At every
//      non-empty level, the last node in that level's frontier is guaranteed
//      to be the rightmost visible node for that depth. Record its value and
//      build the next level's frontier from each node's left then right child.
//      Time Complexity:  O(n) — each node is visited once.
//      Space Complexity: O(w) where w <= n/2 + 1 is the maximum tree width.
//   2. Right-first Preorder DFS (secondary):
//      Visit `node`, then `node->right`, then `node->left`, passing the current
//      depth. The first time traversal reaches a given `depth` (detected by
//      `depth == result.size()`), the current node is guaranteed to be the
//      rightmost node at that depth.
//      Time Complexity:  O(n).
//      Space Complexity: O(h) recursion stack where h is the tree height.
//
// Data Structures:
//   - `std::vector<const TreeNode*>` double-buffered level frontier for BFS.
//   - Implicit call stack for right-first DFS.
//
// Real-World Applications:
//   - Silhouette / occlusion culling and horizon-profile extraction in 2D/3D
//     scene graphs and UI layout trees.
//   - Boundary extraction in hierarchical organizational charts or file-system
//     summaries where only the latest/rightmost branch per tier is surfaced.
//   - Level-wise telemetry sampling (e.g., recording the latest version node at
//     each depth of a revision tree).
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (foundational level-by-level BFS pattern)
//   - #116 Populating Next Right Pointers in Each Node [done] (horizontal level linkage across tree tiers)
//   - #515 Find Largest Value in Each Tree Row [done] (per-level aggregation during BFS/DFS)
//   - #545 Boundary of Binary Tree [done] (generalizes right view to full tree perimeter)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <utility>
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
  // Computes the right-side view via level-by-level BFS.
  // Time: O(n), Space: O(w) where w is maximum level width.
  std::vector<int> rightSideView(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    std::vector<int> result;
    std::vector<const TreeNode*> level = {root};

    while (!level.empty()) {
      // Because children are appended left-to-right, `level.back()` is always
      // the rightmost node at the current depth.
      result.push_back(level.back()->val);

      // Build the next depth's frontier left-to-right.
      std::vector<const TreeNode*> next_level;
      next_level.reserve(level.size() * 2uz);
      for (const TreeNode* node : level) {
        if (node->left != nullptr) {
          next_level.push_back(node->left);
        }
        if (node->right != nullptr) {
          next_level.push_back(node->right);
        }
      }
      level = std::move(next_level);
    }

    return result;
  }

  // Computes the right-side view via right-first preorder DFS.
  // Time: O(n), Space: O(h) recursion stack.
  std::vector<int> rightSideViewDfs(const TreeNode* root) {
    std::vector<int> result;

    // Visiting right child before left ensures the first node seen at any
    // depth `d` is the rightmost node at depth `d`.
    auto dfs = [&](this auto&& self, const TreeNode* node,
                   std::size_t depth) -> void {
      if (node == nullptr) {
        return;
      }
      if (depth == result.size()) {
        result.push_back(node->val);
      }
      self(node->right, depth + 1);
      self(node->left, depth + 1);
    };

    dfs(root, 0uz);
    return result;
  }
};

LC_PROBLEM(199, "Binary Tree Right Side View", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example", [&] {
    //     1
    //    / \
    //   2   3
    //    \   \
    //     5   4
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->right = make(3);
    root->left->right = make(5);
    root->right->right = make(4);

    const std::vector<int> expected = {1, 3, 4};
    ctx.expect_eq(sol.rightSideView(root), expected);
    ctx.expect_eq(sol.rightSideViewDfs(root), expected);
  });

  ctx.test("deeper_left_branch_visible", [&] {
    //     1
    //    / \
    //   2   3
    //    \   \
    //     5   4
    //      \
    //       7
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->left->right = make(5);
    root->left->right->right = make(7);
    root->right = make(3);
    root->right->right = make(4);

    const std::vector<int> expected = {1, 3, 4, 7};
    ctx.expect_eq(sol.rightSideView(root), expected);
    ctx.expect_eq(sol.rightSideViewDfs(root), expected);
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(sol.rightSideView(nullptr), std::vector<int>{});
    ctx.expect_eq(sol.rightSideViewDfs(nullptr), std::vector<int>{});

    TreeNode single(42);
    ctx.expect_eq(sol.rightSideView(&single), std::vector<int>{42});
    ctx.expect_eq(sol.rightSideViewDfs(&single), std::vector<int>{42});
  });

  ctx.test("left_skewed_tree", [&] {
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(10);
    root->left = make(20);
    root->left->left = make(30);

    const std::vector<int> expected = {10, 20, 30};
    ctx.expect_eq(sol.rightSideView(root), expected);
    ctx.expect_eq(sol.rightSideViewDfs(root), expected);
  });
}

}  // namespace
