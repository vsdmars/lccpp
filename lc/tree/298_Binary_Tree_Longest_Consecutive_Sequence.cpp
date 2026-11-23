// LeetCode 298: Binary Tree Longest Consecutive Sequence
// https://leetcode.com/problems/binary-tree-longest-consecutive-sequence/
//
// Problem:
//   Given the root of a binary tree, return the length of the longest
//   consecutive sequence path. A consecutive sequence path is a downward path
//   along parent-child edges where each child's value is strictly one greater
//   than its parent's value (child.val == parent.val + 1). The path may start
//   at any node and end at any descendant; it cannot reverse upward from child
//   to parent. An empty tree has length 0.
//
// Examples:
//   Example 1:
//     Input:  root = [1, null, 3, 2, 4, null, null, null, 5]
//     Output: 3
//     Explanation:
//       - Start DFS at root 1 with streak = 1 and update best = 1.
//       - Visit right child 3: since 3 != 1 + 1, reset streak = 1 while best stays
//         1.
//       - Visit left child 2 of 3 (2 != 3 + 1, streak = 1), then visit right child
//         4 of 3 (4 == 3 + 1), extending streak = 2 and updating best = 2.
//       - Visit right child 5 of 4: since 5 == 4 + 1, extend streak = 3 and update
//         best = 3.
//
//   Example 2:
//     Input:  root = [2, null, 3, 2, null, 1]
//     Output: 2
//     Explanation:
//       - Start DFS at root 2 with streak = 1 and set best = 1.
//       - Visit right child 3: since 3 == 2 + 1, extend streak = 2 and update best
//         = 2.
//       - Visit left child 2 of 3: since 2 != 3 + 1, reset streak = 1.
//       - Visit left child 1 of 2: since 1 != 2 + 1, streak remains 1, returning
//         final best = 2.
//
//   Example 3:
//     Input:  root = [2147483647, -2147483648, -3, null, null, -2, null, null, -1,
//             0]
//     Output: 4
//     Explanation:
//       - Start DFS at root 2147483647 (INT_MAX) with streak = 1 and set best = 1.
//       - Check left child -2147483648 (INT_MIN) in 64-bit arithmetic: 2147483647LL
//         + 1 != -2147483648LL, avoiding signed overflow wrap-around and resetting
//         streak = 1.
//       - Visit right child -3 (-3 != 2147483648, so streak = 1) and descend along
//         -3 -> -2 -> -1 -> 0.
//       - At each step (-2, -1, 0), child.val == parent.val + 1 holds, advancing
//         streak to 2, 3, and 4 so best = 4.
//
// Algorithm:
//   Use top-down Depth-First Search (both recursive and iterative stack-based
//   variants are provided). At each node, maintain the length of the longest
//   consecutive sequence ending at that node:
//     - The root starts a sequence of length 1.
//     - For each non-null child, if child.val == node.val + 1 (checked in
//       64-bit integer arithmetic to prevent signed overflow at INT_MAX), the
//       streak extends to `streak + 1`; otherwise a new streak of length 1
//       begins at the child.
//     - Update the running global maximum at every visited node.
//   Because consecutive paths only descend from parent to child, the length
//   ending at a node depends solely on its parent's streak.
//   Time Complexity: O(N), where N is the number of nodes (each visited once).
//   Space Complexity: O(H), where H is the tree height (call stack / explicit
//   stack), which is O(N) in the worst case for a skewed tree.
//
// Data Structures:
//   - Binary tree (`TreeNode` with value and left/right child pointers).
//   - Implicit recursion call stack (primary DFS) or explicit `std::vector`
//     stack storing `(const TreeNode*, int)` state pairs (iterative DFS).
//
// Real-World Applications:
//   - Version-control and commit-history analyzers detecting longest monotonic
//     sequential release or revision chains down branching histories.
//   - Telemetry and call-tree profilers identifying longest strictly
//     incrementing state-machine transition sequences along execution paths.
//   - Hierarchical file-system or AST linters finding consecutive numbered
//     nesting chains.
//
// Related LeetCode Problems:
//   - #549 Binary Tree Longest Consecutive Sequence II [done] (paths may turn at a node and go up or down)
//   - #128 Longest Consecutive Sequence [done] (consecutive run in an unsorted array via hash set)
//   - #124 Binary Tree Maximum Path Sum [done] (DFS maintaining a global best path metric across tree nodes)
//   - #687 Longest Univalue Path [done] (longest path of equal-valued nodes in a binary tree)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <deque>
#include <limits>
#include <utility>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Finds the longest downward parent-to-child consecutive increasing path
  // using top-down recursive DFS.
  // Time: O(N), Space: O(H).
  int longestConsecutive(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    int best = 0;

    // `streak` is the length of the consecutive chain ending at `node`.
    auto dfs = [&](this auto&& self, const TreeNode* node, int streak) -> void {
      best = std::max(best, streak);

      // Helper to compute the child's streak using 64-bit arithmetic so
      // `INT_MAX + 1` never invokes signed integer overflow.
      auto next_streak = [&](const TreeNode* child) -> int {
        const auto expected = static_cast<std::int64_t>(node->val) + 1;
        return static_cast<std::int64_t>(child->val) == expected ? streak + 1
                                                                 : 1;
      };

      if (node->left != nullptr) {
        self(node->left, next_streak(node->left));
      }
      if (node->right != nullptr) {
        self(node->right, next_streak(node->right));
      }
    };

    dfs(root, 1);
    return best;
  }

  // Iterative stack-based DFS variant carrying `(node, streak)` pairs.
  // Time: O(N), Space: O(H).
  int longestConsecutiveIterative(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    int best = 0;
    std::vector<std::pair<const TreeNode*, int>> stack;
    stack.emplace_back(root, 1);

    while (!stack.empty()) {
      const auto [node, streak] = stack.back();
      stack.pop_back();
      best = std::max(best, streak);

      const auto expected = static_cast<std::int64_t>(node->val) + 1;
      if (node->left != nullptr) {
        const int left_streak =
            static_cast<std::int64_t>(node->left->val) == expected ? streak + 1
                                                                   : 1;
        stack.emplace_back(node->left, left_streak);
      }
      if (node->right != nullptr) {
        const int right_streak =
            static_cast<std::int64_t>(node->right->val) == expected ? streak + 1
                                                                    : 1;
        stack.emplace_back(node->right, right_streak);
      }
    }

    return best;
  }
};

// Arena helper that allocates TreeNodes with RAII lifetime (zero leaks).
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(TreeNode{.val = val, .left = left, .right = right});
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(298, "Binary Tree Longest Consecutive Sequence", "tree", ctx) {
  ctx.test("example_1_chain_3_4_5", [&] {
    // 1 -> right: 3 -> (left: 2, right: 4 -> right: 5)
    TreeArena arena;
    TreeNode* n5 = arena.make(5);
    TreeNode* n4 = arena.make(4, nullptr, n5);
    TreeNode* n2 = arena.make(2);
    TreeNode* n3 = arena.make(3, n2, n4);
    TreeNode* root = arena.make(1, nullptr, n3);

    Solution sol;
    ctx.expect_eq(sol.longestConsecutive(root), 3);
    ctx.expect_eq(sol.longestConsecutiveIterative(root), 3);
  });

  ctx.test("example_2_cannot_reverse_decreasing", [&] {
    // 2 -> right: 3 -> left: 2 -> left: 1 (longest increasing is 2-3)
    TreeArena arena;
    TreeNode* n1 = arena.make(1);
    TreeNode* n2_low = arena.make(2, n1, nullptr);
    TreeNode* n3 = arena.make(3, n2_low, nullptr);
    TreeNode* root = arena.make(2, nullptr, n3);

    Solution sol;
    ctx.expect_eq(sol.longestConsecutive(root), 2);
    ctx.expect_eq(sol.longestConsecutiveIterative(root), 2);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    TreeNode* single = arena.make(7);

    Solution sol;
    ctx.expect_eq(sol.longestConsecutive(nullptr), 0);
    ctx.expect_eq(sol.longestConsecutiveIterative(nullptr), 0);
    ctx.expect_eq(sol.longestConsecutive(single), 1);
    ctx.expect_eq(sol.longestConsecutiveIterative(single), 1);
  });

  ctx.test("negative_and_int_max_boundary", [&] {
    TreeArena arena;
    // Negative consecutive run: -3 -> -2 -> -1 -> 0 (length 4)
    TreeNode* n0 = arena.make(0);
    TreeNode* nm1 = arena.make(-1, n0, nullptr);
    TreeNode* nm2 = arena.make(-2, nullptr, nm1);
    TreeNode* nm3 = arena.make(-3, nm2, nullptr);

    // INT_MAX parent with INT_MIN child must NOT wrap around as consecutive.
    TreeNode* min_child = arena.make(std::numeric_limits<int>::min());
    TreeNode* max_root =
        arena.make(std::numeric_limits<int>::max(), min_child, nm3);

    Solution sol;
    ctx.expect_eq(sol.longestConsecutive(max_root), 4);
    ctx.expect_eq(sol.longestConsecutiveIterative(max_root), 4);
  });
}

}  // namespace
