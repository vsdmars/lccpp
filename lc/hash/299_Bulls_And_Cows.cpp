// LeetCode 299: Bulls And Cows
// https://leetcode.com/problems/bulls-and-cows/
//
// Problem:
//   Given two equal-length strings `secret` and `guess` consisting solely of
//   decimal digits (`'0'`-`'9'`), return a Mastermind-style hint formatted as
//   `"<bulls>A<cows>B"`. A "bull" is a digit that matches in both value and
//   index position. A "cow" is a digit present in `secret` that appears in
//   `guess` at a different position, where each unmatched digit instance can
//   be paired at most once (duplicate digits are allowed in both strings).
//
// Algorithm:
//   Scan `secret` and `guess` simultaneously in one pass:
//     - Whenever `secret[i] == guess[i]`, increment `bulls`.
//     - Otherwise, increment the unmatched frequency of `secret[i]` in
//       `secret_counts` and of `guess[i]` in `guess_counts`.
//   After the pass, each digit `d` in `0..9` contributes
//   `std::min(secret_counts[d], guess_counts[d])` cows because every unmatched
//   occurrence in `guess` can pair with at most one remaining occurrence of
//   the same digit in `secret`.
//   Time Complexity: O(N), where N is the string length.
//   Space Complexity: O(1) using two fixed 10-element arrays.
//
// Data Structures:
//   - `std::array<int, 10>`: fixed-size direct-addressed frequency tables for
//     unmatched digits `'0'`-`'9'` in `secret` and `guess`, eliminating hash
//     table overhead entirely.
//
// Real-World Applications:
//   - Word-guessing and code-breaking game evaluators (Wordle, Mastermind)
//     requiring exact-position vs. misplaced-multiset scoring.
//   - Lightweight multiset diffing and edit-distance lower bounds on fixed
//     alphabets (e.g., nucleotide sequence / PIN verification scoring).
//   - Data quality validation comparing expected vs. actual fixed-width record
//     codes while distinguishing transposition from substitution errors.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (fixed-alphabet frequency counting)
//   - #383 Ransom Note [done] (multiset frequency comparison over a small alphabet)
//   - #350 Intersection Of Two Arrays II [done] (min-multiplicity intersection of two multisets)
//   - #843 Guess the Word [done] (exact positional match scoring in an interactive guessing game)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Computes the Bulls and Cows hint string `"<bulls>A<cows>B"` for equal-length
  // digit strings `secret` and `guess`.
  // Time: O(N), Space: O(1).
  std::string getHint(std::string_view secret, std::string_view guess) {
    static constexpr std::size_t kNumDigits = 10uz;

    std::array<int, kNumDigits> secret_counts{};
    std::array<int, kNumDigits> guess_counts{};
    int bulls = 0;

    // Partition each position into either an exact bull match or a pair of
    // unmatched digits tallied for cow matching.
    const std::size_t len = std::min(secret.size(), guess.size());
    for (std::size_t i = 0uz; i < len; ++i) {
      if (secret[i] == guess[i]) {
        ++bulls;
      } else {
        ++secret_counts[static_cast<std::size_t>(secret[i] - '0')];
        ++guess_counts[static_cast<std::size_t>(guess[i] - '0')];
      }
    }

    // For each digit, the number of cows is bounded by the smaller unmatched
    // frequency across `secret` and `guess`.
    int cows = 0;
    for (std::size_t d = 0uz; d < kNumDigits; ++d) {
      cows += std::min(secret_counts[d], guess_counts[d]);
    }

    return std::to_string(bulls) + "A" + std::to_string(cows) + "B";
  }
};

LC_PROBLEM(299, "Bulls And Cows", "hash", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.getHint("1807", "7810"), "1A3B");
  });

  ctx.test("example_2_duplicate_digits", [&] {
    Solution sol;
    ctx.expect_eq(sol.getHint("1123", "0111"), "1A1B");
  });

  ctx.test("all_cows_no_bulls", [&] {
    Solution sol;
    ctx.expect_eq(sol.getHint("1100", "0011"), "0A4B");
  });

  ctx.test("single_digit_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.getHint("1", "0"), "0A0B");
    ctx.expect_eq(sol.getHint("1", "1"), "1A0B");
  });

  ctx.test("over_guessed_duplicate_bulls", [&] {
    Solution sol;
    ctx.expect_eq(sol.getHint("1122", "1222"), "3A0B");
    ctx.expect_eq(sol.getHint("1122", "2211"), "0A4B");
  });
}

}  // namespace
