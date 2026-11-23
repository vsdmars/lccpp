// LeetCode 666: Path Sum IV
// https://leetcode.com/problems/path-sum-iv/
//
// Problem:
//   A binary tree of depth less than 5 is encoded as a sorted ascending list
//   of three-digit integers `DPV`, where:
//     - `D` (hundreds digit, 1 <= D <= 4) is the 1-indexed depth of the node,
//     - `P` (tens digit, 1 <= P <= 8) is the 1-indexed position within level D
//       as in a full binary tree, and
//     - `V` (units digit, 0 <= V <= 9) is the node's integer value.
//   Given a valid connected binary tree in this representation, return the sum
//   of all root-to-leaf path sums.
//
// Examples:
//   Example 1:
//     Input:  nums = [113, 215, 221]
//     Output: 12
//     Explanation:
//       - Reverse iteration visits 221 (level 2, col 2, val 1) first; neither child
//         (3, 3) nor (3, 4) exists, so leaf_count[202] = 1 and subtree_sum[202] = 0
//         + 0 + 1 * 1 = 1.
//       - Next, 215 (level 2, col 1, val 5) has no level-3 children (3, 1) or (3,
//         2), so leaf_count[201] = 1 and subtree_sum[201] = 1 * 5 = 5.
//       - Finally, root 113 (level 1, col 1, val 3) combines children 201 and 202:
//         leaf_count[101] = 1 + 1 = 2 and subtree_sum[101] = 5 + 1 + 2 * 3 = 12.
//
//   Example 2:
//     Input:  nums = [112, 212, 223, 322, 433]
//     Output: 14
//     Explanation:
//       - Reverse iteration starts at leaf 433 (level 4, col 3, val 3) with
//         leaf_count[403] = 1 and subtree_sum[403] = 3, then processes parent 322
//         (level 3, col 2, val 2) whose left child is 403 to get leaf_count[302] =
//         1 and subtree_sum[302] = 3 + 0 + 1 * 2 = 5.
//       - Leaf 223 (level 2, col 2, val 3) has no level-3 children, yielding
//         leaf_count[202] = 1 and subtree_sum[202] = 3.
//       - Node 212 (level 2, col 1, val 2) aggregates right child 302 into
//         leaf_count[201] = 1 and subtree_sum[201] = 0 + 5 + 1 * 2 = 7.
//       - Root 112 (level 1, col 1, val 2) merges children 201 and 202 to give
//         leaf_count[101] = 1 + 1 = 2 and subtree_sum[101] = 7 + 3 + 2 * 2 = 14.
//
//   Example 3:
//     Input:  nums = [119]
//     Output: 9
//     Explanation:
//       - Reverse iteration visits the sole node 119 (level 1, col 1, val 9) and
//         looks up absent child keys 201 and 202, yielding 0 child leaves.
//       - Clamping child_leaves = 0 with std::max(1, 0) sets leaf_count[101] = 1
//         and subtree_sum[101] = 0 + 0 + 1 * 9 = 9, which is returned.
//
// Algorithm:
//   Because `nums` is sorted ascending by `(D, P)`, iterating in reverse visits
//   every node strictly after its left child `(D + 1, 2*P - 1)` and right child
//   `(D + 1, 2*P)` — an implicit bottom-up post-order traversal without
//   constructing explicit pointer-based tree nodes.
//
//   For each node `(D, P, V)`:
//     1. Let `leaf_count[D, P]` be the number of root-to-leaf paths passing
//        through this node: if it has no children in the map, it is a leaf and
//        contributes `1` path; otherwise it equals the sum of its children's
//        leaf counts.
//     2. Every root-to-leaf path passing through `(D, P)` includes `V`, so the
//        cumulative path sum contributed by the subtree rooted at `(D, P)` is:
//        `subtree_sum[D, P] = left_sum + right_sum + leaf_count[D, P] * V`.
//   After processing the root `(1, 1)`, `subtree_sum[1, 1]` holds the answer.
//   Time Complexity:  O(n), a single reverse pass over the n encoded nodes.
//   Space Complexity: O(n) for the leaf-count and subtree-sum hash maps.
//
// Data Structures:
//   - Hash maps (`std::unordered_map<int, int>`) keyed by encoded `(depth, pos)`
//     coordinates storing each node's descendant leaf count and accumulated
//     subtree path sum.
//
// Real-World Applications:
//   - Implicit/array-backed heap and segment-tree aggregation where parent-child
//     relationships are computed arithmetically without pointer overhead.
//   - Bottom-up cost rollup in compactly serialized decision trees and
//     hierarchical bill-of-materials (BOM) structures.
//   - Telemetry/call-tree aggregation where leaf execution counts weight each
//     ancestor's inclusive metric contribution.
//
// Related LeetCode Problems:
//   - #112 Path Sum [done] (root-to-leaf path sum check on pointer binary tree)
//   - #113 Path Sum II [done] (enumerating all root-to-leaf paths matching a sum)
//   - #437 Path Sum III [done] (counting downward paths with target sum)
//   - #129 Sum Root to Leaf Numbers [done] (aggregating all root-to-leaf paths)
//   - #257 Binary Tree Paths [done] (traversing every root-to-leaf path)

#include "framework/lc_framework.h"

#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the sum of all root-to-leaf paths in a compactly encoded binary
  // tree using a single reverse (bottom-up) pass.
  // Time: O(n), Space: O(n).
  int pathSum(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    // Encodes (level, col) into a collision-free integer key. Since
    // 1 <= col <= 8 (and child queries reach at most 2 * 8 = 16 < 100),
    // multiplying level by 100 cleanly separates the two coordinates.
    auto encode = [](int level, int col) -> int {
      return (level * 100) + col;
    };

    // leaf_count[key]: number of leaves in the subtree rooted at key.
    // subtree_sum[key]: sum of node values across all paths from key to its
    // descendant leaves.
    std::unordered_map<int, int> leaf_count;
    std::unordered_map<int, int> subtree_sum;

    auto lookup = [](const std::unordered_map<int, int>& table, int key) -> int {
      const auto it = table.find(key);
      return it != table.end() ? it->second : 0;
    };

    // Reverse iteration guarantees children are evaluated before their parent.
    for (const int code : nums | std::views::reverse) {
      const int level = code / 100;
      const int col = (code % 100) / 10;
      const int val = code % 10;

      const int left_key = encode(level + 1, (col * 2) - 1);
      const int right_key = encode(level + 1, col * 2);
      const int cur_key = encode(level, col);

      // A leaf has 0 children in the maps, so clamp its path multiplicity to 1.
      const int child_leaves =
          lookup(leaf_count, left_key) + lookup(leaf_count, right_key);
      const int paths_here = std::max(1, child_leaves);
      leaf_count[cur_key] = paths_here;

      // This node's value appears once in each root-to-leaf path passing
      // through it.
      subtree_sum[cur_key] = lookup(subtree_sum, left_key) +
                             lookup(subtree_sum, right_key) +
                             (paths_here * val);
    }

    return lookup(subtree_sum, encode(1, 1));
  }
};

LC_PROBLEM(666, "Path Sum IV", "tree", ctx) {
  ctx.test("example1_two_leaves", [&] {
    //     3
    //    / \
    //   5   1
    // (3 + 5) + (3 + 1) = 12
    Solution sol;
    ctx.expect_eq(sol.pathSum({113, 215, 221}), 12);
  });

  ctx.test("example2_single_right_path", [&] {
    //   3
    //    \
    //     1
    // (3 + 1) = 4
    Solution sol;
    ctx.expect_eq(sol.pathSum({113, 221}), 4);
  });

  ctx.test("deeper_asymmetric_tree", [&] {
    //       2 (1,1)
    //      / \
    //     2   3 (2,2)
    //      \
    //       2 (3,2)
    //      /
    //     3 (4,3)
    // Paths: (2+2+2+3) + (2+3) = 9 + 5 = 14
    Solution sol;
    ctx.expect_eq(sol.pathSum({112, 212, 223, 322, 433}), 14);
  });

  ctx.test("single_root_node", [&] {
    Solution sol;
    ctx.expect_eq(sol.pathSum({119}), 9);
  });

  ctx.test("zero_valued_nodes", [&] {
    //     0
    //    / \
    //   0   5
    Solution sol;
    ctx.expect_eq(sol.pathSum({110, 210, 225}), 5);
  });

  ctx.test("full_depth_four_leaves", [&] {
    //       1
    //      / \
    //     2   3
    //    / \ / \
    //   4  5 6  7
    // Paths: (1+2+4)=7, (1+2+5)=8, (1+3+6)=10, (1+3+7)=11 => 36
    Solution sol;
    ctx.expect_eq(sol.pathSum({111, 212, 223, 314, 325, 336, 347}), 36);
  });
}

}  // namespace
