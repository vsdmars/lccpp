// LeetCode 105: Construct Binary Tree From Preorder And Inorder Traversal
// https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/
//
// Problem:
//   Given two integer arrays `preorder` and `inorder`, where `preorder` is the
//   preorder traversal (root, left, right) and `inorder` is the inorder
//   traversal (left, root, right) of the same binary tree with unique node
//   values, reconstruct and return the binary tree.
//
// Algorithm:
//   Divide-and-conquer reconstruction using a hash map for O(1) inorder pivot
//   lookup. Because every node value is unique:
//   1. Precompute `inorder_index[val] = idx` for all elements of `inorder`.
//   2. Maintain a single advancing cursor `preorder_pos` into `preorder` and a
//      half-open slice `[in_left, in_right)` of `inorder` representing the
//      current subtree.
//   3. The next unconsumed element `preorder[preorder_pos++]` is guaranteed to
//      be the root of `[in_left, in_right)`. Looking up its position `split`
//      in `inorder_index` partitions the remaining nodes into left subtree
//      `[in_left, split)` and right subtree `[split + 1, in_right)`.
//   4. Crucially, recurse on the left subtree BEFORE the right subtree so the
//      sequential consumption of `preorder_pos` matches preorder order
//      (Root -> Left -> Right).
//   - Time Complexity: O(N), where N is the number of nodes (O(N) hash map
//     construction + O(1) work per node).
//   - Space Complexity: O(N) for the `inorder_index` hash map, O(H) recursion
//     stack (O(N) worst case for skewed trees), and O(N) for the output nodes.
//
// Data Structures:
//   - `std::unordered_map<int, std::size_t>` mapping each value to its index
//     in `inorder`, eliminating O(N) linear scans per recursive call.
//   - Half-open `std::size_t` interval `[in_left, in_right)` avoiding vector
//     slicing copies and signed index underflow.
//   - Dynamically allocated `TreeNode` tree owned in tests via RAII
//     `std::unique_ptr<TreeNode, TreeDeleter>`.
//
// Real-World Applications:
//   - Deserializing expression trees, DOM trees, or AST snapshots stored as
//     dual traversal streams in compilers and wire protocols.
//   - Reconstructing phylogenetic or hierarchical taxonomy trees from
//     complementary ordering logs.
//   - Tree diffing and replication protocols that transmit compact traversal
//     signatures rather than explicit pointer graphs.
//
// Related LeetCode Problems:
//   - #106 Construct Binary Tree from Inorder and Postorder Traversal [done] (mirror problem consuming postorder from the back, right subtree before left)
//   - #889 Construct Binary Tree from Preorder and Postorder Traversal [new] (reconstruction when inorder is absent; result may not be unique)
//   - #1008 Construct Binary Search Tree from Preorder Traversal [done] (BST ordering replaces the explicit inorder array)
//   - #297 Serialize and Deserialize Binary Tree [done] (general tree encoding/decoding with null sentinels and duplicate values allowed)

#include "framework/lc_framework.h"

#include <cstddef>
#include <memory>
#include <queue>
#include <unordered_map>
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
  // Reconstructs a binary tree from its preorder and inorder traversals.
  // Caller takes ownership of the returned tree nodes.
  // Time: O(N), Space: O(N).
  TreeNode* buildTree(const std::vector<int>& preorder,
                      const std::vector<int>& inorder) {
    if (preorder.empty() || preorder.size() != inorder.size()) {
      return nullptr;
    }

    // Map each unique value to its index in `inorder` so splitting left/right
    // subtrees takes O(1) time instead of O(N) linear search.
    std::unordered_map<int, std::size_t> inorder_index;
    inorder_index.reserve(inorder.size());
    for (std::size_t i = 0uz; i < inorder.size(); ++i) {
      inorder_index.emplace(inorder[i], i);
    }

    std::size_t preorder_pos = 0uz;

    // Recursively builds the subtree covering `inorder[in_left .. in_right)`.
    auto build = [&](this auto&& self, std::size_t in_left,
                     std::size_t in_right) -> TreeNode* {
      if (in_left >= in_right) {
        return nullptr;
      }

      // The next element in `preorder` is always the root of this subtree.
      const int root_val = preorder[preorder_pos++];
      const std::size_t split = inorder_index.at(root_val);

      auto* node = new TreeNode(root_val);
      // Preorder visits Root -> Left -> Right, so we must build left before
      // right to keep `preorder_pos` synchronized with the traversal.
      node->left = self(in_left, split);
      node->right = self(split + 1uz, in_right);
      return node;
    };

    return build(0uz, inorder.size());
  }
};

// Stateless RAII deleter for dynamically allocated TreeNode trees in tests.
struct TreeDeleter {
  void operator()(TreeNode* node) const noexcept { destroy(node); }

  static void destroy(TreeNode* node) noexcept {
    if (node == nullptr) {
      return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node;
  }
};

using OwnedTree = std::unique_ptr<TreeNode, TreeDeleter>;

// Collects the preorder traversal of a reconstructed tree for verification.
std::vector<int> collect_preorder(const TreeNode* root) {
  std::vector<int> out;
  auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
    if (node == nullptr) {
      return;
    }
    out.push_back(node->val);
    self(node->left);
    self(node->right);
  };
  dfs(root);
  return out;
}

// Collects the inorder traversal of a reconstructed tree for verification.
std::vector<int> collect_inorder(const TreeNode* root) {
  std::vector<int> out;
  auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
    if (node == nullptr) {
      return;
    }
    self(node->left);
    out.push_back(node->val);
    self(node->right);
  };
  dfs(root);
  return out;
}

// Collects BFS level-order values (omitting nulls) for structural checks.
std::vector<int> collect_bfs(const TreeNode* root) {
  std::vector<int> out;
  if (root == nullptr) {
    return out;
  }
  std::queue<const TreeNode*> frontier;
  frontier.push(root);
  while (!frontier.empty()) {
    const TreeNode* curr = frontier.front();
    frontier.pop();
    out.push_back(curr->val);
    if (curr->left != nullptr) {
      frontier.push(curr->left);
    }
    if (curr->right != nullptr) {
      frontier.push(curr->right);
    }
  }
  return out;
}

LC_PROBLEM(105, "Construct Binary Tree From Preorder And Inorder Traversal",
           "tree", ctx) {
  Solution sol;

  ctx.test("example_1_and_python_build", [&] {
    //     3
    //    / \
    //   9  20
    //     /  \
    //    15   7
    const std::vector<int> preorder = {3, 9, 20, 15, 7};
    const std::vector<int> inorder = {9, 3, 15, 20, 7};
    const OwnedTree root(sol.buildTree(preorder, inorder));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(collect_preorder(root.get()), preorder);
    ctx.expect_eq(collect_inorder(root.get()), inorder);
    ctx.expect_eq(collect_bfs(root.get()), std::vector<int>{3, 9, 20, 15, 7});
    ctx.expect_eq(root->val, 3);
    ctx.expect_true(root->left != nullptr && root->left->val == 9);
    ctx.expect_true(root->right != nullptr && root->right->val == 20);
  });

  ctx.test("single_node", [&] {
    const std::vector<int> preorder = {-1};
    const std::vector<int> inorder = {-1};
    const OwnedTree root(sol.buildTree(preorder, inorder));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(root->val, -1);
    ctx.expect_true(root->left == nullptr);
    ctx.expect_true(root->right == nullptr);
  });

  ctx.test("empty_input", [&] {
    const OwnedTree root(sol.buildTree({}, {}));
    ctx.expect_true(root == nullptr);
  });

  ctx.test("left_skewed_tree", [&] {
    const std::vector<int> preorder = {1, 2, 3};
    const std::vector<int> inorder = {3, 2, 1};
    const OwnedTree root(sol.buildTree(preorder, inorder));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(collect_preorder(root.get()), preorder);
    ctx.expect_eq(collect_inorder(root.get()), inorder);
    ctx.expect_true(root->right == nullptr);
    ctx.expect_true(root->left != nullptr && root->left->left != nullptr &&
                    root->left->left->val == 3);
  });

  ctx.test("right_skewed_tree", [&] {
    const std::vector<int> preorder = {1, 2, 3};
    const std::vector<int> inorder = {1, 2, 3};
    const OwnedTree root(sol.buildTree(preorder, inorder));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(collect_preorder(root.get()), preorder);
    ctx.expect_eq(collect_inorder(root.get()), inorder);
    ctx.expect_true(root->left == nullptr);
    ctx.expect_true(root->right != nullptr && root->right->right != nullptr &&
                    root->right->right->val == 3);
  });
}

}  // namespace
