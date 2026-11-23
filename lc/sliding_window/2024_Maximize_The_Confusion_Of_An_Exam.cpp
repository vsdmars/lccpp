// LeetCode 2024: Maximize The Confusion Of An Exam
// https://leetcode.com/problems/maximize-the-confusion-of-an-exam/
//
// Problem:
//   Given a string `answerKey` consisting only of `'T'` and `'F'` characters
//   and an integer `k`, you may flip at most `k` characters (changing `'T'` to
//   `'F'` or `'F'` to `'T'`). Return the maximum length of a contiguous
//   substring consisting of all `'T'`s or all `'F'`s achievable after at most
//   `k` flips.
//   Constraints: 1 <= answerKey.length <= 5 * 10^4,
//   answerKey[i] in {'T', 'F'}, 1 <= k <= answerKey.length.
//
// Examples:
//   Example 1:
//     Input:  answerKey = "TTFF", k = 2
//     Output: 4
//     Explanation:
//       - Run maxWindowWithFlips("TTFF", 'T', 2) to maximize 'F's: remaining_flips
//         drops 2 -> 1 -> 0 on "TT" and stays 0 on "FF", leaving left = 0 and
//         length 4 - 0 = 4.
//       - Run maxWindowWithFlips("TTFF", 'F', 2) to maximize 'T's: remaining_flips
//         stays 2 on "TT" and drops 2 -> 1 -> 0 on "FF", leaving left = 0 and
//         length 4 - 0 = 4.
//       - Return std::max(4, 4) = 4.
//
//   Example 2:
//     Input:  answerKey = "TTFTTFTT", k = 1
//     Output: 5
//     Explanation:
//       - Run maxWindowWithFlips("TTFTTFTT", 'T', 1) to count consecutive 'F's:
//         remaining_flips drops below 0 at index 1 and later 'T's advance left to
//         6, yielding 8 - 6 = 2.
//       - Run maxWindowWithFlips("TTFTTFTT", 'F', 1) to count consecutive 'T's:
//         remaining_flips hits 0 at index 2 ('F'), allowing window [0..4] ("TTFTT")
//         of length 5.
//       - At indices 5..7 ('F', 'T', 'T'), remaining_flips drops to -1 and left
//         advances through indices 0, 1, and 2 (where 'F' restores remaining_flips
//         to 0), ending at left = 3.
//       - Return std::max(2, 8 - 3) = 5.
//
//   Example 3:
//     Input:  answerKey = "TTFTTFTT", k = 0
//     Output: 2
//     Explanation:
//       - With flip_char = 'T' and k = 0, every 'T' shifts left forward except
//         across 'F's at indices 2 and 5, finishing at left = 7 for length 8 - 7 =
//         1.
//       - With flip_char = 'F' and k = 0, remaining_flips becomes -1 at indices 2
//         and 5 while advancing left restores it at indices 2 and 5, finishing at
//         left = 6 for length 8 - 6 = 2.
//       - Return std::max(1, 2) = 2 (matching the longest unflipped run "TT").
//
// Algorithm:
//   Run a non-shrinking sliding window twice — once treating `'T'` as the
//   character to flip (producing a run of `'F'`s) and once treating `'F'` as
//   the character to flip (producing a run of `'T'`s) — and take the maximum
//   of the two lengths.
//   For a fixed character `flip_char` to overwrite:
//     - Maintain a window `[left, right]` and a budget `remaining_flips`
//       initialized to `k`.
//     - Advance `right` by one; if `answer_key[right] == flip_char`, decrement
//       `remaining_flips`.
//     - If `remaining_flips < 0`, the window requires more than `k` flips, so
//       advance `left` by one (restoring `remaining_flips` if
//       `answer_key[left] == flip_char`).
//   Because `left` moves forward by at most one whenever `right` moves forward
//   by one, the window size `right - left + 1` never shrinks below the longest
//   valid window found so far. At loop termination, `n - left` equals the
//   exact maximum valid window length.
//   Time Complexity:  O(n) — two linear passes over `answer_key`.
//   Space Complexity: O(1) — operates directly on a `std::string_view` with
//                     constant scalar variables.
//
// Data Structures:
//   - Two index pointers (`left`, `right`) and an integer flip budget counter
//     implementing an in-place non-shrinking sliding window over
//     `std::string_view`.
//
// Real-World Applications:
//   - Telemetry and uptime SLA analysis: finding the longest consecutive
//     healthy (or degraded) service streak allowing up to `k` transient blip
//     alerts to be masked.
//   - Error-tolerant pattern matching in bioinformatics / DNA sequences:
//     finding the longest homopolymer or binary-feature run tolerating up to
//     `k` point mutations or sequencing errors.
//   - Network packet-loss smoothing: measuring the longest continuous reception
//     window recoverable with up to `k` forward error correction (FEC) repairs.
//
// Related LeetCode Problems:
//   - #1004 Max Consecutive Ones III [done] (identical sliding window restricted to flipping 0s in a binary array)
//   - #424 Longest Repeating Character Replacement [done] (generalizes from 2 characters 'T'/'F' to all 26 uppercase letters)
//   - #487 Max Consecutive Ones II [new] (special case allowing at most k = 1 flip)
//   - #1493 Longest Subarray of 1's After Deleting One Element [new] (sliding window tolerating at most one mismatch)
//   - #1438 Longest Continuous Subarray With Absolute Diff Less Than Or Equal To Limit [done] (uses the same non-shrinking sliding window technique)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the maximum number of consecutive identical answers ('T' or 'F')
  // achievable after flipping at most `k` positions in `answer_key`.
  //
  // Time: O(n), Space: O(1).
  [[nodiscard]] int maxConsecutiveAnswers(std::string_view answer_key, int k) {
    if (answer_key.empty() || k < 0) {
      return 0;
    }

    // Try maximizing consecutive 'F's (by flipping at most `k` 'T's) and
    // maximizing consecutive 'T's (by flipping at most `k` 'F's).
    const int max_falses = maxWindowWithFlips(answer_key, 'T', k);
    const int max_trues = maxWindowWithFlips(answer_key, 'F', k);
    return std::max(max_falses, max_trues);
  }

 private:
  // Computes the longest contiguous substring of `answer_key` containing at
  // most `k` occurrences of `flip_char`.
  //
  // Invariant:
  //   At every iteration, the window size `(right - left) + 1` is monotonic
  //   non-decreasing and equals the maximum valid window length seen across
  //   `answer_key[0..right]`.
  //
  // Time: O(n), Space: O(1).
  [[nodiscard]] static int maxWindowWithFlips(std::string_view answer_key,
                                              char flip_char, int k) {
    std::size_t left = 0;
    int remaining_flips = k;

    for (const char ch : answer_key) {
      if (ch == flip_char) {
        --remaining_flips;
      }

      // When the flip budget is exceeded, shift the left edge by one to keep
      // the window width fixed at the best valid length found so far.
      if (remaining_flips < 0) {
        if (answer_key[left] == flip_char) {
          ++remaining_flips;
        }
        ++left;
      }
    }

    return static_cast<int>(answer_key.size() - left);
  }
};

LC_PROBLEM(2024, "Maximize The Confusion Of An Exam", "sliding_window", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("TTFF", 2), 4);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("TFFT", 1), 3);
  });

  ctx.test("example_3", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("TTFTTFTT", 1), 5);
  });

  ctx.test("single_character", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("T", 1), 1);
    ctx.expect_eq(sol.maxConsecutiveAnswers("F", 0), 1);
  });

  ctx.test("zero_flips_allowed", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("TTFTTFTT", 0), 2);
    ctx.expect_eq(sol.maxConsecutiveAnswers("TFTFTF", 0), 1);
  });

  ctx.test("k_covers_entire_string", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("TFTFTFTF", 8), 8);
  });

  ctx.test("already_uniform", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("FFFFFF", 2), 6);
  });

  ctx.test("empty_string", [&] {
    ctx.expect_eq(sol.maxConsecutiveAnswers("", 1), 0);
  });
}

}  // namespace
