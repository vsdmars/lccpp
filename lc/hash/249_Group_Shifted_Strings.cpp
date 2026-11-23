// LeetCode 249: Group Shifted Strings
// https://leetcode.com/problems/group-shifted-strings/
//
// Problem:
//   We can perform a cyclic "right shift" on a lowercase English string by
//   replacing every letter with its successive letter in the alphabet
//   ('a' -> 'b', ..., 'z' -> 'a'). Given a list of strings, group all strings
//   that belong to the same shifting sequence (i.e., one can be transformed
//   into another by applying some number of cyclic shifts) and return the
//   groups in any order.
//   Constraints: 1 <= strings.length <= 200, 1 <= strings[i].length <= 50,
//   lowercase English letters only.
//
// Examples:
//   Example 1:
//     Input:  strings = ["abc", "bcd", "acef", "xyz", "az", "ba", "a", "z"]
//     Output: [["a", "z"], ["abc", "bcd", "xyz"], ["acef"], ["az", "ba"]]
//     Explanation:
//       - Compute `shiftKey` for each string relative to its first character modulo
//         26: `"abc"`, `"bcd"`, and `"xyz"` all normalize to `"abc"` (offsets `0,
//         1, 2`).
//       - `"acef"` maps to `"acef"` (offsets `0, 2, 4, 5`), while `"a"` and `"z"`
//         both map to `"a"` (offset `0`).
//       - `"az"` maps to `"az"` (`('z' - 'a' + 26) % 26 = 25`), and `"ba"` also
//         maps to `"az"` (`('a' - 'b' + 26) % 26 = 25`).
//       - Move the four `buckets` vectors into `result`, forming the equivalence
//         groups `["a", "z"]`, `["abc", "bcd", "xyz"]`, `["acef"]`, and `["az",
//         "ba"]` (shown sorted).
//
//   Example 2:
//     Input:  strings = ["az", "ba"]
//     Output: [["az", "ba"]]
//     Explanation:
//       - For `"az"`, `shiftKey` uses `base = 'a'` to compute offsets `(0, 25)` and
//         canonical key `"az"`.
//       - For `"ba"`, `shiftKey` uses `base = 'b'` to compute `(('b' - 'b' + 26) %
//         26 = 0, ('a' - 'b' + 26) % 26 = 25)`, producing the same key `"az"`.
//       - Both strings append to `buckets["az"]`, returning the single group
//         `[["az", "ba"]]`.
//
//   Example 3:
//     Input:  strings = ["ab", "ab", "cd"]
//     Output: [["ab", "ab", "cd"]]
//     Explanation:
//       - `shiftKey("ab")` produces `"ab"` (offsets `0, 1`), and `shiftKey("cd")`
//         also produces `"ab"` (offsets `0, 1`).
//       - All three input strings—including the duplicate `"ab"`—are appended to
//         `buckets["ab"]`.
//       - Moving `buckets["ab"]` into `result` yields `[["ab", "ab", "cd"]]`.
//
// Algorithm:
//   Two strings belong to the same shift equivalence class iff they have the
//   same length and identical cyclic character offsets relative to their first
//   character. Normalize each string `s` by shifting all its characters backward
//   by `s[0] - 'a'` modulo 26 so that the canonical key always begins with 'a':
//     canonical[i] = 'a' + ((s[i] - s[0] + 26) % 26).
//   For example, "az" -> "az" and "ba" -> "az", while "abc", "bcd", and "xyz"
//   all normalize to "abc". Group all original strings by their canonical key
//   in a hash map.
//   Time:  O(N * K), where N = strings.size() and K = max string length.
//   Space: O(N * K)
//
// Data Structures:
//   - std::string canonical key: Compact representation of the cyclic offset
//     pattern anchored at 'a'.
//   - std::unordered_map<std::string, std::vector<std::string>>: Maps each
//     shift-normalized signature to all matching input strings.
//
// Real-World Applications:
//   - Cryptanalysis: clustering ciphertexts produced by Caesar / rotational
//     ciphers that share the same underlying plaintext structure.
//   - Music information retrieval: grouping melodies that are identical up to
//     musical transposition (key-invariant pitch-interval signatures).
//   - Time-series and geometric pattern matching: indexing sequences invariant
//     under constant offset shifts.
//
// Related LeetCode Problems:
//   - #49 Group Anagrams [done] (grouping strings by order-invariant frequency keys)
//   - #205 Isomorphic Strings [done] (general character-substitution equivalence)
//   - #890 Find and Replace Pattern [new] (canonicalizing strings by first-occurrence IDs)
//   - #242 Valid Anagram [done] (character multiset equivalence)

#include "framework/lc_framework.h"

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr int kAlphabetSize = 26;

class Solution {
 public:
  // Groups strings that can be transformed into one another via cyclic letter
  // shifts.
  // Invariant: two strings share the same `shiftKey` iff one is a cyclic shift
  // of the other.
  // Time: O(N * K), Space: O(N * K).
  std::vector<std::vector<std::string>> groupStrings(
      const std::vector<std::string>& strings) {
    std::unordered_map<std::string, std::vector<std::string>> buckets;
    buckets.reserve(strings.size());

    for (const std::string& s : strings) {
      buckets[shiftKey(s)].push_back(s);
    }

    std::vector<std::vector<std::string>> result;
    result.reserve(buckets.size());
    for (auto& entry : buckets) {
      result.push_back(std::move(entry.second));
    }
    return result;
  }

 private:
  // Shifts `s` cyclically so its first character becomes 'a', producing a
  // unique canonical representative for its shift equivalence class.
  static std::string shiftKey(std::string_view s) {
    if (s.empty()) {
      return {};
    }

    std::string key;
    key.reserve(s.size());
    const int base = static_cast<int>(static_cast<unsigned char>(s[0]));

    for (const char ch : s) {
      // Add kAlphabetSize before modulo so wrap-around diffs stay non-negative.
      const int offset =
          ((static_cast<int>(static_cast<unsigned char>(ch)) - base) +
           kAlphabetSize) %
          kAlphabetSize;
      key.push_back(static_cast<char>('a' + offset));
    }
    return key;
  }
};

// Sorts each group and the outer list of groups for deterministic assertions.
std::vector<std::vector<std::string>> normalizeGroups(
    std::vector<std::vector<std::string>> groups) {
  for (auto& group : groups) {
    std::ranges::sort(group);
  }
  std::ranges::sort(groups);
  return groups;
}

LC_PROBLEM(249, "Group Shifted Strings", "hash", ctx) {
  Solution sol;

  ctx.test("classic_example", [&] {
    const std::vector<std::string> input = {
        "abc", "bcd", "acef", "xyz", "az", "ba", "a", "z"};
    const std::vector<std::vector<std::string>> expected = {
        {"a", "z"}, {"abc", "bcd", "xyz"}, {"acef"}, {"az", "ba"}};
    ctx.expect_eq(normalizeGroups(sol.groupStrings(input)), expected);
  });

  ctx.test("wrap_around_pair", [&] {
    const std::vector<std::string> input = {"az", "ba"};
    const std::vector<std::vector<std::string>> expected = {{"az", "ba"}};
    ctx.expect_eq(normalizeGroups(sol.groupStrings(input)), expected);
  });

  ctx.test("single_and_duplicates", [&] {
    const std::vector<std::string> single = {"zx"};
    ctx.expect_eq(normalizeGroups(sol.groupStrings(single)),
                  std::vector<std::vector<std::string>>{{"zx"}});

    const std::vector<std::string> dups = {"ab", "ab", "cd"};
    ctx.expect_eq(normalizeGroups(sol.groupStrings(dups)),
                  std::vector<std::vector<std::string>>{{"ab", "ab", "cd"}});

    const std::vector<std::string> empty_input = {};
    ctx.expect_eq(sol.groupStrings(empty_input),
                  std::vector<std::vector<std::string>>{});
  });
}

}  // namespace
