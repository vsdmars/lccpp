// LeetCode 995: Minimum Number Of K Consecutive Bit Flips
// https://leetcode.com/problems/minimum-number-of-k-consecutive-bit-flips/
//
// Problem:
//   Given a binary array nums (elements are 0 or 1, 1 <= nums.length <= 10^5)
//   and an integer k (1 <= k <= nums.length), a k-bit flip chooses any
//   contiguous subarray of length k and toggles every 0 to 1 and every 1 to 0.
//   Return the minimum number of k-bit flips required so that every element in
//   nums becomes 1, or -1 if it is impossible.
//
// Examples:
//   Example 1:
//     Input:  nums = [0, 1, 0], k = 1
//     Output: 2
//     Explanation:
//       - At i = 0, active_flip is 0 and nums[0] == active_flip (0 == 0), so a flip
//         starts at index 0: active_flip becomes 1, is_flipped[0] = 1, and
//         total_flips = 1.
//       - At i = 1 (i >= 1), expiring is_flipped[0] updates active_flip ^= 1 to 0;
//         nums[1] = 1 != 0, so no flip is needed.
//       - At i = 2, expiring is_flipped[1] (0) keeps active_flip = 0, and nums[2]
//         == active_flip triggers a flip at index 2 (2 + 1 <= 3), incrementing
//         total_flips to 2.
//
//   Example 2:
//     Input:  nums = [1, 1, 0], k = 2
//     Output: -1
//     Explanation:
//       - At i = 0 and i = 1, active_flip remains 0 and both nums[0] and nums[1]
//         equal 1 != active_flip, so neither index initiates a flip.
//       - At i = 2 (i >= 2), expiring is_flipped[0] (0) keeps active_flip = 0, and
//         nums[2] == active_flip (0 == 0) requires a flip, but i + window = 4 > 3
//         so the function returns -1.
//
//   Example 3:
//     Input:  nums = [0, 0, 0, 1, 0, 1, 1, 0], k = 3
//     Output: 3
//     Explanation:
//       - At i = 0, nums[0] == active_flip (0 == 0) starts a flip (is_flipped[0] =
//         1, active_flip = 1, total_flips = 1), which makes indices 1 and 2
//         effectively 1 so no flips start there.
//       - At i = 3, expiring is_flipped[0] flips active_flip back to 0, leaving
//         nums[3] = 1 effectively 1; at i = 4, expiring is_flipped[1] (0) keeps
//         active_flip = 0, so nums[4] == 0 starts a second flip (is_flipped[4] = 1,
//         active_flip = 1, total_flips = 2).
//       - At i = 5, expiring is_flipped[2] (0) leaves active_flip = 1 and nums[5]
//         == active_flip (1 == 1) forces a third flip (is_flipped[5] = 1,
//         active_flip = 0, total_flips = 3).
//       - At i = 6, active_flip stays 0 (nums[6] = 1 is satisfied), and at i = 7
//         expiring is_flipped[4] sets active_flip = 1 so nums[7] = 0 is satisfied,
//         returning total_flips = 3.
//
// Algorithm:
//   Greedy left-to-right sweep with sliding XOR parity tracking:
//   - Key observation: Flip operations commute (order does not matter), and
//     flipping the same starting index twice cancels out. Therefore, when
//     visiting the leftmost position i whose effective value is 0, the ONLY
//     remaining operation that can fix index i without disturbing already-fixed
//     indices [0, i - 1] is to flip the window [i, i + k - 1].
//   - Rather than flipping k elements explicitly in O(k), maintain a running
//     parity bit `active_flip` equal to (number of active flips covering i) % 2
//     and an array `is_flipped` recording whether a flip started at each index.
//   - At index i:
//       1. If i >= k, the flip starting at i - k leaves the active window, so
//          update `active_flip ^= is_flipped[i - k]`.
//       2. The effective bit at i is `nums[i] ^ active_flip`. It is 0 iff
//          `nums[i] == active_flip`.
//       3. If a flip is required and `i + k > n`, no valid length-k window
//          fits, so return -1. Otherwise toggle `active_flip ^= 1`, mark
//          `is_flipped[i] = 1`, and increment `flip_count`.
//   Time Complexity: O(n).
//   Space Complexity: O(n) for `is_flipped` (can be reduced to O(1) by
//   reusing `nums` in-place with a sentinel offset of +2).
//
// Data Structures:
//   - std::vector<std::uint8_t> `is_flipped`: difference-like circular record
//     storing whether a length-k flip was initiated at each index.
//   - Scalar parity flag `active_flip`: XOR accumulator of flips currently
//     affecting the swept index.
//
// Real-World Applications:
//   - Prefix/difference-array range-toggle optimization in database bitmaps
//     and graphics scanline fill algorithms.
//   - Lights-Out / linear binary control over GF(2) for fault-tolerant relay
//     arrays and hardware self-calibration sequences.
//   - Sliding-window event expiration (delta coding) in network rate limiters
//     and time-series stream processors.
//
// Related LeetCode Problems:
//   - #319 Bulb Switcher [done] (parity of toggle operations over positions)
//   - #672 Bulb Switcher II [done] (group of simultaneous toggle operations)
//   - #1529 Minimum Suffix Flips [new] (simpler suffix-flip greedy parity)
//   - #3191 Minimum Operations to Make Binary Array Elements Equal to One I [new] (fixed k=3 version)
//   - #1094 Car Pooling [done] (difference array for fixed-span range updates)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum number of length-k subarray bit flips needed to make
  // all elements of nums equal to 1, or returns -1 if impossible.
  // Time: O(n), Space: O(n).
  int minKBitFlips(const std::vector<int>& nums, int k) {
    const std::size_t n = nums.size();
    if (k <= 0 || std::cmp_greater(k, n)) {
      return -1;
    }

    const auto window = static_cast<std::size_t>(k);
    // is_flipped[i] == 1 iff we initiated a k-bit flip starting at index i.
    std::vector<std::uint8_t> is_flipped(n, 0);
    // Running XOR parity of all active flips covering the current index i.
    int active_flip = 0;
    int total_flips = 0;

    for (std::size_t i = 0uz; i < n; ++i) {
      // Remove the contribution of the flip that ended at index i - 1.
      if (i >= window) {
        active_flip ^= static_cast<int>(is_flipped[i - window]);
      }

      // Effective value is (nums[i] ^ active_flip). It is 0 iff
      // nums[i] == active_flip, forcing a greedy flip starting at i.
      if (nums[i] == active_flip) {
        if ((i + window) > n) {
          // Not enough remaining elements to perform a full length-k flip.
          return -1;
        }
        active_flip ^= 1;
        is_flipped[i] = 1;
        ++total_flips;
      }
    }

    return total_flips;
  }
};

LC_PROBLEM(995, "Minimum Number Of K Consecutive Bit Flips", "bit_manipulation",
           ctx) {
  Solution sol;

  ctx.test("example_1_k_equals_1", [&] {
    ctx.expect_eq(sol.minKBitFlips({0, 1, 0}, 1), 2);
  });

  ctx.test("example_2_impossible", [&] {
    ctx.expect_eq(sol.minKBitFlips({1, 1, 0}, 2), -1);
  });

  ctx.test("example_3_overlapping_flips", [&] {
    ctx.expect_eq(sol.minKBitFlips({0, 0, 0, 1, 0, 1, 1, 0}, 3), 3);
  });

  ctx.test("python_build_cases", [&] {
    ctx.expect_eq(sol.minKBitFlips({0, 1, 1, 1}, 3), -1);
    ctx.expect_eq(sol.minKBitFlips({0, 1, 1}, 3), -1);
    ctx.expect_eq(sol.minKBitFlips({0, 1, 1, 0}, 3), 2);
  });

  ctx.test("edge_cases_already_ones_and_full_array_flip", [&] {
    ctx.expect_eq(sol.minKBitFlips({1, 1, 1, 1}, 2), 0);
    ctx.expect_eq(sol.minKBitFlips({0, 0, 0, 0}, 4), 1);
    ctx.expect_eq(sol.minKBitFlips({0}, 1), 1);
    ctx.expect_eq(sol.minKBitFlips({1}, 1), 0);
  });
}

}  // namespace
