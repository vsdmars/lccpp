// LeetCode 1644: Lowest Common Ancestor Of A Binary Tree II
// https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-tree-ii/
//
// Problem:
//   Given the root of a binary tree and two target nodes p and q (with p != q
//   and all node values unique), return their lowest common ancestor (LCA),
//   where a node is allowed to be a descendant of itself. Unlike LeetCode #236,
//   either p or q (or both) may NOT exist in the tree; in that case return
//   nullptr.
//   Constraints: 1 to 10^4 nodes; -10^9 <= Node.val <= 10^9.
//
// Examples:
//   Example 1:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 1
//     Output: 3 (TreeNode with val 3)
//     Explanation:
//       - Phase 1 DFS traverses all 9 nodes from root 3 and populates parent
//         pointers such as parent[3] = nullptr, parent[5] = 3, and parent[1] = 3.
//       - Phase 2 checks parent.contains(p) and parent.contains(q); both node 5 and
//         node 1 are present in the map.
//       - Phase 3 walks upward from p = 5 via parent pointers, inserting {5, 3}
//         into the ancestors set.
//       - Phase 4 climbs from q = 1: node 1 is not in ancestors, so it steps to
//         parent[1] = 3, finds 3 in ancestors, and returns node 3.
//
//   Example 2:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 4
//     Output: 5 (TreeNode with val 5)
//     Explanation:
//       - Phase 1 DFS populates parent for all 9 nodes, and Phase 2 confirms both
//         node 5 and node 4 exist in parent.
//       - Phase 3 collects ancestors of p = 5 (including p itself), storing {5, 3}
//         in ancestors.
//       - Phase 4 walks upward from q = 4: node 4 misses, parent[4] = 2 misses, and
//         parent[2] = 5 matches ancestors, returning node 5.
//
//   Example 3:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 10
//             (detached node)
//     Output: nullptr
//     Explanation:
//       - Phase 1 DFS visits only the 9 nodes reachable from root 3, so parent
//         holds {3, 5, 1, 6, 2, 0, 8, 7, 4}.
//       - Phase 2 evaluates parent.contains(p) && parent.contains(q) and finds that
//         detached node 10 is absent from parent.
//       - Because both target nodes must belong to the tree, lowestCommonAncestor
//         immediately returns nullptr.
//
// Algorithm:
//   Traverse the entire tree once with DFS to populate a parent-pointer map
//   `parent[node] = parent_node` (with `parent[root] = nullptr`). Because every
//   node in the tree is inserted as a key, checking membership of both `p` and
//   `q` in `parent` immediately verifies that both exist in the tree (returning
//   nullptr otherwise). Next, walk from `p` up to the root, inserting every
//   ancestor into an `unordered_set`. Finally, walk from `q` up to the root;
//   the first ancestor present in `p`'s ancestor set is guaranteed to be the
//   lowest common ancestor. (Note: a single-pass full post-order DFS with two
//   found flags is an alternative O(H)-space approach.)
//   Time Complexity:  O(N), traversing the tree once and walking up to H steps.
//   Space Complexity: O(N) for the parent map, ancestor set, and recursion
//                     stack.
//
// Data Structures:
//   - std::unordered_map<TreeNode*, TreeNode*>: maps each node in the tree to
//     its unique parent pointer and simultaneously serves as a membership set.
//   - std::unordered_set<TreeNode*>: stores all ancestors on the path from p
//     to root for O(1) average-time intersection lookup while climbing from q.
//
// Real-World Applications:
//   - Git merge-base resolution in commit trees where one requested reference
//     may belong to an unrelated or orphaned history.
//   - DOM event-bubbling and style-scoping queries verifying that two element
//     handles belong to the same mounted document before computing their LCA.
//   - File-system path normalization and permission inheritance across mounted
//     directory trees.
//
// Related LeetCode Problems:
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (guarantees both p and q exist)
//   - #235 Lowest Common Ancestor of a Binary Search Tree [done] (exploits BST ordering)
//   - #1650 Lowest Common Ancestor of a Binary Tree III [done] (nodes already carry parent pointers)
//   - #1676 Lowest Common Ancestor of a Binary Tree IV [new] (LCA of k target nodes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
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

  // Builds a binary tree from LeetCode level-order representation and records
  // value-to-node lookup for concise test setup.
  [[nodiscard]] TreeNode* build(const std::vector<std::optional<int>>& vals,
                                std::unordered_map<int, TreeNode*>& by_val) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*vals[0]);
    by_val[*vals[0]] = root;
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;

    while (!q.empty() && idx < vals.size()) {
      auto* cur = q.front();
      q.pop();

      if (idx < vals.size() && vals[idx].has_value()) {
        cur->left = make(*vals[idx]);
        by_val[*vals[idx]] = cur->left;
        q.push(cur->left);
      }
      ++idx;

      if (idx < vals.size() && vals[idx].has_value()) {
        cur->right = make(*vals[idx]);
        by_val[*vals[idx]] = cur->right;
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
  // Returns the lowest common ancestor of `p` and `q` in the binary tree
  // rooted at `root`, or nullptr if either `p` or `q` is not in the tree.
  // Time: O(N), Space: O(N).
  TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (root == nullptr || p == nullptr || q == nullptr) {
      return nullptr;
    }

    // Phase 1: Build parent pointer map across the entire tree so we know both
    // node membership and upward links for every node.
    std::unordered_map<TreeNode*, TreeNode*> parent;
    parent.emplace(root, nullptr);

    auto build_parent_map = [&](this auto&& self, TreeNode* node) -> void {
      if (node->left != nullptr) {
        parent.emplace(node->left, node);
        self(node->left);
      }
      if (node->right != nullptr) {
        parent.emplace(node->right, node);
        self(node->right);
      }
    };

    build_parent_map(root);

    // Phase 2: Verify both target nodes actually belong to this tree.
    if (!parent.contains(p) || !parent.contains(q)) {
      return nullptr;
    }

    // Phase 3: Collect all ancestors of `p` (including `p` itself).
    std::unordered_set<TreeNode*> ancestors;
    for (TreeNode* cur = p; cur != nullptr; cur = parent[cur]) {
      ancestors.insert(cur);
    }

    // Phase 4: Climb from `q`; the first hit in `ancestors` is the LCA.
    for (TreeNode* cur = q; cur != nullptr; cur = parent[cur]) {
      if (ancestors.contains(cur)) {
        return cur;
      }
    }

    return nullptr;
  }
};

LC_PROBLEM(1644, "Lowest Common Ancestor Of A Binary Tree II", "tree", ctx) {
  ctx.test("example_1_different_subtrees", [&] {
    TreeArena arena;
    std::unordered_map<int, TreeNode*> by_val;
    TreeNode* root = arena.build(
        {3, 5, 1, 6, 2, 0, 8, std::nullopt, std::nullopt, 7, 4}, by_val);
    Solution sol;
    ctx.expect_eq(sol.lowestCommonAncestor(root, by_val[5], by_val[1]),
                  by_val[3]);
  });

  ctx.test("example_2_node_is_ancestor_of_other", [&] {
    TreeArena arena;
    std::unordered_map<int, TreeNode*> by_val;
    TreeNode* root = arena.build(
        {3, 5, 1, 6, 2, 0, 8, std::nullopt, std::nullopt, 7, 4}, by_val);
    Solution sol;
    ctx.expect_eq(sol.lowestCommonAncestor(root, by_val[5], by_val[4]),
                  by_val[5]);
  });

  ctx.test("example_3_one_node_missing", [&] {
    TreeArena arena;
    std::unordered_map<int, TreeNode*> by_val;
    TreeNode* root = arena.build(
        {3, 5, 1, 6, 2, 0, 8, std::nullopt, std::nullopt, 7, 4}, by_val);
    TreeNode* detached = arena.make(10);
    Solution sol;
    ctx.expect_eq(sol.lowestCommonAncestor(root, by_val[5], detached), nullptr);
    ctx.expect_eq(sol.lowestCommonAncestor(root, detached, by_val[5]), nullptr);
  });

  ctx.test("source_build_fixture", [&] {
    // Matches Python build():
    //       1
    //      / \
    //     2   5
    //    / \
    //   4   3
    // Detached nodes: 6, 7.
    TreeArena arena;
    TreeNode* n1 = arena.make(1);
    TreeNode* n2 = arena.make(2);
    TreeNode* n3 = arena.make(3);
    TreeNode* n4 = arena.make(4);
    TreeNode* n5 = arena.make(5);
    TreeNode* n6 = arena.make(6);
    TreeNode* n7 = arena.make(7);
    n1->left = n2;
    n1->right = n5;
    n2->left = n4;
    n2->right = n3;

    Solution sol;
    ctx.expect_eq(sol.lowestCommonAncestor(n1, n5, n4), n1);
    ctx.expect_eq(sol.lowestCommonAncestor(n1, n4, n3), n2);
    ctx.expect_eq(sol.lowestCommonAncestor(n1, n6, n7), nullptr);
  });

  ctx.test("null_root_and_root_as_target", [&] {
    TreeArena arena;
    TreeNode* n1 = arena.make(1);
    TreeNode* n2 = arena.make(2);
    n1->left = n2;

    Solution sol;
    ctx.expect_eq(sol.lowestCommonAncestor(nullptr, n1, n2), nullptr);
    ctx.expect_eq(sol.lowestCommonAncestor(n1, n1, n2), n1);
  });
}

}  // namespace
