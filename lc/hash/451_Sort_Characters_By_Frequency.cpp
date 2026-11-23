// LeetCode 451: Sort Characters By Frequency
// https://leetcode.com/problems/sort-characters-by-frequency/
//
// Problem:
//   Given a string s consisting of uppercase and lowercase English letters and
//   digits (general ASCII characters), sort it in decreasing order based on
//   the frequency of each character. Identical characters must appear
//   contiguously in the output. If multiple characters share the same
//   frequency, any valid ordering among them is accepted.
//
// Examples:
//   Example 1:
//     Input:  s = "tree"
//     Output: "eetr"
//     Explanation:
//       - Tally character occurrences in freq, obtaining {'e': 2, 't': 1, 'r': 1}.
//       - Collect (count, char) pairs into entries: [(2, 'e'), (1, 't'), (1, 'r')].
//       - Sort entries descending with std::ranges::greater{}, ordering by count
//         descending and breaking the count-1 tie by ASCII code ('t' = 116 before
//         'r' = 114) to give [(2, 'e'), (1, 't'), (1, 'r')].
//       - Append count copies of each character in sorted order into result,
//         returning "eetr".
//
//   Example 2:
//     Input:  s = "Aabb"
//     Output: "bbaA"
//     Explanation:
//       - Tally character occurrences in freq, keeping uppercase and lowercase
//         distinct: {'b': 2, 'a': 1, 'A': 1}.
//       - Materialize entries as [(2, 'b'), (1, 'a'), (1, 'A')] and sort with
//         std::ranges::greater{}.
//       - Place (2, 'b') first and break the count-1 tie by descending character
//         code ('a' = 97 before 'A' = 65), yielding [(2, 'b'), (1, 'a'), (1, 'A')].
//       - Emit each character count times to build and return "bbaA".
//
//   Example 3:
//     Input:  s = ""
//     Output: ""
//     Explanation:
//       - Scan the empty input s, leaving freq empty and producing an empty entries
//         vector.
//       - Sort and iterate over the empty entries list without appending any
//         characters, returning "".
//
// Algorithm:
//   1. Count the frequency of every distinct character in s using an unordered
//      hash map (std::unordered_map<char, int>).
//   2. Collect all (frequency, character) pairs into a contiguous vector and
//      sort them in descending order (using character value descending as a
//      deterministic tie-breaker).
//   3. Reconstruct the output string by appending `frequency` copies of each
//      character in sorted order.
//   Time Complexity:  O(n + k log k), where n = s.size() and k is the number
//                     of distinct characters (k <= 128 for ASCII, so O(n)).
//   Space Complexity: O(k) auxiliary space for the frequency map and pair list
//                     (plus O(n) for the returned string).
//
// Data Structures:
//   - std::unordered_map<char, int>: tallies character frequencies in O(1)
//     amortized time per character.
//   - std::vector<std::pair<int, char>>: stores distinct (count, char) entries
//     for cache-friendly descending sorting.
//
// Real-World Applications:
//   - Huffman coding and entropy-based compression pipelines (ordering symbols
//     by frequency before constructing prefix-free trees).
//   - Log analytics and telemetry dashboards ranking the most frequent error
//     codes, tags, or event keys.
//   - Text mining, stop-word detection, and autocomplete candidate ranking.
//
// Related LeetCode Problems:
//   - #347 Top K Frequent Elements [done] (frequency map with top-k selection)
//   - #692 Top K Frequent Words [done] (frequency sorting with lexicographic tie-breaking)
//   - #1636 Sort Array by Increasing Frequency [done] (ascending frequency sort with value tie-break)
//   - #387 First Unique Character in a String [done] (single-pass character frequency counting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Sorts the characters of `s` in non-increasing order of frequency so that
  // identical characters are grouped together.
  // Time: O(n + k log k), Space: O(k) auxiliary (k = distinct characters).
  std::string frequencySort(std::string_view s) {
    // Phase 1: Tally character occurrences in O(n) expected time.
    std::unordered_map<char, int> freq;
    for (char ch : s) {
      ++freq[ch];
    }

    // Phase 2: Materialize (frequency, character) pairs and sort descending.
    // Putting frequency first lets std::ranges::greater order primarily by
    // frequency and break ties deterministically by descending character code.
    std::vector<std::pair<int, char>> entries;
    entries.reserve(freq.size());
    for (const auto& [ch, count] : freq) {
      entries.emplace_back(count, ch);
    }
    std::ranges::sort(entries, std::ranges::greater{});

    // Phase 3: Reconstruct the sorted string by emitting contiguous blocks of
    // each character according to its frequency.
    std::string result;
    result.reserve(s.size());
    for (const auto& [count, ch] : entries) {
      result.append(static_cast<std::size_t>(count), ch);
    }
    return result;
  }
};

// Verifies that `output` is a valid frequency-sorted rearrangement of `input`:
// same character multiset, identical characters grouped contiguously, and
// run lengths monotonically non-increasing.
bool isValidFrequencySort(std::string_view input, std::string_view output) {
  if (input.size() != output.size()) {
    return false;
  }

  std::unordered_map<char, int> expected_freq;
  for (char ch : input) {
    ++expected_freq[ch];
  }

  std::unordered_map<char, int> seen_freq;
  std::size_t prev_run = output.size() + 1uz;
  std::size_t idx = 0uz;
  while (idx < output.size()) {
    char ch = output[idx];
    // If we already finished a run of `ch`, identical characters were split.
    if (seen_freq.contains(ch)) {
      return false;
    }
    std::size_t run_end = idx;
    while (run_end < output.size() && output[run_end] == ch) {
      ++run_end;
    }
    std::size_t run_len = run_end - idx;
    if (run_len > prev_run) {
      return false;
    }
    seen_freq[ch] = static_cast<int>(run_len);
    prev_run = run_len;
    idx = run_end;
  }

  return seen_freq == expected_freq;
}

LC_PROBLEM(451, "Sort Characters By Frequency", "hash", ctx) {
  ctx.test("example_tree", [&] {
    Solution sol;
    std::string res = sol.frequencySort("tree");
    ctx.expect_true(isValidFrequencySort("tree", res));
    ctx.expect_eq(res, std::string("eetr"));
  });

  ctx.test("example_cccaaa_tie", [&] {
    Solution sol;
    std::string res = sol.frequencySort("cccaaa");
    ctx.expect_true(isValidFrequencySort("cccaaa", res));
    ctx.expect_eq(res, std::string("cccaaa"));
  });

  ctx.test("example_case_sensitive_Aabb", [&] {
    Solution sol;
    std::string res = sol.frequencySort("Aabb");
    ctx.expect_true(isValidFrequencySort("Aabb", res));
    ctx.expect_eq(res, std::string("bbaA"));
  });

  ctx.test("empty_and_single_char", [&] {
    Solution sol;
    ctx.expect_eq(sol.frequencySort(""), std::string(""));
    ctx.expect_eq(sol.frequencySort("z"), std::string("z"));
  });

  ctx.test("digits_and_mixed_frequencies", [&] {
    Solution sol;
    const std::string input = "2a554442f544asfasssffffasss";
    std::string res = sol.frequencySort(input);
    ctx.expect_true(isValidFrequencySort(input, res));
  });
}

}  // namespace
