// LeetCode 408: Valid Word Abbreviation
// https://leetcode.com/problems/valid-word-abbreviation/
//
// Problem:
//   Given a non-empty lowercase string `word` and an abbreviation string `abbr`
//   consisting of lowercase letters and digits, return true iff `abbr` is a
//   valid abbreviation of `word`. A valid abbreviation may replace any number
//   of non-adjacent, non-empty substrings with their decimal lengths (without
//   leading zeroes; e.g., "01" and "0" are invalid).
//
// Algorithm:
//   Lockstep two-pointer scan with index `w` over `word` and index `a` over
//   `abbr`:
//     - While both pointers are in bounds:
//       * If `abbr[a]` is an ASCII digit:
//         - Immediately reject leading zero (`abbr[a] == '0'`).
//         - Parse the maximal contiguous run of digits into `skip`, checking
//           after every digit that `skip <= word.size() - w`. This early exit
//           simultaneously rejects oversized abbreviations and guarantees
//           `skip` can never overflow `std::size_t` even on huge digit runs.
//         - Advance `w += skip`.
//       * Otherwise `abbr[a]` is a literal letter:
//         - Verify `word[w] == abbr[a]`; if not, return false.
//         - Advance both `w` and `a` by 1.
//     - The abbreviation is valid iff both pointers reach the exact ends of
//       their strings (`w == word.size() && a == abbr.size()`).
//   Time complexity: O(|abbr|) (at most O(|word| + |abbr|)).
//   Space complexity: O(1).
//
// Data Structures:
//   - Two non-owning std::string_view inputs (`word`, `abbr`): zero-copy read
//     access.
//   - Two cursor indices (`w`, `a`) and an integer accumulator (`skip`).
//
// Real-World Applications:
//   - Numeronym / identifier validation (e.g., "i18n", "k8s", "l10n", "a11y")
//     and CLI shorthand matching.
//   - Run-length / wildcard skip-pattern matching in log filters, binary wire
//     parsers, and genomic CIGAR string alignment verification (e.g., "12M3I").
//
// Related LeetCode Problems:
//   - #320 Generalized Abbreviation [new] (generates all valid abbreviations via backtracking)
//   - #288 Unique Word Abbreviation [done] (hash-table lookup of word abbreviations)
//   - #527 Word Abbreviation [new] (computes minimal unique abbreviations for a dictionary)
//   - #443 String Compression [done] (inverse direction: run-length encoding a char array)
//   - #411 Minimum Unique Word Abbreviation [new] (shortest valid abbreviation avoiding collisions)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

// Locale-independent ASCII digit check avoiding signed-char UB in std::isdigit.
constexpr bool is_ascii_digit(char ch) noexcept {
  return ch >= '0' && ch <= '9';
}

class Solution {
 public:
  // Returns true iff `abbr` is a valid abbreviation of `word`.
  //
  // Invariant: at the start of every outer loop iteration, word[0..w - 1]
  // matches abbr[0..a - 1] exactly.
  //
  // Time: O(|abbr|), Space: O(1).
  bool validWordAbbreviation(std::string_view word, std::string_view abbr) {
    std::size_t w = 0;
    std::size_t a = 0;

    while (w < word.size() && a < abbr.size()) {
      if (is_ascii_digit(abbr[a])) {
        // Leading zeroes (and zero-length skips) are forbidden by the problem.
        if (abbr[a] == '0') {
          return false;
        }

        // Parse the full multi-digit skip count while bounding against the
        // remaining characters in `word` to prevent integer overflow.
        const std::size_t remaining = word.size() - w;
        std::size_t skip = 0;
        while (a < abbr.size() && is_ascii_digit(abbr[a])) {
          const auto digit = static_cast<std::size_t>(abbr[a] - '0');
          skip = (skip * 10) + digit;
          ++a;
          if (skip > remaining) {
            return false;
          }
        }
        w += skip;
      } else {
        if (word[w] != abbr[a]) {
          return false;
        }
        ++w;
        ++a;
      }
    }

    // Both strings must be completely consumed with no trailing mismatch.
    return w == word.size() && a == abbr.size();
  }
};

LC_PROBLEM(408, "Valid Word Abbreviation", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_valid", [&] {
    ctx.expect_true(sol.validWordAbbreviation("internationalization", "i12iz4n"));
  });

  ctx.test("example_2_invalid", [&] {
    ctx.expect_false(sol.validWordAbbreviation("apple", "a2e"));
  });

  ctx.test("all_valid_abbreviations_of_word", [&] {
    constexpr std::array<std::string_view, 16> kValidAbbrs{
        "word", "1ord", "w1rd", "wo1d", "wor1", "2rd", "w2d", "wo2",
        "1o1d", "1or1", "w1r1", "1o2",  "2r1",  "3d",  "w3",  "4",
    };
    for (const std::string_view abbr : kValidAbbrs) {
      ctx.expect_true(sol.validWordAbbreviation("word", abbr), abbr);
    }
  });

  ctx.test("leading_zeroes_rejected", [&] {
    ctx.expect_false(sol.validWordAbbreviation("substitution", "s010n"));
    ctx.expect_false(sol.validWordAbbreviation("a", "01"));
    ctx.expect_false(sol.validWordAbbreviation("word", "0word"));
  });

  ctx.test("length_mismatch_and_overflow_guard", [&] {
    ctx.expect_false(sol.validWordAbbreviation("hi", "3"));
    ctx.expect_false(sol.validWordAbbreviation("hi", "1"));
    ctx.expect_false(sol.validWordAbbreviation("hi", "hi1"));
    ctx.expect_false(
        sol.validWordAbbreviation("a", "99999999999999999999999999"));
  });

  ctx.test("multi_digit_skips", [&] {
    ctx.expect_true(sol.validWordAbbreviation("substitution", "s10n"));
    ctx.expect_true(sol.validWordAbbreviation("substitution", "sub4u4"));
    ctx.expect_true(sol.validWordAbbreviation("substitution", "12"));
  });
}

}  // namespace
