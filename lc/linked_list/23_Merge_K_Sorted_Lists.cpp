// LeetCode 23: Merge K Sorted Lists
// https://leetcode.com/problems/merge-k-sorted-lists/
//
// Problem:
//   Given an array of `k` sorted singly-linked lists `lists`, merge all `k`
//   lists into one sorted singly-linked list by splicing their nodes together
//   and return its head.
//
// Examples:
//   Example 1:
//     Input:  lists = [[1, 4, 5], [1, 3, 4], [2, 6]]
//     Output: [1, 1, 2, 3, 4, 4, 5, 6]
//     Explanation:
//       - Seed min_heap with the three non-null list heads: [1 (list 0), 1 (list
//         1), 2 (list 2)].
//       - Pop 1 (list 0) and push its successor 4, then pop 1 (list 1) and push 3,
//         producing merged prefix [1, 1] and heap [2, 3, 4].
//       - Pop 2 (list 2) and push 6, then pop 3 (list 1) and push 4, then pop 4
//         (list 0) and push 5, extending merged output to [1, 1, 2, 3, 4] with heap
//         [4, 5, 6].
//       - Pop remaining nodes 4 (list 1), 5 (list 0), and 6 (list 2) in order (none
//         have successors), set tail->next = nullptr, and return [1, 1, 2, 3, 4, 4,
//         5, 6].
//
//   Example 2:
//     Input:  lists = [[], [2, 5], [], [-1, 3]]
//     Output: [-1, 2, 3, 5]
//     Explanation:
//       - Skip the two nullptr entries and seed min_heap with non-empty heads [-1
//         (list 3), 2 (list 1)].
//       - Pop -1 and push 3 (merged: [-1], heap: [2, 3]), then pop 2 and push 5
//         (merged: [-1, 2], heap: [3, 5]).
//       - Pop 3 then 5 (no successors left), terminate with tail->next = nullptr,
//         and return [-1, 2, 3, 5].
//
//   Example 3:
//     Input:  lists = [[], []]
//     Output: []
//     Explanation:
//       - Iterate over lists and skip both nullptr heads, leaving min_heap empty.
//       - Skip the while (!min_heap.empty()) loop, set tail->next = nullptr on
//         dummy, and return dummy.next (nullptr), representing [].
//
// Algorithm:
//   K-way merge using a binary min-heap (`std::priority_queue`):
//   1. Seed the min-heap with the head pointer of every non-empty input list.
//      At all times, the heap holds at most `k` frontier nodes—one per active
//      list.
//   2. Repeatedly pop the node with the smallest `val`, splice it onto the
//      merged list's `tail`, and—if that node has a successor—push its `next`
//      pointer back into the heap.
//   3. Terminate when the heap is empty and return `dummy.next`.
//   Time Complexity: O(N log k), where N is the total number of nodes across
//   all lists and k is the number of non-empty lists.
//   Space Complexity: O(k) auxiliary space for the min-heap.
//
// Data Structures:
//   - Binary min-heap (`std::priority_queue<ListNode*, std::vector<ListNode*>,
//     NodeGreater>`) maintaining the current minimum across all k frontiers.
//   - Stack-allocated dummy sentinel `ListNode` for clean tail splicing.
//
// Real-World Applications:
//   - Multi-way external merge sort in databases, MapReduce/Spark shuffle
//     stages, and distributed search-index merging.
//   - Merging timestamp-ordered telemetry, metrics, or log partitions from k
//     sharded nodes into a globally ordered stream.
//
// Related LeetCode Problems:
//   - #21 Merge Two Sorted Lists [done] (2-way base case)
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (min-heap k-way merge)
//   - #632 Smallest Range Covering Elements from K Lists [done] (k-list frontier heap)
//   - #373 Find K Pairs with Smallest Sums [done] (lazy min-heap frontier expansion)
//   - #148 Sort List [done] (linked-list merge sort)

#include "framework/lc_framework.h"

#include <deque>
#include <queue>
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
  // Merges `k` sorted linked lists into a single sorted list using a min-heap
  // over active list heads.
  // Time: O(N log k), Space: O(k).
  ListNode* mergeKLists(const std::vector<ListNode*>& lists) {
    // Min-heap comparator: returns true when `lhs` has lower priority (larger
    // value) than `rhs`.
    struct NodeGreater {
      bool operator()(const ListNode* lhs, const ListNode* rhs) const noexcept {
        return lhs->val > rhs->val;
      }
    };

    std::priority_queue<ListNode*, std::vector<ListNode*>, NodeGreater>
        min_heap;

    // Seed the frontier with the head of every non-empty list.
    for (ListNode* head : lists) {
      if (head != nullptr) {
        min_heap.push(head);
      }
    }

    ListNode dummy(0);
    ListNode* tail = &dummy;

    // Repeatedly extract the globally smallest frontier node and advance its
    // list into the heap.
    while (!min_heap.empty()) {
      ListNode* smallest = min_heap.top();
      min_heap.pop();

      tail->next = smallest;
      tail = smallest;

      if (smallest->next != nullptr) {
        min_heap.push(smallest->next);
      }
    }

    tail->next = nullptr;
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

LC_PROBLEM(23, "Merge K Sorted Lists", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_three_lists", [&] {
    ListArena arena;
    const std::vector<ListNode*> lists = {
        arena.build({1, 4, 5}),
        arena.build({1, 3, 4}),
        arena.build({2, 6}),
    };
    ctx.expect_eq(to_vector(sol.mergeKLists(lists)),
                  std::vector<int>{1, 1, 2, 3, 4, 4, 5, 6});
  });

  ctx.test("empty_outer_vector", [&] {
    ctx.expect_eq(sol.mergeKLists({}), nullptr);
  });

  ctx.test("only_empty_lists", [&] {
    const std::vector<ListNode*> lists = {nullptr, nullptr};
    ctx.expect_eq(sol.mergeKLists(lists), nullptr);
  });

  ctx.test("mixed_empty_and_non_empty", [&] {
    ListArena arena;
    const std::vector<ListNode*> lists = {
        nullptr,
        arena.build({2, 5}),
        nullptr,
        arena.build({-1, 3}),
    };
    ctx.expect_eq(to_vector(sol.mergeKLists(lists)),
                  std::vector<int>{-1, 2, 3, 5});
  });

  ctx.test("single_list", [&] {
    ListArena arena;
    const std::vector<ListNode*> lists = {arena.build({1, 2, 3})};
    ctx.expect_eq(to_vector(sol.mergeKLists(lists)),
                  std::vector<int>{1, 2, 3});
  });

  ctx.test("five_lists_with_negatives_and_duplicates", [&] {
    ListArena arena;
    const std::vector<ListNode*> lists = {
        arena.build({-10, -5, 0}),
        arena.build({-5, 2, 2}),
        arena.build({1}),
        arena.build({-100, 100}),
        arena.build({0, 3, 4}),
    };
    ctx.expect_eq(
        to_vector(sol.mergeKLists(lists)),
        std::vector<int>{-100, -10, -5, -5, 0, 0, 1, 2, 2, 3, 4, 100});
  });
}

}  // namespace
