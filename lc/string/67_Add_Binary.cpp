// LeetCode 67: Add Binary
// https://leetcode.com/problems/add-binary/
//
// Problem:
//   Given two binary strings `a` and `b` consisting of '0' and '1' characters,
//   return their sum as a binary string without leading zeros (except "0").
//
// Examples:
//   Example 1:
//     Input:  a = "11", b = "1"
//     Output: "100"
//     Explanation:
//       - Initialize reverse cursors i = 2, j = 1, carry = 0, and an empty result
//         buffer.
//       - Step 1 (--i -> 1, --j -> 0): sum = 0 + 1 + 1 = 2, append '0' (2 % 2) and
//         set carry = 1 (2 / 2).
//       - Step 2 (--i -> 0, j == 0): sum = 1 + 1 = 2, append '0' and keep carry =
//         1.
//       - Step 3 (i == 0, j == 0, carry = 1): sum = 1, append '1' and set carry =
//         0, then reverse "001" to return "100".
//
//   Example 2:
//     Input:  a = "1010", b = "1011"
//     Output: "10101"
//     Explanation:
//       - Initialize i = 4, j = 4, carry = 0, and process LSBs (i = 3, j = 3): sum
//         = 0 + 1 + 0 = 1, append '1', carry = 0.
//       - Process (i = 2, j = 2): sum = 1 + 1 + 0 = 2, append '0', carry = 1; then
//         (i = 1, j = 1): sum = 0 + 0 + 1 = 1, append '1', carry = 0.
//       - Process (i = 0, j = 0): sum = 1 + 1 + 0 = 2, append '0', carry = 1.
//       - Consume remaining carry = 1 by appending '1', then reverse "10101" to
//         return "10101".
//
//   Example 3:
//     Input:  a = "0", b = "0"
//     Output: "0"
//     Explanation:
//       - Initialize i = 1, j = 1, carry = 0, and an empty result buffer.
//       - Run one loop step (--i -> 0, --j -> 0): sum = 0 + 0 + 0 = 0, append '0',
//         and set carry = 0.
//       - Exit the loop and reverse "0" to return "0".
//
// Algorithm:
//   Perform schoolbook right-to-left bit-by-bit addition using two reverse
//   indices and an integer carry (a software ripple-carry full adder). At each
//   step, sum the available bits from `a`, `b`, and the incoming carry; append
//   `sum % 2` to the result buffer and set the next carry to `sum / 2`. Once
//   both strings and the carry are exhausted, reverse the accumulated string.
//   This achieves the exact bit-level full-adder semantics of the recursive
//   formulation while avoiding quadratic string-slicing overhead.
//   Time Complexity:  O(max(M, N)), where M = |a| and N = |b|.
//   Space Complexity: O(max(M, N)) for the output string (O(1) auxiliary).
//
// Data Structures:
//   - `std::string` output buffer pre-reserved to `max(|a|, |b|) + 1` bytes
//     to avoid repeated reallocations during bit accumulation.
//   - Two `std::size_t` reverse cursor indices and an `int` carry register.
//
// Real-World Applications:
//   - Arbitrary-precision (BigInt) arithmetic libraries in cryptography,
//     symbolic math engines, and financial computation.
//   - Hardware ALU / ripple-carry adder simulation in digital logic design.
//   - Checksum and binary protocol frame arithmetic on variable-width bit
//     fields.
//
// Related LeetCode Problems:
//   - #2 Add Two Numbers [done] (elementary addition with carry on linked lists)
//   - #43 Multiply Strings [done] (arbitrary-precision decimal multiplication)
//   - #66 Plus One [done] (carry propagation on a base-10 digit array)
//   - #415 Add Strings [done] (same two-pointer carry algorithm in base 10)
//   - #989 Add to Array-Form of Integer [done] (digit-array addition with carry)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Computes the binary sum of two binary strings `a` and `b`.
  // Time: O(max(|a|, |b|)), Space: O(max(|a|, |b|)).
  std::string addBinary(std::string_view a, std::string_view b) {
    std::string result;
    // Maximum possible result length is max(|a|, |b|) + 1 (if final carry = 1).
    result.reserve(std::max(a.size(), b.size()) + 1uz);

    std::size_t i = a.size();
    std::size_t j = b.size();
    int carry = 0;

    // Loop invariant: `result` holds the reversed binary sum of the already
    // processed suffixes of `a` and `b`, and `carry` is the carry-out into the
    // next higher bit position.
    while (i > 0uz || j > 0uz || carry != 0) {
      int sum = carry;
      if (i > 0uz) {
        --i;
        sum += a[i] - '0';
      }
      if (j > 0uz) {
        --j;
        sum += b[j] - '0';
      }

      // Emit the least-significant bit of `sum` and propagate the carry bit.
      result.push_back(static_cast<char>('0' + (sum % 2)));
      carry = sum / 2;
    }

    // Bits were emitted from LSB to MSB; reverse to restore MSB-first order.
    std::ranges::reverse(result);
    return result;
  }
};

LC_PROBLEM(67, "Add Binary", "string", ctx) {
  Solution sol;

  ctx.test("leetcode examples", [&] {
    ctx.expect_eq(sol.addBinary("11", "1"), "100");
    ctx.expect_eq(sol.addBinary("1010", "1011"), "10101");
  });

  ctx.test("source build cases", [&] {
    ctx.expect_eq(sol.addBinary("101", "1"), "110");
    ctx.expect_eq(sol.addBinary("1", "1"), "10");
    ctx.expect_eq(sol.addBinary("110010", "10111"), "1001001");
  });

  ctx.test("edge cases", [&] {
    ctx.expect_eq(sol.addBinary("0", "0"), "0");
    ctx.expect_eq(sol.addBinary("0", "1011"), "1011");
    ctx.expect_eq(sol.addBinary("11111111", "1"), "100000000");
    ctx.expect_eq(sol.addBinary("1", "11111111"), "100000000");
  });
}

}  // namespace
