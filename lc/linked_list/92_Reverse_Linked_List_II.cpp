// LeetCode 92: Reverse Linked List II
// https://leetcode.com/problems/reverse-linked-list-ii/
//
// Problem:
//   Given the head of a singly linked list and two 1-indexed integers `left`
//   and `right` with `1 <= left <= right <= n`, reverse the nodes of the list
//   from position `left` to position `right` in one pass, and return the
//   modified list head.
//   Constraints: 1 <= n <= 500, -500 <= Node.val <= 500, 1 <= left <= right <= n.
//
// Algorithm:
//   1. Handle trivial inputs (`head == nullptr` or `left >= right`) early.
//   2. Anchor a stack-allocated dummy sentinel before `head` so reversing a
//      prefix starting at position 1 requires no special-case head logic.
//   3. Advance `before_left` by `left - 1` steps so `before_left->next` points
//      to the `left`-th node (`sublist_tail`), which will become the tail of
//      the reversed segment.
//   4. Run standard iterative 3-pointer reversal for exactly
//      `count = right - left + 1` nodes:
//      - `flip_prev` accumulates the head of the reversed sublist.
//      - When the loop finishes, `flip_prev` is the original `right`-th node
//        and `cur` is the `(right + 1)`-th node (or `nullptr`).
//   5. Reconnect the reversed segment back into the list in O(1):
//      - `sublist_tail->next = cur` attaches the untouched suffix.
//      - `before_left->next = flip_prev` attaches the untouched prefix.
//   Time Complexity: O(n) in a single pass.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Singly linked list (`ListNode`) with a stack-allocated dummy sentinel.
//   - Local pointer quartet (`before_left`, `sublist_tail`, `flip_prev`, `cur`)
//     managing in-place segment reversal and boundary splicing.
//
// Real-World Applications:
//   - Undo/redo and edit-history range inversions in linked rope/piece-table
//     text editors.
//   - In-place sub-tour reversal (2-opt moves) over linked route lists in
//     vehicle-routing and TSP local-search heuristics.
//   - Reordering a contiguous slice of chained packet buffers or animation
//     keyframes without allocating temporary arrays.
//
// Related LeetCode Problems:
//   - #206 Reverse Linked List [done] (full-list iterative reversal building block)
//   - #25 Reverse Nodes in k-Group [done] (repeated fixed-size segment reversal)
//   - #24 Swap Nodes in Pairs [done] (special case of length-2 adjacent reversals)
//   - #61 Rotate List [done] (dummy-node boundary splicing)
//   - #234 Palindrome Linked List [done] (reversing a half-list in place)

#include "framework/lc_framework.h"

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

class Solution {
 public:
  // Reverses the 1-indexed inclusive range [left, right] in one pass.
  // Invariant: `before_left` stays immediately before the segment; after `count`
  // flips, `flip_prev` is the new segment head, `sublist_tail` is the new
  // segment tail, and `cur` is the first node after the segment.
  // Time: O(n), Space: O(1).
  ListNode* reverseBetween(ListNode* head, int left, int right) {
    if (head == nullptr || left >= right) {
      return head;
    }

    // Dummy node eliminates special casing when `left == 1`.
    ListNode dummy(0, head);
    ListNode* before_left = &dummy;

    // Walk `left - 1` steps to land directly before position `left`.
    for (int i = 1; i < left && before_left->next != nullptr; ++i) {
      before_left = before_left->next;
    }

    // The node originally at `left` will become the tail after reversal.
    ListNode* sublist_tail = before_left->next;
    if (sublist_tail == nullptr) {
      return dummy.next;
    }

    // Iteratively reverse `right - left + 1` links in place.
    ListNode* cur = sublist_tail;
    ListNode* flip_prev = nullptr;
    const int count = (right - left) + 1;
    for (int i = 0; i < count && cur != nullptr; ++i) {
      ListNode* next_node = cur->next;
      cur->next = flip_prev;
      flip_prev = cur;
      cur = next_node;
    }

    // Splice the reversed segment between the prefix (`before_left`) and the
    // remaining suffix (`cur`).
    sublist_tail->next = cur;
    before_left->next = flip_prev;

    return dummy.next;
  }
};

// RAII node arena for leak-free test lists.
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

std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* cur = head; cur != nullptr; cur = cur->next) {
    out.push_back(cur->val);
  }
  return out;
}

LC_PROBLEM(92, "Reverse Linked List II", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_middle_segment", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.reverseBetween(head, 2, 4)),
                  std::vector<int>{1, 4, 3, 2, 5});
  });

  ctx.test("python_build_adjacent_pair", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5, 6, 7});
    ctx.expect_eq(to_vector(sol.reverseBetween(head, 2, 3)),
                  std::vector<int>{1, 3, 2, 4, 5, 6, 7});
  });

  ctx.test("single_node_and_noop_range", [&] {
    ListArena arena;
    ListNode* single = arena.build({5});
    ctx.expect_eq(to_vector(sol.reverseBetween(single, 1, 1)),
                  std::vector<int>{5});

    ListNode* noop = arena.build({1, 2, 3});
    ctx.expect_eq(to_vector(sol.reverseBetween(noop, 2, 2)),
                  std::vector<int>{1, 2, 3});
  });

  ctx.test("full_prefix_and_suffix_reversals", [&] {
    ListArena arena;
    ListNode* full = arena.build({1, 2, 3, 4});
    ctx.expect_eq(to_vector(sol.reverseBetween(full, 1, 4)),
                  std::vector<int>{4, 3, 2, 1});

    ListNode* prefix = arena.build({1, 2, 3, 4});
    ctx.expect_eq(to_vector(sol.reverseBetween(prefix, 1, 2)),
                  std::vector<int>{2, 1, 3, 4});

    ListNode* suffix = arena.build({1, 2, 3, 4});
    ctx.expect_eq(to_vector(sol.reverseBetween(suffix, 3, 4)),
                  std::vector<int>{1, 2, 4, 3});
  });

  ctx.test("empty_list", [&] {
    ctx.expect_eq(to_vector(sol.reverseBetween(nullptr, 1, 1)),
                  std::vector<int>{});
  });
}

}  // namespace
