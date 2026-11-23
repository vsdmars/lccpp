// LeetCode 409: Longest Palindrome
// https://leetcode.com/problems/longest-palindrome/
//
// Problem:
//   Given a string s consisting of lowercase and/or uppercase English letters,
//   return the length of the longest palindrome that can be constructed using
//   those letters. Characters are case-sensitive (e.g., "Aa" is not a
//   palindrome).
//
// Examples:
//   Example 1:
//     Input:  s = "abccccdd"
//     Output: 7
//     Explanation:
//       - Phase 1 counts ASCII character frequencies into freq: 'a': 1, 'b': 1,
//         'c': 4, 'd': 2.
//       - Phase 2 accumulates (cnt / 2) * 2 across all buckets: 'c' adds 4 and 'd'
//         adds 2 for length = 6, while 'a' and 'b' set has_center = true.
//       - Since has_center is true, one unpaired character is placed in the middle,
//         returning length + 1 = 7.
//
//   Example 2:
//     Input:  s = "bananas"
//     Output: 5
//     Explanation:
//       - Phase 1 populates freq with 'a': 3, 'n': 2, 'b': 1, and 's': 1.
//       - Phase 2 adds (3 / 2) * 2 = 2 for 'a' and (2 / 2) * 2 = 2 for 'n' to reach
//         length = 4, while odd counts 'a', 'b', and 's' set has_center = true.
//       - At most one leftover odd character can occupy the palindrome center, so
//         returning length + 1 yields 5.
//
//   Example 3:
//     Input:  s = "Aa"
//     Output: 1
//     Explanation:
//       - Phase 1 increments separate case-sensitive buckets freq[65] = 1 ('A') and
//         freq[97] = 1 ('a').
//       - Phase 2 computes (1 / 2) * 2 = 0 pairs for both buckets (length = 0) and
//         sets has_center = true.
//       - Returning length + 1 produces 1 because 'A' and 'a' cannot pair
//         symmetrically.
//
// Algorithm:
//   A palindrome mirrors characters on its left and right halves, plus at most
//   one unpaired character placed in the exact center:
//   1. Count the frequency of every ASCII character in a fixed-size 128-slot
//      array (preserving case sensitivity between 'A'-'Z' and 'a'-'z').
//   2. For each character with frequency `cnt`, we can always pair up
//      `(cnt / 2) * 2` copies symmetrically (even when `cnt` is odd, `cnt - 1`
//      copies still contribute pairs!).
//   3. If at least one character has an odd frequency, we may place one
//      leftover copy in the middle, adding 1 to the total length.
//   Time Complexity: O(n), where n = s.size().
//   Space Complexity: O(1) (fixed 128-integer ASCII frequency table).
//
// Data Structures:
//   - std::array<int, 128>: direct-addressed ASCII frequency counter handling
//     both uppercase and lowercase letters in O(1) without hashing overhead.
//
// Real-World Applications:
//   - Bioinformatics sequence analysis checking symmetric / inverted-repeat
//     character budgets in DNA/RNA motifs.
//   - Parity-based payload packing and symmetric frame construction in
//     communication protocols.
//   - Combinatorial frequency-parity checks in puzzle solvers and text
//     analytics.
//
// Related LeetCode Problems:
//   - #266 Palindrome Permutation [done] (checks whether a full palindrome can be formed, i.e., at most one odd count)
//   - #2131 Longest Palindrome by Concatenating Two Letter Words [done] (pair-matching and single-center extension over 2-char tokens)
//   - #5 Longest Palindromic Substring [done] (contiguous palindrome substring vs. rearranged multiset)
//   - #387 First Unique Character in a String [done] (direct-addressed character frequency counting)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAsciiSize = 128;

class Solution {
 public:
  // Returns the maximum length of a case-sensitive palindrome buildable by
  // rearranging a subset (or all) of the characters in s.
  //
  // Invariant: Every character with count `cnt` contributes `(cnt / 2) * 2`
  // paired characters; at most one unpaired character from any odd-count
  // bucket can occupy the palindrome center.
  //
  // Time Complexity: O(n).
  // Space Complexity: O(1).
  int longestPalindrome(std::string_view s) {
    // Phase 1: Count occurrences of each ASCII character (case-sensitive).
    std::array<int, kAsciiSize> freq{};
    for (const char ch : s) {
      ++freq[static_cast<std::size_t>(static_cast<unsigned char>(ch))];
    }

    // Phase 2: Sum all available symmetric pairs and note if any odd count
    // leaves a spare character for the center.
    int length = 0;
    bool has_center = false;
    for (const int cnt : freq) {
      length += (cnt / 2) * 2;
      if ((cnt % 2) != 0) {
        has_center = true;
      }
    }

    return has_center ? (length + 1) : length;
  }
};

LC_PROBLEM(409, "Longest Palindrome", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_abccccdd", [&] {
    ctx.expect_eq(sol.longestPalindrome("abccccdd"), 7);
  });

  ctx.test("example_2_single_character", [&] {
    ctx.expect_eq(sol.longestPalindrome("a"), 1);
  });

  ctx.test("case_sensitivity", [&] {
    ctx.expect_eq(sol.longestPalindrome("Aa"), 1);
    ctx.expect_eq(sol.longestPalindrome("AAAccccdd"), 9);
  });

  ctx.test("multiple_odd_counts_bananas", [&] {
    // 'a':3, 'n':2, 'b':1, 's':1 -> pairs: 2 ('a') + 2 ('n') + 1 center = 5.
    ctx.expect_eq(sol.longestPalindrome("bananas"), 5);
  });

  ctx.test("all_even_and_empty", [&] {
    ctx.expect_eq(sol.longestPalindrome("aabbccZZ"), 8);
    ctx.expect_eq(sol.longestPalindrome(""), 0);
  });
}

}  // namespace
