// LeetCode 156: Binary Tree Upside Down
// https://leetcode.com/problems/binary-tree-upside-down/
//
// Problem:
//   Given the root of a binary tree where every right node is either a leaf
//   node with a sibling (a left node sharing the same parent) or empty, flip
//   the tree upside down so that:
//     1. The original left child becomes the new root.
//     2. The original right child becomes the new left child (a leaf).
//     3. The original root becomes the new right child.
//   Return the root of the transformed tree.
//
// Algorithm:
//   1. Recursive left-spine reversal (primary):
//      Base case: an empty tree or a single node with no left child is already
//      flipped, so return `root`. Otherwise, recursively flip `root->left` to
//      obtain `new_root` (the leftmost leaf of the original tree). Key
//      invariant: after flipping `root->left`, `root->left` itself becomes the
//      rightmost leaf of the flipped left subtree. Therefore we rewire in O(1)
//      without scanning for the rightmost node:
//        root->left->left  = root->right;
//        root->left->right = root;
//        root->left        = nullptr;
//        root->right       = nullptr;
//      Time Complexity:  O(n) (visits each left-spine node once).
//      Space Complexity: O(h) recursion stack where h is tree height (O(n)
//                        worst case for a left-skewed tree).
//   2. Iterative level-order + stack reversal (secondary):
//      Collect each level `[left_node, optional_right_leaf]` from top to bottom
//      via BFS onto a stack, then pop levels from bottom to top, linking each
//      level's left node as the right child of the previous level's node and
//      attaching its sibling as the new left child.
//      Time Complexity:  O(n).
//      Space Complexity: O(n) to hold all levels on the stack.
//
// Data Structures:
//   - Binary tree (`TreeNode` pointers) rewired strictly in-place.
//   - Implicit call stack (recursive approach) or explicit `std::vector` stack
//     of levels (iterative approach).
//
// Real-World Applications:
//   - Re-rooting and restructuring left-leaning AST / expression chains in
//     compilers (converting left-associative parse trees into right-associative
//     evaluation chains).
//   - Reversing hierarchical parent-child linked views (e.g., undo history
//     spines or breadcrumb trees with attached metadata leaves).
//   - In-place pointer rotation primitives in balanced tree libraries.
//
// Related LeetCode Problems:
//   - #206 Reverse Linked List [done] (in-place pointer reversal on a linear spine)
//   - #226 Invert Binary Tree [done] (structural in-place binary tree mirror transformation)
//   - #114 Flatten Binary Tree to Linked List [done] (in-place tree rewiring into a right-skewed spine)
//   - #199 Binary Tree Right Side View [done] (level-order tree traversal and edge inspection)

#include "framework/lc_framework.h"

#include <deque>
#include <optional>
#include <utility>
#include <vector>

namespace {

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Recursively flips a valid upside-down binary tree in-place along its left
  // spine and returns the new root (original leftmost node).
  // Time: O(n), Space: O(h) recursion stack.
  TreeNode* upsideDownBinaryTree(TreeNode* root) {
    // Base case: empty tree or reached the leftmost node, which becomes the
    // new root of the entire flipped tree.
    if (root == nullptr || root->left == nullptr) {
      return root;
    }

    // Recursively flip the left subtree; `new_root` stays fixed as the
    // original leftmost leaf and propagates all the way back up.
    TreeNode* new_root = upsideDownBinaryTree(root->left);

    // Invariant: `root->left` is now the rightmost node of the flipped left
    // subtree. Attach `root->right` as its new left child and `root` as its
    // new right child.
    root->left->left = root->right;
    root->left->right = root;

    // Clear `root`'s old child links so the original root becomes a proper leaf
    // (no dangling cycles).
    root->left = nullptr;
    root->right = nullptr;

    return new_root;
  }

  // Iteratively flips the binary tree upside down by recording BFS levels onto
  // a stack and rewiring them bottom-up in-place.
  // Time: O(n), Space: O(n).
  TreeNode* upsideDownBinaryTreeIterative(TreeNode* root) {
    if (root == nullptr) {
      return nullptr;
    }

    // Phase 1: BFS level-by-level. Because every right child is a leaf, each
    // level contains at most two nodes: [left_spine_node, optional_right_leaf].
    std::vector<std::vector<TreeNode*>> levels;
    std::vector<TreeNode*> current_level = {root};
    while (!current_level.empty()) {
      levels.push_back(current_level);
      std::vector<TreeNode*> next_level;
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

    // Phase 2: Pop levels from bottom to top, chaining each level's primary
    // node along the right spine and attaching its sibling as the left leaf.
    TreeNode* new_root = nullptr;
    TreeNode* prev = nullptr;
    while (!levels.empty()) {
      const auto level = std::move(levels.back());
      levels.pop_back();

      TreeNode* spine_node = level[0];
      if (prev != nullptr) {
        prev->right = spine_node;
      } else {
        new_root = spine_node;
      }

      // If this level had a right sibling, it becomes the new left child;
      // otherwise clear the old left link.
      spine_node->left = level.size() > 1 ? level[1] : nullptr;
      prev = spine_node;
    }

    // Terminate the new rightmost leaf to avoid cycles back to old children.
    if (prev != nullptr) {
      prev->right = nullptr;
    }

    return new_root;
  }
};

// Serializes a binary tree into trimmed level-order format using std::nullopt
// for missing children so test assertions verify exact topology and values.
std::vector<std::optional<int>> to_level_order(const TreeNode* root) {
  std::vector<std::optional<int>> out;
  if (root == nullptr) {
    return out;
  }

  std::deque<const TreeNode*> q;
  q.push_back(root);
  while (!q.empty()) {
    const TreeNode* node = q.front();
    q.pop_front();
    if (node == nullptr) {
      out.emplace_back(std::nullopt);
      continue;
    }
    out.emplace_back(node->val);
    q.push_back(node->left);
    q.push_back(node->right);
  }

  // Trim trailing nulls to match canonical LeetCode level-order encoding.
  while (!out.empty() && !out.back().has_value()) {
    out.pop_back();
  }
  return out;
}

LC_PROBLEM(156, "Binary Tree Upside Down", "tree", ctx) {
  Solution sol;

  ctx.test("example_12345_recursive", [&] {
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->right = make(3);
    root->left->left = make(4);
    root->left->right = make(5);

    TreeNode* flipped = sol.upsideDownBinaryTree(root);
    const std::vector<std::optional<int>> expected = {
        4, 5, 2, std::nullopt, std::nullopt, 3, 1};
    ctx.expect_true(to_level_order(flipped) == expected);
  });

  ctx.test("example_12345_iterative", [&] {
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->right = make(3);
    root->left->left = make(4);
    root->left->right = make(5);

    TreeNode* flipped = sol.upsideDownBinaryTreeIterative(root);
    const std::vector<std::optional<int>> expected = {
        4, 5, 2, std::nullopt, std::nullopt, 3, 1};
    ctx.expect_true(to_level_order(flipped) == expected);
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(sol.upsideDownBinaryTree(nullptr), nullptr);
    ctx.expect_eq(sol.upsideDownBinaryTreeIterative(nullptr), nullptr);

    TreeNode single(7);
    TreeNode* r1 = sol.upsideDownBinaryTree(&single);
    ctx.expect_true(to_level_order(r1) == std::vector<std::optional<int>>{7});

    TreeNode single2(9);
    TreeNode* r2 = sol.upsideDownBinaryTreeIterative(&single2);
    ctx.expect_true(to_level_order(r2) == std::vector<std::optional<int>>{9});
  });

  ctx.test("left_skewed_no_right_children", [&] {
    std::deque<TreeNode> arena1;
    auto make1 = [&](int v) -> TreeNode* { return &arena1.emplace_back(v); };
    TreeNode* root1 = make1(1);
    root1->left = make1(2);
    root1->left->left = make1(3);

    std::deque<TreeNode> arena2;
    auto make2 = [&](int v) -> TreeNode* { return &arena2.emplace_back(v); };
    TreeNode* root2 = make2(1);
    root2->left = make2(2);
    root2->left->left = make2(3);

    const std::vector<std::optional<int>> expected = {
        3, std::nullopt, 2, std::nullopt, 1};
    ctx.expect_true(to_level_order(sol.upsideDownBinaryTree(root1)) ==
                    expected);
    ctx.expect_true(to_level_order(sol.upsideDownBinaryTreeIterative(root2)) ==
                    expected);
  });

  ctx.test("missing_right_child_at_root", [&] {
    std::deque<TreeNode> arena1;
    auto make1 = [&](int v) -> TreeNode* { return &arena1.emplace_back(v); };
    TreeNode* root1 = make1(1);
    root1->left = make1(2);
    root1->left->left = make1(4);
    root1->left->right = make1(5);

    std::deque<TreeNode> arena2;
    auto make2 = [&](int v) -> TreeNode* { return &arena2.emplace_back(v); };
    TreeNode* root2 = make2(1);
    root2->left = make2(2);
    root2->left->left = make2(4);
    root2->left->right = make2(5);

    const std::vector<std::optional<int>> expected = {
        4, 5, 2, std::nullopt, std::nullopt, std::nullopt, 1};
    ctx.expect_true(to_level_order(sol.upsideDownBinaryTree(root1)) ==
                    expected);
    ctx.expect_true(to_level_order(sol.upsideDownBinaryTreeIterative(root2)) ==
                    expected);
  });
}

}  // namespace
