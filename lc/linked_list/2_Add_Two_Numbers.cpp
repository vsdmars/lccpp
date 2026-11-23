// LeetCode 2: Add Two Numbers
// https://leetcode.com/problems/add-two-numbers/
//
// Problem:
//   Given two non-empty singly-linked lists representing two non-negative
//   integers whose digits are stored in reverse order (least significant digit
//   at the head, one digit per node), add the two numbers and return the sum
//   as a newly constructed linked list in the same reverse-digit format.
//   Neither input has leading zeros except the number 0 itself.
//
// Algorithm:
//   Simulate elementary school column-by-column addition from least significant
//   to most significant digit using a sentinel dummy head and a running `carry`.
//   In a single pass, while either list still has nodes or `carry != 0`:
//   1. Sum the available digit(s) plus `carry`.
//   2. Append a new node with value `sum % 10` to the result tail.
//   3. Update `carry = sum / 10` and advance any non-null list pointers.
//   Time Complexity: O(max(m, n)), where m and n are the lengths of l1 and l2.
//   Space Complexity: O(max(m, n)) for the output list (O(1) extra workspace).
//
// Data Structures:
//   - Singly-linked list (`ListNode`) for inputs and output.
//   - Stack-allocated dummy sentinel node to unify head/tail insertion.
//   - RAII `OwnedList` wrapper to guarantee exception safety and zero leaks.
//
// Real-World Applications:
//   - Arbitrary-precision (BigInt) arithmetic libraries used in cryptography
//     (RSA/ECC), financial ledgers, and scientific computing.
//   - Streaming adder circuits and ripple-carry hardware arithmetic units.
//
// Related LeetCode Problems:
//   - #445 Add Two Numbers II [done] (forward-order digits using stacks/reversal)
//   - #415 Add Strings [done] (base-10 addition over digit strings)
//   - #67 Add Binary [done] (base-2 addition with carry propagation)
//   - #66 Plus One [done] (incrementing a digit array with carry)
//   - #43 Multiply Strings [done] (arbitrary-precision multiplication)

#include "framework/lc_framework.h"

#include <deque>
#include <memory>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode singly-linked list node.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  explicit ListNode(int x, ListNode* nxt = nullptr) : val(x), next(nxt) {}
};

// RAII owner for heap-allocated ListNode chains so dynamically constructed
// lists are always freed cleanly without leaks.
struct OwnedList {
  ListNode* head = nullptr;

  OwnedList() = default;
  explicit OwnedList(ListNode* h) : head(h) {}
  OwnedList(const OwnedList&) = delete;
  OwnedList& operator=(const OwnedList&) = delete;
  OwnedList(OwnedList&& other) noexcept
      : head(std::exchange(other.head, nullptr)) {}
  OwnedList& operator=(OwnedList&& other) noexcept {
    if (this != &other) {
      reset();
      head = std::exchange(other.head, nullptr);
    }
    return *this;
  }
  ~OwnedList() { reset(); }

  void reset() noexcept {
    while (head != nullptr) {
      ListNode* next = head->next;
      delete head;
      head = next;
    }
  }

  ListNode* release() noexcept { return std::exchange(head, nullptr); }
};

class Solution {
 public:
  // Adds two reverse-order non-negative integers represented as linked lists.
  // Returns a newly allocated linked list representing their sum.
  // Time: O(max(m, n)), Space: O(max(m, n)) for output (O(1) auxiliary).
  ListNode* addTwoNumbers(const ListNode* l1, const ListNode* l2) {
    if (l1 == nullptr && l2 == nullptr) {
      return nullptr;
    }

    static constexpr int kBase = 10;
    ListNode dummy(0);
    ListNode* tail = &dummy;
    OwnedList guard;
    int carry = 0;

    // Continue until both input lists are exhausted AND no carry remains.
    while (l1 != nullptr || l2 != nullptr || carry != 0) {
      int sum = carry;
      if (l1 != nullptr) {
        sum += l1->val;
        l1 = l1->next;
      }
      if (l2 != nullptr) {
        sum += l2->val;
        l2 = l2->next;
      }

      carry = sum / kBase;
      auto node = std::make_unique<ListNode>(sum % kBase);
      tail->next = node.release();
      tail = tail->next;

      // Hand ownership of the head node to `guard` on the first allocation so
      // all subsequent linked nodes are freed if an allocation ever throws.
      if (guard.head == nullptr) {
        guard.head = dummy.next;
      }
    }

    return guard.release();
  }
};

// Arena helper for building non-owning test input lists without manual delete.
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

LC_PROBLEM(2, "Add Two Numbers", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_342_plus_465", [&] {
    ListArena arena;
    const ListNode* l1 = arena.build({2, 4, 3});
    const ListNode* l2 = arena.build({5, 6, 4});
    const OwnedList res(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(res.head), std::vector<int>{7, 0, 8});
  });

  ctx.test("single_digit_with_carry", [&] {
    ListArena arena;
    const ListNode* l1 = arena.build({5});
    const ListNode* l2 = arena.build({5});
    const OwnedList res(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(res.head), std::vector<int>{0, 1});
  });

  ctx.test("unequal_lengths_no_final_carry", [&] {
    // 3427 + 465 = 3892
    ListArena arena;
    const ListNode* l1 = arena.build({7, 2, 4, 3});
    const ListNode* l2 = arena.build({5, 6, 4});
    const OwnedList res(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(res.head), std::vector<int>{2, 9, 8, 3});
  });

  ctx.test("cascading_carry_unequal_lengths", [&] {
    // 9999999 + 9999 = 10009998
    ListArena arena;
    const ListNode* l1 = arena.build({9, 9, 9, 9, 9, 9, 9});
    const ListNode* l2 = arena.build({9, 9, 9, 9});
    const OwnedList res(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(res.head),
                  std::vector<int>{8, 9, 9, 9, 0, 0, 0, 1});
  });

  ctx.test("zero_plus_zero", [&] {
    ListArena arena;
    const ListNode* l1 = arena.build({0});
    const ListNode* l2 = arena.build({0});
    const OwnedList res(sol.addTwoNumbers(l1, l2));
    ctx.expect_eq(to_vector(res.head), std::vector<int>{0});
  });

  ctx.test("both_empty", [&] {
    const OwnedList res(sol.addTwoNumbers(nullptr, nullptr));
    ctx.expect_eq(res.head, nullptr);
  });
}

}  // namespace
