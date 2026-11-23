// LeetCode 762: Prime Number Of Set Bits In Binary Representation
// https://leetcode.com/problems/prime-number-of-set-bits-in-binary-representation/
//
// Problem:
//   Given two integers left and right (1 <= left <= right <= 10^6, with
//   right - left <= 10^4), count how many integers in the inclusive range
//   [left, right] have a prime number of set bits (1s) in their binary
//   representation. Note that 1 is not a prime number.
//
// Examples:
//   Example 1:
//     Input:  left = 6, right = 10
//     Output: 4
//     Explanation:
//       - For num = 6 (110_2) and num = 7 (111_2), std::popcount yields 2 and 3;
//         bits 2 and 3 are set in kPrimeMask, advancing count to 2.
//       - For num = 8 (1000_2), std::popcount yields 1, and ((kPrimeMask >> 1) &
//         1u) is 0 because 1 is not prime, so count stays 2.
//       - For num = 9 (1001_2) and num = 10 (1010_2), std::popcount yields 2 for
//         each; bit 2 is set in kPrimeMask, bringing the returned count to 4.
//
//   Example 2:
//     Input:  left = 10, right = 15
//     Output: 5
//     Explanation:
//       - Evaluating num = 10 through 14 yields popcounts of 2 (10, 12) and 3 (11,
//         13, 14), all of which match set bits in kPrimeMask and increment count to
//         5.
//       - For num = 15 (1111_2), std::popcount yields 4, and ((kPrimeMask >> 4) &
//         1u) is 0, so count remains 5 and is returned.
//
//   Example 3:
//     Input:  left = 1, right = 1
//     Output: 0
//     Explanation:
//       - The loop runs once for num = 1 (1_2), computing bits = std::popcount(1u)
//         = 1.
//       - Checking ((kPrimeMask >> 1) & 1u) evaluates to 0u since 1 is not prime,
//         so count stays 0 and the function returns 0.
//
// Algorithm:
//   For every integer num in [left, right], compute its Hamming weight
//   (number of 1-bits) in O(1) via C++20/23 std::popcount. Since any 32-bit
//   non-negative integer has at most 31 set bits, we encode all primes <= 31
//   ({2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31}) in a single compile-time
//   32-bit bitmask kPrimeMask. Testing primality then reduces to a single
//   bit-shift and mask check: ((kPrimeMask >> bits) & 1u) != 0u.
//   A secondary trial-division variant is also provided to mirror the
//   general primality-check approach from the Python solution.
//   Time Complexity: O(right - left + 1), with O(1) work per integer.
//   Space Complexity: O(1).
//
// Data Structures:
//   A compile-time 32-bit lookup bitmask (kPrimeMask) acting as an O(1)
//   constant-space membership set for small primes up to 31.
//
// Real-World Applications:
//   - Error-correcting codes and Hamming-weight filtering in digital
//     communications and storage controllers (e.g., constant-weight codes).
//   - Hardware-accelerated POPCNT analytics in bitmap indexes (Roaring
//     Bitmaps) and Bloom-filter cardinality / occupancy monitoring.
//   - Compile-time bitset lookup tables for ultra-low-latency classification
//     of small integer domains in packet parsers and compilers.
//
// Related LeetCode Problems:
//   - #191 Number of 1 Bits [done] (core popcount / Hamming weight primitive)
//   - #338 Counting Bits [done] (computing set-bit counts across a range)
//   - #204 Count Primes [done] (primality testing and sieve fundamentals)
//   - #461 Hamming Distance [done] (popcount of XOR between two integers)
//   - #868 Binary Gap [done] (inspecting set-bit patterns in binary form)

#include "framework/lc_framework.h"

#include <bit>
#include <cstdint>

namespace {

// Bitmask whose bit p is 1 iff p is prime (covers all popcounts of 32-bit
// signed integers, i.e., 0..31).
constexpr std::uint32_t kPrimeMask =
    (1u << 2u) | (1u << 3u) | (1u << 5u) | (1u << 7u) | (1u << 11u) |
    (1u << 13u) | (1u << 17u) | (1u << 19u) | (1u << 23u) | (1u << 29u) |
    (1u << 31u);

class Solution {
 public:
  // Counts integers in [left, right] whose popcount is prime using an O(1)
  // precomputed prime bitmask.
  // Time: O(right - left + 1), Space: O(1).
  int countPrimeSetBits(int left, int right) {
    int count = 0;
    for (int num = left; num <= right; ++num) {
      const auto bits =
          static_cast<std::uint32_t>(std::popcount(static_cast<std::uint32_t>(num)));
      if (((kPrimeMask >> bits) & 1u) != 0u) {
        ++count;
      }
    }
    return count;
  }

  // Alternative implementation using explicit trial division for primality.
  // Time: O((right - left + 1) * sqrt(log right)), Space: O(1).
  int countPrimeSetBitsTrialDivision(int left, int right) {
    int count = 0;
    for (int num = left; num <= right; ++num) {
      const int bits = std::popcount(static_cast<std::uint32_t>(num));
      if (isPrime(bits)) {
        ++count;
      }
    }
    return count;
  }

 private:
  // Returns true iff n is prime via odd trial division up to sqrt(n).
  static bool isPrime(int n) {
    if (n == 2) {
      return true;
    }
    if (n <= 1 || (n % 2) == 0) {
      return false;
    }
    // Check odd divisors using division bound to avoid multiplication overflow.
    for (int divisor = 3; divisor <= n / divisor; divisor += 2) {
      if ((n % divisor) == 0) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(762, "Prime Number Of Set Bits In Binary Representation",
           "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_six_to_ten", [&] {
    // 6(2), 7(3), 8(1 - not prime), 9(2), 10(2) -> 4
    ctx.expect_eq(sol.countPrimeSetBits(6, 10), 4);
    ctx.expect_eq(sol.countPrimeSetBitsTrialDivision(6, 10), 4);
  });

  ctx.test("example_2_ten_to_fifteen", [&] {
    // 10(2), 11(3), 12(2), 13(3), 14(3), 15(4 - not prime) -> 5
    ctx.expect_eq(sol.countPrimeSetBits(10, 15), 5);
    ctx.expect_eq(sol.countPrimeSetBitsTrialDivision(10, 15), 5);
  });

  ctx.test("single_element_ranges", [&] {
    // 1 = 1_2 (1 set bit, not prime)
    ctx.expect_eq(sol.countPrimeSetBits(1, 1), 0);
    ctx.expect_eq(sol.countPrimeSetBitsTrialDivision(1, 1), 0);
    // 3 = 11_2 (2 set bits, prime)
    ctx.expect_eq(sol.countPrimeSetBits(3, 3), 1);
    ctx.expect_eq(sol.countPrimeSetBitsTrialDivision(3, 3), 1);
    // 21 = 10101_2 (3 set bits, prime)
    ctx.expect_eq(sol.countPrimeSetBits(21, 21), 1);
    ctx.expect_eq(sol.countPrimeSetBitsTrialDivision(21, 21), 1);
  });

  ctx.test("larger_values_near_constraint_limit", [&] {
    // 2^19 - 1 = 524287 has 19 set bits (prime); 2^18 - 1 = 262143 has 18 (not).
    ctx.expect_eq(sol.countPrimeSetBits(524287, 524287), 1);
    ctx.expect_eq(sol.countPrimeSetBits(262143, 262143), 0);

    // [999995..1000000]: popcounts are 11, 10, 11, 11, 12, 7 -> 4 primes.
    ctx.expect_eq(sol.countPrimeSetBits(999995, 1000000), 4);
    ctx.expect_eq(sol.countPrimeSetBitsTrialDivision(999995, 1000000), 4);

    // Cross-check both implementations across a full 10^4-wide range.
    const int fast = sol.countPrimeSetBits(990000, 1000000);
    const int trial = sol.countPrimeSetBitsTrialDivision(990000, 1000000);
    ctx.expect_eq(fast, trial);
    ctx.expect_true(fast > 0);
  });
}

}  // namespace
