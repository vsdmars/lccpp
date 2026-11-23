// LeetCode 445: Add Two Numbers II
// https://leetcode.com/problems/add-two-numbers-ii/
//
// Problem:
//   Given two non-empty singly linked lists representing two non-negative
//   integers in most-significant-digit-first order (each node stores a single
//   base-10 digit and there are no leading zeros except for 0 itself), compute
//   their sum and return it as a new linked list in the same forward order.
//   Follow-up constraint: do not modify or reverse the input linked lists.
//
// Examples:
//   Example 1:
//     Input:  l1 = [7, 2, 4, 3], l2 = [5, 6, 4]
//     Output: [7, 8, 0, 7]
//     Explanation:
//       - Push digits onto immutable stacks stack_a = [7, 2, 4, 3] and stack_b =
//         [5, 6, 4], starting with result_head = nullptr and carry = 0.
//       - Pop 3 + 4 + 0 = 7 (carry = 0) and prepend 7, then pop 4 + 6 + 0 = 10
//         (carry = 1) and prepend 0 to form [0, 7].
//       - Pop 2 + 5 + 1 = 8 (carry = 0) and prepend 8, then pop remaining 7 from
//         stack_a (sum = 7, carry = 0) and prepend 7.
//       - Both stacks and carry are now empty, returning [7, 8, 0, 7] without
//         mutating l1 or l2.
//
//   Example 2:
//     Input:  l1 = [9, 9, 9, 9], l2 = [1]
//     Output: [1, 0, 0, 0, 0]
//     Explanation:
//       - Populate stack_a = [9, 9, 9, 9] and stack_b = [1] with result_head =
//         nullptr and carry = 0.
//       - First pop computes 9 + 1 + 0 = 10 (carry = 1) and prepends 0; the next
//         three pops each compute 9 + 1 = 10 from stack_a and carry, prepending 0
//         to form [0, 0, 0, 0].
//       - Both stacks are empty while carry == 1, so one final iteration computes
//         sum = 1 (carry = 0) and prepends 1 to return [1, 0, 0, 0, 0].
//
//   Example 3:
//     Input:  l1 = [0], l2 = [0]
//     Output: [0]
//     Explanation:
//       - Push the single digit of each list so stack_a = [0] and stack_b = [0],
//         with carry = 0.
//       - Pop both tops to get sum = 0 + 0 + 0 = 0 (carry = 0) and prepend node 0
//         to result_head.
//       - Both stacks are empty and carry is 0, so the loop finishes and returns
//         [0].
//
// Algorithm:
//   Two-stack reverse-order digit addition with head insertion:
//   1. Traverse `l1` and `l2` without mutating them, pushing their digit values
//      onto two explicit stacks (`stack_a` and `stack_b`) so the least
//      significant digits sit on top.
//   2. Maintain a running `carry` and a `result_head` pointer (initially
//      `nullptr`). While either stack is non-empty or `carry != 0`, pop
//      available digits, compute `sum = d1 + d2 + carry`, update
//      `carry = sum / 10`, and prepend a new node `sum % 10` at the front of
//      `result_head`.
//   3. Prepending each computed digit naturally reconstructs the sum in
//      most-significant-digit-first order without a final reversal pass.
//   Time Complexity: O(m + n), where m and n are the lengths of `l1` and `l2`.
//   Space Complexity: O(m + n) for the two digit stacks (plus the output list).
//
// Data Structures:
//   - Two LIFO stacks (`std::vector<int>`): invert digit traversal order from
//     MSD-first to LSD-first while keeping the input linked lists immutable.
//   - Singly linked list (`ListNode`): built back-to-front via head prepending.
//
// Real-World Applications:
//   - Arbitrary-precision (BigInt) decimal arithmetic in financial ledgers and
//     cryptographic libraries where immutable operand buffers cannot be mutated.
//   - Multi-word network protocol sequence/counter aggregation over big-endian
//     byte streams.
//   - Immutable persistent data structure arithmetic in functional runtimes.
//
// Related LeetCode Problems:
//   - #2 Add Two Numbers [done] (same addition with least-significant digit first)
//   - #415 Add Strings [done] (base-10 carry propagation from right to left)
//   - #67 Add Binary [done] (carry-chain addition in base 2)
//   - #369 Plus One Linked List [new] (MSD-first linked list increment with carry)
//   - #1290 Convert Binary Number In A Linked List To Integer [done] (MSD-first list decoding)

#include "framework/lc_framework.h"

#include <deque>
#include <utility>
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

// RAII arena for immutable input lists in tests.
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

// RAII guard that owns heap-allocated ListNodes returned by Solution.
class OwnedListGuard {
 public:
  explicit OwnedListGuard(ListNode* head) noexcept : head_(head) {}

  OwnedListGuard(const OwnedListGuard&) = delete;
  OwnedListGuard& operator=(const OwnedListGuard&) = delete;
  OwnedListGuard(OwnedListGuard&& other) noexcept
      : head_(std::exchange(other.head_, nullptr)) {}
  OwnedListGuard& operator=(OwnedListGuard&& other) noexcept {
    if (this != &other) {
      reset();
      head_ = std::exchange(other.head_, nullptr);
    }
    return *this;
  }

  ~OwnedListGuard() { reset(); }

  [[nodiscard]] const ListNode* get() const noexcept { return head_; }

 private:
  void reset() noexcept {
    while (head_ != nullptr) {
      ListNode* next = head_->next;
      delete head_;
      head_ = next;
    }
  }

  ListNode* head_ = nullptr;
};

// Serializes a singly linked list into a vector of digit values.
[[nodiscard]] std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> result;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    result.push_back(curr->val);
  }
  return result;
}

class Solution {
 public:
  // Adds two MSD-first non-negative integers represented as linked lists
  // without modifying either input list.
  // Time: O(m + n), Space: O(m + n).
  ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    // Push all digits onto stacks so we can consume them from LSD to MSD.
    std::vector<int> stack_a;
    for (const ListNode* curr = l1; curr != nullptr; curr = curr->next) {
      stack_a.push_back(curr->val);
    }

    std::vector<int> stack_b;
    for (const ListNode* curr = l2; curr != nullptr; curr = curr->next) {
      stack_b.push_back(curr->val);
    }

    ListNode* result_head = nullptr;
    int carry = 0;

    // Drain both stacks and any final overflow carry, prepending each digit so
    // the resulting list is immediately in MSD-first order.
    while (!stack_a.empty() || !stack_b.empty() || carry != 0) {
      int sum = carry;
      if (!stack_a.empty()) {
        sum += stack_a.back();
        stack_a.pop_back();
      }
      if (!stack_b.empty()) {
        sum += stack_b.back();
        stack_b.pop_back();
      }

      carry = sum / 10;
      const int digit = sum % 10;

      // Link the newly created higher-place digit in front of the current head.
      result_head = new ListNode(digit, result_head);
    }

    return result_head;
  }
};

LC_PROBLEM(445, "Add Two Numbers II", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_preserves_inputs", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({7, 2, 4, 3});
    ListNode* l2 = arena.build({5, 6, 4});
    const OwnedListGuard result(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(result.get()), std::vector<int>({7, 8, 0, 7}));
    // Confirm follow-up requirement: neither input list was mutated.
    ctx.expect_eq(to_vector(l1), std::vector<int>({7, 2, 4, 3}));
    ctx.expect_eq(to_vector(l2), std::vector<int>({5, 6, 4}));
  });

  ctx.test("example_2_same_length", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({2, 4, 3});
    ListNode* l2 = arena.build({5, 6, 4});
    const OwnedListGuard result(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(result.get()), std::vector<int>({8, 0, 7}));
  });

  ctx.test("example_3_both_zero", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({0});
    ListNode* l2 = arena.build({0});
    const OwnedListGuard result(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(result.get()), std::vector<int>({0}));
  });

  ctx.test("carry_propagation_creates_new_head", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({1});
    ListNode* l2 = arena.build({9, 9});
    const OwnedListGuard result(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(result.get()), std::vector<int>({1, 0, 0}));
  });

  ctx.test("multi_digit_carry_chain", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({9, 9, 9, 9});
    ListNode* l2 = arena.build({1});
    const OwnedListGuard result(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(result.get()), std::vector<int>({1, 0, 0, 0, 0}));
  });

  ctx.test("add_zero_to_multi_digit", [&] {
    ListArena arena;
    ListNode* l1 = arena.build({0});
    ListNode* l2 = arena.build({4, 0, 9});
    const OwnedListGuard result(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(result.get()), std::vector<int>({4, 0, 9}));
  });
}

}  // namespace
