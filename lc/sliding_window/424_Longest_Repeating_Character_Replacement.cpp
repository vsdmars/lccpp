// LeetCode 424: Longest Repeating Character Replacement
// https://leetcode.com/problems/longest-repeating-character-replacement/
//
// Problem:
//   Given a string s of uppercase English letters (1 <= s.length <= 10^5) and
//   an integer k (0 <= k <= s.length), you may replace at most k characters in
//   s with any other uppercase English letter. Return the length of the
//   longest contiguous substring containing only a single repeating letter
//   achievable after at most k replacements.
//
// Examples:
//   Example 1:
//     Input:  s = "ABAB", k = 2
//     Output: 4
//     Explanation:
//       - For right = 0..1 ("AB"), counts['A'] = 1, counts['B'] = 1, max_freq = 1,
//         and window_len = 2 <= 1 + 2.
//       - At right = 2 ('A'), counts['A'] = 2 updates max_freq = 2 with window_len
//         = 3 <= 2 + 2.
//       - At right = 3 ('B'), counts['B'] = 2 keeps max_freq = 2 and window_len = 4
//         <= 4, so left stays 0 and the result is 4 - 0 = 4.
//
//   Example 2:
//     Input:  s = "AABABBA", k = 1
//     Output: 4
//     Explanation:
//       - Across right = 0..3 ("AABA"), counts['A'] reaches 3 so max_freq = 3 and
//         window_len = 4 <= 3 + 1 keeps left = 0.
//       - At right = 4 ('B'), window_len = 5 > 3 + 1, so decrement counts['A'] for
//         s[0] and advance left to 1 (window "ABAB").
//       - At right = 5 ('B') and right = 6 ('A'), counts['B'] reaches 3 (keeping
//         max_freq = 3) while each step exceeds 3 + 1 and shifts left to 2 and then
//         3.
//       - Return s.size() - left = 7 - 3 = 4.
//
//   Example 3:
//     Input:  s = "AABBBAA", k = 0
//     Output: 3
//     Explanation:
//       - Across right = 0..1 ("AA"), counts['A'] = 2 sets max_freq = 2 and
//         window_len = 2 <= 2 + 0.
//       - At right = 2..3 ("BB"), window_len = 3 > 2 + 0 shifts left twice to 2
//         while counts['B'] climbs to 2.
//       - At right = 4 ('B'), counts['B'] = 3 sets max_freq = 3 with window_len = 3
//         <= 3 + 0 on [2..4] ("BBB").
//       - At right = 5..6 ("AA"), window_len = 4 > 3 + 0 shifts left to 3 and then
//         4, returning 7 - 4 = 3.
//
// Algorithm:
//   Maintain a sliding window [left, right], character frequencies inside the
//   window (`counts[26]`), and `max_freq`, the maximum frequency of any single
//   letter observed in a window so far. A window of length L can be turned
//   into a single repeated character iff L - max_freq <= k.
//   At each step, add s[right] and update `max_freq`. If the window length
//   exceeds `max_freq + k`, shift `left` right by 1 and decrement the count of
//   s[left]. Crucially, we never need to decrease `max_freq` or shrink the
//   window below its best length found so far: a strictly longer valid window
//   can only be achieved by a strictly larger `max_freq`. Thus the window size
//   is monotonically non-decreasing, and `s.size() - left` at loop termination
//   equals the optimal answer.
//   Time Complexity: O(n), where n = s.size().
//   Space Complexity: O(1) — fixed 26-element uppercase frequency array.
//
// Data Structures:
//   std::array<int, 26>: constant-size frequency table for 'A'..'Z' within the
//   active sliding window.
//
// Real-World Applications:
//   - Fault-tolerant sequence alignment and DNA/RNA homopolymer run detection
//     allowing up to k sequencing base-call errors.
//   - Time-series stability analysis: finding the longest operational window
//     in a dominant state while tolerating up to k transient anomaly ticks.
//
// Related LeetCode Problems:
//   - #1004 Max Consecutive Ones III [done] (binary specialization with k flips)
//   - #76 Minimum Window Substring [done] (variable-size frequency window)
//   - #340 Longest Substring with At Most K Distinct Characters [done] (sliding window)
//   - #2024 Maximize the Confusion of an Exam [done] (same k-replacement window)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAlphabetSize = 26;

// Maps an uppercase ASCII letter 'A'..'Z' to its 0-based table index.
constexpr std::size_t upperIndex(char ch) {
  return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                  static_cast<unsigned char>('A'));
}

class Solution {
 public:
  // Returns the maximum length of a substring that can be made uniform with at
  // most k single-character replacements.
  // Time: O(n), Space: O(1).
  int characterReplacement(std::string_view s, int k) {
    if (s.empty()) {
      return 0;
    }

    std::array<int, kAlphabetSize> counts{};
    std::size_t left = 0uz;
    int max_freq = 0;

    for (std::size_t right = 0uz; right < s.size(); ++right) {
      const std::size_t r_idx = upperIndex(s[right]);
      ++counts[r_idx];
      // Track the highest single-letter frequency seen; it never needs to be
      // lowered because only a larger max_freq can beat the best window size.
      max_freq = std::max(max_freq, counts[r_idx]);

      const std::size_t window_len = (right - left) + 1uz;
      const std::size_t max_allowed =
          static_cast<std::size_t>(max_freq) + static_cast<std::size_t>(k);

      if (window_len > max_allowed) {
        --counts[upperIndex(s[left])];
        ++left;
      }
    }

    // Because `left` advances by at most 1 per step, the final window length
    // equals the maximum valid window length attained during the sweep.
    return static_cast<int>(s.size() - left);
  }
};

LC_PROBLEM(424, "Longest Repeating Character Replacement", "sliding_window",
           ctx) {
  Solution sol;

  ctx.test("example_1_abab", [&] {
    ctx.expect_eq(sol.characterReplacement("ABAB", 2), 4);
  });

  ctx.test("example_2_aababba", [&] {
    ctx.expect_eq(sol.characterReplacement("AABABBA", 1), 4);
  });

  ctx.test("python_build_case", [&] {
    ctx.expect_eq(sol.characterReplacement("AABBC", 2), 4);
  });

  ctx.test("zero_replacements", [&] {
    ctx.expect_eq(sol.characterReplacement("AAAA", 0), 4);
    ctx.expect_eq(sol.characterReplacement("ABCD", 0), 1);
    ctx.expect_eq(sol.characterReplacement("AABBBAA", 0), 3);
  });

  ctx.test("k_covers_entire_string_and_single_char", [&] {
    ctx.expect_eq(sol.characterReplacement("ABCDEF", 5), 6);
    ctx.expect_eq(sol.characterReplacement("Z", 0), 1);
    ctx.expect_eq(sol.characterReplacement("", 2), 0);
  });
}

}  // namespace
