// LeetCode 868: Binary Gap
// https://leetcode.com/problems/binary-gap/
//
// Problem:
//   Given a positive integer n (1 <= n <= 10^9), find and return the longest
//   distance between any two adjacent 1-bits in the binary representation of
//   n. Two 1s are adjacent if only 0s (possibly none) lie between them, and
//   their distance is the absolute difference between their bit positions.
//   If n contains fewer than two 1-bits, return 0.
//
// Algorithm:
//   Scan the bits of n from least significant to most significant while
//   tracking the current bit index (bit_pos) and the index of the most
//   recently seen 1-bit (last_one, initialized to -1). Whenever the current
//   LSB is 1:
//     - If last_one != -1, update max_gap = max(max_gap, bit_pos - last_one).
//     - Update last_one = bit_pos.
//   Right-shift the unsigned value by 1 and increment bit_pos until value == 0.
//   Because pairwise distances between adjacent 1s are symmetric under bit
//   reversal, scanning LSB-to-MSB produces the exact same maximum gap as
//   scanning MSB-to-LSB without allocating a binary string or index vector.
//   Time Complexity: O(log n) — at most 30 iterations for n <= 10^9.
//   Space Complexity: O(1).
//
// Data Structures:
//   Scalar integer state variables (unsigned shift register, current bit
//   position, previous 1-bit position, and running maximum gap).
//
// Real-World Applications:
//   - Run-length encoding (RLE) and sparse bitmap compression analysis, where
//     the maximum zero-run between set bits determines word-alignment savings.
//   - Signal-processing / RLL (Run-Length Limited) magnetic and optical
//     storage channel constraints (bounding maximum distance between flux
//     transitions for clock synchronization).
//   - Hardware bit-scan scheduling and priority-encoder latency profiling.
//
// Related LeetCode Problems:
//   - #693 Binary Number With Alternating Bits [done] (checks fixed gap == 2)
//   - #485 Max Consecutive Ones [done] (maximum contiguous run in a bit array)
//   - #191 Number of 1 Bits [done] (counting set bits via shifting)
//   - #762 Prime Number Of Set Bits In Binary Representation [done] (bit analysis)
//   - #1004 Max Consecutive Ones III [done] (longest run with bounded flips)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>

namespace {

class Solution {
 public:
  // Returns the maximum bit-position distance between any two adjacent 1-bits
  // in the binary representation of n, or 0 if fewer than two 1-bits exist.
  // Time: O(log n), Space: O(1).
  int binaryGap(int n) {
    if (n <= 0) {
      return 0;
    }

    auto value = static_cast<std::uint32_t>(n);
    int last_one = -1;
    int bit_pos = 0;
    int max_gap = 0;

    // Invariant: max_gap holds the largest distance between adjacent 1s among
    // all bits strictly below bit_pos, and last_one is the highest 1-bit seen.
    while (value != 0u) {
      if ((value & 1u) != 0u) {
        if (last_one != -1) {
          max_gap = std::max(max_gap, bit_pos - last_one);
        }
        last_one = bit_pos;
      }
      value >>= 1u;
      ++bit_pos;
    }
    return max_gap;
  }
};

LC_PROBLEM(868, "Binary Gap", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_twenty_two", [&] {
    // 22 = 10110_2 -> adjacent 1-bit distances are 1 and 2 -> max = 2
    ctx.expect_eq(sol.binaryGap(22), 2);
  });

  ctx.test("power_of_two_no_adjacent_ones", [&] {
    // 8 = 1000_2 and 1 = 1_2 have only one 1-bit -> 0
    ctx.expect_eq(sol.binaryGap(8), 0);
    ctx.expect_eq(sol.binaryGap(1), 0);
    ctx.expect_eq(sol.binaryGap(1 << 29), 0);
  });

  ctx.test("standard_examples", [&] {
    // 5 = 101_2 -> 2
    ctx.expect_eq(sol.binaryGap(5), 2);
    // 6 = 110_2 -> 1
    ctx.expect_eq(sol.binaryGap(6), 1);
    // 9 = 1001_2 -> 3
    ctx.expect_eq(sol.binaryGap(9), 3);
  });

  ctx.test("large_gap_between_endpoints", [&] {
    // (1 << 29) | 1 has 1-bits at positions 0 and 29 -> gap = 29
    ctx.expect_eq(sol.binaryGap((1 << 29) | 1), 29);
    // All ones: 0x3FFFFFFF -> every adjacent pair has distance 1
    ctx.expect_eq(sol.binaryGap(0x3FFFFFFF), 1);
  });
}

}  // namespace
