// LeetCode 771: Jewels And Stones
// https://leetcode.com/problems/jewels-and-stones/
//
// Problem:
//   Given a string `jewels` of distinct ASCII English letters representing
//   stone types that are jewels, and a string `stones` representing the stones
//   you own, count how many characters in `stones` also appear in `jewels`.
//   Matching is case-sensitive ('a' and 'A' are distinct types).
//   Constraints: 1 <= jewels.length, stones.length <= 50, English letters only.
//
// Algorithm:
//   Use a fixed 128-entry direct-address ASCII membership table (`is_jewel`):
//     1. Mark `is_jewel[c] = true` for every character `c` in `jewels`.
//     2. Scan every character `s` in `stones` and increment the count whenever
//        `is_jewel[s]` is true.
//   This achieves O(1) worst-case lookup per stone with zero dynamic memory
//   allocation, preserving the O(|J| + |S|) hash-set membership approach.
//   Time Complexity: O(|J| + |S|), where |J| = jewels.size(), |S| = stones.size().
//   Space Complexity: O(1) (fixed 128-byte lookup table).
//
// Data Structures:
//   - Direct-address boolean table (`std::array<bool, 128>`): acts as a
//     perfect hash set over the 7-bit ASCII domain for single-instruction
//     membership queries.
//
// Real-World Applications:
//   - Fast character-class filtering and counting in lexers, sanitizers, and
//     log/text parsers (e.g., counting delimiters or special characters).
//   - Allowlist/denylist membership filtering over bounded symbol alphabets
//     such as nucleotide bases, protocol flags, or HTTP header characters.
//   - Bloom-filter / bitmap pre-filtering before heavier string inspection.
//
// Related LeetCode Problems:
//   - #383 Ransom Note [done] (character frequency lookup in fixed alphabet array)
//   - #387 First Unique Character in a String [done] (direct-address ASCII counting)
//   - #1684 Count the Number of Consistent Strings [new] (allowed-character bitmask filtering)
//   - #1832 Check if the Sentence Is Pangram [new] (character presence tracking)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Counts how many characters in `stones` belong to the case-sensitive set
  // `jewels`.
  // Time: O(|jewels| + |stones|), Space: O(1).
  int numJewelsInStones(std::string_view jewels, std::string_view stones) {
    // Populate direct-address ASCII lookup table for O(1) membership checks.
    std::array<bool, kAsciiSize> is_jewel{};
    for (const char ch : jewels) {
      const auto code = static_cast<unsigned char>(ch);
      if (code < kAsciiSize) {
        is_jewel[code] = true;
      }
    }

    // Tally every stone whose character code is flagged as a jewel.
    int count = 0;
    for (const char ch : stones) {
      const auto code = static_cast<unsigned char>(ch);
      if (code < kAsciiSize && is_jewel[code]) {
        ++count;
      }
    }

    return count;
  }

 private:
  static constexpr std::size_t kAsciiSize = 128uz;
};

LC_PROBLEM(771, "Jewels And Stones", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    ctx.expect_eq(sol.numJewelsInStones("aA", "aAAbbbb"), 3);
  });

  ctx.test("leetcode_example_2_case_sensitive", [&] {
    ctx.expect_eq(sol.numJewelsInStones("z", "ZZ"), 0);
  });

  ctx.test("all_stones_are_jewels", [&] {
    ctx.expect_eq(sol.numJewelsInStones("abcABC", "AaBbCc"), 6);
  });

  ctx.test("empty_and_repeated_cases", [&] {
    ctx.expect_eq(sol.numJewelsInStones("xyz", ""), 0);
    ctx.expect_eq(sol.numJewelsInStones("", "abc"), 0);
    ctx.expect_eq(sol.numJewelsInStones("A", "AAAAA"), 5);
  });
}

}  // namespace
