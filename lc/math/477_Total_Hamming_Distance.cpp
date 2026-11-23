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
// Examples:
//   Example 1:
//     Input:  nums = [4, 14, 2]
//     Output: 6
//     Explanation:
//       - In binary (bits 3 down to 0), the n = 3 elements are 4 = 0100, 14 = 1110,
//         and 2 = 0010.
//       - At bit = 0, all three elements have bit 0 (ones = 0, zeros = 3), adding 0
//         * 3 = 0 to total_distance.
//       - At bit = 1, 14 and 2 have bit 1 while 4 has bit 0 (ones = 2, zeros = 1),
//         adding 2 * 1 = 2 to total_distance.
//       - At bit = 2, 4 and 14 have bit 1 while 2 has bit 0 (ones = 2, zeros = 1),
//         adding 2 (total_distance = 4); at bit = 3, only 14 has bit 1 (ones = 1,
//         zeros = 2), adding 2 (total_distance = 6).
//       - Columns bit = 4 through 31 have ones = 0 across all elements and add 0,
//         so the function returns 6.
//
//   Example 2:
//     Input:  nums = [1, 2, 4, 8]
//     Output: 12
//     Explanation:
//       - With n = 4, the binary values 1 = 0001, 2 = 0010, 4 = 0100, and 8 = 1000
//         each have a single 1 bit in a distinct column in [0, 3].
//       - For each of bit = 0, 1, 2, and 3, ones = 1 and zeros = 4 - 1 = 3,
//         contributing 1 * 3 = 3 to total_distance.
//       - Columns bit = 4 through 31 all have ones = 0 and contribute 0, resulting
//         in total_distance = 4 * 3 = 12.
//
//   Example 3:
//     Input:  nums = [7]
//     Output: 0
//     Explanation:
//       - The element count n = static_cast<std::int64_t>(nums.size()) equals 1.
//       - The guard if (n <= 1) triggers immediately because a single-element array
//         forms no unordered pairs, returning 0.
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
