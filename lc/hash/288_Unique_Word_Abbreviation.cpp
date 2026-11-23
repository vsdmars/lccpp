// LeetCode 288: Unique Word Abbreviation
// https://leetcode.com/problems/unique-word-abbreviation/
//
// Problem:
//   The abbreviation of a word is formed by keeping its first and last
//   characters and replacing the middle characters with their count:
//   `<first><len - 2><last>` when `word.size() > 2`, or the word itself when
//   `word.size() <= 2` (for example, `"it" -> "it"`, `"dog" -> "d1g"`,
//   `"internationalization" -> "i18n"`). Given a dictionary of words, design a
//   data structure `ValidWordAbbr` that supports `isUnique(word)`, returning
//   `true` if and only if either:
//     1. No word in the dictionary has the same abbreviation as `word`, or
//     2. Every word in the dictionary sharing that abbreviation is equal to
//        `word` itself.
//
// Algorithm:
//   Precompute each dictionary word's abbreviation in the constructor and
//   store a single representative word per abbreviation in a hash map. If two
//   distinct dictionary words produce the same abbreviation, mark that
//   abbreviation as ambiguous (`std::nullopt`) so it can never be unique for
//   any query. Duplicate occurrences of the exact same word in the dictionary
//   preserve that single representative.
//   To answer `isUnique(word)`, compute its abbreviation and look it up in
//   O(L) time: return `true` if the abbreviation is absent, or if the stored
//   representative is non-null and equals `word`.
//   Time Complexity: O(N * L) preprocessing for N words of average length L;
//                    O(L) per `isUnique` query.
//   Space Complexity: O(N * L) to store abbreviations and representatives.
//
// Data Structures:
//   - `std::unordered_map<std::string, std::optional<std::string>>`: maps each
//     dictionary abbreviation to its sole distinct dictionary word, or
//     `std::nullopt` once a collision between two distinct words is detected.
//     Using `std::optional` safely handles empty strings without needing a
//     magic sentinel value.
//
// Real-World Applications:
//   - Numeronym and identifier collision detection (e.g., i18n, l10n, k8s) in
//     documentation linters and API naming validators.
//   - Command-line flag and URL-slug disambiguation where short aliases are
//     allowed only when uniquely resolvable.
//   - Symbol table compression and auto-completion engines checking whether a
//     shorthand uniquely identifies a canonical token.
//
// Related LeetCode Problems:
//   - #320 Generalized Abbreviation [new] (backtracking generation of all word abbreviations)
//   - #408 Valid Word Abbreviation [done] (matching a word against an abbreviation string)
//   - #411 Minimum Unique Word Abbreviation [new] (shortest abbreviation unique against a dictionary)
//   - #527 Word Abbreviation [new] (assigning minimal unique abbreviations to a word list)

#include "framework/lc_framework.h"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

// Indexes a dictionary by word abbreviation to answer uniqueness queries in
// O(L) time per query.
class ValidWordAbbr {
 public:
  // Precomputes abbreviation ownership for all words in `dictionary`.
  // Time: O(sum of word lengths), Space: O(sum of distinct word lengths).
  explicit ValidWordAbbr(const std::vector<std::string>& dictionary) {
    abbr_to_word_.reserve(dictionary.size());
    for (const std::string& word : dictionary) {
      const auto [it, inserted] =
          abbr_to_word_.try_emplace(abbreviate(word), word);
      // If a different dictionary word already maps to this abbreviation, mark
      // the abbreviation permanently ambiguous with std::nullopt.
      if (!inserted && it->second.has_value() && *it->second != word) {
        it->second.reset();
      }
    }
  }

  // Returns true iff no *other* distinct word in the dictionary shares the
  // same abbreviation as `word`.
  // Time: O(word.size()), Space: O(word.size()) for the abbreviation key.
  [[nodiscard]] bool isUnique(std::string_view word) const {
    const std::string abbr = abbreviate(word);
    const auto it = abbr_to_word_.find(abbr);
    if (it == abbr_to_word_.end()) {
      // No dictionary word shares this abbreviation.
      return true;
    }
    // Unique iff the dictionary only contains this exact word under `abbr`.
    return it->second.has_value() && *it->second == word;
  }

 private:
  // Computes the `<first><middle_count><last>` abbreviation for words longer
  // than two characters, leaving shorter words unchanged.
  [[nodiscard]] static std::string abbreviate(std::string_view word) {
    if (word.size() <= 2uz) {
      return std::string(word);
    }
    const std::string mid = std::to_string(word.size() - 2uz);
    std::string abbr;
    abbr.reserve(mid.size() + 2uz);
    abbr.push_back(word.front());
    abbr.append(mid);
    abbr.push_back(word.back());
    return abbr;
  }

  // Maps abbreviation -> sole matching dictionary word, or std::nullopt if 2+
  // distinct dictionary words collide on the same abbreviation.
  std::unordered_map<std::string, std::optional<std::string>> abbr_to_word_;
};

LC_PROBLEM(288, "Unique Word Abbreviation", "hash", ctx) {
  ctx.test("leetcode_example", [&] {
    const ValidWordAbbr vwa({"deer", "door", "cake", "card"});
    ctx.expect_false(vwa.isUnique("dear"));
    ctx.expect_true(vwa.isUnique("cart"));
    ctx.expect_false(vwa.isUnique("cane"));
    ctx.expect_true(vwa.isUnique("make"));
    ctx.expect_true(vwa.isUnique("cake"));
    ctx.expect_false(vwa.isUnique("door"));
  });

  ctx.test("single_word_dictionary", [&] {
    const ValidWordAbbr vwa({"hello"});
    ctx.expect_true(vwa.isUnique("hello"));
    ctx.expect_false(vwa.isUnique("hallo"));
    ctx.expect_true(vwa.isUnique("world"));
  });

  ctx.test("duplicate_words_in_dictionary", [&] {
    const ValidWordAbbr vwa({"cake", "cake", "a", "a"});
    ctx.expect_true(vwa.isUnique("cake"));
    ctx.expect_false(vwa.isUnique("cane"));
    ctx.expect_true(vwa.isUnique("a"));
  });

  ctx.test("short_and_empty_words", [&] {
    const ValidWordAbbr vwa({"a", "", "it"});
    ctx.expect_true(vwa.isUnique("a"));
    ctx.expect_true(vwa.isUnique(""));
    ctx.expect_true(vwa.isUnique("it"));
    ctx.expect_true(vwa.isUnique("is"));
  });

  ctx.test("long_numeronyms", [&] {
    const ValidWordAbbr vwa({"internationalization", "localization"});
    ctx.expect_true(vwa.isUnique("internationalization"));
    ctx.expect_false(vwa.isUnique("i123456789012345678n"));
    ctx.expect_true(vwa.isUnique("localization"));
  });
}

}  // namespace
