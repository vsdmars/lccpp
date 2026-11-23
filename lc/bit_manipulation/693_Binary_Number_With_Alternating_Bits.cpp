// LeetCode 693: Binary Number With Alternating Bits
// https://leetcode.com/problems/binary-number-with-alternating-bits/
//
// Problem:
//   Given a positive integer n (1 <= n <= 2^31 - 1), determine whether its
//   binary representation consists strictly of alternating bits—that is, no
//   two adjacent bits have the same value (e.g., 5 = 101_2 returns true,
//   while 7 = 111_2 and 11 = 1011_2 return false).
//
// Examples:
//   Example 1:
//     Input:  n = 5
//     Output: true
//     Explanation:
//       - Convert n = 5 (101_2) to value = 5u and initialize expected_bit = 5u & 1u
//         = 1u.
//       - Iteration 1 verifies (5u & 1u) == 1u, toggles expected_bit to 0u, and
//         shifts value right to 2u (10_2).
//       - Iteration 2 verifies (2u & 1u) == 0u, toggles expected_bit to 1u, and
//         shifts value right to 1u (1_2).
//       - Iteration 3 verifies (1u & 1u) == 1u, toggles expected_bit to 0u, and
//         shifts value to 0u, exiting the loop and returning true (matching 5 ^ 2 =
//         7 with (7 & 8) == 0).
//
//   Example 2:
//     Input:  n = 11
//     Output: false
//     Explanation:
//       - Convert n = 11 (1011_2) to value = 11u and initialize expected_bit = 11u
//         & 1u = 1u.
//       - Iteration 1 verifies (11u & 1u) == 1u, toggles expected_bit to 0u, and
//         shifts value right to 5u (101_2).
//       - Iteration 2 finds (5u & 1u) == 1u != expected_bit (0u), immediately
//         returning false (matching 11 ^ 5 = 14 with (14 & 15) = 14 != 0).
//
//   Example 3:
//     Input:  n = 1
//     Output: true
//     Explanation:
//       - Convert n = 1 (1_2) to value = 1u and initialize expected_bit = 1u & 1u =
//         1u.
//       - Iteration 1 checks (1u & 1u) == 1u, toggles expected_bit to 0u, and
//         shifts value right to 0u.
//       - The while (value != 0u) loop ends and returns true (matching 1 ^ 0 = 1
//         with (1 & 2) == 0).
//
// Algorithm:
//   1. Bit-by-bit scan (primary): Track the expected least-significant bit
//      starting with (n & 1). Repeatedly verify that the current LSB equals
//      expected_bit, toggle expected_bit via XOR with 1, and right-shift the
//      value by 1 until all bits are consumed.
//      Time Complexity: O(log n) — at most 31 iterations for 32-bit ints.
//      Space Complexity: O(1).
//   2. Constant-time bitwise trick (secondary): Let x = n ^ (n >> 1). If n
//      has alternating bits, every bit position from 0 up to MSB(n) differs
//      from its neighbor, so x is a contiguous block of 1-bits (2^k - 1).
//      Testing (x & (x + 1)) == 0 checks whether x + 1 is a power of two.
//      Time Complexity: O(1). Space Complexity: O(1).
//
// Data Structures:
//   Only fixed-width unsigned integer registers (std::uint32_t / std::uint64_t)
//   for safe bit shifts without signed-overflow undefined behavior.
//
// Real-World Applications:
//   - Hardware clock-recovery and preamble detection in serial protocols
//     (e.g., Ethernet 10101010 preamble synchronization patterns).
//   - Manchester encoding / line-code validation in telecommunications where
//     consecutive identical symbols indicate a framing error.
//   - Built-in self-test (BIST) checkerboard memory pattern verification
//     (0x55555555 / 0xAAAAAAAA) for detecting stuck-at and coupling faults.
//
// Related LeetCode Problems:
//   - #191 Number of 1 Bits [done] (fundamental bit-inspection and shifting)
//   - #231 Power of Two [done] (uses the same x & (x - 1) == 0 bit-mask test)
//   - #342 Power of Four [new] (uses alternating bitmask 0x55555555)
//   - #868 Binary Gap [done] (examines spacing between adjacent set bits)
//   - #190 Reverse Bits [done] (bit-by-bit extraction and shifting loop)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>

namespace {

class Solution {
 public:
  // Scans bits from LSB to MSB, asserting that each bit flips from the last.
  // Time: O(log n), Space: O(1).
  bool hasAlternatingBits(int n) {
    if (n <= 0) {
      return false;
    }

    // Use unsigned arithmetic so right shifts are logical shifts.
    auto value = static_cast<std::uint32_t>(n);
    std::uint32_t expected_bit = value & 1u;

    // Invariant: all shifted-out bits alternated, and the next LSB must match
    // expected_bit.
    while (value != 0u) {
      if ((value & 1u) != expected_bit) {
        return false;
      }
      expected_bit ^= 1u;
      value >>= 1u;
    }
    return true;
  }

  // Constant-time bitwise verification using XOR with a 1-bit right shift.
  // Time: O(1), Space: O(1).
  bool hasAlternatingBitsBitwise(int n) {
    if (n <= 0) {
      return false;
    }

    // Widen to 64-bit so (all_ones + 1) cannot overflow even if all 32 bits
    // are set.
    const auto u = static_cast<std::uint64_t>(n);
    const std::uint64_t all_ones = u ^ (u >> 1u);
    return (all_ones & (all_ones + 1u)) == 0u;
  }
};

LC_PROBLEM(693, "Binary Number With Alternating Bits", "bit_manipulation",
           ctx) {
  Solution sol;

  ctx.test("example_1_five", [&] {
    // 5 = 101_2
    ctx.expect_true(sol.hasAlternatingBits(5));
    ctx.expect_true(sol.hasAlternatingBitsBitwise(5));
  });

  ctx.test("example_2_seven", [&] {
    // 7 = 111_2
    ctx.expect_false(sol.hasAlternatingBits(7));
    ctx.expect_false(sol.hasAlternatingBitsBitwise(7));
  });

  ctx.test("example_3_eleven", [&] {
    // 11 = 1011_2
    ctx.expect_false(sol.hasAlternatingBits(11));
    ctx.expect_false(sol.hasAlternatingBitsBitwise(11));
  });

  ctx.test("python_build_six", [&] {
    // 6 = 110_2
    ctx.expect_false(sol.hasAlternatingBits(6));
    ctx.expect_false(sol.hasAlternatingBitsBitwise(6));
  });

  ctx.test("edge_cases_small_and_max_alternating", [&] {
    // 1 = 1_2, 2 = 10_2, 10 = 1010_2
    ctx.expect_true(sol.hasAlternatingBits(1));
    ctx.expect_true(sol.hasAlternatingBitsBitwise(1));
    ctx.expect_true(sol.hasAlternatingBits(2));
    ctx.expect_true(sol.hasAlternatingBitsBitwise(2));
    ctx.expect_true(sol.hasAlternatingBits(10));
    ctx.expect_true(sol.hasAlternatingBitsBitwise(10));

    // Largest 32-bit signed alternating integer: 0x55555555 (1431655765).
    constexpr int kMaxAlternating = 0x55555555;
    ctx.expect_true(sol.hasAlternatingBits(kMaxAlternating));
    ctx.expect_true(sol.hasAlternatingBitsBitwise(kMaxAlternating));

    // 2^31 - 1 = 0x7FFFFFFF (all ones).
    constexpr int kMaxInt = std::numeric_limits<int>::max();
    ctx.expect_false(sol.hasAlternatingBits(kMaxInt));
    ctx.expect_false(sol.hasAlternatingBitsBitwise(kMaxInt));
  });
}

}  // namespace
