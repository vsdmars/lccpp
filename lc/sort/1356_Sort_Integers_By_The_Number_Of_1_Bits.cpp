// LeetCode 1356: Sort Integers By The Number Of 1 Bits
// https://leetcode.com/problems/sort-integers-by-the-number-of-1-bits/
//
// Problem:
//   Given an integer array `arr`, sort its elements in ascending order by the
//   number of 1-bits (Hamming weight / popcount) in their binary
//   representation. If two or more integers have the same number of 1-bits,
//   sort them in ascending numerical order.
//   Constraints: 1 <= arr.length <= 500, 0 <= arr[i] <= 10^4.
//
// Algorithm:
//   Sort `arr` in place using `std::ranges::sort` with a two-level comparator:
//   first compare `std::popcount(static_cast<unsigned int>(x))`, and break ties
//   by the integer value itself (`a < b`). Because `std::popcount` compiles to
//   a single hardware instruction (`POPCNT` / `CNT`) on modern CPUs, each
//   comparison takes O(1) time.
//   Time Complexity:  O(n log n) comparisons, each O(1).
//   Space Complexity: O(log n) auxiliary stack space for introsort.
//
// Data Structures:
//   - `std::vector<int>` sorted in place with a stateless custom comparator.
//
// Real-World Applications:
//   - Ordering bitmask states by subset cardinality in bitmask dynamic
//     programming (e.g., Held-Karp TSP, subset convolution, SOS DP).
//   - Prioritizing sparse feature vectors or Bloom-filter signatures by
//     Hamming weight in information retrieval and approximate nearest-neighbor
//     search.
//   - Scheduling hardware test patterns grouped by switching activity
//     (number of asserted signal lines) to control peak power draw.
//
// Related LeetCode Problems:
//   - #191 Number Of 1 Bits [done] (computes the Hamming weight used as the primary sort key)
//   - #338 Counting Bits [done] (computes popcount for all integers in [0, n] in O(n) via DP)
//   - #1636 Sort Array By Increasing Frequency [done] (multi-key custom comparator sorting)
//   - #461 Hamming Distance [done] (popcount of XOR between two integers)

#include "framework/lc_framework.h"

#include <algorithm>
#include <bit>
#include <vector>

namespace {

class Solution {
 public:
  // Sorts `arr` in ascending order of set-bit count, breaking ties by integer
  // value. Operates in-place on the by-value parameter.
  // Time: O(n log n), Space: O(log n) for sort recursion stack.
  std::vector<int> sortByBits(std::vector<int> arr) {
    // Compare by popcount first (casting non-negative values to unsigned int
    // as required by std::popcount), then fall back to natural numeric order.
    std::ranges::sort(arr, [](int lhs, int rhs) {
      const int lhs_bits = std::popcount(static_cast<unsigned int>(lhs));
      const int rhs_bits = std::popcount(static_cast<unsigned int>(rhs));
      if (lhs_bits != rhs_bits) {
        return lhs_bits < rhs_bits;
      }
      return lhs < rhs;
    });
    return arr;
  }
};

LC_PROBLEM(1356, "Sort Integers By The Number Of 1 Bits", "sort", ctx) {
  Solution sol;

  ctx.test("example1_zero_through_eight", [&] {
    const std::vector<int> input = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    const std::vector<int> expected = {0, 1, 2, 4, 8, 3, 5, 6, 7};
    ctx.expect_eq(sol.sortByBits(input), expected);
  });

  ctx.test("example2_powers_of_two_all_one_bit", [&] {
    const std::vector<int> input = {1024, 512, 256, 128, 64, 32,
                                    16,   8,   4,   2,   1};
    const std::vector<int> expected = {1,  2,  4,   8,   16,  32,
                                       64, 128, 256, 512, 1024};
    ctx.expect_eq(sol.sortByBits(input), expected);
  });

  ctx.test("duplicates_and_zeros", [&] {
    const std::vector<int> input = {3, 0, 2, 3, 1, 0};
    const std::vector<int> expected = {0, 0, 1, 2, 3, 3};
    ctx.expect_eq(sol.sortByBits(input), expected);
  });

  ctx.test("single_and_empty", [&] {
    ctx.expect_eq(sol.sortByBits({42}), std::vector<int>{42});
    ctx.expect_eq(sol.sortByBits({}), std::vector<int>{});
  });

  ctx.test("upper_bound_values", [&] {
    // 0 -> 0 bits, 8192 (2^13) -> 1 bit, 10000 -> 5 bits, 9999 -> 8 bits.
    const std::vector<int> input = {10000, 9999, 0, 8192};
    const std::vector<int> expected = {0, 8192, 10000, 9999};
    ctx.expect_eq(sol.sortByBits(input), expected);
  });
}

}  // namespace
