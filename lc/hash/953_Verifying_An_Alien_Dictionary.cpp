// LeetCode 953: Verifying An Alien Dictionary
// https://leetcode.com/problems/verifying-an-alien-dictionary/
//
// Problem:
//   In an alien language using the 26 lowercase English letters, the alphabet
//   ordering is given by a 26-character permutation `order`. Given a sequence
//   of `words`, return `true` if and only if `words` are sorted
//   lexicographically according to `order`. By standard lexicographical rules,
//   if one word is a strict prefix of another, the shorter word must appear
//   first (`"app" < "apple"`).
//   Constraints: 1 <= words.length <= 100, 1 <= words[i].length <= 20,
//   order.length == 26.
//
// Algorithm:
//   Direct-Indexed Rank Table + Adjacent Pair Comparison:
//   1. Precompute an array `rank[26]` mapping each lowercase letter to its
//      position in `order` so character rank lookup is O(1).
//   2. Because lexicographical order is transitive, the entire array is sorted
//      iff every adjacent pair `(words[i], words[i + 1])` satisfies
//      `words[i] <= words[i + 1]` under `rank`.
//   3. For each adjacent pair `(first, second)`, scan characters left to right
//      up to `min(first.size(), second.size())`. At the first differing
//      character, the pair's ordering is decided solely by `rank[first[j]] <
//      rank[second[j]]`. If all shared prefix characters match, the pair is
//      ordered iff `first.size() <= second.size()`.
//   Time Complexity: O(|Sigma| + M), where |Sigma| = 26 and M is the total
//   number of characters across all words.
//   Space Complexity: O(|Sigma|) = O(1) for the 26-element rank table.
//
// Data Structures:
//   - Direct-address lookup array (`std::array<int, 26>`): maps each ASCII
//     lowercase letter `'a'..'z'` to its alien alphabetical rank in O(1) time.
//
// Real-World Applications:
//   - Database collation verification (ICU / PostgreSQL / MySQL custom locales):
//     validating sorted index keys under language-specific collation rules.
//   - Custom sort-key comparators in search engines, file explorers, and
//     internationalized UI tables.
//   - Data pipeline validation: asserting that partitioned or pre-sorted text
//     streams respect a domain-defined custom symbol ordering.
//
// Related LeetCode Problems:
//   - #269 Alien Dictionary [done] (inverse problem: infer the alphabet order from sorted words via topological sort)
//   - #791 Custom Sort String [done] (sort a single string according to a custom alphabet order)
//   - #944 Delete Columns to Make Sorted [new] (checking lexicographical sortedness across columns)
//   - #1122 Relative Sort Array [new] (sorting elements by a custom rank map)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `words` are sorted lexicographically under `order`.
  // Time: O(26 + total characters), Space: O(1).
  bool isAlienSorted(const std::vector<std::string>& words,
                     std::string_view order) {
    // Map each letter to its 0-based index in the alien alphabet for O(1)
    // rank comparisons.
    RankTable rank{};
    for (std::size_t pos = 0uz; pos < order.size(); ++pos) {
      rank[letter_index(order[pos])] = static_cast<int>(pos);
    }

    // Transitivity of lexicographical order means checking adjacent pairs is
    // both necessary and sufficient.
    for (std::size_t i = 1uz; i < words.size(); ++i) {
      if (!is_pair_sorted(words[i - 1uz], words[i], rank)) {
        return false;
      }
    }
    return true;
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;
  using RankTable = std::array<int, kAlphabetSize>;

  static std::size_t letter_index(char ch) {
    return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                    static_cast<unsigned char>('a'));
  }

  // Compares `first <= second` under the alien `rank` table.
  static bool is_pair_sorted(std::string_view first, std::string_view second,
                             const RankTable& rank) {
    const std::size_t limit = std::min(first.size(), second.size());
    for (std::size_t idx = 0uz; idx < limit; ++idx) {
      if (first[idx] != second[idx]) {
        // First differing character completely determines lexicographical
        // order; remaining characters need not be inspected.
        return rank[letter_index(first[idx])] < rank[letter_index(second[idx])];
      }
    }
    // One word is a prefix of the other: shorter word must come first.
    return first.size() <= second.size();
  }
};

LC_PROBLEM(953, "Verifying An Alien Dictionary", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_sorted", [&] {
    const std::vector<std::string> words = {"hello", "leetcode"};
    ctx.expect_true(sol.isAlienSorted(words, "hlabcdefgijkmnopqrstuvwxyz"));
  });

  ctx.test("example_2_unsorted_middle_char", [&] {
    const std::vector<std::string> words = {"word", "world", "row"};
    ctx.expect_false(sol.isAlienSorted(words, "worldabcefghijkmnpqstuvxyz"));
  });

  ctx.test("example_3_longer_prefix_first_invalid", [&] {
    const std::vector<std::string> words = {"apple", "app"};
    ctx.expect_false(sol.isAlienSorted(words, "abcdefghijklmnopqrstuvwxyz"));
  });

  ctx.test("python_build_custom_orders", [&] {
    ctx.expect_true(
        sol.isAlienSorted({"kuvp", "q"}, "ngxlkthsjuoqcpavbfdermiywz"));
    ctx.expect_true(
        sol.isAlienSorted({"eeec", "eeed"}, "hlabcdefgijkmnopqrstuvwxyz"));
    ctx.expect_true(
        sol.isAlienSorted({"ehlc", "elhd"}, "hlabcdefgijkmnopqrstuvwxyz"));
  });

  ctx.test("edge_cases_single_and_equal_and_prefix", [&] {
    ctx.expect_true(sol.isAlienSorted({"solo"}, "abcdefghijklmnopqrstuvwxyz"));
    ctx.expect_true(
        sol.isAlienSorted({"app", "app"}, "abcdefghijklmnopqrstuvwxyz"));
    ctx.expect_true(
        sol.isAlienSorted({"app", "apple"}, "abcdefghijklmnopqrstuvwxyz"));
  });
}

}  // namespace
