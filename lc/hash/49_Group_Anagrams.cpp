// LeetCode 49: Group Anagrams
// https://leetcode.com/problems/group-anagrams/
//
// Problem:
//   Given an array of strings `strs` consisting of lowercase English letters,
//   group all anagrams together into sublists and return the resulting list of
//   groups in any order. Two strings are anagrams if they contain the exact
//   same characters with the exact same multiplicities.
//   Constraints: 1 <= strs.length <= 10^4, 0 <= strs[i].length <= 100.
//
// Examples:
//   Example 1:
//     Input:  strs = ["eat", "tea", "tan", "ate", "nat", "bat"]
//     Output: [["ate", "eat", "tea"], ["bat"], ["nat", "tan"]]
//     Explanation:
//       - For "eat", "tea", and "ate", counting letters 'a'..'z' yields the
//         identical 26-element FreqKey with 1 at indices 0 ('a'), 4 ('e'), and 19
//         ('t'), grouping all three into one bucket.
//       - For "tan" and "nat", the FreqKey has 1 at indices 0 ('a'), 13 ('n'), and
//         19 ('t'), placing both into a second bucket.
//       - For "bat", the FreqKey has 1 at indices 0 ('a'), 1 ('b'), and 19 ('t'),
//         creating a third bucket containing ["bat"].
//       - Move all buckets from the hash map into result, producing groups
//         equivalent (up to group and element ordering) to [["ate", "eat", "tea"],
//         ["bat"], ["nat", "tan"]].
//
//   Example 2:
//     Input:  strs = ["boo", "bob"]
//     Output: [["bob"], ["boo"]]
//     Explanation:
//       - Build FreqKey for "boo" with 1 at index 1 ('b') and 2 at index 14 ('o'),
//         inserting "boo" into its bucket.
//       - Build FreqKey for "bob" with 2 at index 1 ('b') and 1 at index 14 ('o'),
//         which produces a different key and creates a separate bucket for "bob".
//       - Move both singleton buckets into result, yielding [["bob"], ["boo"]]
//         after canonical sorting.
//
//   Example 3:
//     Input:  strs = ["", ""]
//     Output: [["", ""]]
//     Explanation:
//       - For the first "", the character loop performs zero iterations and inserts
//         "" under the all-zero 26-element FreqKey.
//       - For the second "", the same all-zero FreqKey matches the existing bucket,
//         appending "" and returning [["", ""]].
//
// Algorithm:
//   Two strings belong to the same anagram group if and only if they share the
//   same canonical signature.
//   1. Frequency-Vector Key (primary, `groupAnagrams`):
//      Count occurrences of each lowercase letter ('a'..'z') in a fixed-size
//      26-element array and hash that array directly into an unordered_map.
//      This avoids per-string sorting.
//      Time:  O(N * K), where N = strs.size() and K = max string length.
//      Space: O(N * K) to store all grouped strings and map entries.
//   2. Sorted-String Key (secondary, `groupAnagramsSorted`):
//      Sort a copy of each string lexicographically to obtain its canonical
//      representative and group by that sorted string.
//      Time:  O(N * K log K)
//      Space: O(N * K)
//
// Data Structures:
//   - std::array<int, 26>: Fixed-size character frequency signature per string.
//   - std::unordered_map<FreqKey, std::vector<std::string>, FreqHash>: Maps
//     each canonical frequency signature to its bucket of original strings.
//   - std::unordered_map<std::string, std::vector<std::string>>: Alternative
//     map keyed by each string's sorted representation.
//
// Real-World Applications:
//   - Deduplicating and clustering records by order-invariant feature bags
//     (e.g., canonicalizing unordered tag sets or query parameter sets).
//   - Word-game solvers, spell-checkers, and crossword/Scrabble dictionaries
//     indexed by letter multiset.
//   - Detecting structurally equivalent signatures in static analysis and
//     compiler common-subexpression grouping for commutative operators.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (pairwise check of the same frequency invariant)
//   - #249 Group Shifted Strings [done] (grouping strings by a different canonical signature)
//   - #438 Find All Anagrams in a String [done] (sliding-window frequency matching)
//   - #2273 Find Resultant Array After Removing Anagrams [new] (adjacent anagram filtering)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

// 26-letter frequency signature uniquely identifying an anagram equivalence class.
using FreqKey = std::array<int, kAlphabetSize>;

// FNV-1a 64-bit hash functor for fixed-size 26-letter frequency arrays.
struct FreqHash {
  static constexpr std::uint64_t kFnvOffsetBasis = 14695981039346656037ULL;
  static constexpr std::uint64_t kFnvPrime = 1099511628211ULL;

  std::size_t operator()(const FreqKey& freq) const noexcept {
    std::uint64_t hash = kFnvOffsetBasis;
    for (int count : freq) {
      hash ^= static_cast<std::uint64_t>(count);
      hash *= kFnvPrime;
    }
    return static_cast<std::size_t>(hash);
  }
};

class Solution {
 public:
  // Groups anagrams using a 26-letter frequency array as the hash key.
  // Invariant: two strings collide onto the same FreqKey iff they are anagrams.
  // Time: O(N * K), Space: O(N * K).
  std::vector<std::vector<std::string>> groupAnagrams(
      const std::vector<std::string>& strs) {
    std::unordered_map<FreqKey, std::vector<std::string>, FreqHash> buckets;
    buckets.reserve(strs.size());

    for (const std::string& word : strs) {
      // Build the canonical 26-letter frequency vector for `word`.
      FreqKey freq{};
      for (char ch : word) {
        ++freq[static_cast<std::size_t>(ch - 'a')];
      }
      buckets[freq].push_back(word);
    }

    // Move each completed anagram bucket into the output vector.
    std::vector<std::vector<std::string>> result;
    result.reserve(buckets.size());
    for (auto& entry : buckets) {
      result.push_back(std::move(entry.second));
    }
    return result;
  }

  // Alternative approach: groups anagrams by sorting each string into its
  // lexicographically smallest permutation as the hash key.
  // Time: O(N * K log K), Space: O(N * K).
  std::vector<std::vector<std::string>> groupAnagramsSorted(
      const std::vector<std::string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> buckets;
    buckets.reserve(strs.size());

    for (const std::string& word : strs) {
      std::string canonical = word;
      std::ranges::sort(canonical);
      buckets[std::move(canonical)].push_back(word);
    }

    std::vector<std::vector<std::string>> result;
    result.reserve(buckets.size());
    for (auto& entry : buckets) {
      result.push_back(std::move(entry.second));
    }
    return result;
  }
};

// Sorts each group and the outer list of groups so unordered outputs can be
// compared deterministically in unit tests.
std::vector<std::vector<std::string>> normalizeGroups(
    std::vector<std::vector<std::string>> groups) {
  for (auto& group : groups) {
    std::ranges::sort(group);
  }
  std::ranges::sort(groups);
  return groups;
}

LC_PROBLEM(49, "Group Anagrams", "hash", ctx) {
  Solution sol;

  ctx.test("classic_example", [&] {
    const std::vector<std::string> input = {
        "eat", "tea", "tan", "ate", "nat", "bat"};
    const std::vector<std::vector<std::string>> expected = {
        {"ate", "eat", "tea"}, {"bat"}, {"nat", "tan"}};
    ctx.expect_eq(normalizeGroups(sol.groupAnagrams(input)), expected);
    ctx.expect_eq(normalizeGroups(sol.groupAnagramsSorted(input)), expected);
  });

  ctx.test("docstring_boo_bob", [&] {
    const std::vector<std::string> input = {"boo", "bob"};
    const std::vector<std::vector<std::string>> expected = {{"bob"}, {"boo"}};
    ctx.expect_eq(normalizeGroups(sol.groupAnagrams(input)), expected);
    ctx.expect_eq(normalizeGroups(sol.groupAnagramsSorted(input)), expected);
  });

  ctx.test("source_build_sample", [&] {
    const std::vector<std::string> input = {
        "cab", "pug", "pei", "nay", "ron", "rae", "ems", "ida", "mes"};
    const std::vector<std::vector<std::string>> expected = {
        {"cab"}, {"ems", "mes"}, {"ida"}, {"nay"},
        {"pei"}, {"pug"},        {"rae"}, {"ron"}};
    ctx.expect_eq(normalizeGroups(sol.groupAnagrams(input)), expected);
    ctx.expect_eq(normalizeGroups(sol.groupAnagramsSorted(input)), expected);
  });

  ctx.test("empty_strings_and_single", [&] {
    const std::vector<std::string> empty_list = {};
    ctx.expect_eq(sol.groupAnagrams(empty_list),
                  std::vector<std::vector<std::string>>{});

    const std::vector<std::string> two_empties = {"", ""};
    const std::vector<std::vector<std::string>> expected_empties = {{"", ""}};
    ctx.expect_eq(normalizeGroups(sol.groupAnagrams(two_empties)),
                  expected_empties);
    ctx.expect_eq(normalizeGroups(sol.groupAnagramsSorted(two_empties)),
                  expected_empties);

    const std::vector<std::string> single = {"a"};
    const std::vector<std::vector<std::string>> expected_single = {{"a"}};
    ctx.expect_eq(normalizeGroups(sol.groupAnagrams(single)), expected_single);
  });
}

}  // namespace
