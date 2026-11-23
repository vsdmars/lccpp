// LeetCode 987: Vertical Order Traversal Of A Binary Tree
// https://leetcode.com/problems/vertical-order-traversal-of-a-binary-tree/
//
// Problem:
//   Given the `root` of a binary tree, assign coordinates `(row, col)` to every
//   node such that the root is at `(0, 0)`, a left child is at
//   `(row + 1, col - 1)`, and a right child is at `(row + 1, col + 1)`.
//   Return the vertical order traversal as a list of non-empty columns ordered
//   from leftmost (`col` ascending) to rightmost. Within each column, nodes are
//   ordered from top to bottom (`row` ascending); if multiple nodes share the
//   exact same `(row, col)` position, sort those nodes by value ascending.
//   Constraints: 1 <= number of nodes <= 1000, 0 <= node.val <= 1000.
//
// Algorithm:
//   Two complementary approaches are implemented:
//   1. Level-by-Level BFS (primary, `verticalTraversal`):
//      Process the tree one depth row at a time, tracking `(node, col)`. Because
//      every node in a BFS wave shares the exact same `row`, we collect that
//      wave's values into a per-level map `col -> values`, sort each bucket in
//      ascending value order to break same-`(row, col)` ties, and append those
//      values to the global ordered column map `columns[col]`.
//   2. Coordinate DFS (`verticalTraversalDfs`):
//      Collect `(row, val)` pairs per column during DFS, then sort each column's
//      entries lexicographically by `(row, val)`.
//   Time Complexity:  O(N log N) — due to ordered column keys and sorting ties
//                     within each `(row, col)` bucket / column.
//   Space Complexity: O(N) — storing all node values across columns plus the
//                     BFS frontier or DFS call stack.
//
// Data Structures:
//   - Ordered map (`std::map<int, std::vector<int>>`): keeps columns sorted by
//     ascending column index without manual min/max tracking.
//   - Per-level hash map (`std::unordered_map<int, std::vector<int>>`): groups
//     nodes at identical `(row, col)` so same-position ties can be sorted
//     independently before appending to the final column output.
//   - Frontier vector (`std::vector<std::pair<const TreeNode*, int>>`): drives
//     level-synchronous BFS.
//
// Real-World Applications:
//   - Tree & org-chart layout engines: aligning hierarchical nodes into
//     vertical display lanes with deterministic tie-breaking for overlapping
//     coordinates.
//   - Genealogy / AST pretty-printers: computing horizontal indentation columns
//     for visual rendering of balanced or skewed syntax trees.
//   - Spatial sweep-line indexing: grouping 2D grid-aligned objects by X
//     coordinate and ordering each vertical slice by Y then priority.
//
// Related LeetCode Problems:
//   - #314 Binary Tree Vertical Order Traversal [done] (same column grouping, breaking same-position ties by left-to-right order instead of value)
//   - #102 Binary Tree Level Order Traversal [done] (horizontal level-by-level BFS over a binary tree)
//   - #103 Binary Tree Zigzag Level Order Traversal [done] (level-order BFS with per-level ordering rules)
//   - #199 Binary Tree Right Side View [done] (extracting boundary nodes per depth using BFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int x) : val(x) {}
};

// RAII arena ensuring zero memory leaks for test trees.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val);
    auto* node = &nodes_.back();
    node->left = left;
    node->right = right;
    return node;
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Primary BFS solution: processes one depth row at a time so all nodes in a
  // wave share the same `row`, making same-(row, col) value sorting local to
  // each wave.
  // Time: O(N log N), Space: O(N).
  std::vector<std::vector<int>> verticalTraversal(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    // `columns[col]` accumulates node values top-to-bottom across rows.
    std::map<int, std::vector<int>> columns;
    std::vector<std::pair<const TreeNode*, int>> frontier;
    frontier.emplace_back(root, 0);

    while (!frontier.empty()) {
      std::unordered_map<int, std::vector<int>> row_buckets;
      std::vector<std::pair<const TreeNode*, int>> next_frontier;

      for (const auto [node, col] : frontier) {
        row_buckets[col].push_back(node->val);
        if (node->left != nullptr) {
          next_frontier.emplace_back(node->left, col - 1);
        }
        if (node->right != nullptr) {
          next_frontier.emplace_back(node->right, col + 1);
        }
      }

      // Nodes in the same `row_buckets[col]` have identical `(row, col)`, so
      // sort them by value ascending before appending to `columns[col]`.
      for (auto& [col, vals] : row_buckets) {
        std::ranges::sort(vals);
        auto& target_col = columns[col];
        target_col.insert(target_col.end(), vals.begin(), vals.end());
      }

      frontier = std::move(next_frontier);
    }

    std::vector<std::vector<int>> result;
    result.reserve(columns.size());
    for (auto& entry : columns) {
      result.push_back(std::move(entry.second));
    }
    return result;
  }

  // Alternative DFS solution: gathers `(row, val)` pairs per column and sorts
  // each column lexicographically by `(row, val)`.
  // Time: O(N log N), Space: O(N).
  std::vector<std::vector<int>> verticalTraversalDfs(const TreeNode* root) {
    std::map<int, std::vector<std::pair<int, int>>> columns;

    auto dfs = [&](this auto&& self, const TreeNode* node, int row,
                   int col) -> void {
      if (node == nullptr) {
        return;
      }
      columns[col].emplace_back(row, node->val);
      self(node->left, row + 1, col - 1);
      self(node->right, row + 1, col + 1);
    };
    dfs(root, 0, 0);

    std::vector<std::vector<int>> result;
    result.reserve(columns.size());
    for (auto& entry : columns) {
      auto& entries = entry.second;
      std::ranges::sort(entries);
      std::vector<int> vals;
      vals.reserve(entries.size());
      for (const auto& item : entries) {
        vals.push_back(item.second);
      }
      result.push_back(std::move(vals));
    }
    return result;
  }
};

LC_PROBLEM(987, "Vertical Order Traversal Of A Binary Tree", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_basic_tree", [&] {
    //     3
    //    / \
    //   9  20
    //     /  \
    //    15   7
    TreeArena arena;
    auto* root = arena.make(3, arena.make(9),
                            arena.make(20, arena.make(15), arena.make(7)));
    const std::vector<std::vector<int>> expected = {{9}, {3, 15}, {20}, {7}};
    ctx.expect_eq(sol.verticalTraversal(root), expected);
    ctx.expect_eq(sol.verticalTraversalDfs(root), expected);
  });

  ctx.test("example2_same_position_sorted_by_value", [&] {
    //       1
    //      / \
    //     2   3
    //    / \ / \
    //   4  5 6  7
    TreeArena arena;
    auto* root =
        arena.make(1, arena.make(2, arena.make(4), arena.make(5)),
                   arena.make(3, arena.make(6), arena.make(7)));
    const std::vector<std::vector<int>> expected = {
        {4}, {2}, {1, 5, 6}, {3}, {7}};
    ctx.expect_eq(sol.verticalTraversal(root), expected);
    ctx.expect_eq(sol.verticalTraversalDfs(root), expected);
  });

  ctx.test("tie_break_reorders_right_before_left_by_smaller_value", [&] {
    // From Python reference build():
    //       5
    //      / \
    //     2   3
    //    / \ /
    //   4  7 1
    // At (row=2, col=0), node 7 appears left of node 1, yet 1 < 7 so output
    // for column 0 must be [5, 1, 7] (key difference vs LeetCode #314).
    TreeArena arena;
    auto* root = arena.make(5, arena.make(2, arena.make(4), arena.make(7)),
                            arena.make(3, arena.make(1), nullptr));
    const std::vector<std::vector<int>> expected = {
        {4}, {2}, {5, 1, 7}, {3}};
    ctx.expect_eq(sol.verticalTraversal(root), expected);
    ctx.expect_eq(sol.verticalTraversalDfs(root), expected);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.verticalTraversal(nullptr),
                  std::vector<std::vector<int>>{});
    ctx.expect_eq(sol.verticalTraversalDfs(nullptr),
                  std::vector<std::vector<int>>{});
    auto* single = arena.make(42);
    const std::vector<std::vector<int>> expected = {{42}};
    ctx.expect_eq(sol.verticalTraversal(single), expected);
    ctx.expect_eq(sol.verticalTraversalDfs(single), expected);
  });
}

}  // namespace
