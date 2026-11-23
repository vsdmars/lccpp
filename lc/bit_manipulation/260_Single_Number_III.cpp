// LeetCode 260: Single Number III
// https://leetcode.com/problems/single-number-iii/
//
// Problem:
//   Given an integer array `nums` in which exactly two distinct elements appear
//   only once and all other elements appear exactly twice, find and return the
//   two elements that appear only once (in any order). The algorithm must run
//   in O(n) linear time and O(1) constant auxiliary space.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 1, 3, 2, 5]
//     Output: [3, 5]
//     Explanation:
//       - Pass 1 XORs all elements: total_xor = 1 ^ 2 ^ 1 ^ 3 ^ 2 ^ 5 = 3 ^ 5 = 6
//         (0b110).
//       - Isolate the lowest differing bit: diff_bit = 6 & (~6 + 1U) = 2 (0b010).
//       - Pass 2 XORs only elements with bit 1 set ({2, 3, 2}), cancelling the pair
//         of 2s to yield first = 3.
//       - Compute second = first ^ total_xor = 3 ^ 6 = 5, returning [3, 5].
//
//   Example 2:
//     Input:  nums = [-1, 0]
//     Output: [-1, 0]
//     Explanation:
//       - Pass 1 computes total_xor = 0xFFFFFFFFU ^ 0U = 0xFFFFFFFFU.
//       - Isolate the lowest set bit: diff_bit = 0xFFFFFFFFU & (0U + 1U) = 1U.
//       - Pass 2 selects only elements with bit 0 set (-1), yielding first = -1
//         (0xFFFFFFFFU), and computes second = 0xFFFFFFFFU ^ 0xFFFFFFFFU = 0,
//         returning [-1, 0].
//
//   Example 3:
//     Input:  nums = [-2147483648, 0, 7, 7]
//     Output: [-2147483648, 0]
//     Explanation:
//       - Pass 1 XORs all values in uint32_t: 0x80000000U ^ 0U ^ 7U ^ 7U =
//         0x80000000U.
//       - Compute diff_bit = 0x80000000U & (~0x80000000U + 1U) = 0x80000000U in
//         unsigned arithmetic, avoiding signed negation overflow on INT_MIN.
//       - Pass 2 XORs only elements with bit 31 set (-2147483648), giving first =
//         -2147483648, and sets second = 0x80000000U ^ 0x80000000U = 0, returning
//         [-2147483648, 0].
//
// Algorithm:
//   XOR Partitioning via Lowest Differing Bit:
//   1. XOR all elements of `nums` into `total_xor`. Every duplicated pair
//      cancels out, leaving `total_xor = a ^ b`, where `a` and `b` are the two
//      unique elements.
//   2. Because `a != b`, `total_xor` is nonzero and has at least one set bit.
//      Isolate its lowest set bit: `diff_bit = total_xor & (~total_xor + 1U)`.
//      Using `std::uint32_t` guarantees well-defined modulo-2^32 two's-
//      complement arithmetic even when `total_xor == 0x80000000U` (e.g., when
//      the two singles are `INT_MIN` and `0`, where signed `-total_xor` would
//      overflow).
//   3. At `diff_bit`, one of `{a, b}` has bit 1 and the other has bit 0, while
//      every duplicated pair falls into the same partition. XORing only the
//      elements with `(num & diff_bit) != 0` cancels all pairs in that bucket
//      and yields `first`. Finally, `second = first ^ total_xor`.
//   Time: O(n) (two linear passes), Space: O(1).
//
// Data Structures:
//   - Constant number of 32-bit unsigned integer registers (`total_xor`,
//     `diff_bit`, `first`, `second`).
//
// Real-World Applications:
//   - Distributed storage / RAID-6 dual-parity recovery intuition, separating
//     two corrupted or missing items using independent bit-parity projections.
//   - Memory-constrained embedded diagnostics and streaming anomaly detection
//     where two unpaired events must be isolated without hash tables.
//   - Binary-indexed tree (Fenwick tree) lowbit isolation (`x & -x`), sharing
//     the same lowest-set-bit primitive.
//
// Related LeetCode Problems:
//   - #136 Single Number [done] (single unique element among pairs via one XOR pass)
//   - #137 Single Number II [done] (single unique element among triples via mod-3 bit counters)
//   - #268 Missing Number [done] (XOR cancellation over [0..n])
//   - #645 Set Mismatch [new] (uses the exact same XOR-and-lowbit partition to recover both a duplicate and a missing number in O(1) space)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the two elements that appear exactly once in `nums`.
  // Invariant: `total_xor == a ^ b != 0`, so `diff_bit` strictly separates `a`
  // and `b` into opposite halves while keeping every duplicate pair together.
  // Time: O(n), Space: O(1).
  std::vector<int> singleNumber(const std::vector<int>& nums) {
    // Pass 1: XOR all values; duplicates cancel, leaving `a ^ b`.
    std::uint32_t total_xor = 0U;
    for (int num : nums) {
      total_xor ^= static_cast<std::uint32_t>(num);
    }

    // Isolate the least significant 1-bit of `total_xor`, which marks a bit
    // position where `a` and `b` necessarily differ.
    // Computing `~total_xor + 1U` in uint32_t avoids signed overflow when
    // `total_xor == 0x80000000U`.
    const std::uint32_t diff_bit = total_xor & (~total_xor + 1U);

    // Pass 2: XOR only elements having `diff_bit` set to isolate one unique
    // number (`first`); every duplicate pair in this subset cancels out.
    std::uint32_t first = 0U;
    for (int num : nums) {
      const auto bits = static_cast<std::uint32_t>(num);
      if ((bits & diff_bit) != 0U) {
        first ^= bits;
      }
    }

    // Recover the second unique number from `first ^ (first ^ second)`.
    const std::uint32_t second = first ^ total_xor;
    return {static_cast<int>(first), static_cast<int>(second)};
  }
};

LC_PROBLEM(260, "Single Number III", "bit_manipulation", ctx) {
  Solution sol;

  auto sorted_result = [&](const std::vector<int>& nums) {
    std::vector<int> ans = sol.singleNumber(nums);
    std::ranges::sort(ans);
    return ans;
  };

  ctx.test("example_1_basic", [&] {
    ctx.expect_eq(sorted_result({1, 2, 1, 3, 2, 5}), std::vector<int>({3, 5}));
  });

  ctx.test("example_2_two_elements_negative_and_zero", [&] {
    ctx.expect_eq(sorted_result({-1, 0}), std::vector<int>({-1, 0}));
  });

  ctx.test("example_3_zero_and_one", [&] {
    ctx.expect_eq(sorted_result({0, 1}), std::vector<int>({0, 1}));
  });

  ctx.test("all_negatives_with_pairs", [&] {
    ctx.expect_eq(sorted_result({-5, -5, -10, -20, 4, 4}),
                  std::vector<int>({-20, -10}));
  });

  ctx.test("int_min_and_zero_no_overflow", [&] {
    // Regression test: total_xor == 0x80000000U would overflow signed negation.
    constexpr int kMin = std::numeric_limits<int>::min();
    ctx.expect_eq(sorted_result({kMin, 0, 7, 7}), std::vector<int>({kMin, 0}));
  });
}

}  // namespace
