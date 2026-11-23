// LeetCode 1104: Path In Zigzag Labelled Binary Tree
// https://leetcode.com/problems/path-in-zigzag-labelled-binary-tree/
//
// Problem:
//   Consider an infinite complete binary tree whose nodes are labelled
//   1, 2, 3, ... level by level in alternating directions: odd-numbered rows
//   (1st, 3rd, 5th, ...) are labelled left-to-right, whereas even-numbered
//   rows (2nd, 4th, 6th, ...) are labelled right-to-left. Given a node's
//   `label`, return the sequence of node labels along the path from the root
//   (label 1) down to `label`.
//   Constraints: 1 <= label <= 10^6.
//
// Algorithm:
//   Bottom-up parent inversion using binary-tree power-of-two level bounds.
//   Row d (0-indexed from the root) always contains the exact contiguous range
//   of labels `[level_start, level_end] = [2^d, 2^(d+1) - 1]`, regardless of
//   whether that row is reversed.
//   In a standard (non-zigzag) binary tree, the parent of node `x` is simply
//   `x / 2`. Because every row's ordering is inverted relative to its parent
//   row, the label at the same horizontal position in the non-reversed row is
//   the mirror image across `[level_start, level_end]`:
//       mirrored = level_start + (level_end - label)
//   Dividing this mirrored label by 2 gives the exact label of the parent in
//   the zigzag tree:
//       parent = (level_start + (level_end - label)) / 2
//   Using C++20/23 `std::bit_floor`, we jump directly to `level_start = 2^d`
//   in O(1), walk upward halving `level_start` at each step until reaching the
//   root, and finally reverse the collected path.
//   Time Complexity:  O(log(label)) — one iteration per tree level.
//   Space Complexity: O(log(label)) for the returned root-to-node path.
//
// Data Structures:
//   - std::vector<int>: stores the path from `label` up to root `1`, then
//     reversed in place into root-to-target order.
//
// Real-World Applications:
//   - Spatial indexing & space-filling curves (boustrophedon / serpentine
//     traversal): computing ancestor/parent cells in hierarchical quadtrees or
//     binary space partitions that alternate scan direction for cache locality.
//   - Implicit heap / tournament-tree indexing: deriving parent/child addresses
//     arithmetically without storing explicit pointer links.
//   - Memory-mapped GPU/FPGA scanline address generation where alternating
//     rows reverse direction to minimize routing crossovers.
//
// Related LeetCode Problems:
//   - #103 Binary Tree Zigzag Level Order Traversal [done] (alternating level-order traversal on explicit trees)
//   - #222 Count Complete Tree Nodes [done] (exploiting complete binary tree power-of-two level structure)
//   - #2096 Step-By-Step Directions From a Binary Tree Node to Another [new] (root-to-node path reconstruction in binary trees)
//   - #2509 Cycle Length Queries in a Tree [new] (parent-by-halving ancestor walks in an infinite complete binary tree)

#include "framework/lc_framework.h"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the root-to-target label path in an infinite zigzag-labelled
  // complete binary tree.
  // Time: O(log(label)), Space: O(log(label)).
  [[nodiscard]] std::vector<int> pathInZigZagTree(int label) const {
    if (label <= 0) {
      return {};
    }

    const auto u_label = static_cast<unsigned int>(label);
    // Smallest power of two <= label gives the minimum label on this level.
    int level_start = static_cast<int>(std::bit_floor(u_label));

    std::vector<int> path;
    path.reserve(static_cast<std::size_t>(std::bit_width(u_label)));

    int current = label;
    while (level_start >= 1) {
      path.push_back(current);

      // Level d spans [level_start, (2 * level_start) - 1]. Reflecting
      // `current` across this interval undoes the row reversal relative to its
      // parent row, and integer division by 2 moves up one level.
      const int level_end = (level_start * 2) - 1;
      current = (level_start + (level_end - current)) / 2;
      level_start /= 2;
    }

    // Reverse bottom-up traversal into root-to-target order.
    std::ranges::reverse(path);
    return path;
  }
};

LC_PROBLEM(1104, "Path In Zigzag Labelled Binary Tree", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_label_14", [&] {
    ctx.expect_eq(sol.pathInZigZagTree(14), std::vector<int>{1, 3, 4, 14});
  });

  ctx.test("example_2_label_26", [&] {
    ctx.expect_eq(sol.pathInZigZagTree(26), std::vector<int>{1, 2, 6, 10, 26});
  });

  ctx.test("root_node_label_1", [&] {
    ctx.expect_eq(sol.pathInZigZagTree(1), std::vector<int>{1});
  });

  ctx.test("second_level_nodes", [&] {
    ctx.expect_eq(sol.pathInZigZagTree(2), std::vector<int>{1, 2});
    ctx.expect_eq(sol.pathInZigZagTree(3), std::vector<int>{1, 3});
  });

  ctx.test("power_of_two_boundary", [&] {
    // Level 3 is reversed [15..8], so 8 is the rightmost node under 7 -> 2 -> 1.
    ctx.expect_eq(sol.pathInZigZagTree(8), std::vector<int>{1, 2, 7, 8});
  });
}

}  // namespace
