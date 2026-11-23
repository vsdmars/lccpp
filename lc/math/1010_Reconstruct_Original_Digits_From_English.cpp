// LeetCode 1010: Pairs of Songs With Total Durations Divisible by 60
// https://leetcode.com/problems/pairs-of-songs-with-total-durations-divisible-by-60/
// (Note: registered under the legacy title from the source Python docstring.)
//
// Problem:
//   Given an array `time` where `time[i]` is the duration of the i-th song in
//   seconds, return the number of index pairs (i, j) with i < j such that the
//   combined duration `(time[i] + time[j])` is divisible by 60.
//   Constraints: 1 <= time.length <= 6 * 10^4, 1 <= time[i] <= 500.
//
// Examples:
//   Example 1:
//     Input:  time = [30, 20, 150, 100, 40]
//     Output: 3
//     Explanation:
//       - Initialize remainder_counts (size 60) to zeros and pair_count = 0.
//       - For 30 (rem 30, complement 30) and 20 (rem 20, complement 40), no
//         matching remainders exist yet, so pair_count stays 0 and counts[30] = 1,
//         counts[20] = 1.
//       - For 150 (rem 30, complement 30), add counts[30] = 1 so pair_count = 1 and
//         increment counts[30] to 2.
//       - For 100 (rem 40, complement 20) and 40 (rem 40, complement 20), each adds
//         counts[20] = 1, bringing pair_count to 3 before incrementing counts[40]
//         to 2.
//       - Return pair_count = 3.
//
//   Example 2:
//     Input:  time = [60, 60, 60]
//     Output: 3
//     Explanation:
//       - Initialize remainder_counts to zeros and pair_count = 0.
//       - First 60: rem = 0, complement = (60 - 0) % 60 = 0; add counts[0] = 0 to
//         pair_count and set counts[0] = 1.
//       - Second 60: rem = 0, complement = 0; add counts[0] = 1 (pair_count = 1)
//         and increment counts[0] to 2.
//       - Third 60: rem = 0, complement = 0; add counts[0] = 2 (pair_count = 3) and
//         increment counts[0] to 3, returning 3.
//
//   Example 3:
//     Input:  time = [60]
//     Output: 0
//     Explanation:
//       - Initialize remainder_counts to zeros and pair_count = 0.
//       - Process 60: rem = 0, complement = 0; add counts[0] = 0 to pair_count and
//         increment counts[0] to 1.
//       - Finish the single-element loop and return pair_count = 0.
//
// Algorithm:
//   Single-pass modular complement counting (a Two-Sum variant modulo 60).
//   A pair (time[i], time[j]) sums to a multiple of 60 iff:
//       (time[i] % 60 + time[j] % 60) % 60 == 0
//   Therefore, for each song with remainder `rem = time[j] % 60` in [0, 59],
//   its required partner remainder is `complement = (60 - rem) % 60` (note
//   that when `rem == 0`, `complement` wraps cleanly to `0` rather than `60`).
//   Maintaining a fixed 60-slot frequency array `counts` of previously seen
//   remainders, we add `counts[complement]` to the answer before incrementing
//   `counts[rem]`. This guarantees each unordered pair (i < j) is counted
//   exactly once.
//   Time Complexity:  O(N) where N = time.size().
//   Space Complexity: O(1) — fixed std::array<int, 60>.
//
// Data Structures:
//   - std::array<int, 60> remainder_counts: stack-allocated direct-indexed
//     frequency table for remainders 0..59, avoiding heap allocation and hash
//     overhead entirely.
//
// Real-World Applications:
//   - Broadcast & streaming scheduling: pairing ad spots or audio/video clips
//     to fill exact 60-second commercial or program time slots.
//   - Manufacturing & bin packing: pairing cut-stock remnants or cargo weights
//     that sum to an exact container/pallet multiple.
//   - Time-division multiplexing (TDM) & networking: matching packet frames to
//     align cleanly with periodic transmission windows.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (classic complement lookup in a single pass)
//   - #974 Subarray Sums Divisible By K [done] (modular remainder frequency counting over prefix sums)
//   - #1497 Check If Array Pairs Are Divisible by k [new] (pairing every element into k-divisible pairs via remainder counts)
//   - #3184 Count Pairs That Form a Complete Day I [new] (exact same algorithm modulo 24 instead of 60)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts pairs (i < j) in `time` with (time[i] + time[j]) % 60 == 0.
  // Time: O(N), Space: O(1).
  [[nodiscard]] int numPairsDivisibleBy60(const std::vector<int>& time) const {
    constexpr int kMod = 60;
    std::array<int, static_cast<std::size_t>(kMod)> remainder_counts{};
    std::int64_t pair_count = 0;

    for (const int duration : time) {
      // Normalize remainder into [0, 59] even if a non-positive input appears.
      const int rem = ((duration % kMod) + kMod) % kMod;
      // Partner remainder in [0, 59]; maps rem == 0 to complement == 0.
      const int complement = (kMod - rem) % kMod;

      // Count all earlier songs whose remainder complements this song, then
      // record this song so it can pair with subsequent songs.
      pair_count += remainder_counts[static_cast<std::size_t>(complement)];
      ++remainder_counts[static_cast<std::size_t>(rem)];
    }

    return static_cast<int>(pair_count);
  }
};

LC_PROBLEM(1010, "Reconstruct Original Digits From English", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_from_python_build", [&] {
    // Valid pairs: (30, 150), (20, 100), (20, 40).
    ctx.expect_eq(sol.numPairsDivisibleBy60({30, 20, 150, 100, 40}), 3);
  });

  ctx.test("example_2_all_multiples_of_60", [&] {
    // All 3 choose 2 = 3 pairs sum to 120.
    ctx.expect_eq(sol.numPairsDivisibleBy60({60, 60, 60}), 3);
  });

  ctx.test("all_half_minutes", [&] {
    // 4 choose 2 = 6 pairs of remainder 30.
    ctx.expect_eq(sol.numPairsDivisibleBy60({30, 30, 30, 30}), 6);
  });

  ctx.test("single_valid_pair_among_noise", [&] {
    // Only 451 (rem 31) + 209 (rem 29) = 660 is divisible by 60.
    ctx.expect_eq(
        sol.numPairsDivisibleBy60({15, 63, 451, 213, 37, 209, 343, 319}), 1);
  });

  ctx.test("single_song_and_no_pairs", [&] {
    ctx.expect_eq(sol.numPairsDivisibleBy60({60}), 0);
    ctx.expect_eq(sol.numPairsDivisibleBy60({1, 2, 3, 4}), 0);
  });
}

}  // namespace
