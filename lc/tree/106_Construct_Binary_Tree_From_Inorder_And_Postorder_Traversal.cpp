// LeetCode 106: Construct Binary Tree From Inorder And Postorder Traversal
// https://leetcode.com/problems/construct-binary-tree-from-inorder-and-postorder-traversal/
//
// Problem:
//   Given two integer arrays `inorder` (left, root, right) and `postorder`
//   (left, right, root) of the same binary tree whose node values are unique,
//   reconstruct and return the binary tree.
//
// Examples:
//   Example 1:
//     Input:  inorder = [9, 3, 15, 20, 7], postorder = [9, 15, 7, 20, 3]
//     Output: [3, 9, 20, null, null, 15, 7]
//     Explanation:
//       - Build inorder_index = {9: 0, 3: 1, 15: 2, 20: 3, 7: 4}, initialize
//         post_remaining = 5, and call build(0, 5).
//       - Pre-decrement post_remaining to 4 to consume postorder[4] = 3 (split =
//         1), allocate root 3, and recurse right on [2, 5) before left on [0, 1).
//       - On [2, 5), consume postorder[3] = 20 (split = 3) for 3->right, then
//         consume postorder[2] = 7 on right subrange [4, 5) and postorder[1] = 15
//         on left subrange [2, 3).
//       - Return to [0, 1) and consume postorder[0] = 9 (split = 0) for 3->left,
//         producing [3, 9, 20, null, null, 15, 7].
//
//   Example 2:
//     Input:  inorder = [2, 1], postorder = [2, 1]
//     Output: [1, 2]
//     Explanation:
//       - Build inorder_index = {2: 0, 1: 1}, set post_remaining = 2, and call
//         build(0, 2).
//       - Consume postorder[1] = 1 (split = 1) as the root, then call build(2, 2)
//         for the right child which returns nullptr because in_left >= in_right.
//       - Call build(0, 1) for the left child, consuming postorder[0] = 2 (split =
//         0) with empty child ranges [0, 0) and [1, 1), yielding [1, 2].
//
//   Example 3:
//     Input:  inorder = [1, 2, 3], postorder = [3, 2, 1]
//     Output: [1, null, 2, null, 3]
//     Explanation:
//       - Build inorder_index = {1: 0, 2: 1, 3: 2}, set post_remaining = 3, and
//         call build(0, 3).
//       - Consume postorder[2] = 1 (split = 0) for root 1 and recurse right on [1,
//         3), which consumes postorder[1] = 2 (split = 1) for 1->right.
//       - Recurse right again on [2, 3) to consume postorder[0] = 3 (split = 2) for
//         2->right while all left subranges [0, 0), [1, 1), and [2, 2) return
//         nullptr, yielding [1, null, 2, null, 3].
//
// Algorithm:
//   Reverse-postorder divide-and-conquer with an O(1) inorder index map.
//   1. Precompute `inorder_index[val] = idx` for every value in `inorder`.
//   2. Walk `postorder` backwards using a `post_remaining` counter initialized
//      to `postorder.size()`. Decrementing before indexing yields the next
//      subtree root without unsigned underflow.
//   3. For a half-open inorder subrange `[in_left, in_right)`, consume
//      `root_val = postorder[--post_remaining]` and look up `split =
//      inorder_index[root_val]`.
//   4. Crucially, construct the RIGHT subtree `[split + 1, in_right)` BEFORE
//      the LEFT subtree `[in_left, split)`, because reading postorder in
//      reverse visits nodes in `Root -> Right -> Left` order.
//   - Time Complexity: O(N), where N is the number of nodes (O(N) hash map
//     build + O(1) work per node).
//   - Space Complexity: O(N) for `inorder_index`, O(H) recursion stack (O(N)
//     worst case on skewed trees), and O(N) for the constructed tree nodes.
//
// Data Structures:
//   - `std::unordered_map<int, std::size_t>` for O(1) lookup of each root's
//     split position inside `inorder`.
//   - Half-open `std::size_t` interval `[in_left, in_right)` avoiding vector
//     copies and signed/unsigned mismatches.
//   - Dynamically allocated `TreeNode` tree managed in tests via RAII
//     `std::unique_ptr<TreeNode, TreeDeleter>`.
//
// Real-World Applications:
//   - Rebuilding expression trees from Reverse Polish Notation (postorder)
//     paired with infix (inorder) operator placement in compilers and symbolic
//     math engines.
//   - Restoring directory or dependency hierarchies exported in bottom-up
//     evaluation order alongside a canonical sorted listing.
//   - Compact tree serialization and verification in distributed storage
//     systems.
//
// Related LeetCode Problems:
//   - #105 Construct Binary Tree from Preorder and Inorder Traversal [done] (mirror problem consuming preorder forward, left subtree before right)
//   - #889 Construct Binary Tree from Preorder and Postorder Traversal [new] (reconstruction without inorder traversal)
//   - #297 Serialize and Deserialize Binary Tree [done] (general tree serialization supporting duplicate values)
//   - #94 Binary Tree Inorder Traversal [done] (produces one of the two input traversals)

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
  // Reconstructs a binary tree from its inorder and postorder traversals.
  // Caller takes ownership of the returned tree nodes.
  // Time: O(N), Space: O(N).
  TreeNode* buildTree(const std::vector<int>& inorder,
                      const std::vector<int>& postorder) {
    if (inorder.empty() || inorder.size() != postorder.size()) {
      return nullptr;
    }

    // Index each unique value in `inorder` for O(1) partition lookup.
    std::unordered_map<int, std::size_t> inorder_index;
    inorder_index.reserve(inorder.size());
    for (std::size_t i = 0uz; i < inorder.size(); ++i) {
      inorder_index.emplace(inorder[i], i);
    }

    // Tracks how many elements remain unread in `postorder`; pre-decrementing
    // indexes from back to front without risking unsigned underflow.
    std::size_t post_remaining = postorder.size();

    // Recursively builds the subtree covering `inorder[in_left .. in_right)`.
    auto build = [&](this auto&& self, std::size_t in_left,
                     std::size_t in_right) -> TreeNode* {
      if (in_left >= in_right) {
        return nullptr;
      }

      // The last unread element of `postorder` is the root of this subtree.
      const int root_val = postorder[--post_remaining];
      const std::size_t split = inorder_index.at(root_val);

      auto* node = new TreeNode(root_val);
      // Reading postorder right-to-left yields Root -> Right -> Left, so the
      // right subtree MUST be constructed before the left subtree.
      node->right = self(split + 1uz, in_right);
      node->left = self(in_left, split);
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

// Collects inorder traversal of a reconstructed tree for verification.
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

// Collects postorder traversal of a reconstructed tree for verification.
std::vector<int> collect_postorder(const TreeNode* root) {
  std::vector<int> out;
  auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
    if (node == nullptr) {
      return;
    }
    self(node->left);
    self(node->right);
    out.push_back(node->val);
  };
  dfs(root);
  return out;
}

// Collects BFS level-order values (matching Python's `run()` output).
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

LC_PROBLEM(106, "Construct Binary Tree From Inorder And Postorder Traversal",
           "tree", ctx) {
  Solution sol;

  ctx.test("example_1_and_python_build", [&] {
    //     3
    //    / \
    //   9  20
    //     /  \
    //    15   7
    const std::vector<int> inorder = {9, 3, 15, 20, 7};
    const std::vector<int> postorder = {9, 15, 7, 20, 3};
    const OwnedTree root(sol.buildTree(inorder, postorder));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(collect_inorder(root.get()), inorder);
    ctx.expect_eq(collect_postorder(root.get()), postorder);
    ctx.expect_eq(collect_bfs(root.get()), std::vector<int>{3, 9, 20, 15, 7});
    ctx.expect_eq(root->val, 3);
    ctx.expect_true(root->left != nullptr && root->left->val == 9);
    ctx.expect_true(root->right != nullptr && root->right->val == 20);
  });

  ctx.test("python_two_node_left_child", [&] {
    //   1
    //  /
    // 2
    const std::vector<int> inorder = {2, 1};
    const std::vector<int> postorder = {2, 1};
    const OwnedTree root(sol.buildTree(inorder, postorder));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(root->val, 1);
    ctx.expect_true(root->left != nullptr && root->left->val == 2);
    ctx.expect_true(root->right == nullptr);
  });

  ctx.test("empty_input", [&] {
    const OwnedTree root(sol.buildTree({}, {}));
    ctx.expect_true(root == nullptr);
  });

  ctx.test("single_node", [&] {
    const OwnedTree root(sol.buildTree({-1}, {-1}));
    ctx.expect_true(root != nullptr);
    ctx.expect_eq(root->val, -1);
    ctx.expect_true(root->left == nullptr);
    ctx.expect_true(root->right == nullptr);
  });

  ctx.test("right_skewed_tree", [&] {
    const std::vector<int> inorder = {1, 2, 3};
    const std::vector<int> postorder = {3, 2, 1};
    const OwnedTree root(sol.buildTree(inorder, postorder));

    ctx.expect_true(root != nullptr);
    ctx.expect_eq(collect_inorder(root.get()), inorder);
    ctx.expect_eq(collect_postorder(root.get()), postorder);
    ctx.expect_true(root->left == nullptr);
    ctx.expect_true(root->right != nullptr && root->right->right != nullptr &&
                    root->right->right->val == 3);
  });
}

}  // namespace
