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
