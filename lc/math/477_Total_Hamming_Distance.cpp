// LeetCode 477: Total Hamming Distance
// https://leetcode.com/problems/total-hamming-distance/
//
// Problem:
//   The Hamming distance between two integers is the number of bit positions
//   at which their corresponding bits differ. Given an integer array `nums`,
//   return the sum of Hamming distances between all unordered pairs of
//   integers in `nums`.
//   Constraints: 1 <= nums.size() <= 10^4, 0 <= nums[i] <= 10^9, and the
//   answer is guaranteed to fit in a 32-bit signed integer.
//
// Algorithm:
//   Bitwise column decomposition. Because Hamming distance sums differences
//   independently across bit positions, evaluate each bit column b in [0, 31]
//   separately instead of comparing all O(n^2) pairs directly.
//   In column b, suppose `ones` elements have bit b equal to 1 and
//   `zeros = n - ones` elements have bit b equal to 0. Every pair formed by
//   choosing one element from the 1-group and one from the 0-group contributes
//   1 to column b's total distance, yielding exactly (ones * zeros) pairs
//   (the Cartesian product of the two partitions). Summing (ones * zeros) over
//   all 32 bit positions yields the exact total Hamming distance.
//   Time Complexity:  O(32 * n) = O(n).
//   Space Complexity: O(1) — only fixed scalar counters are used.
//
// Data Structures:
//   - Input `std::vector<int>` scanned once per bit column (no auxiliary
//     containers required).
//
// Real-World Applications:
//   - Error-correcting code (ECC) analysis and minimum-distance profiling for
//     binary codebooks in telecommunications and storage systems.
//   - Locality-sensitive hashing (SimHash) diversity measurement across large
//     collections of document or image fingerprints.
//   - Bioinformatics pairwise SNP divergence aggregation across genomic
//     bit-vectors without quadratic pairwise comparisons.
//
// Related LeetCode Problems:
//   - #461 Hamming Distance [done] (Hamming distance between a single pair of integers)
//   - #191 Number of 1 Bits [done] (counting set bits / popcount of an integer)
//   - #2220 Minimum Bit Flips to Convert Number [new] (equivalent to pairwise Hamming distance)
//   - #3153 Sum of Digit Differences of All Pairs [new] (base-10 generalization of column-wise pair counting)

#include "framework/lc_framework.h"

#include <cstdint>
#include <vector>

namespace {

constexpr int kBitWidth = 32;

class Solution {
 public:
  // Computes the sum of Hamming distances over all unordered pairs in `nums`
  // by counting (ones * zeros) independently at each bit position.
  //
  // Time: O(32 * n) = O(n), Space: O(1).
  int totalHammingDistance(const std::vector<int>& nums) {
    const auto n = static_cast<std::int64_t>(nums.size());
    if (n <= 1) {
      return 0;
    }

    std::int64_t total_distance = 0;

    // Evaluate each bit column independently; bit b contributes ones * zeros.
    for (int bit = 0; bit < kBitWidth; ++bit) {
      std::int64_t ones = 0;
      for (const int num : nums) {
        // Cast to unsigned before shifting to guarantee well-defined logical
        // right shifts across all 32 bit positions.
        const auto bits = static_cast<std::uint32_t>(num);
        ones += static_cast<std::int64_t>((bits >> bit) & 1U);
      }

      const std::int64_t zeros = n - ones;
      total_distance += (ones * zeros);
    }

    return static_cast<int>(total_distance);
  }
};

LC_PROBLEM(477, "Total Hamming Distance", "math", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.totalHammingDistance({4, 14, 2}), 6);
  });

  ctx.test("example_2_duplicates", [&] {
    ctx.expect_eq(sol.totalHammingDistance({4, 14, 4}), 4);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.totalHammingDistance({7}), 0);
  });

  ctx.test("all_equal", [&] {
    ctx.expect_eq(sol.totalHammingDistance({5, 5, 5, 5}), 0);
  });

  ctx.test("distinct_powers_of_two", [&] {
    // 4 powers of two => C(4, 2) = 6 pairs, each differing in exactly 2 bits.
    ctx.expect_eq(sol.totalHammingDistance({1, 2, 4, 8}), 12);
  });

  ctx.test("zero_and_max_constraint", [&] {
    // 1'000'000'000 == 0x3B9ACA00 has 13 set bits.
    ctx.expect_eq(sol.totalHammingDistance({0, 1'000'000'000}), 13);
  });
}

}  // namespace
