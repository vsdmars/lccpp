// LeetCode 328: Odd Even Linked List
// https://leetcode.com/problems/odd-even-linked-list/
//
// Problem:
//   Given the head of a singly linked list, reorder the list so that all nodes
//   at odd 1-based positions (1st, 3rd, 5th, ...) appear first, followed by all
//   nodes at even 1-based positions (2nd, 4th, 6th, ...). The relative order
//   within both the odd group and the even group must be preserved, and the
//   reordering must run in O(n) time and O(1) extra space.
//
// Algorithm:
//   Two-pointer in-place chain splitting:
//   1. Lists with 0 or 1 nodes are already partitioned; return immediately.
//   2. Anchor `odd` at the 1st node (`head`) and `even` at the 2nd node
//      (`head->next`), saving `even_head` so the even chain can be appended
//      once traversal finishes.
//   3. While `even` and `even->next` exist, advance the odd chain to
//      `even->next` and then advance the even chain to the new `odd->next`.
//      This invariant guarantees `odd` is always non-null and ends at the
//      final odd-indexed node, while `even` ends at the final even-indexed
//      node (whose `next` is `nullptr`).
//   4. Splice the two chains together with `odd->next = even_head` and return
//      `head`.
//   Time Complexity: O(n), visiting each node once.
//   Space Complexity: O(1) auxiliary space (relinks existing nodes in place).
//
// Data Structures:
//   - Singly linked list (`ListNode`): rewired in place via three cursor
//     pointers (`odd`, `even`, `even_head`) without allocating new nodes.
//
// Real-World Applications:
//   - Interleaved buffer de-multiplexing (separating left/right audio channels
//     or odd/even scanlines in streaming video decoders in place).
//   - Memory-constrained embedded packet schedulers that partition linked
//     descriptors into alternating priority queues without copying payloads.
//   - In-place Radix/FFT butterfly index partitioning on linked chunks.
//
// Related LeetCode Problems:
//   - #86 Partition List [done] (in-place stable two-chain partition by value)
//   - #725 Split Linked List In Parts [done] (partitioning a linked list into k segments)
//   - #143 Reorder List [done] (splitting and interleaving linked list halves)
//   - #876 Middle Of The Linked List [done] (two-pointer singly linked list traversal)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Singly linked list node matching LeetCode's definition.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  ListNode() = default;
  explicit ListNode(int x) : val(x) {}
  ListNode(int x, ListNode* next_node) : val(x), next(next_node) {}
};

// RAII arena that owns test ListNodes without manual new/delete.
class ListArena {
 public:
  // Builds a linked list from `vals` and returns its head pointer.
  [[nodiscard]] ListNode* build(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* tail = &dummy;
    for (const int v : vals) {
      nodes_.emplace_back(v);
      tail->next = &nodes_.back();
      tail = tail->next;
    }
    return dummy.next;
  }

 private:
  std::deque<ListNode> nodes_;
};

// Serializes a singly linked list into a vector of values for assertions.
[[nodiscard]] std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> result;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    result.push_back(curr->val);
  }
  return result;
}

class Solution {
 public:
  // Groups odd-indexed nodes before even-indexed nodes in place.
  // Invariant: after each iteration, `odd` is the tail of the odd chain and
  // `even` is the tail of the even chain, with `even_head` fixed at node #2.
  // Time: O(n), Space: O(1).
  ListNode* oddEvenList(ListNode* head) {
    // Lists of length 0 or 1 have no even-indexed nodes to relocate.
    if (head == nullptr || head->next == nullptr) {
      return head;
    }

    ListNode* odd = head;
    ListNode* even = head->next;
    ListNode* const even_head = even;

    // Advance in lockstep as long as another odd node (`even->next`) exists.
    // Because we check `even != nullptr && even->next != nullptr`, `odd` never
    // becomes null and finishes on the last odd-indexed node.
    while (even != nullptr && even->next != nullptr) {
      odd->next = even->next;
      odd = odd->next;

      even->next = odd->next;
      even = even->next;
    }

    // Stitch the saved even chain onto the tail of the odd chain.
    odd->next = even_head;
    return head;
  }
};

LC_PROBLEM(328, "Odd Even Linked List", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_odd_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.oddEvenList(head)),
                  std::vector<int>({1, 3, 5, 2, 4}));
  });

  ctx.test("example_2_mixed_values", [&] {
    ListArena arena;
    ListNode* head = arena.build({2, 1, 3, 5, 6, 4, 7});
    ctx.expect_eq(to_vector(sol.oddEvenList(head)),
                  std::vector<int>({2, 3, 6, 7, 1, 5, 4}));
  });

  ctx.test("empty_list", [&] {
    ctx.expect_eq(sol.oddEvenList(nullptr), nullptr);
  });

  ctx.test("single_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({42});
    ctx.expect_eq(to_vector(sol.oddEvenList(head)), std::vector<int>({42}));
  });

  ctx.test("two_nodes", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2});
    ctx.expect_eq(to_vector(sol.oddEvenList(head)), std::vector<int>({1, 2}));
  });

  ctx.test("even_length_list", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5, 6});
    ctx.expect_eq(to_vector(sol.oddEvenList(head)),
                  std::vector<int>({1, 3, 5, 2, 4, 6}));
  });

  ctx.test("negative_and_duplicate_values", [&] {
    ListArena arena;
    ListNode* head = arena.build({-5, 0, -5, 10, 0, -1});
    ctx.expect_eq(to_vector(sol.oddEvenList(head)),
                  std::vector<int>({-5, -5, 0, 0, 10, -1}));
  });
}

}  // namespace
