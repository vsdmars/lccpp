// LeetCode 654: Maximum Binary Tree
// https://leetcode.com/problems/maximum-binary-tree/
//
// Problem:
//   Given an integer array `nums` with no duplicates, construct the maximum
//   binary tree (Cartesian tree) defined recursively as follows:
//   1. Create a root node whose value is the maximum element in `nums`.
//   2. Recursively build the left subtree from the subarray strictly to the
//      left of the maximum element.
//   3. Recursively build the right subtree from the subarray strictly to the
//      right of the maximum element.
//   Array length is in [1, 1000] and elements are distinct in [0, 1000].
//
// Algorithm:
//   1. Divide-and-conquer recursion (`constructMaximumBinaryTree`):
//      Operate on a zero-copy `std::span<const int>` view of the active
//      subarray. Locate the maximum element with `std::ranges::max_element`,
//      create the root node, and recursively build the left and right subtrees
//      from `sub.subspan(0, max_idx)` and `sub.subspan(max_idx + 1)`.
//      Time: O(N^2) worst-case (already sorted input), O(N log N) average.
//      Space: O(N) worst-case recursion depth + O(N) output nodes.
//   2. Monotonic decreasing stack (`constructMaximumBinaryTreeLinear`):
//      Scan `nums` left-to-right while maintaining a stack of nodes along the
//      right spine of the partially built Cartesian tree in strictly decreasing
//      value order:
//      - Pop every stack node smaller than `val`; the last popped node becomes
//        `curr->left` (it is the largest element to the left smaller than `val`).
//      - If the stack is still non-empty, its top is the nearest larger element
//        to the left, so attach `curr` as `stack.back()->right`.
//      - Push `curr` onto the stack.
//      At the end, `stack.front()` is the global maximum (the tree root).
//      Time: O(N) (each node is pushed and popped at most once).
//      Space: O(N) for the monotonic stack and output nodes.
//
// Data Structures:
//   - `std::span<const int>`: non-owning O(1)-slice subarray view for recursive
//     divide-and-conquer without copying vectors.
//   - `std::vector<TreeNode*>` (monotonic decreasing stack): maintains the
//     active right spine of the Cartesian tree in linear time.
//
// Real-World Applications:
//   - Range Maximum Query (RMQ) preprocessing: reducing RMQ to Lowest Common
//     Ancestor (LCA) on a Cartesian tree for O(N) build / O(1) query.
//   - Treaps (randomized balanced BSTs), which are simultaneously BSTs by key
//     and Cartesian max-heaps by priority.
//   - Operator-precedence expression tree parsing in compilers (precedence
//     climbing / shunting-yard monotonic stack reductions).
//
// Related LeetCode Problems:
//   - #998 Maximum Binary Tree II [new] (inserting a new value into a maximum binary tree)
//   - #105 Construct Binary Tree from Preorder and Inorder Traversal [done] (divide-and-conquer tree construction)
//   - #108 Convert Sorted Array to Binary Search Tree [done] (recursive subarray partitioning into a tree)
//   - #84 Largest Rectangle in Histogram [done] (monotonic stack nearest-smaller/larger structure)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <queue>
#include <span>
#include <string>
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
  // Constructs the maximum binary tree via divide-and-conquer over `std::span`.
  // Time: O(N^2) worst-case / O(N log N) average, Space: O(N).
  TreeNode* constructMaximumBinaryTree(const std::vector<int>& nums) {
    auto build = [](this auto&& self, std::span<const int> sub) -> TreeNode* {
      if (sub.empty()) {
        return nullptr;
      }

      // Locate the maximum element in the current subarray slice.
      const auto max_it = std::ranges::max_element(sub);
      const auto max_idx =
          static_cast<std::size_t>(std::distance(sub.begin(), max_it));

      auto* root = new TreeNode(*max_it);
      // Partition around `max_idx` in O(1) without allocating temporary arrays.
      root->left = self(sub.subspan(0, max_idx));
      root->right = self(sub.subspan(max_idx + 1uz));
      return root;
    };

    return build(nums);
  }

  // Optimal O(N) Cartesian tree construction using a decreasing monotonic stack.
  // Time: O(N), Space: O(N).
  TreeNode* constructMaximumBinaryTreeLinear(const std::vector<int>& nums) {
    if (nums.empty()) {
      return nullptr;
    }

    // Invariant: `spine` holds nodes on the current rightmost path in strictly
    // decreasing order of `val`, so `spine.front()` is always the max seen so far.
    std::vector<TreeNode*> spine;
    spine.reserve(nums.size());

    for (const int val : nums) {
      auto* curr = new TreeNode(val);

      // Every node popped is smaller than `val` and lies to its left; because
      // `spine` is decreasing, each popped node already contains the previous
      // popped node as its right child, so the last popped node is the root of
      // `curr`'s entire left subtree.
      while (!spine.empty() && spine.back()->val < val) {
        curr->left = spine.back();
        spine.pop_back();
      }

      // If a larger ancestor remains on the stack, `curr` becomes its new right
      // child (replacing the smaller subtree we just moved under `curr->left`).
      if (!spine.empty()) {
        spine.back()->right = curr;
      }

      spine.push_back(curr);
    }

    return spine.front();
  }
};

// RAII owner for heap-allocated trees returned by `Solution`.
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

LC_PROBLEM(654, "Maximum Binary Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    const std::vector<int> nums = {3, 2, 1, 6, 0, 5};
    const std::vector<std::string> expected = {"6", "3", "5", "null", "2",
                                               "0", "null", "null", "1"};

    OwnedTree t1(sol.constructMaximumBinaryTree(nums));
    OwnedTree t2(sol.constructMaximumBinaryTreeLinear(nums));
    ctx.expect_eq(serialize(t1.get()), expected);
    ctx.expect_eq(serialize(t2.get()), expected);
  });

  ctx.test("example_2_strictly_decreasing", [&] {
    const std::vector<int> nums = {3, 2, 1};
    const std::vector<std::string> expected = {"3", "null", "2", "null", "1"};

    OwnedTree t1(sol.constructMaximumBinaryTree(nums));
    OwnedTree t2(sol.constructMaximumBinaryTreeLinear(nums));
    ctx.expect_eq(serialize(t1.get()), expected);
    ctx.expect_eq(serialize(t2.get()), expected);
  });

  ctx.test("strictly_increasing", [&] {
    const std::vector<int> nums = {1, 2, 3, 4};
    const std::vector<std::string> expected = {"4", "3", "null", "2", "null", "1"};

    OwnedTree t1(sol.constructMaximumBinaryTree(nums));
    OwnedTree t2(sol.constructMaximumBinaryTreeLinear(nums));
    ctx.expect_eq(serialize(t1.get()), expected);
    ctx.expect_eq(serialize(t2.get()), expected);
  });

  ctx.test("single_element_and_empty", [&] {
    OwnedTree single1(sol.constructMaximumBinaryTree({7}));
    OwnedTree single2(sol.constructMaximumBinaryTreeLinear({7}));
    ctx.expect_eq(serialize(single1.get()), std::vector<std::string>{"7"});
    ctx.expect_eq(serialize(single2.get()), std::vector<std::string>{"7"});

    OwnedTree empty1(sol.constructMaximumBinaryTree({}));
    OwnedTree empty2(sol.constructMaximumBinaryTreeLinear({}));
    ctx.expect_eq(empty1.get(), nullptr);
    ctx.expect_eq(empty2.get(), nullptr);
  });
}

}  // namespace
