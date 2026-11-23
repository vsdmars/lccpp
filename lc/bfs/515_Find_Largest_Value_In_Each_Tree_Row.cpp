// LeetCode 515: Find Largest Value In Each Tree Row
// https://leetcode.com/problems/find-largest-value-in-each-tree-row/
//
// Problem:
//   Given the `root` of a binary tree, return an array containing the largest
//   node value in each row (depth level) of the tree, ordered from top (depth
//   0) to bottom. For an empty tree, return an empty array.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 3, 2, 5, 3, null, 9]
//     Output: [1, 3, 9]
//     Explanation:
//       - Push root node 1 onto level_queue.
//       - At level 0 (level_size = 1), pop 1, record row_max = 1, push children 3
//         and 2, and append 1 to result.
//       - At level 1 (level_size = 2), pop 3 (pushing 5 and 3) and 2 (pushing 9),
//         compute row_max = max(3, 2) = 3, and append 3.
//       - At level 2 (level_size = 3), pop 5, 3, and 9 with no further children,
//         compute row_max = max(5, 3, 9) = 9, and return [1, 3, 9].
//
//   Example 2:
//     Input:  root = [-10, -20, -5, -2147483648]
//     Output: [-10, -5, -2147483648]
//     Explanation:
//       - Push root node -10 onto level_queue.
//       - At level 0 (level_size = 1), pop -10, update row_max from INT_MIN to -10,
//         push children -20 and -5, and append -10.
//       - At level 1 (level_size = 2), pop -20 (pushing its left child -2147483648)
//         and -5, compute row_max = max(-20, -5) = -5, and append -5.
//       - At level 2 (level_size = 1), pop -2147483648, keep row_max = -2147483648,
//         and return [-10, -5, -2147483648].
//
//   Example 3:
//     Input:  root = []
//     Output: []
//     Explanation:
//       - Check root == nullptr at the start of largestValues, which is true for
//         the empty tree.
//       - Return an empty vector {} immediately without allocating or pushing onto
//         level_queue.
//
// Algorithm:
//   Perform level-order Breadth-First Search (BFS) using a FIFO queue:
//     1. If `root` is null, return an empty vector.
//     2. Enqueue `root`.
//     3. For each level, snapshot `level_size = queue.size()`, which isolates
//        the exact nodes belonging to the current row.
//     4. Initialize `row_max` to `std::numeric_limits<int>::min()`, pop
//        `level_size` nodes while updating `row_max = max(row_max, node->val)`,
//        and enqueue each non-null left/right child for the next level.
//     5. Append `row_max` to the result vector.
//   Time Complexity:  O(n), where n is the number of nodes in the tree (every
//                     node is enqueued and dequeued exactly once).
//   Space Complexity: O(w) <= O(n), where w is the maximum number of nodes at
//                     any single depth level (up to ceil(n / 2) for a complete
//                     binary tree).
//
// Data Structures:
//   - TreeNode: Standard binary tree node with integer value and left/right
//     child pointers.
//   - std::queue<const TreeNode*>: FIFO queue supporting level-by-level
//     traversal by batching `queue.size()` elements per depth.
//   - std::vector<int>: Output array holding the maximum value per row.
//
// Real-World Applications:
//   - Organizational hierarchy analytics (e.g., finding the highest salary,
//     budget, or latency metric at each management tier).
//   - Profiling call trees or DOM/scene-graph hierarchies by computing peak
//     resource consumption per nesting depth.
//   - Network broadcast tree telemetry (maximum RTT or packet loss per hop
//     distance from the source router).
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (collect all node values per level)
//   - #637 Average of Levels in Binary Tree [done] (compute mean value per level via BFS)
//   - #199 Binary Tree Right Side View [done] (keep the rightmost node at each level)
//   - #513 Find Bottom Left Tree Value [done] (level-order traversal tracking first node of last row)
//   - #1161 Maximum Level Sum of a Binary Tree [done] (find level with maximum total sum)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <limits>
#include <queue>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int value) : val(value) {}
  TreeNode(int value, TreeNode* left_child, TreeNode* right_child)
      : val(value), left(left_child), right(right_child) {}
};

// RAII arena that owns test TreeNode instances in a std::deque so raw pointers
// stay valid across insertions and are freed automatically with zero leaks.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns the maximum value present at each depth level of the binary tree.
  // Time: O(n), Space: O(w) where w is the maximum tree width.
  std::vector<int> largestValues(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    std::vector<int> result;
    std::queue<const TreeNode*> level_queue;
    level_queue.push(root);

    while (!level_queue.empty()) {
      // Snapshot the current level width so children pushed below do not mix
      // into this row's maximum computation.
      const std::size_t level_size = level_queue.size();
      int row_max = std::numeric_limits<int>::min();

      for (std::size_t i = 0uz; i < level_size; ++i) {
        const auto* node = level_queue.front();
        level_queue.pop();

        row_max = std::max(row_max, node->val);
        if (node->left != nullptr) {
          level_queue.push(node->left);
        }
        if (node->right != nullptr) {
          level_queue.push(node->right);
        }
      }

      result.push_back(row_max);
    }

    return result;
  }
};

LC_PROBLEM(515, "Find Largest Value In Each Tree Row", "bfs", ctx) {
  ctx.test("example_1_standard_tree", [&] {
    //       1
    //      / \
    //     3   2
    //    / \   \
    //   5   3   9
    TreeArena arena;
    TreeNode* root = arena.make(
        1, arena.make(3, arena.make(5), arena.make(3)),
        arena.make(2, nullptr, arena.make(9)));

    Solution sol;
    ctx.expect_eq(sol.largestValues(root), std::vector<int>{1, 3, 9});
  });

  ctx.test("empty_tree", [&] {
    Solution sol;
    ctx.expect_eq(sol.largestValues(nullptr), std::vector<int>{});
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(42);
    Solution sol;
    ctx.expect_eq(sol.largestValues(root), std::vector<int>{42});
  });

  ctx.test("negative_values_and_int_min", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    TreeArena arena;
    TreeNode* root = arena.make(
        -10, arena.make(-20, arena.make(kMin), nullptr), arena.make(-5));

    Solution sol;
    ctx.expect_eq(sol.largestValues(root), std::vector<int>{-10, -5, kMin});
  });

  ctx.test("left_skewed_chain", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.make(4, arena.make(2, arena.make(7, arena.make(-1), nullptr),
                                 nullptr),
                   nullptr);

    Solution sol;
    ctx.expect_eq(sol.largestValues(root), std::vector<int>{4, 2, 7, -1});
  });
}

}  // namespace
