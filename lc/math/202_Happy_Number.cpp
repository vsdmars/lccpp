// LeetCode 202: Happy Number
// https://leetcode.com/problems/happy-number/
//
// Problem:
//   Determine whether a positive integer `n` (1 <= n <= 2^31 - 1) is a "happy
//   number". Starting from `n`, repeatedly replace the number with the sum of
//   the squares of its base-10 digits. If the sequence reaches 1, `n` is happy
//   (return true); if it enters a cycle that does not contain 1, return false.
//
// Examples:
//   Example 1:
//     Input:  n = 19
//     Output: true
//     Explanation:
//       - In isHappy, start with n = 19 and an empty seen set; insert 19 and
//         compute 1^2 + 9^2 = 82.
//       - Insert 82 into seen and advance to 8^2 + 2^2 = 68, then insert 68 and
//         advance to 6^2 + 8^2 = 100.
//       - Insert 100 into seen and compute 1^2 + 0^2 + 0^2 = 1, exiting the while
//         loop and returning n == 1 (true).
//
//   Example 2:
//     Input:  n = 2
//     Output: false
//     Explanation:
//       - In isHappy, start with n = 2 and empty seen, inserting each visited state
//         along 2 -> 4 -> 16 -> 37 -> 58 -> 89 -> 145 -> 42 -> 20.
//       - From n = 20, next_digit_square_sum(20) produces 2^2 + 0^2 = 4, which is
//         already in seen.
//       - The loop condition !seen.contains(n) fails at n = 4, so isHappy returns n
//         == 1 (false).
//
//   Example 3:
//     Input:  n = 1
//     Output: true
//     Explanation:
//       - For n = 1, the while loop guard n != 1 && !seen.contains(n) in isHappy is
//         immediately false.
//       - Return n == 1 (true) without computing any transitions or inserting into
//         seen; isHappyFloyd also stops immediately since fast =
//         next_digit_square_sum(1) = 1.
//
// Algorithm:
//   The digit-square-sum transition f(x) maps every d-digit number with d >= 4
//   strictly downward (e.g. 9999 -> 324, 2^31 - 1 -> <= 730), so every orbit
//   cannot diverge to infinity—it must either hit 1 or enter a finite cycle.
//   Two cycle-detection methods are provided:
//   1. `isHappy` (Hash-Set Cycle Detection, matching the Python approach):
//      Track visited states in an `std::unordered_set<int>` and stop as soon as
//      `n == 1` (happy) or `n` is already in `seen` (cycle).
//      Time Complexity: O(log n), Space Complexity: O(log n).
//   2. `isHappyFloyd` (Floyd's Tortoise and Hare):
//      Advance `slow` by 1 step and `fast` by 2 steps until `fast == 1` or
//      `slow == fast`.
//      Time Complexity: O(log n), Space Complexity: O(1).
//
// Data Structures:
//   - std::unordered_set<int>: records visited values in the hash-set variant
//     for O(1) expected cycle detection.
//   - Two integer state pointers (`slow`, `fast`): detect cycles in O(1) space
//     in the Floyd variant without storing prior states.
//
// Real-World Applications:
//   - Cycle detection in pseudo-random number generators (PRNGs), hash chains,
//     and finite-state machines.
//   - Detecting infinite redirect loops or circular symlink / reference chains
//     using constant-memory Floyd/Brent algorithms.
//   - Pollard's rho integer factorization algorithm, which relies on the same
//     functional-graph cycle detection principle.
//
// Related LeetCode Problems:
//   - #141 Linked List Cycle [done] (Floyd's tortoise-and-hare cycle detection on linked lists)
//   - #142 Linked List Cycle II [done] (finding the entry point of a cycle)
//   - #258 Add Digits [done] (iterated digit-sum reduction / digital root)
//   - #263 Ugly Number [new] (number-theoretic property check via repeated reduction)

#include "framework/lc_framework.h"

#include <limits>
#include <unordered_set>

namespace {

constexpr int kDecimalBase = 10;

class Solution {
 public:
  // Checks whether `n` is a happy number using a hash set to detect cycles.
  //
  // Invariant: `seen` contains every state visited prior to the current `n`.
  // Since the state space collapses below 243 after at most a few steps, this
  // loop always terminates in bounded iterations.
  //
  // Time: O(log n), Space: O(log n).
  bool isHappy(int n) {
    std::unordered_set<int> seen;

    while (n != 1 && !seen.contains(n)) {
      seen.insert(n);
      n = next_digit_square_sum(n);
    }

    return n == 1;
  }

  // Constant-space alternative using Floyd's Tortoise and Hare algorithm on
  // the implicit functional graph induced by `next_digit_square_sum`.
  //
  // Time: O(log n), Space: O(1).
  bool isHappyFloyd(int n) {
    int slow = n;
    int fast = next_digit_square_sum(n);

    // Either `fast` reaches the fixed point 1, or `fast` laps `slow` inside
    // the unhappy cycle (4 -> 16 -> 37 -> 58 -> 89 -> 145 -> 42 -> 20 -> 4).
    while (fast != 1 && slow != fast) {
      slow = next_digit_square_sum(slow);
      fast = next_digit_square_sum(next_digit_square_sum(fast));
    }

    return fast == 1;
  }

 private:
  // Computes the sum of the squares of the base-10 digits of `value`.
  static int next_digit_square_sum(int value) {
    int sum = 0;
    while (value > 0) {
      const int digit = value % kDecimalBase;
      sum += (digit * digit);
      value /= kDecimalBase;
    }
    return sum;
  }
};

LC_PROBLEM(202, "Happy Number", "math", ctx) {
  Solution sol;

  ctx.test("example_19_happy", [&] {
    ctx.expect_true(sol.isHappy(19));
    ctx.expect_true(sol.isHappyFloyd(19));
  });

  ctx.test("example_2_unhappy", [&] {
    ctx.expect_false(sol.isHappy(2));
    ctx.expect_false(sol.isHappyFloyd(2));
  });

  ctx.test("base_and_known_numbers", [&] {
    ctx.expect_true(sol.isHappy(1));
    ctx.expect_true(sol.isHappy(7));
    ctx.expect_true(sol.isHappy(100));
    ctx.expect_false(sol.isHappy(4));
    ctx.expect_false(sol.isHappy(20));

    ctx.expect_true(sol.isHappyFloyd(1));
    ctx.expect_true(sol.isHappyFloyd(7));
    ctx.expect_true(sol.isHappyFloyd(100));
    ctx.expect_false(sol.isHappyFloyd(4));
    ctx.expect_false(sol.isHappyFloyd(20));
  });

  ctx.test("int_max", [&] {
    const int kMaxInt = std::numeric_limits<int>::max();
    ctx.expect_false(sol.isHappy(kMaxInt));
    ctx.expect_false(sol.isHappyFloyd(kMaxInt));
  });
}

}  // namespace
