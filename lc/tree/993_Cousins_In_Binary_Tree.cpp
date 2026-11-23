// LeetCode 993: Cousins In Binary Tree
// https://leetcode.com/problems/cousins-in-binary-tree/
//
// Problem:
//   Given the root of a binary tree with unique node values and two distinct
//   target values x and y, return true if and only if the nodes with values x
//   and y are cousins. Two nodes are cousins when they lie at the exact same
//   depth (root has depth 0) but have different parent nodes.
//   Constraints: 2 to 100 nodes; unique integer values in [1, 100].
//
// Algorithm:
//   Run a single depth-first search carrying each node's parent pointer and
//   current depth. Whenever a node's value matches x or y, record its
//   (depth, parent) metadata. Terminate traversal early as soon as both targets
//   have been discovered. Finally, return true iff both targets were found at
//   the same depth with distinct parent pointers.
//   Time Complexity:  O(N), visiting each node at most once.
//   Space Complexity: O(H) for the recursion stack, where H is the tree height
//                     (O(log N) balanced, O(N) skewed).
//
// Data Structures:
//   - std::vector<NodeInfo>: small fixed-capacity buffer (at most 2 entries)
//     storing the recorded (depth, parent pointer) pair for x and y.
//   - Call stack: tracks DFS traversal state without mutating the tree.
//
// Real-World Applications:
//   - Organizational hierarchy analysis (identifying peer-level roles across
//     different managers or departments).
//   - DOM / UI component tree queries checking whether two elements share the
//     same nesting depth under distinct containers.
//   - File-system and taxonomy comparisons verifying whether two entries reside
//     at the same directory depth in separate branches.
//
// Related LeetCode Problems:
//   - #2641 Cousins in Binary Tree II [new] (replaces node values with cousin sums)
//   - #102 Binary Tree Level Order Traversal [done] (BFS alternative for depth grouping)
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (ancestor relationships in trees)
//   - #1644 Lowest Common Ancestor Of A Binary Tree II [done] (target lookup with existence check)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
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

// RAII arena managing test TreeNode allocations without memory leaks.
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
  // Returns true iff nodes with values `x` and `y` have the same depth and
  // different parents in the binary tree rooted at `root`.
  // Time: O(N), Space: O(H).
  bool isCousins(const TreeNode* root, int x, int y) {
    if (root == nullptr) {
      return false;
    }

    struct NodeInfo {
      int depth = 0;
      const TreeNode* parent = nullptr;
    };

    // Stores discovery records for x and y (at most two entries because node
    // values are guaranteed unique).
    std::vector<NodeInfo> found;
    found.reserve(2uz);

    auto dfs = [&](this auto&& self, const TreeNode* node,
                   const TreeNode* parent, int depth) -> void {
      // Stop descending once we hit a null leaf or have already located both.
      if (node == nullptr || found.size() == 2uz) {
        return;
      }

      if (node->val == x || node->val == y) {
        found.push_back(NodeInfo{.depth = depth, .parent = parent});
        if (found.size() == 2uz) {
          return;
        }
      }

      self(node->left, node, depth + 1);
      self(node->right, node, depth + 1);
    };

    dfs(root, nullptr, 0);

    // Cousins must both exist, share the exact same depth, and have distinct
    // parents (comparing parent pointers avoids relying on sentinel values).
    return found.size() == 2uz && found[0].depth == found[1].depth &&
           found[0].parent != found[1].parent;
  }
};

LC_PROBLEM(993, "Cousins In Binary Tree", "tree", ctx) {
  ctx.test("example_1_different_depths", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({1, 2, 3, 4});
    Solution sol;
    ctx.expect_false(sol.isCousins(root, 4, 3));
  });

  ctx.test("example_2_and_source_build_valid_cousins", [&] {
    // Matches Example 2 and Python build(): [1, 2, 3, null, 4, null, 5].
    TreeArena arena;
    const TreeNode* root =
        arena.build({1, 2, 3, std::nullopt, 4, std::nullopt, 5});
    Solution sol;
    ctx.expect_true(sol.isCousins(root, 5, 4));
    ctx.expect_true(sol.isCousins(root, 4, 5));
  });

  ctx.test("example_3_siblings_are_not_cousins", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({1, 2, 3, std::nullopt, 4});
    Solution sol;
    ctx.expect_false(sol.isCousins(root, 2, 3));
  });

  ctx.test("root_cannot_be_cousin", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({1, 2, 3});
    Solution sol;
    ctx.expect_false(sol.isCousins(root, 1, 2));
  });

  ctx.test("deep_cousins_vs_deep_siblings", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.build({1, 2, 3, 4, 5, 6, 7});
    Solution sol;
    // 4 and 6 are at depth 2 with parents 2 and 3 respectively -> cousins.
    ctx.expect_true(sol.isCousins(root, 4, 6));
    // 4 and 5 share parent 2 -> siblings, not cousins.
    ctx.expect_false(sol.isCousins(root, 4, 5));
  });
}

}  // namespace
