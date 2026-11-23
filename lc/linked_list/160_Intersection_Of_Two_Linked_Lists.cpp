// LeetCode 160: Intersection Of Two Linked Lists
// https://leetcode.com/problems/intersection-of-two-linked-lists/
//
// Problem:
//   Given the heads of two acyclic singly linked lists `headA` and `headB`,
//   return the node at which the two lists intersect (by pointer identity, not
//   merely equal value). If the two linked lists have no intersection, return
//   `nullptr`. Both lists must retain their original structure after the
//   function returns.
//
// Algorithm:
//   Two-pointer path-equalization traversal:
//   Initialize `pa = headA` and `pb = headB`. On each iteration while
//   `pa != pb`, advance `pa` to `pa->next` (or wrap to `headB` once `pa` hits
//   `nullptr`), and symmetrically advance `pb` to `pb->next` (or wrap to
//   `headA` once `pb` hits `nullptr`).
//   Invariant: Let `a` and `b` be the lengths of the unique prefixes of list A
//   and list B, and `c` be the length of their shared tail (`c = 0` if
//   disjoint). After traversing `a + c + b` steps, both pointers arrive
//   simultaneously at the first shared node (or simultaneously reach `nullptr`
//   after `a + b` steps when `c = 0`), terminating the loop in at most
//   `m + n` iterations.
//   Time Complexity: O(m + n), where m and n are the lengths of the two lists.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Two read-only traversal pointers (`pa`, `pb`) over existing singly linked
//   lists; no hash set or mutation of list links is required.
//
// Real-World Applications:
//   Finding the merge-base (lowest common ancestor commit) of two linear Git
//   branches, detecting shared suffix chains in persistent/immutable functional
//   linked lists (structural sharing in Clojure/Haskell), and coalescing shared
//   call-stack frames in profilers.
//
// Related LeetCode Problems:
//   - #142 Linked List Cycle II [done] (reducing intersection to cycle detection or vice versa)
//   - #1650 Lowest Common Ancestor of a Binary Tree III [done] (exact same two-pointer parent-swap trick on trees with parent pointers)
//   - #599 Minimum Index Sum of Two Lists [new] (finding common elements across sequences)
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (generalized common-ancestor finding)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Standard LeetCode singly linked list node.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  ListNode() = default;
  explicit ListNode(int value) : val(value) {}
  ListNode(int value, ListNode* next_node) : val(value), next(next_node) {}
};

class Solution {
 public:
  // Returns the first shared node where `head_a` and `head_b` intersect, or
  // `nullptr` if the two lists are disjoint. Leaves both lists unmodified.
  // Time: O(m + n), Space: O(1).
  ListNode* getIntersectionNode(ListNode* head_a, ListNode* head_b) {
    if (head_a == nullptr || head_b == nullptr) {
      return nullptr;
    }

    ListNode* pa = head_a;
    ListNode* pb = head_b;

    // Each pointer traverses A + B (or B + A), equalizing any prefix-length
    // difference so both pointers align on the first shared node or nullptr.
    while (pa != pb) {
      pa = (pa != nullptr) ? pa->next : head_b;
      pb = (pb != nullptr) ? pb->next : head_a;
    }

    return pa;
  }
};

// RAII arena that owns test ListNodes in a std::deque without manual deletes.
class ListArena {
 public:
  // Builds a linked list from `prefix` values and attaches `shared_tail` to the
  // end (returning `shared_tail` directly if `prefix` is empty).
  ListNode* build_with_tail(const std::vector<int>& prefix,
                            ListNode* shared_tail = nullptr) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (const int val : prefix) {
      tail->next = &nodes_.emplace_back(val);
      tail = tail->next;
    }
    tail->next = shared_tail;
    return dummy.next;
  }

 private:
  std::deque<ListNode> nodes_;
};

// Serializes a singly linked list into a vector of values for structure checks.
std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    out.push_back(curr->val);
  }
  return out;
}

LC_PROBLEM(160, "Intersection Of Two Linked Lists", "linked_list", ctx) {
  ctx.test("example_1_different_prefix_lengths", [&] {
    ListArena arena;
    ListNode* shared = arena.build_with_tail({8, 4, 5});
    ListNode* head_a = arena.build_with_tail({4, 1}, shared);
    ListNode* head_b = arena.build_with_tail({5, 6, 1}, shared);

    Solution sol;
    ctx.expect_eq(sol.getIntersectionNode(head_a, head_b), shared);
    // Verify original structures remain unmodified.
    ctx.expect_eq(to_vector(head_a), std::vector<int>{4, 1, 8, 4, 5});
    ctx.expect_eq(to_vector(head_b), std::vector<int>{5, 6, 1, 8, 4, 5});
  });

  ctx.test("example_2_shorter_second_prefix", [&] {
    ListArena arena;
    ListNode* shared = arena.build_with_tail({2, 4});
    ListNode* head_a = arena.build_with_tail({1, 9, 1}, shared);
    ListNode* head_b = arena.build_with_tail({3}, shared);

    Solution sol;
    ctx.expect_eq(sol.getIntersectionNode(head_a, head_b), shared);
  });

  ctx.test("disjoint_lists_including_duplicate_values", [&] {
    ListArena arena;
    ListNode* head_a = arena.build_with_tail({1, 2, 3, 4, 5});
    ListNode* head_b = arena.build_with_tail({6, 7, 3, 4, 5});

    Solution sol;
    ctx.expect_eq(sol.getIntersectionNode(head_a, head_b), nullptr);
  });

  ctx.test("intersection_at_head_same_list", [&] {
    ListArena arena;
    ListNode* shared = arena.build_with_tail({10, 20, 30});

    Solution sol;
    ctx.expect_eq(sol.getIntersectionNode(shared, shared), shared);
  });

  ctx.test("one_list_is_suffix_of_other", [&] {
    ListArena arena;
    ListNode* shared = arena.build_with_tail({3, 4, 5});
    ListNode* head_a = arena.build_with_tail({1, 2}, shared);

    Solution sol;
    ctx.expect_eq(sol.getIntersectionNode(head_a, shared), shared);
  });

  ctx.test("empty_inputs", [&] {
    ListArena arena;
    ListNode* list = arena.build_with_tail({1, 2});
    Solution sol;
    ctx.expect_eq(sol.getIntersectionNode(nullptr, list), nullptr);
    ctx.expect_eq(sol.getIntersectionNode(list, nullptr), nullptr);
    ctx.expect_eq(sol.getIntersectionNode(nullptr, nullptr), nullptr);
  });
}

}  // namespace
