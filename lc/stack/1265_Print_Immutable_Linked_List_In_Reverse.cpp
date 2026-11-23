// LeetCode 1265: Print Immutable Linked List In Reverse
// https://leetcode.com/problems/print-immutable-linked-list-in-reverse/
//
// Problem:
//   You are given the head of an immutable singly linked list accessible only
//   through two opaque methods on `ImmutableListNode`:
//     - `printValue()`: outputs the current node's value.
//     - `getNext()`: returns a pointer to the next node (or `nullptr`).
//   Print all node values in reverse order without modifying the list.
//
// Algorithm:
//   Primary (Explicit Stack):
//     Traverse the list from head to tail via `getNext()`, pushing each node
//     pointer onto a stack. Then pop nodes one by one and invoke
//     `printValue()`, naturally reversing the traversal order without risking
//     call-stack overflow on long lists.
//     Time Complexity:  O(n), where n is the list length.
//     Space Complexity: O(n) for the stack of node pointers.
//
//   Follow-up (Square-Root Decomposition):
//     Count the list length `n` in one pass, choose block size
//     `b = ceil(sqrt(n))`, record the starting node of every `b`-th position
//     (O(sqrt(n)) block heads), and then process blocks from last to first
//     using a small O(sqrt(n)) stack per block.
//     Time Complexity:  O(n) (three linear passes).
//     Space Complexity: O(sqrt(n)).
//
// Data Structures:
//   - Stack (`std::vector<ImmutableListNode*>`): stores node pointers for LIFO
//     reverse printing.
//   - Block-head table (`std::vector<ImmutableListNode*>` in sqrt-space
//     variant): checkpoints every ceil(sqrt(n))-th node to achieve sub-linear
//     space while keeping linear time.
//
// Real-World Applications:
//   - Replaying append-only write-ahead logs (WAL) or immutable blockchain /
//     Git commit chains backward without mutating storage.
//   - Checkpoint-based reverse-mode automatic differentiation (gradient
//     checkpointing uses sqrt(n) memory to backpropagate in linear time).
//   - Reversing read-only streaming iterators or paged forward-only cursors.
//
// Related LeetCode Problems:
//   - #206 Reverse Linked List [done] (mutable in-place pointer reversal)
//   - #234 Palindrome Linked List [done] (reverse traversal / half-reversal)
//   - #92 Reverse Linked List II [done] (sublist reversal)
//   - #143 Reorder List [done] (combining forward and reverse list halves)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <vector>

namespace {

// Mock judge API matching LeetCode's immutable linked list interface.
// Records printed values into an external sink so tests can assert exact order.
class ImmutableListNode {
 public:
  ImmutableListNode(int value, ImmutableListNode* next,
                    std::vector<int>* output_sink)
      : value_(value), next_(next), output_sink_(output_sink) {}

  void printValue() const {
    if (output_sink_ != nullptr) {
      output_sink_->push_back(value_);
    }
  }

  [[nodiscard]] ImmutableListNode* getNext() const { return next_; }

 private:
  int value_ = 0;
  ImmutableListNode* next_ = nullptr;
  std::vector<int>* output_sink_ = nullptr;
};

// RAII test harness that owns all ImmutableListNode instances in a std::deque
// (guaranteeing pointer stability and zero heap leaks) and captures output.
class MockImmutableList {
 public:
  explicit MockImmutableList(const std::vector<int>& values) {
    // Construct nodes from tail to head so each node's immutable next_ pointer
    // is set once at construction. std::deque preserves element addresses on
    // emplace_front.
    ImmutableListNode* next_node = nullptr;
    for (std::size_t i = values.size(); i > 0; --i) {
      nodes_.emplace_front(values[i - 1], next_node, &printed_);
      next_node = &nodes_.front();
    }
    head_ = next_node;
  }

  MockImmutableList(const MockImmutableList&) = delete;
  MockImmutableList& operator=(const MockImmutableList&) = delete;

  [[nodiscard]] ImmutableListNode* head() const { return head_; }
  [[nodiscard]] const std::vector<int>& printed() const { return printed_; }

 private:
  std::vector<int> printed_;
  std::deque<ImmutableListNode> nodes_;
  ImmutableListNode* head_ = nullptr;
};

class Solution {
 public:
  // Prints all nodes in reverse using an explicit LIFO stack.
  // Time: O(n), Space: O(n).
  void printLinkedListInReverse(ImmutableListNode* head) {
    std::vector<ImmutableListNode*> stack;
    for (ImmutableListNode* cur = head; cur != nullptr; cur = cur->getNext()) {
      stack.push_back(cur);
    }

    // Pop in reverse order and invoke the opaque printValue() API.
    while (!stack.empty()) {
      stack.back()->printValue();
      stack.pop_back();
    }
  }

  // Follow-up: Linear time O(n) with sub-linear O(sqrt(n)) space using
  // square-root block checkpointing.
  void printLinkedListInReverseSqrtSpace(ImmutableListNode* head) {
    if (head == nullptr) {
      return;
    }

    // Pass 1: measure total list length.
    std::size_t length = 0;
    for (ImmutableListNode* cur = head; cur != nullptr; cur = cur->getNext()) {
      ++length;
    }

    // Compute integer ceil(sqrt(length)) without floating-point conversions.
    std::size_t block_size = 1;
    while ((block_size * block_size) < length) {
      ++block_size;
    }

    // Pass 2: checkpoint the first node of each block of size `block_size`.
    std::vector<ImmutableListNode*> block_heads;
    block_heads.reserve((length / block_size) + 1uz);
    std::size_t index = 0;
    for (ImmutableListNode* cur = head; cur != nullptr; cur = cur->getNext()) {
      if ((index % block_size) == 0uz) {
        block_heads.push_back(cur);
      }
      ++index;
    }

    // Pass 3: print each block from last to first using an O(sqrt(n)) buffer.
    std::vector<ImmutableListNode*> block_stack;
    block_stack.reserve(block_size);
    for (std::size_t b = block_heads.size(); b > 0; --b) {
      ImmutableListNode* cur = block_heads[b - 1];
      for (std::size_t step = 0; step < block_size && cur != nullptr; ++step) {
        block_stack.push_back(cur);
        cur = cur->getNext();
      }
      while (!block_stack.empty()) {
        block_stack.back()->printValue();
        block_stack.pop_back();
      }
    }
  }
};

LC_PROBLEM(1265, "Print Immutable Linked List In Reverse", "stack", ctx) {
  auto verify_both = [&](const std::vector<int>& input,
                         const std::vector<int>& expected) {
    Solution sol;

    MockImmutableList list_stack(input);
    sol.printLinkedListInReverse(list_stack.head());
    ctx.expect_eq(list_stack.printed(), expected);

    MockImmutableList list_sqrt(input);
    sol.printLinkedListInReverseSqrtSpace(list_sqrt.head());
    ctx.expect_eq(list_sqrt.printed(), expected);
  };

  ctx.test("example_1", [&] { verify_both({1, 2, 3, 4}, {4, 3, 2, 1}); });

  ctx.test("example_2_negatives",
           [&] { verify_both({0, -4, -1, 3, -5}, {-5, 3, -1, -4, 0}); });

  ctx.test("example_3_duplicates",
           [&] { verify_both({-2, 0, 6, 4, 4, -6}, {-6, 4, 4, 6, 0, -2}); });

  ctx.test("single_node", [&] { verify_both({42}, {42}); });

  ctx.test("empty_list", [&] { verify_both({}, {}); });

  ctx.test("non_square_length_for_sqrt_blocks", [&] {
    std::vector<int> input;
    std::vector<int> expected;
    for (int i = 1; i <= 17; ++i) {
      input.push_back(i);
      expected.push_back(18 - i);
    }
    verify_both(input, expected);
  });
}

}  // namespace
