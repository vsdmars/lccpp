// LeetCode 623: Add One Row To Tree
// https://leetcode.com/problems/add-one-row-to-tree/
//
// Problem:
//   Given the root of a binary tree, an integer `val`, and a 1-indexed positive
//   integer `depth`, insert a new row of nodes with value `val` at depth
//   `depth`. The original root is at depth 1.
//   - If `depth == 1`, create a new node with value `val` as the new root and
//     attach the entire original tree as its left subtree.
//   - Otherwise, for every non-null node `cur` at depth `depth - 1`, create two
//     new nodes with value `val`: make one `cur->left` with `cur`'s original
//     left subtree as its left child, and make the other `cur->right` with
//     `cur`'s original right subtree as its right child.
//
// Examples:
//   Example 1:
//     Input:  root = [4, 2, 6, 3, 1, 5], val = 1, depth = 2
//     Output: [4, 1, 1, 2, null, null, 6, 3, 1, 5]
//     Explanation:
//       - Since depth == 2 > 1, initialize current_level = [4] at curr_depth = 1;
//         the BFS loop condition (curr_depth < 1) does not advance further.
//       - Splice new nodes onto parent 4: parent->left = new TreeNode(1, 2,
//         nullptr) and parent->right = new TreeNode(1, nullptr, 6).
//       - Original subtrees rooted at 2 (with children 3, 1) and 6 (with left child
//         5) stay on the outer left and right edges, producing [4, 1, 1, 2, null,
//         null, 6, 3, 1, 5].
//
//   Example 2:
//     Input:  root = [4, 2, null, 3, 1], val = 1, depth = 3
//     Output: [4, 2, null, 1, 1, 3, null, null, 1]
//     Explanation:
//       - Start with current_level = [4] at curr_depth = 1, then advance one BFS
//         step (curr_depth < 2) to collect depth-2 frontier current_level = [2].
//       - For node 2 at depth 2, set 2->left = new TreeNode(1, 3, nullptr) and
//         2->right = new TreeNode(1, nullptr, 1).
//       - Level-order traversal yields [4, 2, null, 1, 1, 3, null, null, 1].
//
//   Example 3:
//     Input:  root = [4, 2, 6], val = 9, depth = 1
//     Output: [9, 4, null, 2, 6]
//     Explanation:
//       - Trigger the depth == 1 base case before entering the BFS loop.
//       - Allocate new TreeNode(9, root, nullptr) so the original tree [4, 2, 6]
//         becomes the left subtree of 9.
//       - Return the new root 9, whose level-order serialization is [9, 4, null, 2,
//         6].
//
// Algorithm:
//   Use breadth-first search (level-order traversal) to collect all nodes at
//   depth `depth - 1`, then perform pointer splicing on that frontier.
//   - Handle `depth == 1` directly as a base case (avoids a dummy sentinel that
//     would otherwise allocate an unused right child).
//   - Starting from `level = {root}` (depth 1), advance `depth - 2` levels by
//     expanding each node's non-null children into the next level vector.
//   - For every node remaining in `level` (all nodes at depth `depth - 1`),
//     splice new left and right wrapper nodes around the existing child links.
//   Time: O(N), where N is the number of nodes in the tree (at most every node
//   above `depth` is visited once).
//   Space: O(W), where W <= N is the maximum width of the tree up to `depth - 1`.
//
// Data Structures:
//   - `std::vector<TreeNode*>` (double-buffered BFS frontier): holds all nodes
//     at the current depth without per-element queue overhead.
//   - Binary tree nodes (`TreeNode`): updated in place via pointer rewiring.
//
// Real-World Applications:
//   - Inserting intermediate routing, middleware, or proxy layers into a
//     hierarchical network topology or service dependency tree at a fixed tier.
//   - Refactoring AST / DOM / UI layout trees by wrapping all nodes at a given
//     nesting depth in a container or instrumentation node.
//   - Adding a uniform intermediate aggregation level in organizational or
//     billing hierarchy trees.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (core BFS level-by-level scan)
//   - #116 Populating Next Right Pointers in Each Node [done] (level-wise tree rewiring)
//   - #515 Find Largest Value in Each Tree Row [done] (per-depth BFS processing)
//   - #637 Average of Levels in Binary Tree [done] (level-order frontier iteration)

#include "framework/lc_framework.h"

#include <cstddef>
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
  // Inserts a full row of nodes with value `val` at 1-indexed `depth`.
  // Time: O(N), Space: O(W) where W is the widest level up to `depth - 1`.
  TreeNode* addOneRow(TreeNode* root, int val, int depth) {
    // Special case: depth 1 wraps the entire original tree as the left child
    // of a new root node.
    if (depth == 1) {
      return new TreeNode(val, root, nullptr);
    }

    // Nothing to modify if the tree is empty and `depth > 1`.
    if (root == nullptr) {
      return nullptr;
    }

    // Walk level by level from depth 1 down to depth `depth - 1`.
    std::vector<TreeNode*> current_level = {root};
    for (int curr_depth = 1; curr_depth < depth - 1 && !current_level.empty();
         ++curr_depth) {
      std::vector<TreeNode*> next_level;
      next_level.reserve(current_level.size() * 2uz);
      for (TreeNode* node : current_level) {
        if (node->left != nullptr) {
          next_level.push_back(node->left);
        }
        if (node->right != nullptr) {
          next_level.push_back(node->right);
        }
      }
      current_level = std::move(next_level);
    }

    // Every node in `current_level` sits at depth `depth - 1`. Insert new
    // `val` nodes on both sides while preserving original subtrees on their
    // corresponding outer edges.
    for (TreeNode* parent : current_level) {
      parent->left = new TreeNode(val, parent->left, nullptr);
      parent->right = new TreeNode(val, nullptr, parent->right);
    }

    return root;
  }
};

// RAII owner for heap-allocated binary trees so tests never leak memory,
// including nodes spliced in by `Solution::addOneRow`.
class OwnedTree {
 public:
  explicit OwnedTree(TreeNode* root = nullptr) noexcept : root_(root) {}
  ~OwnedTree() { destroy(root_); }

  OwnedTree(const OwnedTree&) = delete;
  OwnedTree& operator=(const OwnedTree&) = delete;
  OwnedTree(OwnedTree&& other) noexcept
      : root_(std::exchange(other.root_, nullptr)) {}
  OwnedTree& operator=(OwnedTree&& other) noexcept {
    if (this != &other) {
      destroy(root_);
      root_ = std::exchange(other.root_, nullptr);
    }
    return *this;
  }

  [[nodiscard]] TreeNode* get() const noexcept { return root_; }

  // Transfers ownership out to a mutating API; caller must re-wrap the result.
  TreeNode* release() noexcept { return std::exchange(root_, nullptr); }

  void reset(TreeNode* new_root = nullptr) noexcept {
    destroy(root_);
    root_ = new_root;
  }

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

// Builds a heap-allocated `OwnedTree` from LeetCode level-order notation.
// Every newly allocated node is immediately attached to `tree` so even partial
// failure stays leak-free.
OwnedTree build_tree(const std::vector<std::optional<int>>& vals) {
  if (vals.empty() || !vals[0].has_value()) {
    return OwnedTree{nullptr};
  }

  OwnedTree tree(new TreeNode(*vals[0]));
  std::queue<TreeNode*> q;
  q.push(tree.get());

  std::size_t idx = 1;
  while (!q.empty() && idx < vals.size()) {
    TreeNode* curr = q.front();
    q.pop();

    if (vals[idx].has_value()) {
      curr->left = new TreeNode(*vals[idx]);
      q.push(curr->left);
    }
    ++idx;

    if (idx < vals.size() && vals[idx].has_value()) {
      curr->right = new TreeNode(*vals[idx]);
      q.push(curr->right);
    }
    ++idx;
  }
  return tree;
}

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

LC_PROBLEM(623, "Add One Row To Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_depth_2", [&] {
    OwnedTree tree = build_tree({4, 2, 6, 3, 1, 5});
    tree.reset(sol.addOneRow(tree.release(), 1, 2));
    ctx.expect_eq(
        serialize(tree.get()),
        std::vector<std::string>{"4", "1", "1", "2", "null", "null", "6", "3", "1", "5"});
  });

  ctx.test("example_2_depth_3", [&] {
    OwnedTree tree = build_tree({4, 2, std::nullopt, 3, 1});
    tree.reset(sol.addOneRow(tree.release(), 1, 3));
    ctx.expect_eq(
        serialize(tree.get()),
        std::vector<std::string>{"4", "2", "null", "1", "1", "3", "null", "null", "1"});
  });

  ctx.test("insert_below_leaves_depth_4", [&] {
    // Matches the Python `build()` test: [1, 2, 3, 4], val=5, depth=4.
    OwnedTree tree = build_tree({1, 2, 3, 4});
    tree.reset(sol.addOneRow(tree.release(), 5, 4));
    ctx.expect_eq(
        serialize(tree.get()),
        std::vector<std::string>{"1", "2", "3", "4", "null", "null", "null", "5", "5"});
  });

  ctx.test("depth_1_replaces_root", [&] {
    OwnedTree tree = build_tree({4, 2, 6});
    tree.reset(sol.addOneRow(tree.release(), 9, 1));
    ctx.expect_eq(serialize(tree.get()),
                  std::vector<std::string>{"9", "4", "null", "2", "6"});
  });

  ctx.test("single_node_insert_at_depth_2", [&] {
    OwnedTree tree = build_tree({1});
    tree.reset(sol.addOneRow(tree.release(), 2, 2));
    ctx.expect_eq(serialize(tree.get()), std::vector<std::string>{"1", "2", "2"});
  });

  ctx.test("empty_tree_depth_gt_1", [&] {
    OwnedTree tree(sol.addOneRow(nullptr, 7, 2));
    ctx.expect_eq(tree.get(), nullptr);
  });
}

}  // namespace
