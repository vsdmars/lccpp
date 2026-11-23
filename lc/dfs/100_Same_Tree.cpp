// LeetCode 100: Same Tree
// https://leetcode.com/problems/same-tree/
//
// Problem:
//   Given the roots `p` and `q` of two binary trees, return true if the two
//   trees are structurally identical and every pair of corresponding nodes
//   holds the same integer value; otherwise return false.
//
// Examples:
//   Example 1:
//     Input:  p = [1, 2, 3], q = [1, 2, 3]
//     Output: true
//     Explanation:
//       - Compare roots p (val 1) and q (val 1): both are non-null with matching
//         values, so recurse into left and right children.
//       - Recurse left to p->left (val 2) and q->left (val 2): values match, and
//         their null left/right child pairs both hit the base case (p == nullptr &&
//         q == nullptr) returning true.
//       - Recurse right to p->right (val 3) and q->right (val 3): values match, and
//         their null child pairs return true.
//       - Combine left (true) && right (true) at the root to return true.
//
//   Example 2:
//     Input:  p = [1, 2], q = [1, null, 2]
//     Output: false
//     Explanation:
//       - Compare roots p (val 1) and q (val 1): both are non-null with equal
//         values, so check left subtrees next.
//       - Recurse left to p->left (val 2) and q->left (nullptr): one pointer is
//         null while the other is non-null, returning false immediately.
//       - Short-circuit && at the root without inspecting the right subtrees and
//         return false.
//
//   Example 3:
//     Input:  p = [1, 2, 1], q = [1, 1, 2]
//     Output: false
//     Explanation:
//       - Compare roots p (val 1) and q (val 1): values match, so recurse into the
//         left subtrees.
//       - Compare p->left (val 2) and q->left (val 1): p->val != q->val triggers an
//         immediate false return.
//       - Short-circuit the && conjunction at the root and return false.
//
// Algorithm:
//   Simultaneous pre-order Depth-First Search (DFS) over both trees.
//   At each pair of nodes (p, q):
//     - If both pointers are null, the empty subtrees match -> return true.
//     - If only one pointer is null or p->val != q->val, the trees diverge ->
//       return false immediately (short-circuiting remaining subtrees).
//     - Otherwise recursively verify both left subtrees and both right
//       subtrees match.
//   Comparing nodes directly avoids the extra allocation and serialization
//   cost of encoding both trees into strings/arrays.
//   Time Complexity:  O(min(N, M)), visiting each aligned node pair at most
//                     once until the first mismatch or full traversal.
//   Space Complexity: O(min(H_p, H_q)) recursion stack space (O(log N) for
//                     balanced trees, O(N) worst case for skewed trees).
//
// Data Structures:
//   - Binary tree nodes (`TreeNode` with value, left, and right pointers).
//   - Implicit DFS recursion stack holding paired node cursors.
//   - RAII `TreeArena` (`std::deque<TreeNode>`) in tests for leak-free node
//     allocation with stable pointers.
//
// Real-World Applications:
//   - Abstract Syntax Tree (AST) equivalence checking in compilers, linters,
//     and refactoring tools (e.g., common subexpression / clone detection).
//   - Virtual DOM diffing and UI reconciliation in frontend frameworks.
//   - Merkle tree and hierarchical configuration diffing in distributed
//     storage and version-control systems.
//
// Related LeetCode Problems:
//   - #101 Symmetric Tree [done] (mirror-image structural DFS comparison)
//   - #572 Subtree of Another Tree [done] (uses Same Tree check per root)
//   - #104 Maximum Depth Of Binary Tree [done] (foundational binary tree DFS)
//   - #951 Flip Equivalent Binary Trees [done] (same tree allowing child swaps)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

// RAII arena that owns TreeNode instances in a std::deque so raw pointers
// stay valid as new nodes are appended and are automatically freed on exit.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    TreeNode& node = nodes_.emplace_back();
    node.val = val;
    node.left = left;
    node.right = right;
    return &node;
  }

  // Constructs a binary tree from LeetCode's level-order array representation.
  TreeNode* fromLevelOrder(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;
    while (!q.empty() && idx < vals.size()) {
      TreeNode* cur = q.front();
      q.pop();
      if (vals[idx].has_value()) {
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
  // Checks whether trees rooted at `p` and `q` have identical structure and
  // node values via lockstep pre-order DFS.
  // Time: O(min(N, M)), Space: O(min(H_p, H_q)) recursion stack.
  bool isSameTree(const TreeNode* p, const TreeNode* q) {
    // Base case: both subtrees are empty.
    if (p == nullptr && q == nullptr) {
      return true;
    }
    // Structural mismatch (one null, one non-null) or value mismatch.
    if (p == nullptr || q == nullptr || p->val != q->val) {
      return false;
    }
    // Both left and right subtrees must match recursively.
    return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
  }
};

LC_PROBLEM(100, "Same Tree", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_identical", [&] {
    TreeArena arena;
    const TreeNode* p = arena.fromLevelOrder({1, 2, 3});
    const TreeNode* q = arena.fromLevelOrder({1, 2, 3});
    ctx.expect_true(sol.isSameTree(p, q));
  });

  ctx.test("example_2_different_structure", [&] {
    TreeArena arena;
    const TreeNode* p = arena.fromLevelOrder({1, 2});
    const TreeNode* q = arena.fromLevelOrder({1, std::nullopt, 2});
    ctx.expect_false(sol.isSameTree(p, q));
  });

  ctx.test("example_3_different_values", [&] {
    TreeArena arena;
    const TreeNode* p = arena.fromLevelOrder({1, 2, 1});
    const TreeNode* q = arena.fromLevelOrder({1, 1, 2});
    ctx.expect_false(sol.isSameTree(p, q));
  });

  ctx.test("python_build_deep_leaf_diff", [&] {
    TreeArena arena;
    const TreeNode* p = arena.fromLevelOrder({1, 2, 2, 3, 4, 4, 3});
    const TreeNode* q = arena.fromLevelOrder({1, 2, 2, 3, 4, 2, 3});
    ctx.expect_false(sol.isSameTree(p, q));
  });

  ctx.test("empty_trees", [&] {
    TreeArena arena;
    const TreeNode* single = arena.make(42);
    ctx.expect_true(sol.isSameTree(nullptr, nullptr));
    ctx.expect_false(sol.isSameTree(single, nullptr));
    ctx.expect_false(sol.isSameTree(nullptr, single));
  });
}

}  // namespace
