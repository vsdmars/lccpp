// LeetCode 234: Palindrome Linked List
// https://leetcode.com/problems/palindrome-linked-list/
//
// Problem:
//   Given the head of a singly linked list, return `true` if the sequence of
//   node values reads the same forwards and backwards (is a palindrome), and
//   `false` otherwise. Target O(n) time and O(1) extra space.
//
// Algorithm:
//   Midpoint discovery + in-place second-half reversal + restoration:
//   1. Find midpoint: Advance `slow` by 1 step and `fast` by 2 steps from
//      `head` while `fast != nullptr && fast->next != nullptr`. For even-length
//      lists, `slow` lands on the first node of the second half; for odd-length
//      lists, `slow` lands on the exact middle node.
//   2. Reverse second half: Reverse the sublist starting at `slow` in-place to
//      obtain `reversed_second_half`.
//   3. Compare: Walk `front` from `head` and `back` from `reversed_second_half`
//      in lockstep until `back` reaches `nullptr`, checking `front->val ==
//      back->val` at every position.
//   4. Restore: Reverse `reversed_second_half` back in-place before returning
//      so the caller's linked list retains its original structure.
//   Time Complexity: O(n).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Singly linked list traversed and temporarily rewired in-place using raw
//   pointers (`slow`, `fast`, `front`, `back`); no stack or array copy needed.
//
// Real-World Applications:
//   Symmetric packet/frame integrity validation in streaming protocols without
//   buffering full payloads, DNA/RNA inverted-repeat palindrome checks over
//   streamed node representations, and memory-constrained embedded sequence
//   verification.
//
// Related LeetCode Problems:
//   - #206 Reverse Linked List [done] (in-place reversal subroutine used here)
//   - #143 Reorder List [done] (same slow/fast midpoint + reverse-second-half pattern)
//   - #125 Valid Palindrome [done] (two-pointer palindrome check on strings)
//   - #2130 Maximum Twin Sum of a Linked List [new] (same O(1)-space half-reversal technique)

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
  // Determines whether the singly linked list is a palindrome in O(n) time and
  // O(1) extra space, restoring the list to its original order before
  // returning.
  bool isPalindrome(ListNode* head) {
    if (head == nullptr || head->next == nullptr) {
      return true;
    }

    // Phase 1: Locate the start of the second half using slow/fast pointers.
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
      slow = slow->next;
      fast = fast->next->next;
    }

    // Phase 2: Reverse the second half in-place starting at `slow`.
    // Note that the last node of the first half still points to `slow`, which
    // becomes the tail of the reversed half; reversing it back later seamlessly
    // restores the entire list.
    ListNode* back_head = reverse_list(slow);

    // Phase 3: Compare the first half against the reversed second half.
    bool palindrome = true;
    ListNode* front = head;
    ListNode* back = back_head;
    while (back != nullptr && front != nullptr) {
      if (front->val != back->val) {
        palindrome = false;
        break;
      }
      front = front->next;
      back = back->next;
    }

    // Phase 4: Restore the second half so callers observe an unmodified list.
    reverse_list(back_head);

    return palindrome;
  }

 private:
  // Reverses a singly linked list in-place and returns its new head.
  static ListNode* reverse_list(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (curr != nullptr) {
      ListNode* next_node = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next_node;
    }
    return prev;
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

// Serializes a singly linked list into a vector of values for structure checks.
std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    out.push_back(curr->val);
  }
  return out;
}

LC_PROBLEM(234, "Palindrome Linked List", "linked_list", ctx) {
  ctx.test("example_1_even_palindrome", [&] {
    ListArena arena;
    const std::vector<int> input = {1, 2, 2, 1};
    ListNode* head = arena.build(input);
    Solution sol;
    ctx.expect_true(sol.isPalindrome(head));
    ctx.expect_eq(to_vector(head), input);
  });

  ctx.test("example_2_odd_palindrome", [&] {
    ListArena arena;
    const std::vector<int> input = {1, 2, 3, 2, 1};
    ListNode* head = arena.build(input);
    Solution sol;
    ctx.expect_true(sol.isPalindrome(head));
    ctx.expect_eq(to_vector(head), input);
  });

  ctx.test("python_build_two_node_non_palindrome", [&] {
    ListArena arena;
    const std::vector<int> input = {1, 2};
    ListNode* head = arena.build(input);
    Solution sol;
    ctx.expect_false(sol.isPalindrome(head));
    ctx.expect_eq(to_vector(head), input);
  });

  ctx.test("longer_non_palindrome", [&] {
    ListArena arena;
    const std::vector<int> input = {1, 2, 3, 4, 2, 1};
    ListNode* head = arena.build(input);
    Solution sol;
    ctx.expect_false(sol.isPalindrome(head));
    ctx.expect_eq(to_vector(head), input);
  });

  ctx.test("empty_single_and_equal_pair", [&] {
    Solution sol;
    ctx.expect_true(sol.isPalindrome(nullptr));

    ListArena arena1;
    ListNode* single = arena1.build({7});
    ctx.expect_true(sol.isPalindrome(single));
    ctx.expect_eq(to_vector(single), std::vector<int>{7});

    ListArena arena2;
    ListNode* pair = arena2.build({5, 5});
    ctx.expect_true(sol.isPalindrome(pair));
    ctx.expect_eq(to_vector(pair), std::vector<int>{5, 5});
  });
}

}  // namespace
