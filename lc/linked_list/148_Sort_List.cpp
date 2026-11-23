// LeetCode 148: Sort List
// https://leetcode.com/problems/sort-list/
//
// Problem:
//   Given the head of a singly linked list, return the list after sorting it in
//   ascending order. Target O(n log n) time complexity and minimal extra space
//   (O(log n) for top-down merge sort, or O(1) for bottom-up iterative merge
//   sort).
//
// Algorithm:
//   1. Top-Down Merge Sort (`sortList`):
//      - Base case: an empty list or single-node list is already sorted.
//      - Split: Use slow/fast pointers (`fast` initialized to `head->next`) to
//        find the midpoint and sever the list into two roughly equal halves.
//      - Conquer: Recursively sort both halves.
//      - Combine: Merge the two sorted halves stably in O(n) time using a
//        stack-allocated sentinel node.
//      Time Complexity: O(n log n).
//      Space Complexity: O(log n) recursion stack depth.
//
//   2. Bottom-Up Iterative Merge Sort (`sortListBottomUp`):
//      - Compute list length `n` in one pass.
//      - For run lengths `step = 1, 2, 4, ... < n`, sweep through the list,
//        detaching consecutive pairs of runs of size `step`, merging each pair
//        in-place, and reattaching the merged run to the running tail.
//      Time Complexity: O(n log n).
//      Space Complexity: O(1) auxiliary space (no recursion stack).
//
// Data Structures:
//   Singly linked list rewired in-place with sentinel (`dummy`) nodes and
//   pointer-based run splitting/merging; zero auxiliary arrays or node copies.
//
// Real-World Applications:
//   Sorting intrusive kernel lists (`std::list::sort` / Linux `list_sort`),
//   external merge sorting of streaming/disk-backed runs where random access is
//   unavailable, and stable sorting of database cursor result streams.
//
// Related LeetCode Problems:
//   - #21 Merge Two Sorted Lists [done] (exact combine step used by merge sort)
//   - #23 Merge k Sorted Lists [done] (generalizes divide-and-conquer list merging)
//   - #147 Insertion Sort List [new] (O(n^2) in-place linked list sorting)
//   - #912 Sort an Array [new] (comparison-based O(n log n) sorting on arrays)
//   - #315 Count of Smaller Numbers After Self [done] (merge-sort divide-and-conquer augmentation)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <utility>
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
  // Sorts a singly linked list in ascending order using top-down merge sort.
  // Time: O(n log n), Space: O(log n) recursion stack.
  ListNode* sortList(ListNode* head) {
    if (head == nullptr || head->next == nullptr) {
      return head;
    }

    // Divide the list into two non-empty halves and recursively sort each.
    ListNode* second_half = split_middle(head);
    ListNode* left_sorted = sortList(head);
    ListNode* right_sorted = sortList(second_half);

    // Combine the two sorted halves into one stably sorted list.
    return merge_sorted(left_sorted, right_sorted).first;
  }

  // Sorts a singly linked list in ascending order using bottom-up iterative
  // merge sort for strict O(1) auxiliary space.
  // Time: O(n log n), Space: O(1).
  ListNode* sortListBottomUp(ListNode* head) {
    if (head == nullptr || head->next == nullptr) {
      return head;
    }

    std::size_t length = 0uz;
    for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
      ++length;
    }

    ListNode dummy(0, head);
    // Repeatedly double the sorted run size and merge adjacent pairs of runs.
    for (std::size_t step = 1uz; step < length; step *= 2uz) {
      ListNode* prev = &dummy;
      ListNode* curr = dummy.next;

      while (curr != nullptr) {
        ListNode* left = curr;
        ListNode* right = split_prefix(left, step);
        curr = split_prefix(right, step);

        auto [merged_head, merged_tail] = merge_sorted(left, right);
        prev->next = merged_head;
        if (merged_tail != nullptr) {
          prev = merged_tail;
        }
      }
    }

    return dummy.next;
  }

 private:
  // Splits a list of at least two nodes into two halves in-place and returns
  // the head of the second half.
  static ListNode* split_middle(ListNode* head) {
    // Starting `fast` one node ahead guarantees `slow` stops at the last node
    // of the first half, ensuring strictly smaller subproblems on 2-node lists.
    ListNode* slow = head;
    ListNode* fast = head->next;
    while (fast != nullptr && fast->next != nullptr) {
      slow = slow->next;
      fast = fast->next->next;
    }

    ListNode* second_half = slow->next;
    slow->next = nullptr;
    return second_half;
  }

  // Detaches the first `count` nodes starting at `head` by setting the `count`-th
  // node's `next` to nullptr, and returns the head of the remaining suffix.
  static ListNode* split_prefix(ListNode* head, std::size_t count) {
    if (head == nullptr) {
      return nullptr;
    }
    ListNode* curr = head;
    for (std::size_t i = 1uz; i < count && curr->next != nullptr; ++i) {
      curr = curr->next;
    }
    ListNode* remainder = curr->next;
    curr->next = nullptr;
    return remainder;
  }

  // Stably merges two sorted lists and returns `{merged_head, merged_tail}`.
  static std::pair<ListNode*, ListNode*> merge_sorted(ListNode* left,
                                                      ListNode* right) {
    if (left == nullptr && right == nullptr) {
      return {nullptr, nullptr};
    }

    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (left != nullptr && right != nullptr) {
      // Prefer `left` on ties (`<=`) so the merge sort remains stable.
      if (left->val <= right->val) {
        tail->next = left;
        left = left->next;
      } else {
        tail->next = right;
        right = right->next;
      }
      tail = tail->next;
    }

    tail->next = (left != nullptr) ? left : right;
    while (tail->next != nullptr) {
      tail = tail->next;
    }

    ListNode* merged_tail = (tail == &dummy) ? nullptr : tail;
    return {dummy.next, merged_tail};
  }
};

// RAII arena that owns test ListNodes in a std::deque without manual deletes.
class ListArena {
 public:
  // Builds a linked list from `values` and returns its head pointer.
  ListNode* build(const std::vector<int>& values) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (const int val : values) {
      tail->next = &nodes_.emplace_back(val);
      tail = tail->next;
    }
    return dummy.next;
  }

 private:
  std::deque<ListNode> nodes_;
};

// Serializes a singly linked list into a vector of values for assertions.
std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    out.push_back(curr->val);
  }
  return out;
}

LC_PROBLEM(148, "Sort List", "linked_list", ctx) {
  ctx.test("example_1_four_elements", [&] {
    ListArena arena;
    ListNode* head = arena.build({4, 2, 1, 3});
    Solution sol;
    ctx.expect_eq(to_vector(sol.sortList(head)), std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("example_2_negatives_and_odd_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({-1, 5, 3, 4, 0});
    Solution sol;
    ctx.expect_eq(to_vector(sol.sortList(head)),
                  std::vector<int>{-1, 0, 3, 4, 5});
  });

  ctx.test("empty_and_single_node", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortList(nullptr), nullptr);

    ListArena arena;
    ListNode* single = arena.build({42});
    ctx.expect_eq(to_vector(sol.sortList(single)), std::vector<int>{42});
  });

  ctx.test("duplicates_and_reverse_sorted", [&] {
    ListArena arena;
    ListNode* head = arena.build({5, 3, 3, 1, 5, -2, -2, 0});
    Solution sol;
    ctx.expect_eq(to_vector(sol.sortList(head)),
                  std::vector<int>{-2, -2, 0, 1, 3, 3, 5, 5});
  });

  ctx.test("bottom_up_o1_space_matches_all_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortListBottomUp(nullptr), nullptr);

    ListArena arena1;
    ListNode* h1 = arena1.build({4, 2, 1, 3});
    ctx.expect_eq(to_vector(sol.sortListBottomUp(h1)),
                  std::vector<int>{1, 2, 3, 4});

    ListArena arena2;
    ListNode* h2 = arena2.build({-1, 5, 3, 4, 0});
    ctx.expect_eq(to_vector(sol.sortListBottomUp(h2)),
                  std::vector<int>{-1, 0, 3, 4, 5});

    ListArena arena3;
    ListNode* h3 = arena3.build({9, 8, 7, 6, 5, 4, 3, 2, 1});
    ctx.expect_eq(to_vector(sol.sortListBottomUp(h3)),
                  std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  });
}

}  // namespace
