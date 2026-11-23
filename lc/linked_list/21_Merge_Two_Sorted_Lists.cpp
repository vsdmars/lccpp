// LeetCode 21: Merge Two Sorted Lists
// https://leetcode.com/problems/merge-two-sorted-lists/
//
// Problem:
//   Given the heads of two sorted singly-linked lists `list1` and `list2`,
//   merge them into a single sorted linked list by splicing together the
//   existing nodes of the two lists, and return the head of the merged list.
//
// Algorithm:
//   Iterative two-pointer merge with a sentinel dummy node:
//   1. Maintain a `tail` pointer initialized to `&dummy`.
//   2. While both `list1` and `list2` are non-null, compare their front values,
//      splice the smaller node onto `tail->next`, and advance both that input
//      pointer and `tail`.
//   3. Once one list is exhausted, attach the entire remaining suffix of the
//      other list in O(1) time.
//   Time Complexity: O(m + n), where m and n are the lengths of the two lists.
//   Space Complexity: O(1) auxiliary space (in-place node splicing).
//
// Data Structures:
//   - Singly-linked list (`ListNode`) spliced in place without allocating nodes.
//   - Stack-allocated dummy sentinel node to eliminate empty-result branches.
//
// Real-World Applications:
//   - The merge phase of external merge sort and LSM-tree compaction (RocksDB,
//     LevelDB) combining sorted SSTable runs.
//   - Merging two pre-sorted database index scans (Sort-Merge Join) or
//     chronologically ordered log streams.
//
// Related LeetCode Problems:
//   - #23 Merge K Sorted Lists [done] (k-way generalization using a min-heap)
//   - #88 Merge Sorted Array [done] (in-place merge on contiguous arrays)
//   - #148 Sort List [done] (top-down/bottom-up merge sort on linked lists)
//   - #83 Remove Duplicates from Sorted List [done] (single-pass sorted list walk)
//   - #1669 Merge In Between Linked Lists [new] (splicing linked list segments)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Standard LeetCode singly-linked list node.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  explicit ListNode(int x, ListNode* nxt = nullptr) : val(x), next(nxt) {}
};

class Solution {
 public:
  // Splices two non-decreasing linked lists into one sorted list in O(m + n)
  // time and O(1) extra space.
  ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
    ListNode dummy(0);
    ListNode* tail = &dummy;

    // Repeatedly attach the smaller head node to the merged tail.
    while (list1 != nullptr && list2 != nullptr) {
      if (list1->val <= list2->val) {
        tail->next = list1;
        list1 = list1->next;
      } else {
        tail->next = list2;
        list2 = list2->next;
      }
      tail = tail->next;
    }

    // At most one list has remaining nodes; append its entire suffix in O(1).
    tail->next = (list1 != nullptr) ? list1 : list2;
    return dummy.next;
  }
};

// Arena owning all test nodes via `std::deque` (stable pointers, zero leaks).
class ListArena {
 public:
  ListNode* build(const std::vector<int>& vals) {
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    for (const int v : vals) {
      nodes_.emplace_back(v);
      ListNode* curr = &nodes_.back();
      if (head == nullptr) {
        head = curr;
      } else {
        tail->next = curr;
      }
      tail = curr;
    }
    return head;
  }

 private:
  std::deque<ListNode> nodes_;
};

std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  while (head != nullptr) {
    out.push_back(head->val);
    head = head->next;
  }
  return out;
}

LC_PROBLEM(21, "Merge Two Sorted Lists", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_interleaved", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({1, 2, 4});
    ListNode* l2 = arena.build({1, 3, 4});
    ctx.expect_eq(to_vector(sol.mergeTwoLists(l1, l2)),
                  std::vector<int>{1, 1, 2, 3, 4, 4});
  });

  ctx.test("disjoint_ranges", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({1, 2, 3, 4, 5});
    ListNode* l2 = arena.build({6, 7, 8, 9});
    ctx.expect_eq(to_vector(sol.mergeTwoLists(l1, l2)),
                  std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  });

  ctx.test("both_empty", [&] {
    ctx.expect_eq(sol.mergeTwoLists(nullptr, nullptr), nullptr);
  });

  ctx.test("one_empty_list", [&] {
    ListArena arena;
    ListNode* l2 = arena.build({0});
    ctx.expect_eq(to_vector(sol.mergeTwoLists(nullptr, l2)),
                  std::vector<int>{0});

    ListNode* l1 = arena.build({1, 2});
    ctx.expect_eq(to_vector(sol.mergeTwoLists(l1, nullptr)),
                  std::vector<int>{1, 2});
  });

  ctx.test("negative_and_duplicate_values", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({-5, 0, 10});
    ListNode* l2 = arena.build({-3, 0, 2});
    ctx.expect_eq(to_vector(sol.mergeTwoLists(l1, l2)),
                  std::vector<int>{-5, -3, 0, 0, 2, 10});
  });
}

}  // namespace
