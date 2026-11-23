// LeetCode 1404: Number of Steps to Reduce a Number in Binary Representation to One
// https://leetcode.com/problems/number-of-steps-to-reduce-a-number-in-binary-representation-to-one/
//
// Problem:
//   Given a string `s` representing a positive integer in binary
//   (1 <= s.length <= 500, s[0] == '1', containing only '0' and '1'), return
//   the number of steps required to reduce it to 1 under two rules:
//     - If the current number is even, divide it by 2.
//     - If the current number is odd, add 1 to it.
//   It is guaranteed that the number can always be reduced to 1.
//
// Algorithm:
//   Implements both an iterative (`numSteps`) and a recursive
//   (`numStepsRecursive`) right-to-left (LSB to bit 1) carry-propagation scan
//   so arbitrarily long binary strings never overflow fixed-width integers.
//   At each index i from s.size() - 1 down to 1, compute the effective bit
//   `bit = (s[i] - '0') + carry`:
//     - If `bit == 1` (odd): adding 1 turns it into 2 (setting `carry = 1`)
//       and dividing by 2 shifts right, taking 2 steps.
//     - If `bit` is 0 or 2 (even): dividing by 2 shifts right in 1 step while
//       `carry` remains unchanged (0 stays 0; 2 keeps `carry = 1`).
//   At index 0 (the leading '1'), a remaining `carry == 1` means the reduced
//   value is 2 (`10` in binary), requiring 1 final division; otherwise it is 1.
//   - Iterative (`numSteps`):          Time O(n), Space O(1).
//   - Recursive (`numStepsRecursive`): Time O(n), Space O(n) call stack.
//
// Data Structures:
//   - `std::string_view`: Non-owning read-only view of the input binary string
//     `s`, avoiding unnecessary copies.
//   - Scalar integer state (`steps`, `carry`, index `i`): Tracks accumulated
//     operations and binary carry; the recursive variant also uses the call
//     stack via a C++23 deducing-`this` lambda (`dfs`).
//
// Real-World Applications:
//   - Arbitrary-precision (BigInt) bit-level arithmetic and carry-propagation
//     analysis in cryptography and numeric libraries.
//   - Hardware shift-and-add circuit design and cycle-count estimation for
//     binary normalization pipelines.
//   - Static analysis of Collatz-like bit-manipulation state machines.
//
// Related LeetCode Problems:
//   - #1342 Number of Steps to Reduce a Number to Zero [new] (integer reduction with
//     halving and decrementing rules)
//   - #67 Add Binary [done] (right-to-left binary string addition with carry)
//   - #397 Integer Replacement [done] (minimum steps to reach 1 via +1, -1, and /2)
//   - #2139 Minimum Moves to Reach Target Score [new] (greedy reverse reduction
//     using halving and decrement operations)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Processes bits from LSB to bit 1 with a running carry so arbitrarily long
  // binary strings (e.g. up to 500 bits) never overflow fixed-width integers.
  //
  // At bit index i > 0, let effective bit = (s[i] - '0') + carry:
  //   - If odd (1): adding 1 makes it even (2, carry = 1) and dividing by 2
  //     shifts right -> 2 steps, carry becomes 1.
  //   - If even (0 or 2): dividing by 2 shifts right -> 1 step, carry is
  //     unchanged (0 stays 0; 2 keeps carry = 1).
  // At index 0 (leading '1'), carry == 1 means value is 2 (1 final shift).
  //
  // Time:  O(n)
  // Space: O(1)
  int numSteps(std::string_view s) {
    if (s.size() <= 1uz) {
      return 0;
    }

    int steps = 0;
    int carry = 0;
    for (std::size_t i = s.size() - 1uz; i > 0uz; --i) {
      const int bit = (s[i] - '0') + carry;
      if (bit == 1) {
        steps += 2;
        carry = 1;
      } else {
        steps += 1;
      }
    }

    return steps + carry;
  }

  // Recursive formulation of the same LSB-to-MSB carry reduction.
  //
  // Time:  O(n)
  // Space: O(n) recursion stack
  int numStepsRecursive(std::string_view s) {
    if (s.size() <= 1uz) {
      return 0;
    }

    auto dfs = [&](this auto&& self, std::size_t idx, int carry) -> int {
      if (idx == 0uz) {
        return carry;
      }
      const int bit = (s[idx] - '0') + carry;
      if (bit == 1) {
        return 2 + self(idx - 1uz, 1);
      }
      return 1 + self(idx - 1uz, carry);
    };

    return dfs(s.size() - 1uz, 0);
  }
};

LC_PROBLEM(1404,
           "Number Of Steps To Reduce A Number In Binary Representation To One",
           "recursion", ctx) {
  Solution sol;

  auto check_both = [&](std::string_view s, int expected) {
    ctx.expect_eq(sol.numSteps(s), expected);
    ctx.expect_eq(sol.numStepsRecursive(s), expected);
  };

  ctx.test("example_1_1101", [&] { check_both("1101", 6); });

  ctx.test("example_2_10", [&] { check_both("10", 1); });

  ctx.test("example_3_1", [&] { check_both("1", 0); });

  ctx.test("powers_of_two_and_all_ones", [&] {
    check_both("1000", 3);
    check_both("111", 4);
    // 2^65 - 1 -> +1 -> 2^65 -> 65 divisions -> 66 steps (exceeds uint64_t).
    const std::string sixty_five_ones(65uz, '1');
    check_both(sixty_five_ones, 66);
    // 2^100 -> 100 divisions -> 100 steps.
    const std::string pow_2_100 = "1" + std::string(100uz, '0');
    check_both(pow_2_100, 100);
  });
}

}  // namespace
