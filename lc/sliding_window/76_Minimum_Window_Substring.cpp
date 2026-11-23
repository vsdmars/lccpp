// LeetCode 76: Minimum Window Substring
// https://leetcode.com/problems/minimum-window-substring/
//
// Problem:
//   Given two strings s and t of lengths m and n (1 <= m, n <= 10^5, ASCII
//   English letters), return the shortest contiguous substring of s that
//   contains every character in t with at least its multiplicity in t. If no
//   such window exists, return the empty string "".
//
// Algorithm:
//   Two-pointer sliding window with a signed deficit table (`need[256]`):
//   1. Initialize `need[c]` with the frequency of each character `c` in `t`,
//      and set `missing = t.size()`, the total number of character slots still
//      unfulfilled in the current window.
//   2. Expand the right pointer `right` across `s`. If `need[s[right]] > 0`,
//      this character fulfills one required slot, so decrement `missing`.
//      Always decrement `need[s[right]]` (irrelevant or surplus characters
//      become negative, tracking exact excess inside the window).
//   3. Whenever `missing == 0`, the window `[left, right]` covers all of `t`.
//      Update the best window if shorter, then shrink from `left`: increment
//      `need[s[left]]`, and if it becomes positive again, increment `missing`
//      because the window is no longer valid; advance `left`.
//   Time Complexity: O(m + n) — each pointer traverses `s` at most once.
//   Space Complexity: O(1) — fixed 256-entry ASCII frequency array.
//
// Data Structures:
//   std::array<int, 256>: fixed-size signed deficit map indexed by unsigned
//   ASCII byte, avoiding hash-table overhead while supporting both upper- and
//   lowercase characters.
//
// Real-World Applications:
//   - Search-engine snippet generation: locating the shortest text passage
//     containing all query terms (including repeated keywords).
//   - Log correlation and bioinformatics motif scanning: finding the tightest
//     time window or genomic interval containing a required multiset of events
//     or markers.
//
// Related LeetCode Problems:
//   - #424 Longest Repeating Character Replacement [done] (sliding window)
//   - #438 Find All Anagrams in a String [done] (fixed-size frequency window)
//   - #567 Permutation in String [done] (sliding window multiset match)
//   - #30 Substring with Concatenation of All Words [new] (word-level window)
//   - #632 Smallest Range Covering Elements from K Lists [done] (min covering)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>

namespace {

constexpr std::size_t kAsciiSize = 256;

// Converts a char safely to an unsigned byte index in [0, 255].
constexpr std::size_t byteIndex(char ch) {
  return static_cast<std::size_t>(static_cast<unsigned char>(ch));
}

class Solution {
 public:
  // Returns the shortest substring of s covering every character (with
  // multiplicity) in t, or "" if none exists.
  // Time: O(|s| + |t|), Space: O(1).
  std::string minWindow(std::string_view s, std::string_view t) {
    if (t.empty() || s.size() < t.size()) {
      return "";
    }

    // Positive need[c] = still required; 0 = exact match; negative = surplus.
    std::array<int, kAsciiSize> need{};
    for (char ch : t) {
      ++need[byteIndex(ch)];
    }

    std::size_t missing = t.size();
    std::size_t left = 0uz;
    std::size_t best_start = 0uz;
    std::size_t best_len = std::numeric_limits<std::size_t>::max();

    for (std::size_t right = 0uz; right < s.size(); ++right) {
      const std::size_t r_idx = byteIndex(s[right]);
      // Only a positive deficit means this character satisfies a missing slot.
      if (need[r_idx] > 0) {
        --missing;
      }
      --need[r_idx];

      // While the window [left, right] is valid, record if shorter and shrink
      // greedily from the left until a required character is evicted.
      while (missing == 0uz) {
        const std::size_t window_len = (right - left) + 1uz;
        if (window_len < best_len) {
          best_len = window_len;
          best_start = left;
        }

        const std::size_t l_idx = byteIndex(s[left]);
        ++need[l_idx];
        if (need[l_idx] > 0) {
          ++missing;
        }
        ++left;
      }
    }

    if (best_len == std::numeric_limits<std::size_t>::max()) {
      return "";
    }
    return std::string(s.substr(best_start, best_len));
  }
};

LC_PROBLEM(76, "Minimum Window Substring", "sliding_window", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    ctx.expect_eq(sol.minWindow("ADOBECODEBANC", "ABC"), "BANC");
  });

  ctx.test("example_2_single_char_match", [&] {
    ctx.expect_eq(sol.minWindow("a", "a"), "a");
  });

  ctx.test("example_3_insufficient_duplicates", [&] {
    ctx.expect_eq(sol.minWindow("a", "aa"), "");
  });

  ctx.test("python_build_duplicate_target", [&] {
    ctx.expect_eq(sol.minWindow("abcdaadd", "aa"), "aa");
  });

  ctx.test("case_sensitivity_and_duplicates", [&] {
    ctx.expect_eq(sol.minWindow("bba", "ab"), "ba");
    ctx.expect_eq(sol.minWindow("aAbBcC", "ABC"), "AbBcC");
    ctx.expect_eq(sol.minWindow("cabwefgewcwaefgcf", "cae"), "cwae");
  });

  ctx.test("no_match_and_empty_edge", [&] {
    ctx.expect_eq(sol.minWindow("abcdef", "xyz"), "");
    ctx.expect_eq(sol.minWindow("", "a"), "");
    ctx.expect_eq(sol.minWindow("abc", ""), "");
  });
}

}  // namespace
