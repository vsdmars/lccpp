// LeetCode 191: Number Of 1 Bits
// https://leetcode.com/problems/number-of-1-bits/
//
// Problem:
//   Write a function that takes a 32-bit unsigned integer `n` and returns the
//   number of `'1'` bits in its binary representation (also known as its
//   Hamming weight or population count).
//
// Algorithm:
//   1. Brian Kernighan's Bit-Clearing Algorithm (`hammingWeight`):
//      Subtracting 1 from a nonzero unsigned integer flips its lowest set bit
//      from 1 to 0 and all trailing 0s to 1. Therefore `n & (n - 1)` clears
//      exactly the lowest set bit while leaving all higher bits intact.
//      Repeating until `n == 0` loops once per set bit rather than across all
//      32 bit positions.
//      Time: O(k) where k <= 32 is the number of 1-bits (O(1) bounded),
//      Space: O(1).
//   2. Fixed-Width Shift-and-Mask (`hammingWeightShift`, direct port of Python
//      `bits()`):
//      Checks `n & 1` and shifts `n` right by 1 across all 32 bit positions.
//      Time: O(1) (32 iterations), Space: O(1).
//   (Note: C++20/23 also offers `std::popcount(n)` in `<bit>`, which compiles
//   directly to a single hardware `POPCNT` instruction when supported.)
//
// Data Structures:
//   - Scalar 32-bit unsigned integer (`std::uint32_t`) and integer counter.
//
// Real-World Applications:
//   - Cryptographic and information-theory Hamming distance calculations,
//     error-correcting codes (ECC), and locality-sensitive hashing (SimHash).
//   - Succinct data structures (`rank`/`select` bitvectors), compressed bitmap
//     indexes (Roaring Bitmaps), and Bloom-filter saturation monitoring.
//   - Chess engines (bitboard mobility/material counts) and hardware telemetry
//     bitmask diagnostics.
//
// Related LeetCode Problems:
//   - #338 Counting Bits [done] (computes Hamming weight for every integer in [0, n] in O(n) total time)
//   - #231 Power Of Two [done] (checks whether Hamming weight is exactly 1 using `n & (n - 1) == 0`)
//   - #190 Reverse Bits [done] (32-bit unsigned integer bit manipulation)
//   - #461 Hamming Distance [done] (Hamming weight of `x ^ y`)
//   - #477 Total Hamming Distance [done] (sums pairwise Hamming weights column by column)

#include "framework/lc_framework.h"

#include <bit>
#include <cstdint>

namespace {

constexpr int kBitWidth = 32;

class Solution {
 public:
  // Counts the number of set bits using Brian Kernighan's trick.
  // Invariant: `count + popcount(n)` equals the Hamming weight of the original
  // input; each iteration strictly removes the lowest set bit of `n`.
  // Time: O(k) <= O(32) = O(1), Space: O(1).
  int hammingWeight(std::uint32_t n) {
    int count = 0;
    while (n != 0U) {
      // Clear the least significant 1-bit of `n`.
      n &= (n - 1U);
      ++count;
    }
    return count;
  }

  // Secondary approach (direct port of Python `bits()`):
  // Inspects every bit position from LSB to MSB across all 32 bits.
  // Time: O(1) (fixed 32 iterations), Space: O(1).
  int hammingWeightShift(std::uint32_t n) {
    int count = 0;
    for (int i = 0; i < kBitWidth; ++i) {
      count += static_cast<int>(n & 1U);
      n >>= 1U;
    }
    return count;
  }
};

LC_PROBLEM(191, "Number Of 1 Bits", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_eleven", [&] {
    // 11 = 0b1011 -> 3 set bits.
    ctx.expect_eq(sol.hammingWeight(11U), 3);
    ctx.expect_eq(sol.hammingWeightShift(11U), 3);
  });

  ctx.test("example_2_power_of_two", [&] {
    // 128 = 0b10000000 -> 1 set bit.
    ctx.expect_eq(sol.hammingWeight(128U), 1);
    ctx.expect_eq(sol.hammingWeightShift(128U), 1);
  });

  ctx.test("example_3_thirty_one_bits", [&] {
    constexpr std::uint32_t kVal = 4294967293U;  // 0xFFFFFFFD
    ctx.expect_eq(sol.hammingWeight(kVal), 31);
    ctx.expect_eq(sol.hammingWeightShift(kVal), 31);
  });

  ctx.test("edge_cases_and_std_popcount_agreement", [&] {
    for (std::uint32_t val : {0U, 1U, 0x80000000U, 0x55555555U, 0xFFFFFFFFU,
                              0xDEADBEEFU}) {
      const int expected = std::popcount(val);
      ctx.expect_eq(sol.hammingWeight(val), expected);
      ctx.expect_eq(sol.hammingWeightShift(val), expected);
    }
  });
}

}  // namespace
