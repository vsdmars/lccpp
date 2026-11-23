// LeetCode 869: Reordered Power Of 2
// https://leetcode.com/problems/reordered-power-of-2/
//
// Problem:
//   Given an integer `n` (1 <= n <= 10^9), determine whether its base-10 digits
//   can be reordered (including leaving them in their original order) with no
//   leading zero to form a power of two.
//
// Examples:
//   Example 1:
//     Input:  n = 1
//     Output: true
//     Explanation:
//       - Compute target_counts for n = 1, recording counts[1] = 1 and 0 for all
//         other digits.
//       - Start iterating bit from 0 to 30; at bit = 0, power_of_two = 1ULL << 0 =
//         1 has digitCounts(1) equal to target_counts.
//       - Return true immediately on the match.
//
//   Example 2:
//     Input:  n = 10
//     Output: false
//     Explanation:
//       - Compute target_counts for n = 10, yielding counts[0] = 1, counts[1] = 1,
//         and 0 elsewhere.
//       - Test every power of two from 2^0 = 1 through 2^30 = 1073741824; the only
//         two-digit powers of two are 16, 32, and 64, none of which have one '0'
//         and one '1'.
//       - Finish the loop with no matching histogram (eliminating '01' due to the
//         no-leading-zero invariant) and return false.
//
//   Example 3:
//     Input:  n = 4201
//     Output: true
//     Explanation:
//       - Compute target_counts for n = 4201, producing counts[0] = 1, counts[1] =
//         1, counts[2] = 1, counts[4] = 1, and 0 for other digits.
//       - Check powers 2^0 through 2^9 (1 through 512), none of which match
//         target_counts.
//       - At bit = 10, power_of_two = 1024 has digitCounts(1024) matching counts[0]
//         = counts[1] = counts[2] = counts[4] = 1, so return true.
//
// Algorithm:
//   Two positive integers without leading zeros are digit permutations of each
//   other if and only if they have identical frequency counts for every digit
//   '0'..'9'.
//   - Compute the 10-element digit-frequency signature of `n`.
//   - Because n <= 10^9 < 2^30 (and all signed 32-bit positive powers of two
//     are 2^0 through 2^30), iterate over `bit` in [0, 30], compute the digit
//     signature of (1ULL << bit), and check for exact equality.
//   - Leading zeros are automatically impossible because every tested power of
//     two is formatted canonically without leading zeros, and exact count
//     matching enforces the same total digit length and zero count.
//   Time: O(log_2(MAX_N) * log_10(MAX_N)) = O(1), Space: O(1).
//
// Data Structures:
//   Fixed-size `std::array<int, 10>` frequency histograms (zero heap
//   allocations, constant-time equality comparison).
//
// Real-World Applications:
//   - Canonical multiset fingerprinting / anagram indexing in search engines,
//     spell checkers, and deduplication pipelines.
//   - Precomputed signature lookup against a small target domain in telemetry
//     classification and protocol validation.
//
// Related LeetCode Problems:
//   - #231 Power of Two [done] (bit-manipulation test for whether an integer is a power of two)
//   - #326 Power of Three [new] (bounded power membership test within 32-bit integers)
//   - #242 Valid Anagram [done] (fixed-alphabet frequency histogram equality)
//   - #49 Group Anagrams [done] (canonical frequency signatures to bucket permutations)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace {

class Solution {
 public:
  // Returns true iff the decimal digits of `n` can be rearranged (without a
  // leading zero) to equal 2^k for some integer k >= 0.
  //
  // Invariant: A valid rearrangement exists iff `digitCounts(n)` matches
  // `digitCounts(2^k)` for some 0 <= k <= 30 (covering all 32-bit positive
  // powers of two, well past the problem's 10^9 upper bound).
  //
  // Time: O(1), Space: O(1).
  bool reorderedPowerOf2(int n) {
    if (n <= 0) {
      return false;
    }

    const auto target_counts = digitCounts(static_cast<std::uint64_t>(n));

    // Check all powers of two that fit in a signed 32-bit int (2^0 .. 2^30).
    for (unsigned int bit = 0U; bit <= kMaxPowerBit; ++bit) {
      const std::uint64_t power_of_two = std::uint64_t{1} << bit;
      if (digitCounts(power_of_two) == target_counts) {
        return true;
      }
    }
    return false;
  }

 private:
  static constexpr std::size_t kBase = 10uz;
  static constexpr unsigned int kMaxPowerBit = 30U;

  // Builds the base-10 digit frequency histogram for a positive integer.
  static std::array<int, kBase> digitCounts(std::uint64_t value) {
    std::array<int, kBase> counts{};
    while (value > 0U) {
      const auto digit = static_cast<std::size_t>(value % kBase);
      ++counts[digit];
      value /= kBase;
    }
    return counts;
  }
};

LC_PROBLEM(869, "Reordered Power Of 2", "math", ctx) {
  Solution sol;

  ctx.test("example_1_one",
           [&] { ctx.expect_true(sol.reorderedPowerOf2(1)); });

  ctx.test("example_2_ten_no_leading_zero",
           [&] { ctx.expect_false(sol.reorderedPowerOf2(10)); });

  ctx.test("exact_powers_of_two", [&] {
    ctx.expect_true(sol.reorderedPowerOf2(16));
    ctx.expect_true(sol.reorderedPowerOf2(1024));
    ctx.expect_true(sol.reorderedPowerOf2(536'870'912));  // 2^29
  });

  ctx.test("valid_rearrangements", [&] {
    ctx.expect_true(sol.reorderedPowerOf2(46));           // -> 64
    ctx.expect_true(sol.reorderedPowerOf2(4201));         // -> 1024
    ctx.expect_true(sol.reorderedPowerOf2(61));           // -> 16
    ctx.expect_true(sol.reorderedPowerOf2(987'653'210));  // -> 536870912 (2^29)
  });

  ctx.test("invalid_numbers", [&] {
    ctx.expect_false(sol.reorderedPowerOf2(24));
    ctx.expect_false(sol.reorderedPowerOf2(100));
    ctx.expect_false(sol.reorderedPowerOf2(999'999'999));
  });
}

}  // namespace
