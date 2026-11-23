// LeetCode 946: Validate Stack Sequences
// https://leetcode.com/problems/validate-stack-sequences/
//
// Problem:
//   Given two integer sequences `pushed` and `popped`, each with distinct
//   values, return `true` if and only if `popped` could be the exact sequence
//   of popped values produced by some valid interleaving of `push` and `pop`
//   operations on an initially empty stack given push order `pushed`.
//
// Algorithm:
//   Greedy stack simulation. Because all values are distinct, whenever the
//   current top of the stack matches the next required value `popped[pop_idx]`,
//   popping it immediately is always optimal (delaying the pop would bury it
//   under subsequent pushes and prevent it from ever being popped next).
//   Iterate through each element `val` in `pushed`:
//   1. Push `val` onto `stack`.
//   2. Repeatedly pop from `stack` and advance `pop_idx` while `stack` is
//      non-empty and `stack.back() == popped[pop_idx]`.
//   After processing all elements of `pushed`, the sequence is valid iff
//   `stack` is empty and `pop_idx == popped.size()`.
//   Time Complexity: O(N), where N = pushed.size(); every element is pushed
//   and popped at most once.
//   Space Complexity: O(N) for the simulation stack.
//
// Data Structures:
//   - `std::vector<int> stack`: explicit LIFO simulation stack.
//   - `std::size_t pop_idx`: cursor tracking the next expected value in
//     `popped`.
//
// Real-World Applications:
//   - Railroad shunting / yard-siding feasibility checking (Knuth's stack-
//     sortable permutations problem).
//   - Verifying LIFO execution traces and call-stack unwind logs in debuggers
//     and profilers.
//   - Warehouse/container-yard crane scheduling where items enter and exit a
//     single dead-end lane in LIFO order.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (stack-order validation)
//   - #155 Min Stack [done] (core stack interface)
//   - #1441 Build an Array With Stack Operations [new] (reconstructing push/pop ops)
//   - #232 Implement Queue using Stacks [done] (stack-simulation mechanics)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Simulates push/pop operations greedily to determine whether `popped` is a
  // valid stack-pop permutation of `pushed`.
  // Invariant: `popped[0..pop_idx)` has already been matched and popped, and
  // no element currently at the top of `stack` matches `popped[pop_idx]` at
  // the end of each outer iteration.
  // Time: O(N), Space: O(N).
  bool validateStackSequences(const std::vector<int>& pushed,
                              const std::vector<int>& popped) {
    if (pushed.size() != popped.size()) {
      return false;
    }

    std::vector<int> stack;
    stack.reserve(pushed.size());
    std::size_t pop_idx = 0uz;

    for (const int val : pushed) {
      stack.push_back(val);

      // Because values are distinct, greedily popping every top element that
      // matches the next expected pop is uniquely forced.
      while (!stack.empty() && pop_idx < popped.size() &&
             stack.back() == popped[pop_idx]) {
        stack.pop_back();
        ++pop_idx;
      }
    }

    return stack.empty() && pop_idx == popped.size();
  }
};

LC_PROBLEM(946, "Validate Stack Sequences", "stack", ctx) {
  Solution sol;

  ctx.test("example 1 valid sequence", [&] {
    ctx.expect_true(sol.validateStackSequences({1, 2, 3, 4, 5},
                                               {4, 5, 3, 2, 1}));
  });

  ctx.test("example 2 invalid sequence", [&] {
    ctx.expect_false(sol.validateStackSequences({1, 2, 3, 4, 5},
                                                {4, 3, 5, 1, 2}));
  });

  ctx.test("edge cases", [&] {
    ctx.expect_true(sol.validateStackSequences({}, {}));
    ctx.expect_true(sol.validateStackSequences({1}, {1}));
    ctx.expect_false(sol.validateStackSequences({1}, {2}));
    ctx.expect_true(sol.validateStackSequences({1, 2, 3}, {1, 2, 3}));
    ctx.expect_true(sol.validateStackSequences({1, 2, 3}, {3, 2, 1}));
    ctx.expect_false(sol.validateStackSequences({1, 2, 3}, {3, 1, 2}));
  });
}

}  // namespace
