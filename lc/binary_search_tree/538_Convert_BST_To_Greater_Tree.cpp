// LeetCode 538: Convert BST To Greater Tree
// https://leetcode.com/problems/convert-bst-to-greater-tree/
//
// Problem:
//   Given the root of a Binary Search Tree (BST), convert it in place to a
//   Greater Tree so that every node's original key is replaced by its original
//   key plus the sum of all keys in the BST that are strictly greater than it.
//   Return the root of the modified tree.
//
// Algorithm:
//   Traverse the BST in reverse in-order (right -> node -> left), which visits
//   keys in strictly descending order. Maintain a single running accumulator
//   `greater_sum` equal to the sum of all keys visited so far (which are
//   precisely the keys greater than the current node):
//   1. Visit the entire right subtree first.
//   2. Add `greater_sum` to `node->val` and update `greater_sum = node->val`.
//   3. Visit the left subtree with the updated `greater_sum`.
//   Both the iterative stack-based implementation (`convertBST`) and the
//   recursive DFS implementation (`convertBstRecursive`) from the Python
//   source are provided.
//   Time Complexity: O(n), visiting every node exactly once.
//   Space Complexity: O(h), where h is the tree height (explicit stack or
//   recursion call stack).
//
// Data Structures:
//   - Binary Search Tree (`TreeNode` linked structure) mutated in place.
//   - Explicit `std::stack<TreeNode*>` for iterative reverse in-order traversal.
//   - Scalar running sum (`greater_sum`) carrying the suffix sum of larger keys.
//
// Real-World Applications:
//   - Financial / leaderboard cumulative suffix aggregation: computing each
//     rank's "score plus all higher scores" in one pass over an ordered index.
//   - Database window functions (`SUM(val) OVER (ORDER BY val DESC)`) executed
//     directly over a B-tree / BST index scan.
//   - Probability and statistics: computing complementary cumulative
//     distribution functions (CCDF / tail sums) on sorted buckets.
//
// Related LeetCode Problems:
//   - #1038 Binary Search Tree to Greater Sum Tree [done] (identical problem statement)
//   - #94 Binary Tree Inorder Traversal [done] (standard left-to-right in-order traversal)
//   - #230 Kth Smallest Element in a BST [done] (ordered traversal on a BST)
//   - #938 Range Sum of BST [done] (accumulating node sums using BST ordering)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <stack>
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

// RAII arena that owns all test TreeNodes so pointer graphs never leak memory.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

  // Constructs a binary tree from LeetCode's level-order array representation.
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*level_order[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;
    while (!q.empty() && idx < level_order.size()) {
      TreeNode* cur = q.front();
      q.pop();
      if (level_order[idx].has_value()) {
        cur->left = make(*level_order[idx]);
        q.push(cur->left);
      }
      ++idx;
      if (idx < level_order.size() && level_order[idx].has_value()) {
        cur->right = make(*level_order[idx]);
        q.push(cur->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

// Collects the in-order values of a binary tree to verify transformed results.
std::vector<int> inorder_values(const TreeNode* root) {
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

class Solution {
 public:
  // Primary approach: iterative reverse in-order traversal (right -> node ->
  // left) using an explicit stack to accumulate the running suffix sum.
  // Time: O(n), Space: O(h).
  TreeNode* convertBST(TreeNode* root) {
    std::stack<TreeNode*> st;
    int greater_sum = 0;
    TreeNode* cur = root;

    // Seed the stack with the rightmost spine so the largest key pops first.
    while (cur != nullptr) {
      st.push(cur);
      cur = cur->right;
    }

    while (!st.empty()) {
      TreeNode* node = st.top();
      st.pop();

      // All nodes already popped had strictly greater keys than `node`.
      node->val += greater_sum;
      greater_sum = node->val;

      // Next largest keys live along the rightmost spine of `node->left`.
      cur = node->left;
      while (cur != nullptr) {
        st.push(cur);
        cur = cur->right;
      }
    }

    return root;
  }

  // Alternative approach: recursive reverse in-order DFS accumulating the
  // running greater sum in closure state.
  // Time: O(n), Space: O(h).
  TreeNode* convertBstRecursive(TreeNode* root) {
    int greater_sum = 0;

    auto dfs = [&](this auto&& self, TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }
      self(node->right);
      greater_sum += node->val;
      node->val = greater_sum;
      self(node->left);
    };

    dfs(root);
    return root;
  }
};

LC_PROBLEM(538, "Convert BST To Greater Tree", "binary_search_tree", ctx) {
  Solution sol;

  // Helper that builds two fresh copies of the same tree so iterative and
  // recursive in-place conversions can both be verified independently.
  auto check_both = [&](const std::vector<std::optional<int>>& input,
                        const std::vector<int>& expected_inorder) {
    TreeArena arena_iter;
    TreeNode* root_iter = arena_iter.build(input);
    TreeNode* res_iter = sol.convertBST(root_iter);
    ctx.expect_eq(inorder_values(res_iter), expected_inorder);

    TreeArena arena_rec;
    TreeNode* root_rec = arena_rec.build(input);
    TreeNode* res_rec = sol.convertBstRecursive(root_rec);
    ctx.expect_eq(inorder_values(res_rec), expected_inorder);
  };

  ctx.test("python_docstring_example", [&] {
    //     5              18
    //    / \     ==>    /  \
    //   2  13         20   13
    check_both({5, 2, 13}, {20, 18, 13});
  });

  ctx.test("leetcode_official_large_example", [&] {
    // Original in-order: [0, 1, 2, 3, 4, 5, 6, 7, 8]
    // Suffix sums:       [36, 36, 35, 33, 30, 26, 21, 15, 8]
    check_both({4, 1, 6, 0, 2, 5, 7, std::nullopt, std::nullopt, std::nullopt,
                3, std::nullopt, std::nullopt, std::nullopt, 8},
               {36, 36, 35, 33, 30, 26, 21, 15, 8});
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(sol.convertBST(nullptr), nullptr);
    ctx.expect_eq(sol.convertBstRecursive(nullptr), nullptr);
    check_both({7}, {7});
  });

  ctx.test("negative_values", [&] {
    // Original in-order: [-5, -3, 2] -> suffix sums: [-6, -1, 2]
    check_both({-3, -5, 2}, {-6, -1, 2});
  });

  ctx.test("skewed_trees", [&] {
    // Left-skewed: 10 -> 5 -> 1; suffix sums: [16, 15, 10]
    check_both({10, 5, std::nullopt, 1}, {16, 15, 10});
    // Right-skewed: 1 -> 5 -> 10; suffix sums: [16, 15, 10]
    check_both({1, std::nullopt, 5, std::nullopt, 10}, {16, 15, 10});
  });
}

}  // namespace
