// LeetCode 103: Binary Tree Zigzag Level Order Traversal
// https://leetcode.com/problems/binary-tree-zigzag-level-order-traversal/
//
// Problem:
//   Given the root of a binary tree, return the zigzag level order traversal
//   of its nodes' values: level 0 from left to right, level 1 from right to
//   left, level 2 from left to right, and alternating for each subsequent
//   level. An empty tree returns an empty 2D vector.
//
// Examples:
//   Example 1:
//     Input:  root = [3, 9, 20, null, null, 15, 7]
//     Output: [[3], [20, 9], [15, 7]]
//     Explanation:
//       - Initialize the BFS queue frontier with root node 3 and set left_to_right
//         = true.
//       - At depth 0 (level_size = 1, left_to_right = true), pop 3 into level[0],
//         enqueue children 9 and 20, push [3] to result, and flip left_to_right to
//         false.
//       - At depth 1 (level_size = 2, left_to_right = false), pop 9 (i = 0) into
//         target_idx = 1 and pop 20 (i = 1) into target_idx = 0 while enqueuing 15
//         and 7, producing [20, 9] and flipping left_to_right to true.
//       - At depth 2 (level_size = 2, left_to_right = true), pop 15 into level[0]
//         and 7 into level[1] with no further children, producing [15, 7] and
//         returning [[3], [20, 9], [15, 7]].
//
//   Example 2:
//     Input:  root = [1, 2, 3, 4, null, null, 5, 6, 7]
//     Output: [[1], [3, 2], [4, 5], [7, 6]]
//     Explanation:
//       - At depth 0 (left_to_right = true), pop 1 into level[0], enqueue 2 and 3,
//         and flip left_to_right to false.
//       - At depth 1 (level_size = 2, left_to_right = false), pop 2 to level[1]
//         (enqueuing 4) and 3 to level[0] (enqueuing 5), producing [3, 2] and
//         flipping left_to_right to true.
//       - At depth 2 (level_size = 2, left_to_right = true), pop 4 to level[0]
//         (enqueuing 6 and 7) and 5 to level[1], producing [4, 5] and flipping
//         left_to_right to false.
//       - At depth 3 (level_size = 2, left_to_right = false), pop 6 (i = 0) into
//         level[1] and 7 (i = 1) into level[0], yielding [7, 6] and final output
//         [[1], [3, 2], [4, 5], [7, 6]].
//
//   Example 3:
//     Input:  root = []
//     Output: []
//     Explanation:
//       - Check root == nullptr at the start of zigzagLevelOrder.
//       - Return the empty result vector [] immediately without initializing the
//         BFS queue.
//
// Algorithm:
//   Level-by-level Breadth-First Search (BFS) with indexed placement.
//   Keep the BFS queue strictly in standard left-to-right order across every
//   depth so child enqueuing logic stays simple and uniform. Maintain a
//   boolean `left_to_right` toggled after every level. For a level of size
//   `level_size`, pre-size the output vector `level(level_size)` and write the
//   i-th popped node directly into index `i` when `left_to_right` is true, or
//   `(level_size - 1) - i` when false. This avoids a separate reversal pass.
//   - Time Complexity: O(N), where N is the number of nodes (each node is
//     visited once and placed directly at its final slot in O(1)).
//   - Space Complexity: O(N) for the BFS queue (up to the maximum tree width)
//     and the returned 2D result vector.
//
// Data Structures:
//   - `std::queue<const TreeNode*>` for standard left-to-right BFS traversal.
//   - Pre-sized `std::vector<int>` per level for O(1) direct forward/reverse
//     indexed writes.
//
// Real-World Applications:
//   - Boustrophedon (back-and-forth) scan patterns in CNC milling, 3D
//     printing, robotic lawnmowers, and thermal/laser plotters to minimize
//     head travel time between adjacent rows.
//   - Serpentine traversal in cache-friendly grid processing and display
//     driver LED matrix wiring.
//   - Alternating-direction breadth-first exploration in game trees and
//     hierarchical visualization layouts.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (standard left-to-right BFS without direction flipping)
//   - #107 Binary Tree Level Order Traversal II [done] (reverses the order of levels rather than within levels)
//   - #199 Binary Tree Right Side View [done] (level-order BFS extracting the rightmost node per depth)
//   - #515 Find Largest Value in Each Tree Row [done] (level-order BFS aggregating per-level maximum)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode binary tree node representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Returns zigzag level-order values by keeping the BFS queue left-to-right
  // and placing each node directly at its forward or mirrored index in the
  // pre-sized level vector.
  // Time: O(N), Space: O(N).
  std::vector<std::vector<int>> zigzagLevelOrder(const TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (root == nullptr) {
      return result;
    }

    std::queue<const TreeNode*> frontier;
    frontier.push(root);
    bool left_to_right = true;

    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      std::vector<int> level(level_size);

      for (std::size_t i = 0uz; i < level_size; ++i) {
        const TreeNode* node = frontier.front();
        frontier.pop();

        // Write straight into the target slot so odd levels are reversed in
        // place without an extra reversal pass.
        const std::size_t target_idx =
            left_to_right ? i : ((level_size - 1uz) - i);
        level[target_idx] = node->val;

        if (node->left != nullptr) {
          frontier.push(node->left);
        }
        if (node->right != nullptr) {
          frontier.push(node->right);
        }
      }

      result.push_back(std::move(level));
      left_to_right = !left_to_right;
    }

    return result;
  }
};

// RAII node arena backed by std::deque for leak-free test tree construction.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

  TreeNode* from_level_order(const std::vector<std::optional<int>>& values) {
    if (values.empty() || !values[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*values[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);
    std::size_t idx = 1uz;

    while (!pending.empty() && idx < values.size()) {
      TreeNode* curr = pending.front();
      pending.pop();

      if (idx < values.size() && values[idx].has_value()) {
        curr->left = make(*values[idx]);
        pending.push(curr->left);
      }
      ++idx;

      if (idx < values.size() && values[idx].has_value()) {
        curr->right = make(*values[idx]);
        pending.push(curr->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(103, "Binary Tree Zigzag Level Order Traversal", "tree", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    TreeArena arena;
    //     3
    //    / \
    //   9  20
    //     /  \
    //    15   7
    TreeNode* root =
        arena.from_level_order({3, 9, 20, std::nullopt, std::nullopt, 15, 7});
    const std::vector<std::vector<int>> expected = {{3}, {20, 9}, {15, 7}};
    ctx.expect_eq(sol.zigzagLevelOrder(root), expected);
  });

  ctx.test("python_build_full_tree", [&] {
    TreeArena arena;
    //         1
    //        / \
    //       1   2
    //      / \ / \
    //     3  4 5  6
    TreeNode* root = arena.from_level_order({1, 1, 2, 3, 4, 5, 6});
    const std::vector<std::vector<int>> expected = {
        {1}, {2, 1}, {3, 4, 5, 6}};
    ctx.expect_eq(sol.zigzagLevelOrder(root), expected);
  });

  ctx.test("empty_tree", [&] {
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.zigzagLevelOrder(nullptr), expected);
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);
    const std::vector<std::vector<int>> expected = {{1}};
    ctx.expect_eq(sol.zigzagLevelOrder(root), expected);
  });

  ctx.test("four_level_asymmetric_tree", [&] {
    TreeArena arena;
    //         1
    //        / \
    //       2   3
    //      /     \
    //     4       5
    //    / \
    //   6   7
    TreeNode* n4 = arena.make(4, arena.make(6), arena.make(7));
    TreeNode* n2 = arena.make(2, n4, nullptr);
    TreeNode* n3 = arena.make(3, nullptr, arena.make(5));
    TreeNode* root = arena.make(1, n2, n3);
    const std::vector<std::vector<int>> expected = {
        {1}, {3, 2}, {4, 5}, {7, 6}};
    ctx.expect_eq(sol.zigzagLevelOrder(root), expected);
  });
}

}  // namespace
