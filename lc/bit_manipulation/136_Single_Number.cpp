// LeetCode 136: Single Number
// https://leetcode.com/problems/single-number/
//
// Problem:
//   Given a non-empty array of integers `nums` in which every element appears
//   exactly twice except for one element that appears only once, find and
//   return that single unique element. The solution must run in O(n) linear
//   time and use only O(1) constant auxiliary space.
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 2, 1]
//     Output: 1
//     Explanation:
//       - Initialize uint32_t accumulator acc = 0.
//       - XOR with the first 2 gives acc = 0 ^ 2 = 2, and XOR with the second 2
//         cancels it to acc = 2 ^ 2 = 0.
//       - XOR with 1 produces acc = 0 ^ 1 = 1, which is returned as 1.
//
//   Example 2:
//     Input:  nums = [4, 1, 2, 1, 2]
//     Output: 4
//     Explanation:
//       - Initialize acc = 0, then fold 4 and 1 to get acc = 0 ^ 4 ^ 1 = 5.
//       - Fold 2 to update acc = 5 ^ 2 = 7.
//       - Fold the duplicate 1 (7 ^ 1 = 6) and duplicate 2 (6 ^ 2 = 4) to cancel
//         both pairs and return 4.
//
//   Example 3:
//     Input:  nums = [-3, 7, -3, 0, 7]
//     Output: 0
//     Explanation:
//       - Cast -3 to uint32_t (0xFFFFFFFD) and XOR into acc = 0, giving acc =
//         0xFFFFFFFD.
//       - XOR with 7U yields acc = 0xFFFFFFFA, and XOR with the second -3U cancels
//         -3 to leave acc = 7U.
//       - XOR with 0U preserves acc = 7U, and XOR with the second 7U cancels 7 to
//         0U, returning 0.
//
// Algorithm:
//   XOR Accumulation: Bitwise XOR is associative, commutative, self-inverse
//   (`x ^ x == 0`), and has 0 as its identity element (`x ^ 0 == x`). Folding
//   XOR across all elements cancels every duplicated pair to 0, leaving only
//   the unique value.
//   An alternative set-based arithmetic method (`2 * sum(unique) - sum(nums)`)
//   is also included for comparison (matching the Python `rewrite` method),
//   using 64-bit accumulators to avoid signed overflow.
//   - XOR approach: Time O(n), Space O(1).
//   - Hash-set sum approach: Time O(n), Space O(n).
//
// Data Structures:
//   - Single 32-bit unsigned integer accumulator for the O(1)-space XOR fold.
//   - `std::unordered_set<int>` for the secondary unique-sum approach to
//     deduplicate elements in O(1) expected time per insertion.
//
// Real-World Applications:
//   - RAID-5 / parity-block error recovery, where XORing surviving stripes
//     reconstructs a single missing data block in constant extra memory.
//   - Stream integrity checks and lightweight checksums for detecting an
//     unmatched packet or unpaired open/close transaction ID.
//   - Swapping or toggling symmetric bitmasks in embedded firmware without
//     allocating scratch buffers.
//
// Related LeetCode Problems:
//   - #137 Single Number II [done] (every element appears thrice except one; uses mod-3 bit state machine)
//   - #260 Single Number III [done] (two unique elements; partitions via lowest differing bit of XOR)
//   - #268 Missing Number [done] (uses XOR cancellation between indices and values to find the missing integer)
//   - #389 Find the Difference [done] (finds the single extra character in a shuffled string via XOR)
//   - #287 Find the Duplicate Number [done] (dual problem of finding one repeated element in O(1) space)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the unique integer when all other integers appear exactly twice.
  // Invariant: after processing prefix `nums[0..i]`, `acc` equals the bitwise
  // XOR of all elements seen an odd number of times so far.
  // Time: O(n), Space: O(1).
  int singleNumber(const std::vector<int>& nums) {
    // Accumulate in uint32_t so bitwise operations have uniform two's-complement
    // semantics across all 32 bits, including INT_MIN.
    std::uint32_t acc = 0U;
    for (int num : nums) {
      // Paired values cancel (`x ^ x == 0`); only the unpaired value survives.
      acc ^= static_cast<std::uint32_t>(num);
    }
    return static_cast<int>(acc);
  }

  // Secondary approach (ported from Python `rewrite`):
  // Computes `2 * sum(unique(nums)) - sum(nums)` using 64-bit sums to prevent
  // intermediate signed 32-bit overflow.
  // Time: O(n) expected, Space: O(n).
  int singleNumberSetSum(const std::vector<int>& nums) {
    std::unordered_set<int> seen;
    seen.reserve(nums.size());

    std::int64_t unique_sum = 0;
    std::int64_t total_sum = 0;
    for (int num : nums) {
      total_sum += static_cast<std::int64_t>(num);
      // Only add `num` to `unique_sum` the first time it is inserted.
      if (seen.insert(num).second) {
        unique_sum += static_cast<std::int64_t>(num);
      }
    }
    return static_cast<int>((2 * unique_sum) - total_sum);
  }
};

LC_PROBLEM(136, "Single Number", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_small_array", [&] {
    const std::vector<int> nums = {2, 2, 1};
    ctx.expect_eq(sol.singleNumber(nums), 1);
    ctx.expect_eq(sol.singleNumberSetSum(nums), 1);
  });

  ctx.test("example_2_five_elements", [&] {
    const std::vector<int> nums = {4, 1, 2, 1, 2};
    ctx.expect_eq(sol.singleNumber(nums), 4);
    ctx.expect_eq(sol.singleNumberSetSum(nums), 4);
  });

  ctx.test("single_element", [&] {
    const std::vector<int> nums = {99};
    ctx.expect_eq(sol.singleNumber(nums), 99);
    ctx.expect_eq(sol.singleNumberSetSum(nums), 99);
  });

  ctx.test("negatives_and_zero", [&] {
    const std::vector<int> nums = {-3, 7, -3, 0, 7};
    ctx.expect_eq(sol.singleNumber(nums), 0);
    ctx.expect_eq(sol.singleNumberSetSum(nums), 0);

    const std::vector<int> neg_unique = {5, -12, 5, 8, 8};
    ctx.expect_eq(sol.singleNumber(neg_unique), -12);
    ctx.expect_eq(sol.singleNumberSetSum(neg_unique), -12);
  });

  ctx.test("int32_extremes", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    const std::vector<int> nums = {kMax, kMin, kMax};
    ctx.expect_eq(sol.singleNumber(nums), kMin);
    ctx.expect_eq(sol.singleNumberSetSum(nums), kMin);
  });
}

}  // namespace
