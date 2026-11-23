// LeetCode 314: Binary Tree Vertical Order Traversal
// https://leetcode.com/problems/binary-tree-vertical-order-traversal/
//
// Problem:
//   Given the root of a binary tree, return the vertical order traversal of
//   its nodes' values — column by column from leftmost to rightmost, and from
//   top to bottom within each column. If two nodes occupy the exact same row
//   and column, they must appear from left to right.
//
// Algorithm:
//   Perform a Breadth-First Search (level-order traversal) tagging each node
//   with its horizontal column index:
//     - The root starts at column 0.
//     - Moving to a left child decrements the column (`col - 1`); moving to a
//       right child increments the column (`col + 1`).
//     - Because BFS visits nodes strictly in increasing row order and from
//       left to right within each row (enqueueing left before right),
//       appending each dequeued node's value to `columns[col]` automatically
//       satisfies both top-to-bottom and left-to-right tie-breaking.
//     - Track `min_col` and `max_col` during the traversal. Since column
//       indices of a connected tree form a contiguous integer range
//       `[min_col, max_col]`, we collect columns in order without sorting keys.
//   Time Complexity: O(N), where N is the number of nodes (O(1) amortized hash
//   map insertion per node and linear range sweep at the end).
//   Space Complexity: O(N) for the BFS queue and column buckets.
//
// Data Structures:
//   - FIFO queue (`std::queue<std::pair<const TreeNode*, int>>`): enforces
//     row-by-row, left-to-right visitation order.
//   - Hash map (`std::unordered_map<int, std::vector<int>>`): groups node
//     values by signed horizontal column index in O(1) expected time.
//
// Real-World Applications:
//   - Tree and org-chart layout engines computing horizontal column alignment
//     and vertical slicing for graphical rendering.
//   - Spreadsheet / DAG visualization tools bucketing hierarchical nodes into
//     swimlanes or vertical time-slices.
//   - Spatial indexing and quadtree/BSP column scans.
//
// Related LeetCode Problems:
//   - #987 Vertical Order Traversal of a Binary Tree [done] (stricter variant sorting same-cell ties by value)
//   - #102 Binary Tree Level Order Traversal [done] (foundational BFS level-by-level tree traversal)
//   - #199 Binary Tree Right Side View [done] (BFS extracting boundary nodes per depth level)
//   - #515 Find Largest Value in Each Tree Row [done] (BFS computing per-row aggregates)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>
#include <queue>
#include <unordered_map>
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
  // Returns node values grouped by vertical column (left-to-right), ordered
  // top-to-bottom and left-to-right within each column.
  // Time: O(N), Space: O(N).
  std::vector<std::vector<int>> verticalOrder(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    std::unordered_map<int, std::vector<int>> columns;
    std::queue<std::pair<const TreeNode*, int>> bfs;
    bfs.emplace(root, 0);

    int min_col = 0;
    int max_col = 0;

    // BFS guarantees top-to-bottom, left-to-right ordering inside every bucket.
    while (!bfs.empty()) {
      const auto [node, col] = bfs.front();
      bfs.pop();

      columns[col].push_back(node->val);
      min_col = std::min(min_col, col);
      max_col = std::max(max_col, col);

      if (node->left != nullptr) {
        bfs.emplace(node->left, col - 1);
      }
      if (node->right != nullptr) {
        bfs.emplace(node->right, col + 1);
      }
    }

    // Column indices form a contiguous interval [min_col, max_col], so we can
    // assemble the output directly in O(W) without sorting map keys.
    std::vector<std::vector<int>> result;
    result.reserve(columns.size());
    for (int col = min_col; col <= max_col; ++col) {
      result.push_back(std::move(columns[col]));
    }
    return result;
  }
};

// RAII arena for allocating test TreeNodes without raw new/delete.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(TreeNode{.val = val, .left = left, .right = right});
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(314, "Binary Tree Vertical Order Traversal", "tree", ctx) {
  ctx.test("example_1_basic_tree", [&] {
    // [3, 9, 20, null, null, 15, 7]
    TreeArena arena;
    TreeNode* n15 = arena.make(15);
    TreeNode* n7 = arena.make(7);
    TreeNode* n20 = arena.make(20, n15, n7);
    TreeNode* n9 = arena.make(9);
    TreeNode* root = arena.make(3, n9, n20);

    Solution sol;
    const std::vector<std::vector<int>> expected = {{9}, {3, 15}, {20}, {7}};
    ctx.expect_eq(sol.verticalOrder(root), expected);
  });

  ctx.test("example_2_same_row_and_column_tie", [&] {
    // [3, 9, 8, 4, 0, 1, 7] -> 0 and 1 share row 2, col 0 (left before right)
    TreeArena arena;
    TreeNode* n4 = arena.make(4);
    TreeNode* n0 = arena.make(0);
    TreeNode* n1 = arena.make(1);
    TreeNode* n7 = arena.make(7);
    TreeNode* n9 = arena.make(9, n4, n0);
    TreeNode* n8 = arena.make(8, n1, n7);
    TreeNode* root = arena.make(3, n9, n8);

    Solution sol;
    const std::vector<std::vector<int>> expected = {
        {4}, {9}, {3, 0, 1}, {8}, {7}};
    ctx.expect_eq(sol.verticalOrder(root), expected);
  });

  ctx.test("example_3_crossing_descendants", [&] {
    // 0's right child is 2 (col +1) and 1's left child is 5 (col -1)
    TreeArena arena;
    TreeNode* n4 = arena.make(4);
    TreeNode* n2 = arena.make(2);
    TreeNode* n5 = arena.make(5);
    TreeNode* n0 = arena.make(0, nullptr, n2);
    TreeNode* n1 = arena.make(1, n5, nullptr);
    TreeNode* n7 = arena.make(7);
    TreeNode* n9 = arena.make(9, n4, n0);
    TreeNode* n8 = arena.make(8, n1, n7);
    TreeNode* root = arena.make(3, n9, n8);

    Solution sol;
    const std::vector<std::vector<int>> expected = {
        {4}, {9, 5}, {3, 0, 1}, {8, 2}, {7}};
    ctx.expect_eq(sol.verticalOrder(root), expected);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    TreeNode* single = arena.make(42);

    Solution sol;
    ctx.expect_eq(sol.verticalOrder(nullptr), std::vector<std::vector<int>>{});
    ctx.expect_eq(sol.verticalOrder(single),
                  std::vector<std::vector<int>>{{42}});
  });
}

}  // namespace
