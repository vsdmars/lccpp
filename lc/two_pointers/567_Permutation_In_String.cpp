// LeetCode 567: Permutation In String
// https://leetcode.com/problems/permutation-in-string/
//
// Problem:
//   Given two lowercase English strings `s1` and `s2`, return `true` if `s2`
//   contains any permutation (anagram) of `s1` as a contiguous substring, and
//   `false` otherwise.
//   Constraints: 1 <= s1.length, s2.length <= 10^4; both strings consist of
//   lowercase English letters ('a'..'z').
//
// Examples:
//   Example 1:
//     Input:  s1 = "ab", s2 = "eidbaooo"
//     Output: true
//     Explanation:
//       - Build s1_counts = {a: 1, b: 1} and seed window_counts with the first
//         window - 1 = 1 character s2[0] = 'e' ({e: 1}).
//       - Slide right from 1 to 3 over windows "ei", "id", and "db", incrementing
//         the entering character and decrementing the leaving character; none
//         equals s1_counts.
//       - At right = 4, increment s2[4] = 'a' so window_counts represents "ba" with
//         {a: 1, b: 1}, matching s1_counts and returning true immediately.
//
//   Example 2:
//     Input:  s1 = "ab", s2 = "eidboaoo"
//     Output: false
//     Explanation:
//       - Build s1_counts = {a: 1, b: 1} and seed window_counts with s2[0] = 'e'.
//       - Slide right from 1 to 7 across the seven length-2 windows "ei", "id",
//         "db", "bo", "oa", "ao", and "oo", evicting 'b' at right = 4 before 'a'
//         enters at right = 5.
//       - Finish the loop without window_counts ever matching s1_counts and return
//         false.
//
//   Example 3:
//     Input:  s1 = "abcdef", s2 = "abc"
//     Output: false
//     Explanation:
//       - Verify s1 is non-empty and check the length guard s1.size() > s2.size()
//         (6 > 3).
//       - Return false immediately because no substring of s2 can be long enough to
//         contain a permutation of s1.
//
// Algorithm:
//   Fixed-size sliding window with 26-letter frequency signatures:
//   1. If `s1.size() > s2.size()`, no substring of `s2` can be long enough, so
//      return `false` immediately.
//   2. Compute the 26-entry character frequency array `s1_counts` for `s1`.
//   3. Pre-populate `window_counts` with the first `s1.size() - 1` characters
//      of `s2`.
//   4. Slide the right endpoint `right` from `s1.size() - 1` to `s2.size() - 1`:
//      increment the entering character `s2[right]`, compare `window_counts ==
//      s1_counts` (O(26) = O(1)), and decrement the leaving character at
//      `(right + 1) - s1.size()`.
//   Time Complexity:  O(|s1| + 26 * |s2|) = O(|s1| + |s2|).
//   Space Complexity: O(26) = O(1) fixed-size frequency arrays.
//
// Data Structures:
//   - `std::array<int, 26>` (`s1_counts`, `window_counts`): fixed-size
//     character frequency histograms representing anagram signatures for the
//     pattern and the current sliding window in `s2`.
//
// Real-World Applications:
//   - Intrusion detection and packet payload inspection looking for reordered
//     token/byte signatures within a fixed-length streaming buffer.
//   - Bioinformatics k-mer composition scanning (detecting DNA/protein windows
//     with matching nucleotide or amino-acid composition).
//   - Plagiarism and log-anomaly detection where fixed-window bag-of-symbols
//     histograms are matched across a continuous stream.
//
// Related LeetCode Problems:
//   - #438 Find All Anagrams in a String [done] (collects all start indices instead of returning bool)
//   - #76 Minimum Window Substring [done] (variable-length sliding window with frequency counts)
//   - #242 Valid Anagram [done] (single-pair frequency signature comparison)
//   - #3 Longest Substring Without Repeating Characters [done] (sliding window over character counts)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

// Maps a lowercase English letter ('a'..'z') to its 0-based histogram bucket.
constexpr std::size_t charIndex(char ch) noexcept {
  return static_cast<std::size_t>(ch - 'a');
}

class Solution {
 public:
  // Returns true if any contiguous substring of `s2` of length `s1.size()` has
  // the exact same character frequency histogram as `s1`.
  // Invariant: Immediately after adding `s2[right]`, `window_counts` holds the
  // exact counts of the contiguous slice `s2[(right + 1) - window .. right]`.
  // Time: O(|s1| + |s2|), Space: O(1).
  bool checkInclusion(std::string_view s1, std::string_view s2) {
    if (s1.empty()) {
      return true;
    }
    // Early exit: a permutation of s1 cannot fit inside a shorter s2.
    if (s1.size() > s2.size()) {
      return false;
    }

    const std::size_t window = s1.size();

    // Build target frequency signature for s1.
    std::array<int, kAlphabetSize> s1_counts{};
    for (const char ch : s1) {
      ++s1_counts[charIndex(ch)];
    }

    // Seed the sliding window with the first `window - 1` characters of s2.
    std::array<int, kAlphabetSize> window_counts{};
    for (std::size_t i = 0uz; i < window - 1uz; ++i) {
      ++window_counts[charIndex(s2[i])];
    }

    // Slide the fixed-length window across s2 one character at a time.
    for (std::size_t right = window - 1uz; right < s2.size(); ++right) {
      // Expand window to include s2[right] so window size equals `window`.
      ++window_counts[charIndex(s2[right])];

      if (window_counts == s1_counts) {
        return true;
      }

      // Evict the leftmost character before advancing to the next position.
      const std::size_t left = (right + 1uz) - window;
      --window_counts[charIndex(s2[left])];
    }

    return false;
  }
};

LC_PROBLEM(567, "Permutation In String", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_contains_ba", [&] {
    ctx.expect_true(sol.checkInclusion("ab", "eidbaooo"));
  });

  ctx.test("example_2_separated_chars", [&] {
    ctx.expect_false(sol.checkInclusion("ab", "eidboaoo"));
  });

  ctx.test("python_build_case", [&] {
    ctx.expect_true(sol.checkInclusion("bc", "eidcbaooo"));
  });

  ctx.test("s1_longer_than_s2", [&] {
    ctx.expect_false(sol.checkInclusion("abcdef", "abc"));
  });

  ctx.test("match_at_very_end", [&] {
    ctx.expect_true(sol.checkInclusion("adc", "dcda"));
    ctx.expect_true(sol.checkInclusion("xyz", "abczyx"));
  });

  ctx.test("exact_equal_and_repeated_chars", [&] {
    ctx.expect_true(sol.checkInclusion("aab", "baa"));
    ctx.expect_false(sol.checkInclusion("aab", "bab"));
  });

  ctx.test("single_character", [&] {
    ctx.expect_true(sol.checkInclusion("a", "bza"));
    ctx.expect_false(sol.checkInclusion("z", "abc"));
  });
}

}  // namespace
