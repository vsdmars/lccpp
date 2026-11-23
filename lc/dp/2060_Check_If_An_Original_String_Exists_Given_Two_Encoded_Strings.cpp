// LeetCode 2060: Check If An Original String Exists Given Two Encoded Strings
// https://leetcode.com/problems/check-if-an-original-string-exists-given-two-encoded-strings/
//
// Problem:
//   An original lowercase alphabetic string can be encoded by splitting it into
//   non-empty substrings, replacing some of those substrings with their decimal
//   lengths, and concatenating the result. Given two encoded strings `s1` and
//   `s2` consisting of lowercase English letters (`'a'`-`'z'`) and digits
//   (`'1'`-`'9'`), where no more than 3 consecutive digits appear in either
//   string, return `true` if there exists at least one original string that
//   could have been encoded as both `s1` and `s2`, and `false` otherwise.
//   Constraints: 1 <= s1.length, s2.length <= 40; consecutive digits <= 3.
//
// Examples:
//   Example 1:
//     Input:  s1 = "l123e", s2 = "44"
//     Output: true
//     Explanation:
//       - From canMatch(0, 0, 0), s1[0] is 'l' while s2[0] starts digit run "44",
//         so tryDigitRunS2 calls generatePossibleLengths("44") -> {8, 44} and
//         branches on len = 8 to canMatch(0, 2, 8).
//       - With diff = 8 > 0, matchLetters consumes s1[0] = 'l' and recurses to
//         canMatch(1, 2, 7).
//       - At i = 1, tryDigitRunS1 expands "123" into {6, 15, 24, 123} and takes len
//         = 6 to reach canMatch(4, 2, 7 - 6 = 1).
//       - With diff = 1 > 0, matchLetters consumes s1[4] = 'e' to reach the base
//         case canMatch(5, 2, 0) where diff == 0, returning true.
//
//   Example 2:
//     Input:  s1 = "internationalization", s2 = "i18n"
//     Output: true
//     Explanation:
//       - At canMatch(0, 0, 0), s1[0] == s2[0] == 'i' matches via matchLetters and
//         advances to canMatch(1, 1, 0).
//       - At j = 1, tryDigitRunS2 expands "18" into {9, 18} and tries len = 18,
//         reaching canMatch(1, 3, 18).
//       - With diff = 18 > 0, matchLetters consumes the 18 letters
//         "nternationalizatio" in s1 one step at a time until canMatch(19, 3, 0).
//       - At (19, 3, 0), s1[19] == s2[3] == 'n' matches into the end state
//         canMatch(20, 4, 0) with diff == 0, returning true.
//
//   Example 3:
//     Input:  s1 = "x94", s2 = "x14"
//     Output: false
//     Explanation:
//       - At canMatch(0, 0, 0), s1[0] == s2[0] == 'x' matches and moves to
//         canMatch(1, 1, 0).
//       - tryDigitRunS1 expands "94" into {13, 94}, branching to canMatch(3, 1,
//         -13) and canMatch(3, 1, -94).
//       - From each branch, tryDigitRunS2 expands "14" into {5, 14}, reaching
//         terminal states (3, 3, diff) with diff in {-8, 1, -89, -80}.
//       - Because diff != 0 in all four terminal states, every branch fails, gets
//         marked in failed_, and possiblyEquals returns false.
//
// Algorithm:
//   Memoized DFS over aligned string positions and wildcard length offset:
//   1. Maintain state `(i, j, diff)`, where `i` is the next unread index in
//      `s1`, `j` is the next unread index in `s2`, and `diff` is the signed
//      number of unconsumed wildcard characters by which `s2` leads `s1`:
//      - `diff == 0`: both encodings are aligned at the same original index.
//      - `diff > 0`: `s2` has `diff` wildcards ahead that `s1` must consume.
//      - `diff < 0`: `s1` has `-diff` wildcards ahead that `s2` must consume.
//   2. Digit-run expansion: whenever `s1[i]` (or `s2[j]`) starts a maximal
//      contiguous run of digits (length 1 to 3), generate every valid total
//      length that could have produced that digit run by partitioning it into
//      individual numbers and summing them (e.g., `"123"` -> `{6, 15, 24, 123}`).
//      Branch on each length `len`, advancing past the entire run and updating
//      `diff` to `diff - len` (for `s1`) or `diff + len` (for `s2`).
//   3. Key invariant on `diff`: because every maximal digit run is consumed as
//      a whole before any literal at the same side, `s1` only ever starts a
//      digit run when `diff >= 0` and `s2` only ever starts a digit run when
//      `diff <= 0`. With at most 3 consecutive digits (`len <= 999`), `diff` is
//      strictly bounded to `[-999, +999]` across all reachable states.
//   4. Literal transitions (when neither side starts with a digit):
//      - `diff == 0`: both must have matching literals `s1[i] == s2[j]`,
//        advancing `(i + 1, j + 1, 0)`.
//      - `diff < 0`: `s2[j]` is absorbed by `s1`'s wildcard lead -> `(i, j + 1, diff + 1)`.
//      - `diff > 0`: `s1[i]` is absorbed by `s2`'s wildcard lead -> `(i + 1, j, diff - 1)`.
//   5. Since the search terminates immediately upon finding any `true` path,
//      any revisited state must have previously failed; a flat boolean byte
//      table records visited dead-end states for O(1) pruning.
//
//   Complexity (let N = s1.size(), M = s2.size(), D = 999):
//   - Time:  O(N * M * D) — at most `(N + 1) * (M + 1) * (2 * D + 1)` states,
//            with at most 4 outgoing branches per digit run (<= 3 digits).
//   - Space: O(N * M * D) — flat visited-failure table plus O(N + M) call stack.
//
// Data Structures:
//   - `std::vector<std::uint8_t> failed_`: flat 3D visited table indexed by
//     `(i, j, diff + 999)` marking dead-end states to avoid recomputation.
//   - `std::vector<int>`: small deduplicated list (size <= 4) of candidate
//     lengths generated from a 1-to-3 digit run.
//
// Real-World Applications:
//   - Comparing compressed or run-length-encoded telemetry/log streams where
//     wildcard gaps represent redacted or elided spans of unknown content.
//   - Bioinformatics sequence alignment with ambiguous gap-length annotations
//     and fuzzy pattern equivalence checking in static analysis / regex engines.
//   - Verifying equivalence of obfuscated or abbreviated identifier templates.
//
// Related LeetCode Problems:
//   - #408 Valid Word Abbreviation [done] (matching a literal string against a numeric-length abbreviation)
//   - #44 Wildcard Matching [done] (2D DP matching strings with variable-length wildcards)
//   - #10 Regular Expression Matching [done] (memoized DFS over two string indices with variable consumption)
//   - #97 Interleaving String [done] (2D state DP tracking aligned positions across strings)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

// Maximum length represented by a run of at most 3 digits ("999").
constexpr int kMaxDiff = 999;
constexpr std::ptrdiff_t kDiffOffset = kMaxDiff;
constexpr std::size_t kDiffSpan =
    (2uz * static_cast<std::size_t>(kMaxDiff)) + 1uz;

// Returns true iff `ch` is an ASCII decimal digit (`'0'`..`'9'`).
// Avoids locale dependence and signed-char UB from `<cctype>` `std::isdigit`.
bool isAsciiDigit(char ch) { return ch >= '0' && ch <= '9'; }

// Returns the exclusive end index of the contiguous digit run starting at
// `start` inside `text`.
std::size_t scanDigitRunEnd(std::string_view text, std::size_t start) {
  std::size_t pos = start;
  while (pos < text.size() && isAsciiDigit(text[pos])) {
    ++pos;
  }
  return pos;
}

// Generates all distinct positive lengths that could have been encoded into
// `digits` by splitting `digits` into one or more non-empty numeric substrings
// and summing their decimal values (e.g., `"12"` -> `{3, 12}`).
//
// Time: O(2^d) for a digit run of length `d` (here d <= 3, at most 4 outputs).
std::vector<int> generatePossibleLengths(std::string_view digits) {
  int whole_value = 0;
  for (const char ch : digits) {
    whole_value = (whole_value * 10) + (ch - '0');
  }

  std::vector<int> lengths;
  lengths.push_back(whole_value);

  // Try every split point dividing `digits` into a non-empty prefix and suffix,
  // combining all possible length sums from both halves.
  for (std::size_t split = 1; split < digits.size(); ++split) {
    const auto left_vals = generatePossibleLengths(digits.substr(0, split));
    const auto right_vals = generatePossibleLengths(digits.substr(split));
    for (const int lv : left_vals) {
      for (const int rv : right_vals) {
        lengths.push_back(lv + rv);
      }
    }
  }

  // Deduplicate sums (e.g., `"112"` yields `1 + 12 = 13` and `11 + 2 = 13`).
  std::ranges::sort(lengths);
  const auto duplicates = std::ranges::unique(lengths);
  lengths.erase(duplicates.begin(), duplicates.end());
  return lengths;
}

// Encapsulates memoized DFS state for checking whether two encoded strings
// share a common original string.
class MatchContext {
 public:
  MatchContext(std::string_view s1, std::string_view s2)
      : s1_(s1),
        s2_(s2),
        failed_(((s1.size() + 1) * (s2.size() + 1)) * kDiffSpan, 0) {}

  // Returns true iff suffixes `s1_[i..]` and `s2_[j..]` can represent the same
  // original suffix given wildcard offset `diff` (`s2` leads `s1` by `diff`).
  bool canMatch(std::size_t i, std::size_t j, int diff) {
    if (i == s1_.size() && j == s2_.size()) {
      return diff == 0;
    }

    const std::size_t key = stateIndex(i, j, diff);
    if (failed_[key] != 0) {
      return false;
    }

    bool matched = false;
    if (i < s1_.size() && isAsciiDigit(s1_[i])) {
      matched = tryDigitRunS1(i, j, diff);
    } else if (j < s2_.size() && isAsciiDigit(s2_[j])) {
      matched = tryDigitRunS2(i, j, diff);
    } else {
      matched = matchLetters(i, j, diff);
    }

    // Any state returning false is permanently pruned on future visits.
    if (!matched) {
      failed_[key] = 1;
    }
    return matched;
  }

 private:
  // Maps `(i, j, diff)` into a flat index in `failed_`.
  [[nodiscard]] std::size_t stateIndex(std::size_t i, std::size_t j,
                                       int diff) const {
    const std::size_t cols = s2_.size() + 1;
    const auto diff_bucket =
        static_cast<std::size_t>(static_cast<std::ptrdiff_t>(diff) + kDiffOffset);
    return (((i * cols) + j) * kDiffSpan) + diff_bucket;
  }

  // Expands the entire digit run starting at `s1_[i]` into every valid length.
  bool tryDigitRunS1(std::size_t i, std::size_t j, int diff) {
    const std::size_t run_end = scanDigitRunEnd(s1_, i);
    const auto lengths = generatePossibleLengths(s1_.substr(i, run_end - i));
    // `s1` advances by `len` wildcard characters, decreasing `diff`.
    return std::ranges::any_of(
        lengths, [&](int len) { return canMatch(run_end, j, diff - len); });
  }

  // Expands the entire digit run starting at `s2_[j]` into every valid length.
  bool tryDigitRunS2(std::size_t i, std::size_t j, int diff) {
    const std::size_t run_end = scanDigitRunEnd(s2_, j);
    const auto lengths = generatePossibleLengths(s2_.substr(j, run_end - j));
    // `s2` advances by `len` wildcard characters, increasing `diff`.
    return std::ranges::any_of(
        lengths, [&](int len) { return canMatch(i, run_end, diff + len); });
  }

  // Handles the case where neither side's next character is a digit.
  bool matchLetters(std::size_t i, std::size_t j, int diff) {
    if (diff == 0) {
      // Both strings are aligned: the next literal characters must match.
      if (i < s1_.size() && j < s2_.size() && s1_[i] == s2_[j]) {
        return canMatch(i + 1, j + 1, 0);
      }
      return false;
    }
    if (diff < 0) {
      // `s1` leads via wildcards: consume one literal from `s2` to catch up.
      if (j < s2_.size()) {
        return canMatch(i, j + 1, diff + 1);
      }
      return false;
    }
    // `diff > 0`: `s2` leads via wildcards: consume one literal from `s1`.
    if (i < s1_.size()) {
      return canMatch(i + 1, j, diff - 1);
    }
    return false;
  }

  std::string_view s1_;
  std::string_view s2_;
  std::vector<std::uint8_t> failed_;
};

class Solution {
 public:
  // Returns true if there exists an original lowercase string that can be
  // encoded as both `s1` and `s2`.
  //
  // Time: O(N * M * D), Space: O(N * M * D), where N = s1.size(),
  // M = s2.size(), and D = 999 (max value of a 3-digit run).
  bool possiblyEquals(std::string_view s1, std::string_view s2) {
    MatchContext ctx(s1, s2);
    return ctx.canMatch(0, 0, 0);
  }
};

LC_PROBLEM(2060, "Check If An Original String Exists Given Two Encoded Strings",
           "dp", ctx) {
  Solution sol;

  ctx.test("example_1_internationalization", [&] {
    ctx.expect_true(sol.possiblyEquals("internationalization", "i18n"));
  });

  ctx.test("example_2_leetcode", [&] {
    // "l" + 1 + 2 + 3 + "e" (length 8) matches 4 + 4 (length 8).
    ctx.expect_true(sol.possiblyEquals("l123e", "44"));
  });

  ctx.test("example_3_mismatched_first_letter", [&] {
    ctx.expect_false(sol.possiblyEquals("a5b", "c5b"));
  });

  ctx.test("example_4_split_three_digits", [&] {
    // 1 + 12 + "s" matches "g" + 8 + 4 + 1 (e.g., "gaaaaaaaaaaaas").
    ctx.expect_true(sol.possiblyEquals("112s", "g841"));
  });

  ctx.test("example_5_length_mismatch", [&] {
    ctx.expect_false(sol.possiblyEquals("ab", "a2"));
  });

  ctx.test("python_build_case", [&] {
    // "94" -> {13, 94}, "14" -> {5, 14}: disjoint lengths after 'x'.
    ctx.expect_false(sol.possiblyEquals("x94", "x14"));
  });

  ctx.test("generate_possible_lengths_helper", [&] {
    ctx.expect_eq(generatePossibleLengths("12"), std::vector<int>{3, 12});
    ctx.expect_eq(generatePossibleLengths("123"),
                  std::vector<int>{6, 15, 24, 123});
    ctx.expect_eq(generatePossibleLengths("112"), std::vector<int>{4, 13, 112});
  });

  ctx.test("large_three_digit_overlap", [&] {
    // Both can encode "a" + 998*'b' + "a" (length 1000).
    ctx.expect_true(sol.possiblyEquals("999a", "a999"));
    ctx.expect_true(sol.possiblyEquals("111", "3"));
    ctx.expect_false(sol.possiblyEquals("a2b", "a2c"));
  });

  ctx.test("multi_block_stress", [&] {
    ctx.expect_true(sol.possiblyEquals("999a999b999c", "999a999b999c"));
    ctx.expect_false(sol.possiblyEquals("999a999b999c", "999a999b999d"));
  });
}

}  // namespace
