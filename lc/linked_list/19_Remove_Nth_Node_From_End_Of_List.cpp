// LeetCode 19: Remove Nth Node From End Of List
// https://leetcode.com/problems/remove-nth-node-from-end-of-list/
//
// Problem:
//   Given the head of a singly-linked list and a valid 1-indexed integer `n`,
//   remove the n-th node from the end of the list and return the updated head.
//   Follow-up requirement: solve in a single pass over the list.
//
// Examples:
//   Example 1:
//     Input:  head = [1, 2, 3, 4, 5], n = 2
//     Output: [1, 2, 3, 5]
//     Explanation:
//       - Attach sentinel dummy(0) before node 1 and initialize both slow and fast
//         at &dummy.
//       - Advance fast by n = 2 steps to node 2 so fast leads slow by 2 links.
//       - Advance slow and fast in lockstep until fast reaches tail node 5, moving
//         slow to node 3 (predecessor of node 4).
//       - Unlink removed node 4 by setting 3->next = 5 and return dummy.next ([1,
//         2, 3, 5]).
//
//   Example 2:
//     Input:  head = [1], n = 1
//     Output: []
//     Explanation:
//       - Attach sentinel dummy(0) before node 1 with slow = &dummy and fast =
//         &dummy.
//       - Advance fast by n = 1 step to node 1; because fast->next is nullptr, the
//         lockstep loop does not run and slow stays at &dummy.
//       - Unlink dummy.next (node 1) by setting dummy.next = nullptr and return
//         dummy.next ([]).
//
//   Example 3:
//     Input:  head = [10, 20, 30, 40], n = 4
//     Output: [20, 30, 40]
//     Explanation:
//       - Attach sentinel dummy(0) before node 10 with slow = &dummy and fast =
//         &dummy.
//       - Advance fast by n = 4 steps across 10 -> 20 -> 30 -> 40 until fast sits
//         at tail node 40.
//       - Since fast->next is nullptr, slow remains at &dummy; bypass head node 10
//         via dummy.next = 20 and return [20, 30, 40].
//
// Algorithm:
//   One-pass two-pointer ("fast/slow gap") technique with a sentinel dummy node
//   pointing to `head`:
//   1. Initialize both `slow` and `fast` at `&dummy`.
//   2. Advance `fast` by `n` steps so that `fast` leads `slow` by exactly `n`
//      links.
//   3. Advance both `slow` and `fast` in lockstep until `fast->next == nullptr`
//      (i.e., `fast` sits on the last node). At that point, `slow` sits directly
//      before the n-th node from the end.
//   4. Unlink `slow->next` and return `dummy.next` (which seamlessly handles
//      removing the original head node).
//   Time Complexity: O(L), where L is the number of nodes (single pass).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Singly-linked list (`ListNode`) with a stack-allocated dummy sentinel.
//   - Two traversal pointers (`slow`, `fast`) maintaining a fixed gap of `n`.
//
// Real-World Applications:
//   - Sliding-window eviction over append-only event streams or ring buffers
//     where total stream length is unknown ahead of time.
//   - Removing trailing undo/history entries relative to the end of a linked
//     command log in one pass.
//
// Related LeetCode Problems:
//   - #2095 Delete the Middle Node of a Linked List [new] (fast/slow deletion)
//   - #876 Middle of the Linked List [done] (two-pointer list traversal)
//   - #1721 Swapping Nodes in a Linked List [new] (k-th node from start and end)
//   - #237 Delete Node in a Linked List [done] (in-place node removal)
//   - #61 Rotate List [done] (splitting a list relative to the k-th node from end)

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
  // Removes the n-th node from the end of `head` in one pass and returns the
  // resulting head. Does not call `delete` on the unlinked node so callers
  // (such as arenas or LeetCode's harness) retain ownership.
  // Time: O(L), Space: O(1).
  ListNode* removeNthFromEnd(ListNode* head, int n) {
    if (head == nullptr || n <= 0) {
      return head;
    }

    // Sentinel dummy simplifies removing the first node (when n == length).
    ListNode dummy(0, head);
    ListNode* slow = &dummy;
    ListNode* fast = &dummy;

    // Create a gap of `n` nodes between `slow` and `fast`.
    for (int step = 0; step < n; ++step) {
      if (fast->next == nullptr) {
        // Defensive guard if `n` exceeds list length.
        return dummy.next;
      }
      fast = fast->next;
    }

    // Move both pointers together until `fast` reaches the tail; `slow` then
    // points to the predecessor of the node to remove.
    while (fast->next != nullptr) {
      slow = slow->next;
      fast = fast->next;
    }

    // Detach the target node from the list.
    ListNode* removed = slow->next;
    if (removed != nullptr) {
      slow->next = removed->next;
      removed->next = nullptr;
    }

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

LC_PROBLEM(19, "Remove Nth Node From End Of List", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_middle_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.removeNthFromEnd(head, 2)),
                  std::vector<int>{1, 2, 3, 5});
  });

  ctx.test("single_node_removed", [&] {
    ListArena arena;
    ListNode* head = arena.build({1});
    ctx.expect_eq(to_vector(sol.removeNthFromEnd(head, 1)), std::vector<int>{});
  });

  ctx.test("remove_head_of_two", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2});
    ctx.expect_eq(to_vector(sol.removeNthFromEnd(head, 2)),
                  std::vector<int>{2});
  });

  ctx.test("remove_tail_of_two", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2});
    ctx.expect_eq(to_vector(sol.removeNthFromEnd(head, 1)),
                  std::vector<int>{1});
  });

  ctx.test("remove_head_of_longer_list", [&] {
    ListArena arena;
    ListNode* head = arena.build({10, 20, 30, 40});
    ctx.expect_eq(to_vector(sol.removeNthFromEnd(head, 4)),
                  std::vector<int>{20, 30, 40});
  });

  ctx.test("empty_list", [&] {
    ctx.expect_eq(sol.removeNthFromEnd(nullptr, 1), nullptr);
  });
}

}  // namespace
