// LeetCode 338: Counting Bits
// https://leetcode.com/problems/counting-bits/
//
// Problem:
//   Given a non-negative integer `n`, return an array `ans` of length `n + 1`
//   such that for every `i` in `0 <= i <= n`, `ans[i]` equals the number of
//   `'1'` bits (Hamming weight) in the binary representation of `i`.
//   Follow-up: solve in O(n) linear time in a single pass without built-in
//   popcount intrinsics.
//
// Examples:
//   Example 1:
//     Input:  n = 2
//     Output: [0, 1, 1]
//     Explanation:
//       - Initialize `counts = [0, 0, 0]` of size `n + 1 = 3` with base case
//         `counts[0] = 0`.
//       - For `i = 1` (binary `1`), compute `counts[1] = counts[1 >> 1] + (1 & 1) =
//         counts[0] + 1 = 1`.
//       - For `i = 2` (binary `10`), compute `counts[2] = counts[2 >> 1] + (2 & 1)
//         = counts[1] + 0 = 1`, returning `[0, 1, 1]`.
//
//   Example 2:
//     Input:  n = 5
//     Output: [0, 1, 1, 2, 1, 2]
//     Explanation:
//       - Initialize `counts` of length `6` with `counts[0] = 0`, then compute
//         `counts[1] = counts[0] + 1 = 1` and `counts[2] = counts[1] + 0 = 1`.
//       - For `i = 3` (binary `11`), compute `counts[3] = counts[3 >> 1] + (3 & 1)
//         = counts[1] + 1 = 2`.
//       - For `i = 4` (binary `100`), `counts[4] = counts[2] + 0 = 1`, and for `i =
//         5` (binary `101`), `counts[5] = counts[2] + 1 = 2`.
//       - Return `counts = [0, 1, 1, 2, 1, 2]`.
//
//   Example 3:
//     Input:  n = 0
//     Output: [0]
//     Explanation:
//       - Allocate `counts` of length `0 + 1 = 1` initialized to `[0]`.
//       - Since `limit = 0`, the loop `for (i = 1; i <= 0; ++i)` performs zero
//         iterations and returns `[0]`.
//
// Algorithm:
//   Dynamic Programming on Least Significant Bit (`i >> 1`):
//   Right-shifting any positive integer `i` by 1 drops its least significant
//   bit (`i & 1`) and yields a strictly smaller already-computed index
//   `i >> 1`. Therefore:
//     counts[0] = 0
//     counts[i] = counts[i >> 1] + (i & 1)   for 1 <= i <= n.
//   (Notice this is equivalent to the parity branching in the reference note:
//   even `i` has `counts[i] = counts[i >> 1]`, and odd `i` has
//   `counts[i] = counts[i - 1] + 1 = counts[i >> 1] + 1`.)
//   A second concise DP recurrence using Brian Kernighan's bit-clearing identity
//   `counts[i] = counts[i & (i - 1)] + 1` is also provided.
//   Time: O(n), Space: O(n) for the output vector (O(1) extra scratch space).
//
// Data Structures:
//   - Flat contiguous DP table `std::vector<int>` of length `n + 1`, where
//     `counts[i]` stores the popcount of `i` and doubles as the return buffer.
//
// Real-World Applications:
//   - Precomputing byte-wide or 16-bit popcount lookup tables (LUTs) used by
//     software fallback routines on architectures without hardware `POPCNT`.
//   - Bitmask dynamic programming (subset DP / Traveling Salesman / assignment
//     problems) that groups or iterates subsets by cardinality.
//   - Fast rank queries over bitmaps in succinct trees, columnar analytics
//     engines, and genomic FM-indexes.
//
// Related LeetCode Problems:
//   - #191 Number Of 1 Bits [done] (computes the Hamming weight of a single 32-bit integer)
//   - #231 Power Of Two [done] (identifies numbers whose popcount is exactly 1)
//   - #190 Reverse Bits [done] (bit-level manipulation of binary representations)
//   - #461 Hamming Distance [done] (counts differing bits between two integers via `popcount(x ^ y)`)
//   - #1356 Sort Integers by The Number of 1 Bits [done] (sorts integers using their popcount as primary key)

#include "framework/lc_framework.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Computes popcount(i) for every i in [0, n] in a single O(n) DP pass.
  // Recurrence: `counts[i] = counts[i >> 1] + (i & 1)`.
  // Invariant: when computing `counts[i]`, `counts[0..i-1]` already hold exact
  // bit counts, and `i >> 1 < i` for every `i >= 1`.
  // Time: O(n), Space: O(n) (output array only).
  std::vector<int> countBits(int n) {
    if (n < 0) {
      return {};
    }

    const auto limit = static_cast<std::size_t>(n);
    std::vector<int> counts(limit + 1uz, 0);

    // Base case: counts[0] == 0.
    for (std::size_t i = 1uz; i <= limit; ++i) {
      // Drop the LSB (`i >> 1`) to reuse a previously solved subproblem, then
      // add 1 if the dropped LSB was set (`i & 1`).
      counts[i] = counts[i >> 1U] + static_cast<int>(i & 1uz);
    }
    return counts;
  }

  // Secondary O(n) DP using Brian Kernighan's last-set-bit recurrence:
  // `counts[i] = counts[i & (i - 1)] + 1`, since clearing the lowest set bit
  // reduces popcount by exactly 1 and produces an index strictly less than `i`.
  // Time: O(n), Space: O(n) (output array only).
  std::vector<int> countBitsKernighan(int n) {
    if (n < 0) {
      return {};
    }

    const auto limit = static_cast<std::size_t>(n);
    std::vector<int> counts(limit + 1uz, 0);

    for (std::size_t i = 1uz; i <= limit; ++i) {
      counts[i] = counts[i & (i - 1uz)] + 1;
    }
    return counts;
  }
};

LC_PROBLEM(338, "Counting Bits", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_n_equals_2", [&] {
    const std::vector<int> expected = {0, 1, 1};
    ctx.expect_eq(sol.countBits(2), expected);
    ctx.expect_eq(sol.countBitsKernighan(2), expected);
  });

  ctx.test("example_2_n_equals_5", [&] {
    const std::vector<int> expected = {0, 1, 1, 2, 1, 2};
    ctx.expect_eq(sol.countBits(5), expected);
    ctx.expect_eq(sol.countBitsKernighan(5), expected);
  });

  ctx.test("build_sample_n_equals_11", [&] {
    const std::vector<int> expected = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3};
    ctx.expect_eq(sol.countBits(11), expected);
    ctx.expect_eq(sol.countBitsKernighan(11), expected);
  });

  ctx.test("edge_case_n_equals_0", [&] {
    const std::vector<int> expected = {0};
    ctx.expect_eq(sol.countBits(0), expected);
    ctx.expect_eq(sol.countBitsKernighan(0), expected);
  });

  ctx.test("matches_std_popcount_up_to_64", [&] {
    constexpr int kLimit = 64;
    const std::vector<int> shift_dp = sol.countBits(kLimit);
    const std::vector<int> kernighan_dp = sol.countBitsKernighan(kLimit);

    ctx.expect_eq(shift_dp.size(), static_cast<std::size_t>(kLimit + 1));
    ctx.expect_eq(kernighan_dp.size(), static_cast<std::size_t>(kLimit + 1));
    for (int i = 0; i <= kLimit; ++i) {
      const int expected = std::popcount(static_cast<std::uint32_t>(i));
      const auto idx = static_cast<std::size_t>(i);
      ctx.expect_eq(shift_dp[idx], expected);
      ctx.expect_eq(kernighan_dp[idx], expected);
    }
  });
}

}  // namespace
