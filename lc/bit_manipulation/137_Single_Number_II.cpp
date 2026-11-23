// LeetCode 137: Single Number II
// https://leetcode.com/problems/single-number-ii/
//
// Problem:
//   Given a non-empty integer array `nums` where every element appears exactly
//   three times except for one element that appears exactly once, find and
//   return that single unique element. The solution must run in O(n) linear
//   time and O(1) constant extra space.
//
// Algorithm:
//   Bitwise Modulo-3 Finite State Machine:
//   For every bit position b in [0, 31], summing bit b across all elements
//   produces `3k + u_b`, where `u_b` is bit b of the unique number. Taking the
//   count modulo 3 yields 3 possible states: 0 (00), 1 (01), and 2 (10).
//   We track all 32 bit positions in parallel using two 32-bit unsigned bitmasks
//   `(twos, ones)` representing the high and low bits of each position's mod-3
//   counter. For each incoming element `x`, Karnaugh-map minimization of the
//   state transition table `(00 -> 01 -> 10 -> 00 on bit 1)` gives:
//     ones' = (ones ^ x) & ~twos
//     twos' = (twos ^ x) & ~ones'
//   After scanning the entire array, every bit appearing a multiple of 3 times
//   returns to state 00, while every bit of the unique element sits in state 01
//   (`ones` holds the exact bit pattern of the answer).
//   Time: O(n), Space: O(1).
//
// Data Structures:
//   - Two 32-bit unsigned integer registers (`ones`, `twos`) acting as 32
//     parallel 2-bit modulo-3 counters (bit-sliced state machine).
//
// Real-World Applications:
//   - Triple Modular Redundancy (TMR) and fault-tolerant voting circuits in
//     aerospace/hardware design, using minimal gate counts to track mod-k bit
//     multiplicities across parallel word lanes.
//   - SWAR (SIMD Within A Register) bit-sliced counters in high-throughput
//     packet processing and database bitmap indexes.
//   - Digital logic synthesis and Karnaugh-map state minimization for compact
//     hardware counters.
//
// Related LeetCode Problems:
//   - #136 Single Number [done] (simpler mod-2 counterpart solved with a single XOR register)
//   - #260 Single Number III [done] (two unique elements among pairs, split by a differing bit)
//   - #268 Missing Number [done] (linear-time O(1)-space bit cancellation across a range)
//   - #645 Set Mismatch [new] (recovers duplicated and missing numbers via bit/parity separation)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the sole element appearing once when all others appear three times.
  // Invariant: for every bit index b in [0, 31], the 2-bit pair
  // `(twos[b], ones[b])` equals `(count of 1s seen at bit b) mod 3`:
  //   00 -> 0 mod 3, 01 -> 1 mod 3, 10 -> 2 mod 3 (11 is unreachable).
  // Time: O(n), Space: O(1).
  int singleNumber(const std::vector<int>& nums) {
    // Use uint32_t so bitwise complement and masking are well-defined for all
    // 32 bit positions, including the sign bit (bit 31) of negative numbers.
    std::uint32_t ones = 0U;
    std::uint32_t twos = 0U;

    for (int num : nums) {
      const auto bits = static_cast<std::uint32_t>(num);
      // Advance each bit's mod-3 state when `bits` has a 1 at that position:
      //   (twos=0, ones=0) + 1 -> ones becomes 1, then twos stays 0  (00 -> 01)
      //   (twos=0, ones=1) + 1 -> ones becomes 0, then twos becomes 1 (01 -> 10)
      //   (twos=1, ones=0) + 1 -> ones masked to 0 by ~twos, twos becomes 0 (10 -> 00)
      ones = (ones ^ bits) & ~twos;
      twos = (twos ^ bits) & ~ones;
    }

    // Elements appearing 3 times reset to (0, 0); the unique element remains in `ones`.
    return static_cast<int>(ones);
  }
};

LC_PROBLEM(137, "Single Number II", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    ctx.expect_eq(sol.singleNumber({2, 2, 3, 2}), 3);
  });

  ctx.test("example_2_multiple_triples", [&] {
    ctx.expect_eq(sol.singleNumber({0, 1, 0, 1, 0, 1, 99}), 99);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.singleNumber({42}), 42);
  });

  ctx.test("negative_numbers", [&] {
    ctx.expect_eq(sol.singleNumber({-2, -2, 1, 1, 4, 1, 4, 4, -4, -2}), -4);
    ctx.expect_eq(sol.singleNumber({0, 0, 0, -1}), -1);
  });

  ctx.test("int32_extremes", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    ctx.expect_eq(sol.singleNumber({kMin, kMax, kMax, kMax}), kMin);
    ctx.expect_eq(sol.singleNumber({kMin, kMin, kMin, kMax}), kMax);
  });
}

}  // namespace
