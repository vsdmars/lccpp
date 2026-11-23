// LeetCode 387: First Unique Character In A String
// https://leetcode.com/problems/first-unique-character-in-a-string/
//
// Problem:
//   Given a string s consisting of lowercase English letters, find the first
//   non-repeating character in it and return its 0-based index. If every
//   character appears more than once (or the string is empty), return -1.
//
// Algorithm:
//   Two-pass fixed-alphabet frequency counting:
//   1. Pass 1: Scan s once and increment the frequency bucket for each letter
//      in a 26-element array indexed by `ch - 'a'`.
//   2. Pass 2: Scan s from left to right and return the first index whose
//      character has frequency equal to 1.
//   3. If no character has frequency 1, return -1.
//   Time Complexity: O(n), where n = s.size().
//   Space Complexity: O(1) (fixed 26-integer frequency table).
//
// Data Structures:
//   - std::array<int, 26>: cache-friendly, stack-allocated frequency table
//     mapping each lowercase letter 'a'..'z' to its occurrence count.
//
// Real-World Applications:
//   - Log and telemetry stream inspection to identify the first singleton
//     event or outlier symbol in a sequence.
//   - Lexer and text-processing utilities detecting non-duplicated tokens or
//     unique markers in fixed-alphabet encodings.
//   - Data deduplication pipelines finding the earliest non-colliding key.
//
// Related LeetCode Problems:
//   - #451 Sort Characters By Frequency [done] (character frequency counting followed by bucket/heap ordering)
//   - #242 Valid Anagram [done] (fixed-size 26-letter frequency array comparison)
//   - #383 Ransom Note [done] (26-letter frequency budget check)
//   - #2351 First Letter to Appear Twice [new] (single-pass first-duplicate counterpart)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAlphabetSize = 26;

class Solution {
 public:
  // Returns the 0-based index of the first non-repeating lowercase letter in s,
  // or -1 if no unique character exists.
  //
  // Time Complexity: O(n).
  // Space Complexity: O(1).
  int firstUniqChar(std::string_view s) {
    // Pass 1: Tally the frequency of every lowercase letter in s.
    std::array<int, kAlphabetSize> freq{};
    for (const char ch : s) {
      ++freq[static_cast<std::size_t>(ch - 'a')];
    }

    // Pass 2: Return the earliest index whose character occurs exactly once.
    for (std::size_t i = 0uz; i < s.size(); ++i) {
      if (freq[static_cast<std::size_t>(s[i] - 'a')] == 1) {
        return static_cast<int>(i);
      }
    }

    return -1;
  }
};

LC_PROBLEM(387, "First Unique Character In A String", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_leetcode", [&] {
    ctx.expect_eq(sol.firstUniqChar("leetcode"), 0);
  });

  ctx.test("example_2_loveleetcode", [&] {
    ctx.expect_eq(sol.firstUniqChar("loveleetcode"), 2);
  });

  ctx.test("example_3_all_repeating", [&] {
    ctx.expect_eq(sol.firstUniqChar("aabb"), -1);
  });

  ctx.test("unique_at_end", [&] {
    ctx.expect_eq(sol.firstUniqChar("aabbccd"), 6);
  });

  ctx.test("single_char_and_empty", [&] {
    ctx.expect_eq(sol.firstUniqChar("z"), 0);
    ctx.expect_eq(sol.firstUniqChar(""), -1);
  });
}

}  // namespace
