// LeetCode 109: Convert Sorted List To Binary Search Tree
// https://leetcode.com/problems/convert-sorted-list-to-binary-search-tree/
//
// Problem:
//   Given the head of a singly linked list whose elements are sorted in
//   strictly ascending order, convert it into a height-balanced binary search
//   tree (BST), where the depths of the left and right subtrees of every node
//   differ by at most 1.
//   Constraints: 0 <= number of nodes <= 2 * 10^4, -10^5 <= Node.val <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  head = [-10, -3, 0, 5, 9]
//     Output: [0, -10, 5, null, -3, null, 9]
//     Explanation:
//       - Flatten the linked list into `values = [-10, -3, 0, 5, 9]` and call
//         `build_from_slice(values, 0, 5)` (or split via slow/fast pointers at node
//         `0`).
//       - Select left-biased midpoint `mid = 0 + (5 - 0 - 1) / 2 = 2`, making
//         `values[2] = 0` the root node.
//       - Recurse on left slice `[0, 2)` to pick `mid = 0` (`-10`) with right child
//         `[1, 2)` (`-3`), producing left subtree `[-10, null, -3]`.
//       - Recurse on right slice `[3, 5)` to pick `mid = 3` (`5`) with right child
//         `[4, 5)` (`9`), producing right subtree `[5, null, 9]`.
//       - Combine subtrees under root `0` to obtain the height-balanced BST `[0,
//         -10, 5, null, -3, null, 9]` with inorder `[-10, -3, 0, 5, 9]`.
//
//   Example 2:
//     Input:  head = [1, 2, 3, 4]
//     Output: [2, 1, 3, null, null, null, 4]
//     Explanation:
//       - Copy the list into `values = [1, 2, 3, 4]` and pick `mid = 0 + (4 - 0 -
//         1) / 2 = 1` (`2`) as the root.
//       - Recurse on `[0, 1)` to create left leaf `1` (`mid = 0`).
//       - Recurse on `[2, 4)` to pick `mid = 2 + (4 - 2 - 1) / 2 = 2` (`3`) with
//         right leaf `[3, 4)` (`4`).
//       - Return the balanced BST `[2, 1, 3, null, null, null, 4]` whose subtree
//         heights differ by at most 1.
//
//   Example 3:
//     Input:  head = []
//     Output: []
//     Explanation:
//       - Check `head == nullptr` at the entry of `sortedListToBST` and
//         `build_by_splitting`.
//       - Return `nullptr` (`[]`), an empty tree with height `0` and inorder
//         sequence `[]`.
//
// Algorithm:
//   Two complementary divide-and-conquer methods (matching both approaches in
//   the source solution):
//   1. `sortedListToBST` (Array-Indexed Divide & Conquer — optimal time):
//      Flatten the sorted list into a contiguous `std::vector<int>` in O(n),
//      then recursively choose the midpoint `mid = lo + ((hi - lo - 1) / 2)`
//      of the half-open slice `[lo, hi)` as the root, building the left
//      subtree from `[lo, mid)` and the right subtree from `[mid + 1, hi)`.
//      Time Complexity: O(n).
//      Space Complexity: O(n) for the value array plus O(log n) call stack.
//   2. `sortedListToBSTInPlace` (Slow/Fast Midpoint Splitting — optimal space):
//      Locate the middle node directly on the linked list using slow/fast
//      pointers, sever `prev->next = nullptr` to detach the left half, create
//      the root from the middle node, and recurse on the left and right halves.
//      Time Complexity: O(n log n), since each recursion level scans O(n).
//      Space Complexity: O(log n) recursion stack (no auxiliary value array).
//
// Data Structures:
//   - Singly linked list (`ListNode`) as input.
//   - Binary search tree (`TreeNode`) as output.
//   - Contiguous `std::vector<int>` for O(1) random-access midpoint selection
//     in the primary O(n) method.
//
// Real-World Applications:
//   - Bulk-loading balanced database indexes (B-trees / binary search trees)
//     from pre-sorted WAL segments or merge-sorted SSTable runs.
//   - Rebalancing skewed in-memory search trees by flattening to a sorted
//     chain and rebuilding a perfectly balanced tree (Day-Stout-Warren style).
//   - Constructing static optimal search trees from sorted symbol tables in
//     compilers and routers.
//
// Related LeetCode Problems:
//   - #108 Convert Sorted Array to Binary Search Tree [done] (array version of this problem)
//   - #110 Balanced Binary Tree [done] (verifies the height-balance invariant)
//   - #98 Validate Binary Search Tree [done] (verifies BST ordering invariant)
//   - #876 Middle of the Linked List [done] (slow/fast midpoint finder)
//   - #1382 Balance a Binary Search Tree [done] (inorder flatten + balanced rebuild)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <vector>

namespace {

// Standard LeetCode singly linked list node.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  ListNode() = default;
  explicit ListNode(int x) : val(x) {}
  ListNode(int x, ListNode* nxt) : val(x), next(nxt) {}
};

// Standard LeetCode binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  TreeNode() = default;
  explicit TreeNode(int x) : val(x) {}
  TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

class Solution {
 public:
  // Primary O(n)-time approach: copy list values into a vector for O(1)
  // midpoint access, then build the balanced BST recursively over [lo, hi).
  // Time: O(n), Space: O(n).
  TreeNode* sortedListToBST(const ListNode* head) {
    if (head == nullptr) {
      return nullptr;
    }

    std::vector<int> values;
    for (const ListNode* cur = head; cur != nullptr; cur = cur->next) {
      values.push_back(cur->val);
    }

    return build_from_slice(values, 0uz, values.size());
  }

  // Secondary O(log n)-space approach: split the linked list in place using
  // slow/fast pointers to find the midpoint at each recursion level.
  // Note: mutates `next` links of the input list while partitioning.
  // Time: O(n log n), Space: O(log n).
  TreeNode* sortedListToBSTInPlace(ListNode* head) {
    return build_by_splitting(head);
  }

 private:
  // Recursively constructs a height-balanced BST from sorted `values[lo..hi)`.
  static TreeNode* build_from_slice(const std::vector<int>& values,
                                    std::size_t lo, std::size_t hi) {
    if (lo >= hi) {
      return nullptr;
    }

    // Left-biased midpoint guarantees |left_size - right_size| <= 1.
    const std::size_t mid = lo + (((hi - lo) - 1uz) / 2uz);
    TreeNode* left = build_from_slice(values, lo, mid);
    TreeNode* right = build_from_slice(values, mid + 1uz, hi);
    return new TreeNode(values[mid], left, right);
  }

  // Finds the left-biased middle node of `head`, detaches the left half, and
  // builds left/right subtrees recursively.
  static TreeNode* build_by_splitting(ListNode* head) {
    if (head == nullptr) {
      return nullptr;
    }

    ListNode* prev = nullptr;
    ListNode* slow = head;
    ListNode* fast = (head->next != nullptr) ? head->next->next : nullptr;

    // Advance `fast` twice as fast as `slow` so `slow` stops at the midpoint.
    while (fast != nullptr && slow->next != nullptr) {
      prev = slow;
      slow = slow->next;
      fast = (fast->next != nullptr) ? fast->next->next : nullptr;
    }

    // Sever the left half from the midpoint node so left recursion terminates.
    if (prev != nullptr) {
      prev->next = nullptr;
    }

    TreeNode* left = (head != slow) ? build_by_splitting(head) : nullptr;
    TreeNode* right = build_by_splitting(slow->next);
    return new TreeNode(slow->val, left, right);
  }
};

// RAII arena for test ListNode instances.
struct ListArena {
  std::deque<ListNode> nodes;

  ListNode* build(const std::vector<int>& values) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (const int v : values) {
      nodes.emplace_back(v);
      tail->next = &nodes.back();
      tail = tail->next;
    }
    return dummy.next;
  }
};

// RAII owner that recursively deletes a heap-allocated TreeNode hierarchy.
struct TreeGuard {
  TreeNode* root = nullptr;

  explicit TreeGuard(TreeNode* r) : root(r) {}
  TreeGuard(const TreeGuard&) = delete;
  TreeGuard& operator=(const TreeGuard&) = delete;
  TreeGuard(TreeGuard&&) = delete;
  TreeGuard& operator=(TreeGuard&&) = delete;
  ~TreeGuard() { free_tree(root); }

 private:
  static void free_tree(TreeNode* node) {
    if (node == nullptr) {
      return;
    }
    free_tree(node->left);
    free_tree(node->right);
    delete node;
  }
};

// Collects inorder traversal values (must equal the sorted input for any BST).
void collect_inorder(const TreeNode* node, std::vector<int>& out) {
  if (node == nullptr) {
    return;
  }
  collect_inorder(node->left, out);
  out.push_back(node->val);
  collect_inorder(node->right, out);
}

// Returns the height of `node`, or -1 if any subtree violates balance (> 1).
int balanced_height(const TreeNode* node) {
  if (node == nullptr) {
    return 0;
  }
  const int lh = balanced_height(node->left);
  if (lh == -1) {
    return -1;
  }
  const int rh = balanced_height(node->right);
  if (rh == -1) {
    return -1;
  }
  if ((std::max(lh, rh) - std::min(lh, rh)) > 1) {
    return -1;
  }
  return 1 + std::max(lh, rh);
}

bool is_valid_balanced_bst(const TreeNode* root,
                           const std::vector<int>& expected_sorted) {
  std::vector<int> inorder;
  collect_inorder(root, inorder);
  return inorder == expected_sorted && balanced_height(root) != -1;
}

LC_PROBLEM(109, "Convert Sorted List To Binary Search Tree", "linked_list",
           ctx) {
  Solution sol;

  auto check_both = [&](const std::vector<int>& input) {
    ListArena arena1;
    ListNode* list1 = arena1.build(input);
    TreeGuard tree1(sol.sortedListToBST(list1));
    ctx.expect_true(is_valid_balanced_bst(tree1.root, input));

    ListArena arena2;
    ListNode* list2 = arena2.build(input);
    TreeGuard tree2(sol.sortedListToBSTInPlace(list2));
    ctx.expect_true(is_valid_balanced_bst(tree2.root, input));
  };

  ctx.test("example_1_five_nodes", [&] { check_both({-10, -3, 0, 5, 9}); });

  ctx.test("empty_and_single_node", [&] {
    check_both({});
    check_both({42});
  });

  ctx.test("small_even_and_odd_lengths", [&] {
    check_both({1, 2});
    check_both({1, 2, 3});
    check_both({1, 2, 3, 4});
  });

  ctx.test("larger_sorted_list",
           [&] { check_both({-50, -20, -10, -1, 0, 3, 7, 15, 29, 100}); });
}

}  // namespace
