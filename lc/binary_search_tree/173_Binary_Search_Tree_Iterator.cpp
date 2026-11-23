// LeetCode 173: Binary Search Tree Iterator
// https://leetcode.com/problems/binary-search-tree-iterator/
//
// Problem:
//   Implement the `BSTIterator` class representing an in-order iterator over a
//   binary search tree (BST):
//   - `BSTIterator(root)` initializes the iterator at the root of the BST,
//     positioned logically before the smallest element.
//   - `hasNext()` returns true if there exists a next number in the traversal.
//   - `next()` advances the iterator and returns the next smallest integer.
//   Requirements: `next()` and `hasNext()` must run in amortized O(1) time and
//   use O(h) memory, where h is the height of the BST.
//
// Algorithm:
//   Controlled iterative inorder traversal using an explicit stack holding the
//   current left-spine path of unvisited ancestors:
//   - On construction, push the root and all of its left descendants onto the
//     stack so the minimum element sits at the top.
//   - `hasNext()` checks whether the stack is non-empty in O(1) time.
//   - `next()` pops the top node (the next smallest value) and pushes the left
//     spine of its right child onto the stack before returning its value.
//   Because every node in the tree is pushed once and popped once across a full
//   traversal, `next()` achieves O(1) amortized time even though an individual
//   call may descend up to O(h) nodes.
//   - Time Complexity:  O(1) amortized for `next()`, O(1) worst-case for
//     `hasNext()`.
//   - Space Complexity: O(h), where h is the height of the BST.
//
// Data Structures:
//   - Explicit stack (`std::vector<const TreeNode*>`) storing only the pending
//     left-spine ancestor chain, bounding peak memory to O(h).
//   - Binary tree nodes (`TreeNode`).
//
// Real-World Applications:
//   - Streaming range-scan cursors in database B-tree / LSM-tree storage
//     engines (yielding sorted rows on demand without materializing the entire
//     result set).
//   - Lazy iterator implementations for ordered containers and persistent trees
//     (similar to `std::map::iterator` without parent pointers).
//   - Merge-join operators combining two sorted tree-indexed relations in
//     streaming O(h) memory.
//
// Related LeetCode Problems:
//   - #94 Binary Tree Inorder Traversal [done] (full inorder traversal via stack)
//   - #230 Kth Smallest Element In A BST [done] (early-stopping inorder traversal)
//   - #285 Inorder Successor In BST [done] (finding the next inorder node from a target)
//   - #1586 Binary Search Tree Iterator II [new] (bidirectional BST iterator supporting prev())
//   - #251 Flatten 2D Vector [done] (lazy iterator design pattern over nested structures)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Binary tree node definition matching LeetCode's standard representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

// RAII arena for test trees; std::deque guarantees pointer stability on growth.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

// Lazy in-order iterator over a BST using O(h) memory and amortized O(1) next().
class BSTIterator {
 public:
  // Initializes the iterator by pushing the root's leftmost path so the
  // smallest node is immediately ready at the top of `stack_`.
  // Time: O(h), Space: O(h).
  explicit BSTIterator(const TreeNode* root) { pushLeftSpine(root); }

  // Returns true iff at least one unvisited node remains in the BST.
  // Time: O(1), Space: O(1).
  [[nodiscard]] bool hasNext() const noexcept { return !stack_.empty(); }

  // Pops and returns the next smallest value, then stages the leftmost path of
  // its right subtree onto `stack_`.
  // Precondition: hasNext() is true.
  // Time: Amortized O(1), Space: O(h).
  int next() {
    const TreeNode* node = stack_.back();
    stack_.pop_back();

    // All values in `node->right` lie between `node` and its inorder ancestor
    // currently sitting underneath it on `stack_`.
    pushLeftSpine(node->right);
    return node->val;
  }

 private:
  // Descends as far left as possible from `node`, pushing each ancestor so the
  // minimum element of this subtree ends up on top of `stack_`.
  void pushLeftSpine(const TreeNode* node) {
    while (node != nullptr) {
      stack_.push_back(node);
      node = node->left;
    }
  }

  std::vector<const TreeNode*> stack_;
};

LC_PROBLEM(173, "Binary Search Tree Iterator", "binary_search_tree", ctx) {
  ctx.test("leetcode_example_interleaved_calls", [&] {
    TreeArena arena;
    //       7
    //      / \
    //     3  15
    //        / \
    //       9  20
    TreeNode* right = arena.make(15, arena.make(9), arena.make(20));
    TreeNode* root = arena.make(7, arena.make(3), right);

    BSTIterator it(root);
    ctx.expect_eq(it.next(), 3);
    ctx.expect_eq(it.next(), 7);
    ctx.expect_true(it.hasNext());
    ctx.expect_eq(it.next(), 9);
    ctx.expect_true(it.hasNext());
    ctx.expect_eq(it.next(), 15);
    ctx.expect_true(it.hasNext());
    ctx.expect_eq(it.next(), 20);
    ctx.expect_false(it.hasNext());
  });

  ctx.test("balanced_bst_full_drain", [&] {
    TreeArena arena;
    //        5
    //      /   \
    //     3     7
    //    / \   / \
    //   2   4 6   8
    TreeNode* left = arena.make(3, arena.make(2), arena.make(4));
    TreeNode* right = arena.make(7, arena.make(6), arena.make(8));
    TreeNode* root = arena.make(5, left, right);

    BSTIterator it(root);
    std::vector<int> actual;
    while (it.hasNext()) {
      actual.push_back(it.next());
    }
    ctx.expect_eq(actual, std::vector<int>{2, 3, 4, 5, 6, 7, 8});
  });

  ctx.test("left_and_right_skewed_trees", [&] {
    TreeArena arena;
    // Left-skewed: 4 -> 3 -> 2 -> 1
    TreeNode* left_root = arena.make(
        4, arena.make(3, arena.make(2, arena.make(1), nullptr), nullptr),
        nullptr);
    BSTIterator it_left(left_root);
    std::vector<int> left_vals;
    while (it_left.hasNext()) {
      left_vals.push_back(it_left.next());
    }
    ctx.expect_eq(left_vals, std::vector<int>{1, 2, 3, 4});

    // Right-skewed: 1 -> 2 -> 3 -> 4
    TreeNode* right_root = arena.make(
        1, nullptr,
        arena.make(2, nullptr, arena.make(3, nullptr, arena.make(4))));
    BSTIterator it_right(right_root);
    std::vector<int> right_vals;
    while (it_right.hasNext()) {
      right_vals.push_back(it_right.next());
    }
    ctx.expect_eq(right_vals, std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    BSTIterator empty_it(nullptr);
    ctx.expect_false(empty_it.hasNext());

    BSTIterator single_it(arena.make(42));
    ctx.expect_true(single_it.hasNext());
    ctx.expect_eq(single_it.next(), 42);
    ctx.expect_false(single_it.hasNext());
  });
}

}  // namespace
