// LeetCode 190: Reverse Bits
// https://leetcode.com/problems/reverse-bits/
//
// Problem:
//   Given a 32-bit unsigned integer `n`, reverse the order of its 32 bits
//   (bit `i` moves to position `31 - i` for all `0 <= i < 32`) and return the
//   resulting 32-bit unsigned integer.
//   Follow-up: optimize when called repeatedly.
//
// Examples:
//   Example 1:
//     Input:  n = 43261596 (0b00000010100101000001111010011100)
//     Output: 964176192 (0b00111001011110000010100101000000)
//     Explanation:
//       - Initialize result = 0 and loop i from 0 to 31 in reverseBits.
//       - In the first 8 iterations, extract the lowest byte 10011100 bit by bit
//         via (n & 1) and shift it into result as 00111001 (0x39).
//       - Across the remaining 24 iterations, shift the upper 24 bits 00000010
//         10010100 00011110 into result as 01111000 00101001 01000000.
//       - After 32 iterations, return result = 0x39782940 = 964176192.
//
//   Example 2:
//     Input:  n = 4294967293 (0xFFFFFFFD)
//     Output: 3221225471 (0xBFFFFFFF)
//     Explanation:
//       - Initialize result = 0 with n = 0xFFFFFFFD (all bits 1 except bit 1 which
//         is 0).
//       - Iteration i = 0 shifts n & 1 = 1 into result (1), and iteration i = 1
//         shifts n & 1 = 0 into result (0b10).
//       - Iterations i = 2 through 31 shift thirty consecutive 1 bits into result,
//         pushing the sole 0 bit from position 1 to position 30.
//       - Return result = 0xBFFFFFFF = 3221225471.
//
//   Example 3:
//     Input:  n = 1 (0x00000001)
//     Output: 2147483648 (0x80000000)
//     Explanation:
//       - Iteration i = 0 reads n & 1 = 1 into result = 1 and shifts n right to 0.
//       - Iterations i = 1 through 31 shift result left 31 times while n & 1
//         remains 0, moving the 1 bit from position 0 to position 31.
//       - Return result = 0x80000000 = 2147483648.
//
// Algorithm:
//   1. Shift-and-Accumulate (`reverseBits`):
//      Iterate exactly 32 times. On each step, shift `result` left by 1 to
//      open the least significant slot, copy `n & 1` into that slot, and shift
//      `n` right by 1. This directly reverses all 32 bits in registers without
//      heap allocation or string formatting (improving on the Python string-
//      reversal approach).
//      Time: O(1) (fixed 32 iterations), Space: O(1).
//   2. Divide-and-Conquer SWAR Swap (`reverseBitsParallel`):
//      Swap adjacent 16-bit halves, then 8-bit bytes, 4-bit nibbles, 2-bit
//      pairs, and 1-bit neighbors using five constant-time mask-and-shift
//      operations — ideal for high-frequency calls without lookup tables.
//      Time: O(1) (5 bitwise steps / O(log W)), Space: O(1).
//
// Data Structures:
//   - Scalar 32-bit unsigned integers (`std::uint32_t`) and compile-time
//     bitmask constants (`constexpr std::uint32_t`).
//
// Real-World Applications:
//   - Bit-reversal permutation indexing in Cooley-Tukey Fast Fourier Transform
//     (FFT) implementations for digital signal and audio processing.
//   - Serial communication protocols (SPI, UART, CRC-32 / Ethernet FCS) that
//     transmit or compute polynomials LSB-first vs. MSB-first.
//   - Graphics and Morton/Z-order curve manipulation and low-level SIMD
//     permutation kernels.
//
// Related LeetCode Problems:
//   - #191 Number Of 1 Bits [done] (inspects individual bits of a 32-bit unsigned integer)
//   - #338 Counting Bits [done] (bit-manipulation DP over binary representations)
//   - #231 Power Of Two [done] (single-bit property check on binary representations)
//   - #476 Number Complement [new] (flips all significant bits of an integer using bitmasks)
//   - #7 Reverse Integer [done] (base-10 digit-reversal counterpart with overflow handling)

#include "framework/lc_framework.h"

#include <cstdint>

namespace {

constexpr int kBitWidth = 32;

class Solution {
 public:
  // Reverses all 32 bits of `n` by shifting out the LSB of `n` and shifting it
  // into the LSB of `result` across 32 fixed iterations.
  // Invariant: after `i` iterations, the lowest `i` bits of `result` are the
  // reverse of the original lowest `i` bits of `n`.
  // Time: O(1) (32 steps), Space: O(1).
  std::uint32_t reverseBits(std::uint32_t n) {
    std::uint32_t result = 0U;
    for (int i = 0; i < kBitWidth; ++i) {
      // Make room at bit 0 of `result` and insert the current LSB of `n`.
      result = (result << 1U) | (n & 1U);
      n >>= 1U;
    }
    return result;
  }

  // Follow-up optimization: bit-parallel divide-and-conquer reversal in 5
  // constant-time mask-and-shift rounds (SWAR).
  // Each round halves the block size being swapped: 16 -> 8 -> 4 -> 2 -> 1.
  // Time: O(1) (5 rounds), Space: O(1).
  std::uint32_t reverseBitsParallel(std::uint32_t n) {
    // Swap adjacent 16-bit halves.
    n = (n >> 16U) | (n << 16U);
    // Swap adjacent 8-bit bytes within each 16-bit half.
    n = ((n & 0xFF00FF00U) >> 8U) | ((n & 0x00FF00FFU) << 8U);
    // Swap adjacent 4-bit nibbles within each byte.
    n = ((n & 0xF0F0F0F0U) >> 4U) | ((n & 0x0F0F0F0FU) << 4U);
    // Swap adjacent 2-bit pairs within each nibble.
    n = ((n & 0xCCCCCCCCU) >> 2U) | ((n & 0x33333333U) << 2U);
    // Swap adjacent individual bits within each 2-bit pair.
    n = ((n & 0xAAAAAAAAU) >> 1U) | ((n & 0x55555555U) << 1U);
    return n;
  }
};

LC_PROBLEM(190, "Reverse Bits", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_leetcode_sample", [&] {
    constexpr std::uint32_t kInput = 43261596U;
    constexpr std::uint32_t kExpected = 964176192U;
    ctx.expect_eq(sol.reverseBits(kInput), kExpected);
    ctx.expect_eq(sol.reverseBitsParallel(kInput), kExpected);
  });

  ctx.test("example_2_high_bits_set", [&] {
    constexpr std::uint32_t kInput = 4294967293U;     // 0xFFFFFFFD
    constexpr std::uint32_t kExpected = 3221225471U;  // 0xBFFFFFFF
    ctx.expect_eq(sol.reverseBits(kInput), kExpected);
    ctx.expect_eq(sol.reverseBitsParallel(kInput), kExpected);
  });

  ctx.test("build_secondary_sample_11", [&] {
    // 11 = 0b...00001011 -> reversed = 0b11010000... = 0xD0000000
    ctx.expect_eq(sol.reverseBits(11U), 0xD0000000U);
    ctx.expect_eq(sol.reverseBitsParallel(11U), 0xD0000000U);
  });

  ctx.test("edge_cases_and_involution", [&] {
    ctx.expect_eq(sol.reverseBits(0U), 0U);
    ctx.expect_eq(sol.reverseBitsParallel(0U), 0U);
    ctx.expect_eq(sol.reverseBits(0xFFFFFFFFU), 0xFFFFFFFFU);
    ctx.expect_eq(sol.reverseBitsParallel(0xFFFFFFFFU), 0xFFFFFFFFU);
    ctx.expect_eq(sol.reverseBits(1U), 0x80000000U);
    ctx.expect_eq(sol.reverseBitsParallel(0x80000000U), 1U);

    // Reversing twice must recover the original value.
    constexpr std::uint32_t kArbitrary = 0x1234ABCDU;
    ctx.expect_eq(sol.reverseBits(sol.reverseBits(kArbitrary)), kArbitrary);
    ctx.expect_eq(sol.reverseBits(kArbitrary),
                  sol.reverseBitsParallel(kArbitrary));
  });
}

}  // namespace
