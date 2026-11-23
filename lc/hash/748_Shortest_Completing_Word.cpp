// LeetCode 748: Shortest Completing Word
// https://leetcode.com/problems/shortest-completing-word/
//
// Problem:
//   Given a string `licensePlate` (containing letters, digits, and spaces) and
//   an array of lowercase words `words`, find the shortest word in `words`
//   that contains all letters from `licensePlate` (ignoring digits, spaces,
//   and letter case, while respecting letter multiplicities). If multiple
//   words tie for the shortest length, return the one that appears earliest in
//   `words`. It is guaranteed that at least one completing word exists.
//   Constraints: 1 <= licensePlate.length <= 7, 1 <= words.length <= 1000,
//   1 <= words[i].length <= 15.
//
// Examples:
//   Example 1:
//     Input:  licensePlate = "1s3 PSt", words = ["step", "steps", "stripe",
//             "stepple"]
//     Output: "steps"
//     Explanation:
//       - countLetters("1s3 PSt") ignores digits and spaces and lowercases letters
//         to produce required = {'p': 1, 's': 2, 't': 1}.
//       - Check "step": have['s'] = 1 < required['s'] = 2, so covers returns false.
//       - Check "steps": have = {'e': 1, 'p': 1, 's': 2, 't': 1} covers required,
//         setting best = "steps" (length 5).
//       - Skip "stripe" (length 6 >= 5) and "stepple" (length 7 >= 5) via the
//         length check and return "steps".
//
//   Example 2:
//     Input:  licensePlate = "1s3 456", words = ["looks", "pest", "stew", "show"]
//     Output: "pest"
//     Explanation:
//       - countLetters("1s3 456") extracts required = {'s': 1}.
//       - "looks" (length 5) covers {'s': 1}, setting best = "looks".
//       - "pest" (length 4 < 5) also covers {'s': 1}, updating best = "pest".
//       - "stew" and "show" both have length 4 >= best.size() (4), so both are
//         skipped to preserve the earliest shortest match "pest".
//
//   Example 3:
//     Input:  licensePlate = "PP", words = ["pair", "supper", "apple"]
//     Output: "apple"
//     Explanation:
//       - countLetters("PP") produces required = {'p': 2}.
//       - "pair" has only have['p'] = 1 < 2, failing covers.
//       - "supper" supplies have['p'] = 2, setting best = "supper" (length 6).
//       - "apple" (length 5 < 6) also supplies have['p'] = 2, updating best to
//         "apple" before returning it.
//
// Algorithm:
//   1. Extract a 26-element case-insensitive letter frequency histogram
//      `required` from `licensePlate`, ignoring non-alphabetic characters.
//   2. Scan each candidate `word` in `words` left-to-right:
//        - Prune early if we already found a completing word `best` with
//          `word.size() >= best.size()` (strictly shorter is required because
//          ties must keep the earliest match).
//        - Build `word`'s 26-element letter histogram and check whether
//          `have[c] >= required[c]` for all 26 letters.
//        - If covered, record `best = word`.
//   3. Return `best`.
//   Time Complexity: O(|P| + sum(|words[i]|) + 26 * N), where |P| is the
//   license plate length and N = words.size().
//   Space Complexity: O(1) auxiliary space (two fixed 26-int frequency arrays).
//
// Data Structures:
//   - Fixed-size frequency arrays (`std::array<int, 26>`): direct-address
//     character histograms for O(1) per-character counting and cache-friendly
//     multiset inclusion checks without heap allocations.
//
// Real-World Applications:
//   - Scrabble / anagram solvers and word-game suggestion engines finding the
//     shortest valid dictionary word covering a rack of required tiles.
//   - Feature/capability matching in package managers and job schedulers
//     selecting the minimal footprint template satisfying required resource
//     multiplicities.
//   - Inventory kit matching (finding the smallest bundle containing a set of
//     required component counts).
//
// Related LeetCode Problems:
//   - #383 Ransom Note [done] (multiset letter coverage using a 26-element histogram)
//   - #242 Valid Anagram [done] (exact 26-letter frequency equality check)
//   - #1160 Find Words That Can Be Formed by Characters [new] (reverse inclusion check)
//   - #916 Word Subsets [done] (maximum letter-frequency requirement across words)

#include "framework/lc_framework.h"

#include <array>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the earliest shortest word in `words` containing every alphabetic
  // letter of `license_plate` (case-insensitive, with multiplicities).
  // Time: O(|P| + sum(|words[i]|) + 26 * N), Space: O(1).
  std::string shortestCompletingWord(std::string_view license_plate,
                                     const std::vector<std::string>& words) {
    const LetterCounts required = countLetters(license_plate);
    std::string_view best;

    for (const std::string& word : words) {
      // Strictly shorter length is required to beat an existing match so ties
      // automatically preserve the earliest occurrence in `words`.
      if (!best.empty() && word.size() >= best.size()) {
        continue;
      }

      if (covers(countLetters(word), required)) {
        best = word;
      }
    }

    return std::string(best);
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;
  using LetterCounts = std::array<int, kAlphabetSize>;

  // Builds a case-insensitive 26-letter histogram, ignoring non-letters.
  [[nodiscard]] static LetterCounts countLetters(std::string_view text) {
    LetterCounts freq{};
    for (const char ch : text) {
      const auto uch = static_cast<unsigned char>(ch);
      if (std::isalpha(uch) != 0) {
        const auto lower = static_cast<unsigned char>(std::tolower(uch));
        const auto idx =
            static_cast<std::size_t>(lower - static_cast<unsigned char>('a'));
        ++freq[idx];
      }
    }
    return freq;
  }

  // Returns true iff `have` supplies at least `need[i]` copies of every letter.
  [[nodiscard]] static bool covers(const LetterCounts& have,
                                   const LetterCounts& need) noexcept {
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      if (have[i] < need[i]) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(748, "Shortest Completing Word", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    const std::vector<std::string> words = {"step", "steps", "stripe",
                                            "stepple"};
    ctx.expect_eq(sol.shortestCompletingWord("1s3 PSt", words), "steps");
  });

  ctx.test("leetcode_example_2_tie_breaks_earliest", [&] {
    const std::vector<std::string> words = {"looks", "pest", "stew", "show"};
    ctx.expect_eq(sol.shortestCompletingWord("1s3 456", words), "pest");
  });

  ctx.test("duplicate_letter_multiplicity", [&] {
    const std::vector<std::string> words = {"pair", "supper", "apple"};
    ctx.expect_eq(sol.shortestCompletingWord("PP", words), "apple");
  });

  ctx.test("mixed_case_and_digits", [&] {
    const std::vector<std::string> words = {
        "suggest", "letter", "of",      "husband", "easy",
        "education", "drug", "prevent", "writer",  "old"};
    ctx.expect_eq(sol.shortestCompletingWord("Ah71752", words), "husband");
  });
}

}  // namespace
