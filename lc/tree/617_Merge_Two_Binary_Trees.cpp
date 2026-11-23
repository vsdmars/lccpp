// LeetCode 617: Merge Two Binary Trees
// https://leetcode.com/problems/merge-two-binary-trees/
//
// Problem:
//   Given the roots of two binary trees `root1` and `root2`, overlay them onto
//   each other to form a single merged binary tree. Whenever two nodes occupy
//   the same structural position, the merged node's value is the sum of both
//   node values. Whenever only one tree has a node at a given position, that
//   non-null node (and its structure) is used in the merged tree. Both trees
//   have between 0 and 2,000 nodes with values in [-10^4, 10^4].
//
// Algorithm:
//   1. Non-mutating recursive DFS (`mergeTrees`):
//      Simultaneously traverse `(r1, r2)` in pre-order. If both pointers are
//      null, return `nullptr`. Otherwise allocate a fresh node whose value is
//      the sum of the present node(s), and recursively build its left and right
//      children from `(r1->left, r2->left)` and `(r1->right, r2->right)`.
//      Time: O(N1 + N2), visiting every node in the union of both trees.
//      Space: O(N1 + N2) for the newly constructed tree plus O(max(H1, H2))
//      recursion stack.
//   2. In-place iterative DFS (`mergeTreesInPlace`):
//      Reuse `root1` as the destination tree and maintain an explicit stack of
//      overlapping `(n1, n2)` pairs. Whenever `n1` lacks a child that `n2` has,
//      attach `n2`'s entire subtree in O(1) without descending into it.
//      Time: O(min(N1, N2)), visiting only overlapping nodes.
//      Space: O(min(H1, H2)) average / O(min(N1, N2)) worst-case stack space.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`): represent hierarchical input and output.
//   - Call stack (recursive DFS) or `std::vector<std::pair<TreeNode*, TreeNode*>>`
//     explicit stack (iterative in-place DFS): tracks paired nodes to merge.
//
// Real-World Applications:
//   - Merging hierarchical configuration trees (e.g., JSON/YAML/Kubernetes
//     overrides where overlapping numeric metrics combine and missing branches
//     inherit defaults).
//   - Combining spatial quadtrees/octrees or BVH/scene-graph layers in game
//     engines and GIS rendering pipelines.
//   - Aggregating distributed flame-graph or call-tree profiling snapshots from
//     multiple worker hosts.
//
// Related LeetCode Problems:
//   - #100 Same Tree [done] (simultaneous dual-tree traversal checking equality)
//   - #101 Symmetric Tree [done] (paired DFS comparing mirrored subtrees)
//   - #226 Invert Binary Tree [done] (structural binary tree transformation)
//   - #572 Subtree of Another Tree [done] (recursive dual-tree matching)
//   - #872 Leaf-Similar Trees [new] (comparing structural summaries of two trees)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode binary tree node definition.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Merges `root1` and `root2` into a newly allocated binary tree without
  // mutating either input tree.
  // Time: O(N1 + N2), Space: O(N1 + N2) for output + O(max(H1, H2)) stack.
  TreeNode* mergeTrees(const TreeNode* root1, const TreeNode* root2) {
    // Base case: neither tree has a node at this position.
    if (root1 == nullptr && root2 == nullptr) {
      return nullptr;
    }

    // Sum whichever input nodes are present at this structural position.
    const int merged_val = (root1 != nullptr ? root1->val : 0) +
                           (root2 != nullptr ? root2->val : 0);

    // Advance each pointer only when its corresponding node exists.
    const TreeNode* left1 = root1 != nullptr ? root1->left : nullptr;
    const TreeNode* left2 = root2 != nullptr ? root2->left : nullptr;
    const TreeNode* right1 = root1 != nullptr ? root1->right : nullptr;
    const TreeNode* right2 = root2 != nullptr ? root2->right : nullptr;

    // Construct the merged node and recursively materialize both subtrees.
    auto* merged = new TreeNode(merged_val);
    merged->left = mergeTrees(left1, left2);
    merged->right = mergeTrees(right1, right2);
    return merged;
  }

  // Merges `root2` into `root1` in place using an explicit stack so only
  // overlapping regions are traversed. Non-overlapping subtrees of `root2` are
  // grafted directly onto `root1`.
  // Time: O(min(N1, N2)), Space: O(min(N1, N2)) worst-case stack.
  TreeNode* mergeTreesInPlace(TreeNode* root1, TreeNode* root2) {
    if (root1 == nullptr) {
      return root2;
    }
    if (root2 == nullptr) {
      return root1;
    }

    // Stack invariant: every pushed pair has a non-null `first` pointer.
    std::vector<std::pair<TreeNode*, TreeNode*>> stack;
    stack.emplace_back(root1, root2);

    while (!stack.empty()) {
      auto [n1, n2] = stack.back();
      stack.pop_back();

      if (n1 == nullptr || n2 == nullptr) {
        continue;
      }

      // Accumulate overlapping node values into `root1`.
      n1->val += n2->val;

      // Graft `n2`'s left subtree when `n1` has no left child; otherwise
      // schedule both left subtrees for further merging.
      if (n1->left == nullptr) {
        n1->left = n2->left;
      } else {
        stack.emplace_back(n1->left, n2->left);
      }

      // Apply the symmetric rule to the right children.
      if (n1->right == nullptr) {
        n1->right = n2->right;
      } else {
        stack.emplace_back(n1->right, n2->right);
      }
    }

    return root1;
  }
};

// RAII arena that owns test input nodes without requiring manual `delete`.
class TreeArena {
 public:
  // Builds a binary tree from LeetCode level-order notation (`std::nullopt`
  // denotes a missing child).
  TreeNode* build(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);

    std::size_t idx = 1;
    while (!q.empty() && idx < vals.size()) {
      TreeNode* curr = q.front();
      q.pop();

      if (vals[idx].has_value()) {
        curr->left = create(*vals[idx]);
        q.push(curr->left);
      }
      ++idx;

      if (idx < vals.size() && vals[idx].has_value()) {
        curr->right = create(*vals[idx]);
        q.push(curr->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  TreeNode* create(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  std::deque<TreeNode> nodes_;
};

// RAII owner for heap-allocated trees returned by `Solution::mergeTrees`.
class OwnedTree {
 public:
  explicit OwnedTree(TreeNode* root = nullptr) noexcept : root_(root) {}
  ~OwnedTree() { destroy(root_); }

  OwnedTree(const OwnedTree&) = delete;
  OwnedTree& operator=(const OwnedTree&) = delete;

  [[nodiscard]] TreeNode* get() const noexcept { return root_; }

 private:
  static void destroy(TreeNode* node) noexcept {
    if (node == nullptr) {
      return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
  }

  TreeNode* root_ = nullptr;
};

// Serializes a binary tree into trimmed LeetCode level-order string tokens.
std::vector<std::string> serialize(const TreeNode* root) {
  if (root == nullptr) {
    return {};
  }

  std::vector<std::string> out;
  std::queue<const TreeNode*> q;
  q.push(root);

  while (!q.empty()) {
    const TreeNode* curr = q.front();
    q.pop();
    if (curr == nullptr) {
      out.emplace_back("null");
    } else {
      out.push_back(std::to_string(curr->val));
      q.push(curr->left);
      q.push(curr->right);
    }
  }

  while (!out.empty() && out.back() == "null") {
    out.pop_back();
  }
  return out;
}

LC_PROBLEM(617, "Merge Two Binary Trees", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_overlapping_trees", [&] {
    TreeArena arena;
    TreeNode* r1 = arena.build({1, 3, 2, 5});
    TreeNode* r2 = arena.build({2, 1, 3, std::nullopt, 4, std::nullopt, 7});

    const std::vector<std::string> expected = {"3", "4", "5", "5", "4", "null", "7"};

    // Non-mutating recursive DFS preserves both original trees.
    OwnedTree merged(sol.mergeTrees(r1, r2));
    ctx.expect_eq(serialize(merged.get()), expected);
    ctx.expect_eq(serialize(r1), std::vector<std::string>{"1", "3", "2", "5"});
    ctx.expect_eq(serialize(r2),
                  std::vector<std::string>{"2", "1", "3", "null", "4", "null", "7"});

    // In-place iterative merge produces the identical merged structure.
    TreeNode* merged_in_place = sol.mergeTreesInPlace(r1, r2);
    ctx.expect_eq(serialize(merged_in_place), expected);
  });

  ctx.test("example_2_single_and_two_nodes", [&] {
    TreeArena arena;
    TreeNode* r1 = arena.build({1});
    TreeNode* r2 = arena.build({1, 2});

    const std::vector<std::string> expected = {"2", "2"};
    OwnedTree merged(sol.mergeTrees(r1, r2));
    ctx.expect_eq(serialize(merged.get()), expected);
    ctx.expect_eq(serialize(sol.mergeTreesInPlace(r1, r2)), expected);
  });

  ctx.test("empty_trees_edge_cases", [&] {
    TreeArena arena;
    TreeNode* non_empty = arena.build({10, -5, 20});
    const std::vector<std::string> expected = {"10", "-5", "20"};

    // Both empty.
    OwnedTree both_empty(sol.mergeTrees(nullptr, nullptr));
    ctx.expect_eq(both_empty.get(), nullptr);
    ctx.expect_eq(sol.mergeTreesInPlace(nullptr, nullptr), nullptr);

    // Left empty, right non-empty.
    OwnedTree left_empty(sol.mergeTrees(nullptr, non_empty));
    ctx.expect_eq(serialize(left_empty.get()), expected);
    ctx.expect_eq(serialize(sol.mergeTreesInPlace(nullptr, non_empty)), expected);

    // Left non-empty, right empty.
    OwnedTree right_empty(sol.mergeTrees(non_empty, nullptr));
    ctx.expect_eq(serialize(right_empty.get()), expected);
    ctx.expect_eq(serialize(sol.mergeTreesInPlace(non_empty, nullptr)), expected);
  });

  ctx.test("negative_values_and_disjoint_skewed_branches", [&] {
    TreeArena arena;
    TreeNode* left_skew = arena.build({-1, -2, std::nullopt, -3});
    TreeNode* right_skew = arena.build({1, std::nullopt, 2, std::nullopt, 3});

    const std::vector<std::string> expected = {"0", "-2", "2", "-3", "null", "null", "3"};
    OwnedTree merged(sol.mergeTrees(left_skew, right_skew));
    ctx.expect_eq(serialize(merged.get()), expected);
    ctx.expect_eq(serialize(sol.mergeTreesInPlace(left_skew, right_skew)), expected);
  });
}

}  // namespace
