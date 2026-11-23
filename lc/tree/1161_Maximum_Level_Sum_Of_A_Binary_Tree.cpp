// LeetCode 1161: Maximum Level Sum Of A Binary Tree
// https://leetcode.com/problems/maximum-level-sum-of-a-binary-tree/
//
// Problem:
//   Given the root of a binary tree whose root is at level 1, its children at
//   level 2, and so on, return the smallest level x such that the sum of all
//   node values at level x is maximal.
//   Constraints: 1 to 10^4 nodes; -10^5 <= Node.val <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 7, 0, 7, -8, null, null]
//     Output: 2
//     Explanation:
//       - Initialize best_sum to INT64_MIN, best_level to 1, level to 1, and
//         current_level to [node 1].
//       - At level 1, sum node 1 (level_sum = 1) and queue children [7, 0] into
//         next_level; since 1 > INT64_MIN, set best_sum = 1 and best_level = 1.
//       - At level 2, sum 7 + 0 (level_sum = 7) and queue children [7, -8] into
//         next_level; since 7 > 1, update best_sum = 7 and best_level = 2.
//       - At level 3, sum 7 + (-8) (level_sum = -1) with empty next_level; since -1
//         > 7 is false, keep best_sum = 7 and best_level = 2, then exit the loop
//         and return 2.
//
//   Example 2:
//     Input:  root = [989, null, 10250, 98693, -89388, null, null, null, -32127]
//     Output: 2
//     Explanation:
//       - At level 1, visit [989] giving level_sum = 989 (> INT64_MIN), so best_sum
//         becomes 989, best_level becomes 1, and next_level collects [10250].
//       - At level 2, visit [10250] giving level_sum = 10250 (> 989), so best_sum
//         updates to 10250, best_level updates to 2, and next_level collects
//         [98693, -89388].
//       - At level 3, visit [98693, -89388] giving level_sum = 9305 (< 10250),
//         keeping best_level = 2 and queueing [-32127] into next_level.
//       - At level 4, visit [-32127] giving level_sum = -32127 (< 10250) with an
//         empty next_level, ending the BFS and returning 2.
//
//   Example 3:
//     Input:  root = [-100, -20, -30, -20, -5, -10, null]
//     Output: 3
//     Explanation:
//       - Initialize best_sum = INT64_MIN so negative level sums can still improve
//         it, starting at level 1 with current_level = [-100].
//       - At level 1, level_sum = -100 > INT64_MIN sets best_sum = -100 and
//         best_level = 1, queueing [-20, -30] for level 2.
//       - At level 2, level_sum = -20 + (-30) = -50 > -100 updates best_sum = -50
//         and best_level = 2, queueing [-20, -5, -10] for level 3.
//       - At level 3, level_sum = -20 + (-5) + (-10) = -35 > -50 updates best_sum =
//         -35 and best_level = 3 with empty next_level, then returns 3.
//
// Algorithm:
//   Traverse the tree level by level with BFS. For each level, sum the values
//   of all nodes currently in the level frontier into a 64-bit accumulator
//   while collecting their non-null children for the next level. Update
//   `best_sum` and `best_level` strictly when `level_sum > best_sum` (not >=)
//   so ties automatically keep the smallest level index.
//   Time Complexity:  O(N), visiting every node once.
//   Space Complexity: O(W), where W <= N/2 + 1 is the maximum tree width.
//
// Data Structures:
//   - std::vector<const TreeNode*>: double-buffered current/next level
//     frontiers for cache-friendly level-order BFS without per-node queue
//     bookkeeping.
//
// Real-World Applications:
//   - Network topology and broadcast-tree analysis (identifying the hop depth
//     handling peak aggregate traffic or power draw).
//   - Memory-hierarchy / Merkle-tree cost profiling across tree depths.
//   - Hierarchical budget or workload rollups by management tier.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (standard level-by-level BFS)
//   - #637 Average of Levels in Binary Tree [done] (level-wise average aggregation)
//   - #515 Find Largest Value in Each Tree Row [done] (per-level maximum)
//   - #1120 Maximum Average Subtree [done] (subtree vs. level-based aggregation)
//   - #2583 Kth Largest Sum in a Binary Tree [new] (selects k-th largest level sum)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  TreeNode() = default;
  explicit TreeNode(int x) : val(x) {}
  TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

// RAII arena owning test TreeNode instances via pointer-stable std::deque.
class TreeArena {
 public:
  [[nodiscard]] TreeNode* make(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  // Builds a binary tree from LeetCode level-order array representation.
  [[nodiscard]] TreeNode* build(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;

    while (!q.empty() && idx < vals.size()) {
      auto* cur = q.front();
      q.pop();

      if (idx < vals.size() && vals[idx].has_value()) {
        cur->left = make(*vals[idx]);
        q.push(cur->left);
      }
      ++idx;

      if (idx < vals.size() && vals[idx].has_value()) {
        cur->right = make(*vals[idx]);
        q.push(cur->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns the 1-indexed smallest level with the maximum sum of node values.
  // Uses level-by-level BFS with strict `>` comparison to break ties in favor
  // of smaller level numbers. Time: O(N), Space: O(W).
  int maxLevelSum(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    std::vector<const TreeNode*> current_level{root};
    std::vector<const TreeNode*> next_level;

    std::int64_t best_sum = std::numeric_limits<std::int64_t>::min();
    int best_level = 1;
    int level = 1;

    while (!current_level.empty()) {
      std::int64_t level_sum = 0;
      next_level.clear();

      for (const auto* node : current_level) {
        level_sum += static_cast<std::int64_t>(node->val);
        if (node->left != nullptr) {
          next_level.push_back(node->left);
        }
        if (node->right != nullptr) {
          next_level.push_back(node->right);
        }
      }

      // Strictly greater ensures ties retain the earliest (smallest) level.
      if (level_sum > best_sum) {
        best_sum = level_sum;
        best_level = level;
      }

      std::swap(current_level, next_level);
      ++level;
    }

    return best_level;
  }
};

LC_PROBLEM(1161, "Maximum Level Sum Of A Binary Tree", "tree", ctx) {
  ctx.test("example_1", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.build({1, 7, 0, 7, -8, std::nullopt, std::nullopt});
    Solution sol;
    ctx.expect_eq(sol.maxLevelSum(root), 2);
  });

  ctx.test("example_2", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({989, std::nullopt, 10250, 98693, -89388,
                                        std::nullopt, std::nullopt,
                                        std::nullopt, -32127});
    Solution sol;
    ctx.expect_eq(sol.maxLevelSum(root), 2);
  });

  ctx.test("source_build_tie_returns_smallest_level", [&] {
    // Matches Python build(): level 1 = 1, level 2 = 7 + 0 = 7,
    // level 3 = 7, level 4 = -8 -> tie between levels 2 and 3 -> returns 2.
    TreeArena arena;
    TreeNode* n1 = arena.make(1);
    TreeNode* n7_a = arena.make(7);
    TreeNode* n0 = arena.make(0);
    TreeNode* n7_b = arena.make(7);
    TreeNode* n_neg8 = arena.make(-8);
    n1->left = n7_a;
    n1->right = n0;
    n7_a->left = n7_b;
    n7_b->right = n_neg8;

    Solution sol;
    ctx.expect_eq(sol.maxLevelSum(n1), 2);
  });

  ctx.test("all_negative_values", [&] {
    TreeArena arena;
    // Level 1 = -100, Level 2 = -20 + -30 = -50, Level 3 = -20 + -5 + -10 = -35.
    const TreeNode* root =
        arena.build({-100, -20, -30, -20, -5, -10, std::nullopt});
    Solution sol;
    ctx.expect_eq(sol.maxLevelSum(root), 3);
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    const TreeNode* root = arena.make(-42);
    Solution sol;
    ctx.expect_eq(sol.maxLevelSum(root), 1);
  });
}

}  // namespace
