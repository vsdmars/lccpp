// LeetCode 1290: Convert Binary Number In A Linked List To Integer
// https://leetcode.com/problems/convert-binary-number-in-a-linked-list-to-integer/
//
// Problem:
//   Given the head of a non-empty singly linked list whose node values are
//   each either 0 or 1, interpret the list as an unsigned binary integer with
//   the most significant bit at the head and return its decimal integer value.
//   The list contains at most 30 nodes, so the result always fits in a signed
//   32-bit `int`.
//
// Examples:
//   Example 1:
//     Input:  head = [1, 0, 1]
//     Output: 5
//     Explanation:
//       - Initialize `value = 0` and start `curr` at the head node `1`.
//       - At node `1`, compute `value = (0 * 2) + 1 = 1` and advance `curr` to `0`.
//       - At node `0`, compute `value = (1 * 2) + 0 = 2` and advance `curr` to `1`.
//       - At node `1`, compute `value = (2 * 2) + 1 = 5`, reach `curr == nullptr`,
//         and return `5`.
//
//   Example 2:
//     Input:  head = [0, 0, 1, 0, 0]
//     Output: 4
//     Explanation:
//       - Initialize `value = 0` and consume the first two `0` nodes, keeping
//         `value = 0`.
//       - At the third node (`1`), compute `value = (0 * 2) + 1 = 1`.
//       - At the fourth node (`0`), compute `value = (1 * 2) + 0 = 2`.
//       - At the fifth node (`0`), compute `value = (2 * 2) + 0 = 4` and return
//         `4`.
//
//   Example 3:
//     Input:  head = [0]
//     Output: 0
//     Explanation:
//       - Initialize `value = 0` with `curr` pointing to the single node `0`.
//       - Compute `value = (0 * 2) + 0 = 0`, advance `curr` to `nullptr`, and
//         return `0`.
//
// Algorithm:
//   Horner's method (online base-2 accumulation) in a single pass:
//   1. Start with `value = 0`.
//   2. For each node from head to tail, shift the accumulated bits left by 1
//      place (multiply by 2) and add the current bit:
//      `value = (value * 2) + curr->val`.
//   3. Invariant: after consuming the first `k` nodes, `value` equals the
//      exact unsigned binary value of that `k`-bit prefix — no prior length
//      pass is needed.
//   Time Complexity: O(n), visiting each node once.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Singly linked list (`ListNode`): streamed once from head to tail with a
//     single scalar accumulator (`int`).
//
// Real-World Applications:
//   - Streaming big-endian bit-field deserialization in network packet parsers
//     (TCP/IP headers, protobuf varints, ASN.1 BER/DER decoders).
//   - Huffman / arithmetic entropy decoders reconstructing integer symbols bit
//     by bit from forward-only bitstreams.
//   - Hardware shift-register simulation and serial bus (SPI/I2C) word
//     assembly.
//
// Related LeetCode Problems:
//   - #445 Add Two Numbers II [done] (MSD-first digit processing on linked lists)
//   - #190 Reverse Bits [done] (bit-by-bit shift-and-accumulate integer construction)
//   - #1018 Binary Prefix Divisible By 5 [new] (online base-2 Horner accumulation)
//   - #1022 Sum of Root To Leaf Binary Numbers [new] (Horner binary accumulation along tree paths)

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

class Solution {
 public:
  // Decodes an MSD-first binary linked list into its base-10 integer value in
  // one pass using Horner's rule.
  // Time: O(n), Space: O(1).
  int getDecimalValue(ListNode* head) {
    int value = 0;

    // Each step shifts the already-parsed prefix one bit higher and appends
    // the next least-significant bit seen so far. With <= 30 bits, `value`
    // stays within [0, 2^30 - 1] and never overflows signed 32-bit `int`.
    for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
      value = (value * 2) + curr->val;
    }

    return value;
  }
};

LC_PROBLEM(1290, "Convert Binary Number In A Linked List To Integer",
           "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_five", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 0, 1});
    ctx.expect_eq(sol.getDecimalValue(head), 5);
  });

  ctx.test("example_2_single_zero", [&] {
    ListArena arena;
    ListNode* head = arena.build({0});
    ctx.expect_eq(sol.getDecimalValue(head), 0);
  });

  ctx.test("single_one", [&] {
    ListArena arena;
    ListNode* head = arena.build({1});
    ctx.expect_eq(sol.getDecimalValue(head), 1);
  });

  ctx.test("leading_zeros", [&] {
    ListArena arena;
    ListNode* head = arena.build({0, 0, 1, 0, 0});
    ctx.expect_eq(sol.getDecimalValue(head), 4);
  });

  ctx.test("fifteen_bit_number", [&] {
    ListArena arena;
    ListNode* head =
        arena.build({1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0});
    ctx.expect_eq(sol.getDecimalValue(head), 18880);
  });

  ctx.test("max_30_bits_all_ones", [&] {
    ListArena arena;
    ListNode* head = arena.build(std::vector<int>(30uz, 1));
    ctx.expect_eq(sol.getDecimalValue(head), 1073741823);
  });
}

}  // namespace
