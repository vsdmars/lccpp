// LeetCode 145: Binary Tree Postorder Traversal
// https://leetcode.com/problems/binary-tree-postorder-traversal/
//
// Problem:
//   Given the root of a binary tree, return the postorder traversal of its
//   nodes' values as a list. In postorder traversal, for every node we visit
//   the entire left subtree, then the entire right subtree, and finally the
//   node itself (Left -> Right -> Root).
//
// Examples:
//   Example 1:
//     Input:  root = [1, null, 2, 3]
//     Output: [3, 2, 1]
//     Explanation:
//       - Recursive dfs(1) sees left is nullptr and recurses into right child 2,
//         which descends into left child 3.
//       - At leaf 3, both children are nullptr, so 3 is appended first to give
//         result = [3].
//       - Unwinding appends 2 (whose right child is nullptr) and then root 1,
//         yielding [3, 2, 1]; postorderIterative produces the same sequence using
//         stack and last_visited tracking.
//
//   Example 2:
//     Input:  root = [3, 9, 20, null, null, 15, 7]
//     Output: [9, 15, 7, 20, 3]
//     Explanation:
//       - postorderIterative descends the left spine pushing 3 and 9, then pops and
//         emits leaf 9 with last_visited = 9.
//       - Peeking 3 sees unvisited right child 20, so it descends left from 20
//         pushing 20 and 15, then pops and emits 15 with last_visited = 15.
//       - Peeking 20 sees unvisited right child 7, pushes and emits 7 with
//         last_visited = 7.
//       - Because last_visited now equals 20->right, 20 is popped and emitted
//         (last_visited = 20), and then 3 is popped and emitted to finish [9, 15,
//         7, 20, 3].
//
//   Example 3:
//     Input:  root = []
//     Output: []
//     Explanation:
//       - In postorderTraversal, dfs(nullptr) hits node == nullptr immediately and
//         returns without modifying result.
//       - In postorderIterative, curr is nullptr and stack is empty, so the
//         traversal loop exits immediately and returns [].
//
// Algorithm:
//   Two approaches are provided:
//     1. Recursive DFS (`postorderTraversal`, matching the Python source):
//        Recursively traverse `node->left`, then `node->right`, and append
//        `node->val` to the result vector after both subtrees complete.
//     2. Iterative Single-Stack Postorder (`postorderIterative`, classic
//        follow-up):
//        Simulate the call stack with an explicit `std::vector<const TreeNode*>`
//        and a `last_visited` pointer. Descend as far left as possible, pushing
//        ancestors onto the stack. Inspect the stack top (`peek`):
//          - If `peek->right` exists and was not the node just emitted
//            (`last_visited != peek->right`), descend into `peek->right`.
//          - Otherwise both subtrees of `peek` have already been emitted, so
//            append `peek->val`, set `last_visited = peek`, and pop `peek`.
//   Time Complexity:  O(N) for both approaches (each node is pushed, peeked,
//                     and emitted a constant number of times).
//   Space Complexity: O(H) auxiliary stack space (excluding the O(N) output),
//                     where H is the tree height (O(log N) balanced, O(N)
//                     worst case).
//
// Data Structures:
//   - Binary tree (`TreeNode`) representing the input hierarchy.
//   - `std::vector<int>` storing the emitted traversal sequence.
//   - `std::vector<const TreeNode*>` explicit stack + `last_visited` pointer
//     in the iterative variant.
//
// Real-World Applications:
//   - Safely freeing/destroying tree structures (children must be deallocated
//     before their parent node).
//   - Evaluating abstract syntax trees (ASTs) and postfix / Reverse Polish
//     Notation (RPN) expressions where operands must be evaluated before their
//     operator.
//   - Computing bottom-up directory disk usage (`du`) or dependency build
//     targets where child sizes/artifacts are required before summarizing the
//     parent.
//
// Related LeetCode Problems:
//   - #94 Binary Tree Inorder Traversal [done] (left-root-right traversal order)
//   - #144 Binary Tree Preorder Traversal [new] (root-left-right traversal order)
//   - #102 Binary Tree Level Order Traversal [done] (breadth-first level-by-level traversal)
//   - #590 N-ary Tree Postorder Traversal [new] (generalizes postorder to arbitrary branching factor)
//   - #124 Binary Tree Maximum Path Sum [done] (bottom-up postorder DP combining child subtree results)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int value) : val(value) {}
  TreeNode(int value, TreeNode* left_child, TreeNode* right_child)
      : val(value), left(left_child), right(right_child) {}
};

// Arena owning TreeNode instances for leak-free test tree construction.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns the postorder (Left -> Right -> Root) traversal using recursive
  // DFS.
  //
  // Time: O(N), Space: O(H) auxiliary stack space.
  std::vector<int> postorderTraversal(const TreeNode* root) {
    std::vector<int> result;

    auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }
      self(node->left);
      self(node->right);
      result.push_back(node->val);
    };

    dfs(root);
    return result;
  }

  // Returns the postorder traversal iteratively using a single explicit stack
  // and a `last_visited` pointer to distinguish returning from a left subtree
  // versus returning from a right subtree.
  //
  // Invariant: for every node currently on `stack`, its left subtree is either
  // being explored or already fully emitted; `last_visited` points to the most
  // recently emitted node.
  // Time: O(N), Space: O(H) auxiliary stack space.
  std::vector<int> postorderIterative(const TreeNode* root) {
    std::vector<int> result;
    std::vector<const TreeNode*> stack;
    const TreeNode* curr = root;
    const TreeNode* last_visited = nullptr;

    while (curr != nullptr || !stack.empty()) {
      // Descend along the left spine as far as possible.
      while (curr != nullptr) {
        stack.push_back(curr);
        curr = curr->left;
      }

      const TreeNode* peek = stack.back();
      // If a right subtree exists and has not yet been completed, descend into
      // it before emitting `peek`.
      if (peek->right != nullptr && last_visited != peek->right) {
        curr = peek->right;
      } else {
        // Both children are done; emit `peek` and record it as last_visited.
        result.push_back(peek->val);
        last_visited = peek;
        stack.pop_back();
      }
    }

    return result;
  }
};

LC_PROBLEM(145, "Binary Tree Postorder Traversal", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 ([1,null,2,3] -> [3,2,1])", [&] {
    //   1
    //    \
    //     2
    //    /
    //   3
    TreeArena arena;
    TreeNode* root = arena.make(1, nullptr, arena.make(2, arena.make(3)));
    const std::vector<int> expected = {3, 2, 1};

    ctx.expect_eq(sol.postorderTraversal(root), expected);
    ctx.expect_eq(sol.postorderIterative(root), expected);
  });

  ctx.test("python build() tree ([3,9,20,null,null,15,7])", [&] {
    //     3
    //    / \
    //   9  20
    //     /  \
    //    15   7
    TreeArena arena;
    TreeNode* root =
        arena.make(3, arena.make(9),
                   arena.make(20, arena.make(15), arena.make(7)));
    const std::vector<int> expected = {9, 15, 7, 20, 3};

    ctx.expect_eq(sol.postorderTraversal(root), expected);
    ctx.expect_eq(sol.postorderIterative(root), expected);
  });

  ctx.test("empty tree and single node", [&] {
    ctx.expect_eq(sol.postorderTraversal(nullptr), std::vector<int>{});
    ctx.expect_eq(sol.postorderIterative(nullptr), std::vector<int>{});

    TreeArena arena;
    TreeNode* single = arena.make(42);
    ctx.expect_eq(sol.postorderTraversal(single), std::vector<int>{42});
    ctx.expect_eq(sol.postorderIterative(single), std::vector<int>{42});
  });

  ctx.test("complete binary tree", [&] {
    //       1
    //      / \
    //     2   3
    //    / \ / \
    //   4  5 6  7
    TreeArena arena;
    TreeNode* root =
        arena.make(1, arena.make(2, arena.make(4), arena.make(5)),
                   arena.make(3, arena.make(6), arena.make(7)));
    const std::vector<int> expected = {4, 5, 2, 6, 7, 3, 1};

    ctx.expect_eq(sol.postorderTraversal(root), expected);
    ctx.expect_eq(sol.postorderIterative(root), expected);
  });

  ctx.test("left-skewed and right-skewed chains", [&] {
    TreeArena arena;
    TreeNode* left_chain = arena.make(1, arena.make(2, arena.make(3)));
    TreeNode* right_chain =
        arena.make(1, nullptr, arena.make(2, nullptr, arena.make(3)));
    const std::vector<int> expected = {3, 2, 1};

    ctx.expect_eq(sol.postorderTraversal(left_chain), expected);
    ctx.expect_eq(sol.postorderIterative(left_chain), expected);
    ctx.expect_eq(sol.postorderTraversal(right_chain), expected);
    ctx.expect_eq(sol.postorderIterative(right_chain), expected);
  });
}

}  // namespace
