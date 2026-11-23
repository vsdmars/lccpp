// LeetCode 1297: Maximum Number Of Occurrences Of A Substring
// https://leetcode.com/problems/maximum-number-of-occurrences-of-a-substring/
//
// Problem:
//   Given a string `s` consisting of lowercase English letters and integers
//   `maxLetters`, `minSize`, and `maxSize`, return the maximum number of
//   occurrences (overlapping allowed) of any substring `sub` of `s` such that:
//     1. The number of distinct characters in `sub` is <= `maxLetters`.
//     2. `minSize <= sub.length <= maxSize`.
//   Constraints:
//     - 1 <= s.length <= 10^5
//     - 1 <= maxLetters <= 26
//     - 1 <= minSize <= maxSize <= min(26, s.length)
//
// Examples:
//   Example 1:
//     Input:  s = "aababcaab", maxLetters = 2, minSize = 3, maxSize = 4
//     Output: 2
//     Explanation:
//       - Slide a fixed window of length minSize = 3 across s (ignoring maxSize = 4
//         because any valid length-4 substring has a valid length-3 prefix
//         occurring at least as often).
//       - Window [0..2] ("aab") has distinct = 2 <= 2 and sets substr_counts["aab"]
//         = 1, while [1..3] ("aba"), [2..4] ("bab"), and [5..7] ("caa") each reach
//         count 1, and [3..5] ("abc") and [4..6] ("bca") are skipped because
//         distinct = 3 > 2.
//       - At right = 8, window [6..8] ("aab") has distinct = 2 <= 2 and increments
//         substr_counts["aab"] to 2, yielding max_occurrences = 2.
//
//   Example 2:
//     Input:  s = "aabcabcab", maxLetters = 2, minSize = 2, maxSize = 3
//     Output: 3
//     Explanation:
//       - Slide a fixed window of length minSize = 2 over s while maintaining
//         char_freq and distinct in O(1) per character.
//       - Every 2-character window has at most 2 distinct letters: "ab" appears at
//         [1..2], [4..5], and [7..8], incrementing substr_counts["ab"] to 1, 2, and
//         3, while "aa", "bc", and "ca" each occur at most twice.
//       - The loop finishes with max_occurrences = 3.
//
//   Example 3:
//     Input:  s = "abcde", maxLetters = 2, minSize = 3, maxSize = 3
//     Output: 0
//     Explanation:
//       - Slide a fixed window of length minSize = 3 across "abcde", examining
//         "abc" at [0..2], "bcd" at [1..3], and "cde" at [2..4].
//       - Each window has distinct = 3, which exceeds maxLetters = 2, so
//         substr_counts is never updated.
//       - Return max_occurrences = 0.
//
// Algorithm:
//   Key greedy observation: If a valid substring T of length L > minSize
//   occurs f times in `s`, then its length-`minSize` prefix P has at most as
//   many unique characters as T (so P is also valid) and occurs at least f
//   times in `s`. Therefore, the global maximum frequency is ALWAYS achieved
//   by a substring of exact length `minSize`, and `maxSize` can be ignored.
//
//   Slide a fixed-size window of length `minSize` across `s` using two
//   pointers (left/right). Maintain character counts in a fixed 26-slot array
//   and a running `distinct` counter in O(1) per step. Whenever the window has
//   size `minSize` and `distinct <= maxLetters`, increment the frequency of
//   the zero-copy `std::string_view` slice in a hash map and update `max_freq`.
//   Time Complexity:  O(N * minSize), where N = s.length() (hashing/comparing
//                     each valid length-minSize window takes O(minSize)).
//   Space Complexity: O(N) for storing `std::string_view` keys pointing into
//                     `s` without copying substring characters.
//
// Data Structures:
//   - `std::array<int, 26>` frequency table tracking lowercase letter counts
//     inside the active sliding window in O(1) time.
//   - `std::unordered_map<std::string_view, int>` mapping each valid
//     fixed-length substring slice directly to its occurrence count.
//
// Real-World Applications:
//   - Bioinformatics k-mer frequency counting in DNA/RNA sequences under
//     alphabet-complexity filters.
//   - Network intrusion detection and payload signature mining for repeated
//     fixed-length byte patterns.
//   - Compression dictionary pre-analysis (identifying high-frequency short
//     motifs for LZ77/BPE tokenization).
//
// Related LeetCode Problems:
//   - #187 Repeated DNA Sequences [new] (fixed-length sliding-window substring frequency counting)
//   - #3 Longest Substring Without Repeating Characters [done] (sliding window with distinct character tracking)
//   - #159 Longest Substring with At Most Two Distinct Characters [done] (sliding window bounded by max unique letters)
//   - #340 Longest Substring with At Most K Distinct Characters [done] (generalized k-distinct sliding window)
//   - #438 Find All Anagrams in a String [done] (fixed-length window with 26-letter frequency array)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <unordered_map>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Returns the maximum number of occurrences of any substring with at most
  // `maxLetters` distinct characters and length in [minSize, maxSize].
  //
  // Because any longer qualifying substring contains a length-`minSize` prefix
  // that occurs at least as often and uses no more unique letters, checking
  // only windows of length `minSize` is both necessary and sufficient.
  // Time: O(N * minSize), Space: O(N).
  int maxFreq(std::string_view s, int maxLetters, int minSize,
              [[maybe_unused]] int maxSize) {
    // Guard against degenerate parameters where no valid window exists.
    if (minSize <= 0 || maxLetters <= 0 ||
        s.size() < static_cast<std::size_t>(minSize)) {
      return 0;
    }

    const auto window_size = static_cast<std::size_t>(minSize);
    std::array<int, kAlphabetSize> char_freq{};
    std::unordered_map<std::string_view, int> substr_counts;
    int distinct = 0;
    int max_occurrences = 0;

    for (std::size_t right = 0uz; right < s.size(); ++right) {
      // Expand window to include s[right].
      const auto in_idx = static_cast<std::size_t>(s[right] - 'a');
      if (char_freq[in_idx] == 0) {
        ++distinct;
      }
      ++char_freq[in_idx];

      // Shrink from the left once the window exceeds `window_size`.
      if (right >= window_size) {
        const auto out_idx =
            static_cast<std::size_t>(s[right - window_size] - 'a');
        --char_freq[out_idx];
        if (char_freq[out_idx] == 0) {
          --distinct;
        }
      }

      // Once the window reaches exact length `minSize`, evaluate eligibility.
      if ((right + 1uz) >= window_size && distinct <= maxLetters) {
        const std::size_t left = (right + 1uz) - window_size;
        const std::string_view candidate = s.substr(left, window_size);
        const int count = ++substr_counts[candidate];
        max_occurrences = std::max(max_occurrences, count);
      }
    }

    return max_occurrences;
  }
};

LC_PROBLEM(1297, "Maximum Number Of Occurrences Of A Substring", "two_pointers",
           ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.maxFreq("aababcaab", 2, 3, 4), 2);
  });

  ctx.test("example_2_overlapping", [&] {
    ctx.expect_eq(sol.maxFreq("aaaa", 1, 3, 3), 2);
  });

  ctx.test("example_3_min_size_dominates", [&] {
    ctx.expect_eq(sol.maxFreq("aabcabcab", 2, 2, 3), 3);
  });

  ctx.test("example_4_none_valid", [&] {
    ctx.expect_eq(sol.maxFreq("abcde", 2, 3, 3), 0);
  });

  ctx.test("single_character_string", [&] {
    ctx.expect_eq(sol.maxFreq("z", 1, 1, 1), 1);
  });

  ctx.test("all_same_characters_min_size_1", [&] {
    ctx.expect_eq(sol.maxFreq("aaaaa", 1, 1, 5), 5);
  });
}

}  // namespace
