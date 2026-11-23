// LeetCode 99: Recover Binary Search Tree
// https://leetcode.com/problems/recover-binary-search-tree/
//
// Problem:
//   You are given the root of a binary search tree (BST) where the values of
//   exactly two nodes were swapped by mistake. Recover the BST in-place by
//   restoring the two node values without changing the tree's structure.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 3, null, null, 2]
//     Output: [3, 1, null, null, 2]
//     Explanation:
//       - Inorder traversal visits nodes in the order 3, 2, 1 with prev initially
//         null.
//       - Advancing from prev = 3 to node = 2 detects the first inversion (3 > 2),
//         recording first = Node(3) and second = Node(2).
//       - Advancing from prev = 2 to node = 1 detects a second inversion (2 > 1),
//         keeping first = Node(3) and updating second = Node(1).
//       - Swap first->val and second->val (3 and 1), restoring the tree to [3, 1,
//         null, null, 2] with inorder sequence [1, 2, 3].
//
//   Example 2:
//     Input:  root = [3, 1, 4, null, null, 2]
//     Output: [2, 1, 4, null, null, 3]
//     Explanation:
//       - Inorder traversal visits node values 1, 3, 2, 4.
//       - Moving from 1 to 3 is ascending, then moving from prev = 3 to node = 2
//         triggers the sole inversion (3 > 2), setting first = Node(3) and second =
//         Node(2).
//       - Moving from prev = 2 to node = 4 is ascending (2 < 4), so first and
//         second remain Node(3) and Node(2).
//       - Swap first->val and second->val (3 and 2), producing [2, 1, 4, null,
//         null, 3] with sorted inorder sequence [1, 2, 3, 4].
//
//   Example 3:
//     Input:  root = [0, -5, 5, 10, null, null, -10]
//     Output: [0, -5, 5, -10, null, null, 10]
//     Explanation:
//       - Inorder traversal visits 10, -5, 0, 5, -10.
//       - At node -5, prev = 10 > -5 records the first inversion with first =
//         Node(10) and second = Node(-5).
//       - Visits at 0 and 5 remain strictly ascending, leaving prev = Node(5).
//       - At node -10, prev = 5 > -10 records the second inversion, keeping first =
//         Node(10) and updating second = Node(-10).
//       - Swap 10 and -10 in place to restore the sorted inorder sequence [-10, -5,
//         0, 5, 10].
//
// Algorithm:
//   An inorder traversal of a valid BST visits node values in strictly
//   ascending order. Swapping two nodes creates either:
//   - One inversion (when the swapped nodes are adjacent in inorder order):
//     `prev->val > cur->val` occurs once, so `first = prev` and `second = cur`.
//   - Two inversions (when the swapped nodes are non-adjacent):
//     The first inversion identifies the larger misplaced node (`first = prev`,
//     recorded only once), and the second inversion identifies the smaller
//     misplaced node (`second = cur`, overwritten on each inversion).
//   After completing the traversal, swapping `first->val` and `second->val`
//   restores the BST.
//   Two implementations are provided:
//   1. `recoverTree`: Recursive inorder DFS.
//      - Time: O(n), Space: O(h) recursion stack.
//   2. `recoverTreeMorris`: Threaded Morris inorder traversal (follow-up).
//      Temporarily links each inorder predecessor's right pointer to the
//      current node to backtrack without a stack, then restores all links.
//      - Time: O(n), Space: O(1) auxiliary space.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`) modified in-place.
//   - Three tracking pointers (`prev`, `first`, `second`) capturing inorder
//     anomalies during traversal.
//
// Real-World Applications:
//   - Online data-structure repair and self-healing ordered indexes when a
//     single key-swap corruption is detected during checksum/audit scans.
//   - Sorted-sequence anomaly detection (identifying minimal transpositions in
//     time-series or log indices).
//   - Memory-constrained embedded tree traversal using Morris threading where
//     stack allocation is bounded.
//
// Related LeetCode Problems:
//   - #98 Validate Binary Search Tree [done] (detecting whether a BST invariant is broken)
//   - #94 Binary Tree Inorder Traversal [done] (foundation for inorder and Morris traversal)
//   - #173 Binary Search Tree Iterator [done] (incremental inorder traversal of a BST)
//   - #230 Kth Smallest Element In A BST [done] (rank queries via inorder traversal)
//   - #501 Find Mode in Binary Search Tree [done] (tracking previous inorder node in O(1) space)

#include "framework/lc_framework.h"

#include <deque>
#include <utility>
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

// Collects the inorder sequence of a tree for verifying BST recovery in tests.
std::vector<int> inorderValues(const TreeNode* root) {
  std::vector<int> values;
  auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
    if (node == nullptr) {
      return;
    }
    self(node->left);
    values.push_back(node->val);
    self(node->right);
  };
  dfs(root);
  return values;
}

class Solution {
 public:
  // Recovers a BST with two swapped nodes via recursive inorder DFS.
  // Time: O(n), Space: O(h).
  void recoverTree(TreeNode* root) {
    TreeNode* prev = nullptr;
    TreeNode* first = nullptr;
    TreeNode* second = nullptr;

    // Inorder DFS maintaining the previously visited node `prev`.
    auto dfs = [&](this auto&& self, TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }

      self(node->left);

      // Detect an ordering inversion: in a valid BST, prev->val < node->val.
      if (prev != nullptr && prev->val > node->val) {
        // Record `first` only on the initial inversion (the larger swapped
        // element moved earlier in the inorder sequence).
        if (first == nullptr) {
          first = prev;
        }
        // Always update `second` so both adjacent (1 inversion) and
        // non-adjacent (2 inversions) swaps capture the smaller swapped node.
        second = node;
      }
      prev = node;

      self(node->right);
    };

    dfs(root);

    if (first != nullptr && second != nullptr) {
      std::swap(first->val, second->val);
    }
  }

  // Follow-up: O(1) auxiliary space recovery using Morris inorder traversal.
  // Time: O(n), Space: O(1).
  void recoverTreeMorris(TreeNode* root) {
    TreeNode* prev = nullptr;
    TreeNode* first = nullptr;
    TreeNode* second = nullptr;
    TreeNode* cur = root;

    // Records any inorder inversion against `prev` and advances `prev`.
    auto visit = [&](TreeNode* node) {
      if (prev != nullptr && prev->val > node->val) {
        if (first == nullptr) {
          first = prev;
        }
        second = node;
      }
      prev = node;
    };

    while (cur != nullptr) {
      if (cur->left == nullptr) {
        // No left subtree: visit `cur` immediately and advance right.
        visit(cur);
        cur = cur->right;
        continue;
      }

      // Find the rightmost node in `cur`'s left subtree (inorder predecessor).
      TreeNode* pred = cur->left;
      while (pred->right != nullptr && pred->right != cur) {
        pred = pred->right;
      }

      if (pred->right == nullptr) {
        // First time reaching `cur`: install temporary thread back to `cur`
        // and descend into the left subtree.
        pred->right = cur;
        cur = cur->left;
      } else {
        // Thread already points to `cur`: left subtree is complete; remove the
        // temporary thread, visit `cur`, and descend into the right subtree.
        pred->right = nullptr;
        visit(cur);
        cur = cur->right;
      }
    }

    if (first != nullptr && second != nullptr) {
      std::swap(first->val, second->val);
    }
  }
};

LC_PROBLEM(99, "Recover Binary Search Tree", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_adjacent_and_root_swap", [&] {
    //    1               3
    //   /               /
    //  3       ==>     1
    //   \               \
    //    2               2
    auto run_case = [&](bool use_morris) {
      TreeArena arena;
      TreeNode* n2 = arena.make(2);
      TreeNode* n3 = arena.make(3, nullptr, n2);
      TreeNode* root = arena.make(1, n3, nullptr);

      if (use_morris) {
        sol.recoverTreeMorris(root);
      } else {
        sol.recoverTree(root);
      }

      ctx.expect_eq(inorderValues(root), std::vector<int>{1, 2, 3});
      ctx.expect_eq(root->val, 3);
      ctx.expect_eq(root->left->val, 1);
      ctx.expect_eq(root->left->right->val, 2);
    };
    run_case(false);
    run_case(true);
  });

  ctx.test("example_2_non_adjacent_swap_python_build", [&] {
    //    3               2
    //   / \             / \
    //  1   4    ==>    1   4
    //     /               /
    //    2               3
    auto run_case = [&](bool use_morris) {
      TreeArena arena;
      TreeNode* n1 = arena.make(1);
      TreeNode* n2 = arena.make(2);
      TreeNode* n4 = arena.make(4, n2, nullptr);
      TreeNode* root = arena.make(3, n1, n4);

      if (use_morris) {
        sol.recoverTreeMorris(root);
      } else {
        sol.recoverTree(root);
      }

      ctx.expect_eq(inorderValues(root), std::vector<int>{1, 2, 3, 4});
      ctx.expect_eq(root->val, 2);
      ctx.expect_eq(root->right->left->val, 3);
    };
    run_case(false);
    run_case(true);
  });

  ctx.test("two_node_tree_swap", [&] {
    auto run_case = [&](bool use_morris) {
      TreeArena arena;
      //   1           2
      //  /    ==>    /
      // 2           1
      TreeNode* root = arena.make(1, arena.make(2), nullptr);
      if (use_morris) {
        sol.recoverTreeMorris(root);
      } else {
        sol.recoverTree(root);
      }
      ctx.expect_eq(inorderValues(root), std::vector<int>{1, 2});
      ctx.expect_eq(root->val, 2);
      ctx.expect_eq(root->left->val, 1);
    };
    run_case(false);
    run_case(true);
  });

  ctx.test("swapped_leaves_with_negative_values", [&] {
    auto run_case = [&](bool use_morris) {
      TreeArena arena;
      // Valid BST: [-10, -5, 0, 5, 10]; swap -10 and 10.
      TreeNode* left = arena.make(-5, arena.make(10), nullptr);
      TreeNode* right = arena.make(5, nullptr, arena.make(-10));
      TreeNode* root = arena.make(0, left, right);

      if (use_morris) {
        sol.recoverTreeMorris(root);
      } else {
        sol.recoverTree(root);
      }

      ctx.expect_eq(inorderValues(root), std::vector<int>{-10, -5, 0, 5, 10});
    };
    run_case(false);
    run_case(true);
  });

  ctx.test("null_root_safe", [&] {
    sol.recoverTree(nullptr);
    sol.recoverTreeMorris(nullptr);
    ctx.expect_true(true);
  });
}

}  // namespace
