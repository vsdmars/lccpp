// LeetCode 3: Longest Substring Without Repeating Characters
// https://leetcode.com/problems/longest-substring-without-repeating-characters/
//
// Problem:
//   Given a string `s`, return the length of the longest contiguous substring
//   that contains no duplicate characters. The input may contain English
//   letters, digits, symbols, and spaces (standard ASCII bytes), and may be
//   empty (in which case the answer is 0).
//
// Algorithm:
//   Sliding window with direct last-seen index tracking. Maintain a valid
//   window `[left, right]` that contains only distinct characters. As `right`
//   scans forward one character at a time, check whether `s[right]` was seen
//   previously; if its last recorded index `prev` lies at or after `left`,
//   advance `left` to `prev + 1` via `std::max(left, prev + 1)`. Taking the
//   maximum prevents `left` from moving backward when encountering a stale
//   duplicate outside the current window (e.g., `"abba"`). Update the
//   character's latest index to `right` and record the best window length
//   `right - left + 1`.
//   Time Complexity:  O(n), where n = s.size() (each index is visited once).
//   Space Complexity: O(1), using a fixed 256-entry ASCII lookup table.
//
// Data Structures:
//   - Fixed-size array (`std::array<int, 256>`): maps each byte value
//     (cast to `unsigned char`) to its most recent index in `s`, initialized
//     to -1 to signify unseen characters.
//   - Two integer window pointers (`left`, `right`): delimit the current
//     duplicate-free substring invariant.
//
// Real-World Applications:
//   - Streaming deduplication and network packet inspection (finding the
//     longest run of distinct protocol tokens or flow keys).
//   - Data compression (LZ77 / DEFLATE sliding-window matchers tracking the
//     most recent occurrence of symbols in a lookback buffer).
//   - Bioinformatics sequence analysis (identifying maximal repeat-free
//     k-mer spans in DNA/protein reads).
//
// Related LeetCode Problems:
//   - #76 Minimum Window Substring [done] (variable-size sliding window with frequency counts)
//   - #159 Longest Substring with At Most Two Distinct Characters [done] (sliding window bounded by distinct-character count)
//   - #340 Longest Substring with At Most K Distinct Characters [done] (generalized k-distinct sliding window)
//   - #424 Longest Repeating Character Replacement [done] (sliding window maximizing valid span under a budget)
//   - #992 Subarrays with K Different Integers [done] (counting subarrays with sliding windows)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the length of the longest substring of `s` with all unique
  // characters using an O(n) sliding window over a 256-slot ASCII table.
  //
  // Invariant: at the end of iteration `right`, the substring
  // `s[left..right]` contains no duplicate characters, and `last_seen[c]`
  // stores the greatest index <= right where byte `c` appeared (or -1).
  //
  // Time: O(n), Space: O(1).
  int lengthOfLongestSubstring(std::string_view s) {
    static constexpr std::size_t kAsciiSize = 256;

    // Initialize every byte slot to -1 so `last_seen[ch] + 1 == 0` never
    // advances `left` before the character has appeared inside the window.
    std::array<int, kAsciiSize> last_seen{};
    last_seen.fill(-1);

    int max_len = 0;
    int left = 0;

    for (std::size_t right = 0uz; right < s.size(); ++right) {
      // Cast through `unsigned char` to avoid negative indexing on platforms
      // where `char` is signed.
      const auto ch = static_cast<unsigned char>(s[right]);
      const int right_idx = static_cast<int>(right);

      // Jump `left` past any prior occurrence of `ch` within the current
      // window. `std::max` guarantees `left` never moves backward on stale
      // occurrences (such as the second 'a' in "abba").
      left = std::max(left, last_seen[ch] + 1);

      // Record the latest position of `ch` and update the best window length.
      last_seen[ch] = right_idx;
      max_len = std::max(max_len, right_idx - left + 1);
    }

    return max_len;
  }
};

LC_PROBLEM(3, "Longest Substring Without Repeating Characters", "two_pointers",
           ctx) {
  Solution sol;

  ctx.test("example_abcabcbb", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstring("abcabcbb"), 3);
  });

  ctx.test("example_all_same", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstring("bbbbb"), 1);
  });

  ctx.test("example_pwwkew", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstring("pwwkew"), 3);
  });

  ctx.test("stale_duplicate_abba", [&] {
    // Exercises why `left` must use `std::max` instead of blind assignment.
    ctx.expect_eq(sol.lengthOfLongestSubstring("abba"), 2);
  });

  ctx.test("overlapping_window_dvdf", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstring("dvdf"), 3);
  });

  ctx.test("empty_and_single_char", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstring(""), 0);
    ctx.expect_eq(sol.lengthOfLongestSubstring("z"), 1);
  });

  ctx.test("all_unique_with_symbols_and_spaces", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstring("a b!c a"), 5);
    ctx.expect_eq(sol.lengthOfLongestSubstring("abcdef"), 6);
  });
}

}  // namespace
