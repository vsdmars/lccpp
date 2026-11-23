// LeetCode 461: Hamming Distance
// https://leetcode.com/problems/hamming-distance/
//
// Problem:
//   Given two non-negative integers `x` and `y` (0 <= x, y <= 2^31 - 1),
//   calculate their Hamming distance — the number of bit positions at which
//   their binary representations differ.
//
// Algorithm:
//   Primary (`std::popcount`): compute `diff = x ^ y` in `std::uint32_t`,
//   whose set bits correspond 1-to-1 with bit positions where `x` and `y`
//   disagree, then count the set bits via C++20/23 `std::popcount(diff)`
//   (lowered to a single hardware population-count instruction).
//   Time Complexity:  O(1).
//   Space Complexity: O(1).
//
//   Alternative (Brian Kernighan's bit-clearing loop): repeatedly clear the
//   lowest set bit via `diff &= (diff - 1U)` and increment a counter until
//   `diff == 0U`. Runs in O(k) where `k <= 31` is the Hamming distance itself.
//
// Data Structures:
//   Scalar 32-bit unsigned bitmask (`std::uint32_t`) storing `x ^ y`.
//
// Real-World Applications:
//   Near-duplicate web-page and image detection via perceptual hashing /
//   SimHash, error-detection and error-correction codes (Hamming / Reed-Solomon
//   minimum distance), and DNA/binary fingerprint similarity search.
//
// Related LeetCode Problems:
//   - #191 Number of 1 Bits [done] (popcount of a single integer)
//   - #477 Total Hamming Distance [done] (sum of pairwise Hamming distances via bit counts)
//   - #338 Counting Bits [done] (popcount for all integers in 0..n)
//   - #2220 Minimum Bit Flips to Convert Number [new] (identical formulation as bit flips)

#include "framework/lc_framework.h"

#include <bit>
#include <cstdint>
#include <limits>

namespace {

class Solution {
 public:
  // Returns the number of differing bit positions between `x` and `y` using
  // bitwise XOR followed by C++23 `std::popcount`.
  // Time: O(1), Space: O(1).
  int hammingDistance(int x, int y) {
    const std::uint32_t diff =
        static_cast<std::uint32_t>(x) ^ static_cast<std::uint32_t>(y);
    return std::popcount(diff);
  }

  // Alternative implementation using Brian Kernighan's algorithm: each step
  // clears the rightmost 1-bit via `diff &= (diff - 1U)`.
  // Time: O(popcount(x ^ y)) <= O(31) = O(1), Space: O(1).
  int hammingDistanceKernighan(int x, int y) {
    std::uint32_t diff =
        static_cast<std::uint32_t>(x) ^ static_cast<std::uint32_t>(y);
    int distance = 0;

    while (diff != 0U) {
      // Subtracting 1 flips the lowest set bit to 0 and all lower 0s to 1;
      // ANDing with `diff` strips that lowest set bit in O(1).
      diff &= (diff - 1U);
      ++distance;
    }

    return distance;
  }
};

LC_PROBLEM(461, "Hamming Distance", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    // 1 (0001) vs 4 (0100) -> 2
    ctx.expect_eq(sol.hammingDistance(1, 4), 2);
    ctx.expect_eq(sol.hammingDistanceKernighan(1, 4), 2);
  });

  ctx.test("python_build_and_example_2", [&] {
    // 4 (0100) vs 9 (1001) -> 3
    ctx.expect_eq(sol.hammingDistance(4, 9), 3);
    ctx.expect_eq(sol.hammingDistanceKernighan(4, 9), 3);
    // 3 (11) vs 1 (01) -> 1
    ctx.expect_eq(sol.hammingDistance(3, 1), 1);
    ctx.expect_eq(sol.hammingDistanceKernighan(3, 1), 1);
  });

  ctx.test("identical_values_zero_distance", [&] {
    ctx.expect_eq(sol.hammingDistance(0, 0), 0);
    ctx.expect_eq(sol.hammingDistanceKernighan(0, 0), 0);
    ctx.expect_eq(sol.hammingDistance(12345, 12345), 0);
    ctx.expect_eq(sol.hammingDistanceKernighan(12345, 12345), 0);
  });

  ctx.test("max_31bit_and_alternating_patterns", [&] {
    const int max_val = std::numeric_limits<int>::max();
    ctx.expect_eq(sol.hammingDistance(0, max_val), 31);
    ctx.expect_eq(sol.hammingDistanceKernighan(0, max_val), 31);
    ctx.expect_eq(sol.hammingDistance(0x55555555, 0x2AAAAAAA), 31);
    ctx.expect_eq(sol.hammingDistanceKernighan(0x55555555, 0x2AAAAAAA), 31);
  });
}

}  // namespace
