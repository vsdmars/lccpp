// LeetCode 1055: Shortest Way To Form String
// https://leetcode.com/problems/shortest-way-to-form-string/
//
// Problem:
//   A subsequence of a string is formed by deleting zero or more characters
//   without changing the relative order of the remaining characters. Given two
//   lowercase English strings `source` and `target`, return the minimum number
//   of subsequences of `source` whose concatenation equals `target`, or `-1` if
//   `target` cannot be formed (i.e., `target` contains a character absent from
//   `source`).
//   Constraints: 1 <= source.length, target.length <= 1000; both strings
//   contain only lowercase English letters `'a'..'z'`.
//
// Algorithm:
//   Greedy left-to-right matching: to minimize the number of `source` copies,
//   each subsequence should greedily consume as long a prefix of the remaining
//   `target` as possible.
//   1. Inverted Index + Binary Search (`shortestWay`, optimal):
//      Precompute, for each letter `'a'..'z'`, the sorted list of indices where
//      it occurs in `source`. Maintain a cursor `source_cursor` indicating the
//      next available position in the current copy of `source`. For each
//      character `c` in `target`:
//      - If `c` has no occurrences in `source`, return `-1`.
//      - Binary search (`std::ranges::lower_bound`) for the first occurrence of
//        `c` at index `>= source_cursor`.
//      - If found at index `pos`, advance `source_cursor = pos + 1`.
//      - Otherwise, wrap around to a new subsequence (`++subsequences`) and
//        consume the earliest occurrence `positions.front()`, setting
//        `source_cursor = positions.front() + 1`.
//      Time Complexity:  O(|S| + |T| log |S|).
//      Space Complexity: O(|S|) for the 26 index lists.
//   2. Two-Pointer Scan (`shortestWayTwoPointers`, baseline):
//      Repeatedly scan `source` from left to right, advancing `target` whenever
//      characters match. If a full pass matches zero characters, return `-1`.
//      Time Complexity:  O(|S| * |T|).
//      Space Complexity: O(1).
//
// Data Structures:
//   - `std::array<std::vector<std::size_t>, 26>` acting as a compact inverted
//     index mapping each lowercase letter to its sorted positions in `source`.
//
// Real-World Applications:
//   - DNA/RNA sequence assembly and oligo synthesis cycle minimization using
//     repeated template passes.
//   - Streaming pattern alignment and log reconstruction from repeating event
//     templates.
//   - Compact text compression / dictionary subsequence encoding.
//
// Related LeetCode Problems:
//   - #392 Is Subsequence [done] (single-pass subsequence check and inverted index)
//   - #792 Number of Matching Subsequences [done] (same character-position binary search)
//   - #524 Longest Word in Dictionary through Deleting [done] (subsequence matching)
//   - #1060 Missing Element In Sorted Array [done] (binary search on sorted index structure)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Finds the minimum number of subsequences of `source` needed to form
  // `target` using an inverted index and binary search.
  // Time: O(|S| + |T| log |S|), Space: O(|S|).
  int shortestWay(std::string_view source, std::string_view target) {
    if (target.empty()) {
      return 0;
    }
    if (source.empty()) {
      return -1;
    }

    // Record sorted positions of each lowercase letter in `source`.
    std::array<std::vector<std::size_t>, kAlphabetSize> char_positions{};
    for (std::size_t i = 0uz; i < source.size(); ++i) {
      const char ch = source[i];
      if (ch >= 'a' && ch <= 'z') {
        char_positions[static_cast<std::size_t>(ch - 'a')].push_back(i);
      }
    }

    int subsequences = 1;
    std::size_t source_cursor = 0uz;

    // Greedily match each character of `target` at the earliest index
    // >= source_cursor in the current copy of `source`, wrapping when needed.
    for (char ch : target) {
      if (ch < 'a' || ch > 'z') {
        return -1;
      }
      const auto& positions =
          char_positions[static_cast<std::size_t>(ch - 'a')];
      if (positions.empty()) {
        return -1;
      }

      const auto it = std::ranges::lower_bound(positions, source_cursor);
      if (it == positions.end()) {
        // No occurrence remains in the current copy of `source`; start a new
        // subsequence and match at the character's first index in `source`.
        ++subsequences;
        source_cursor = positions.front() + 1uz;
      } else {
        source_cursor = *it + 1uz;
      }
    }

    return subsequences;
  }

  // Baseline two-pointer greedy implementation matching `target` by scanning
  // `source` repeatedly from left to right.
  // Time: O(|S| * |T|), Space: O(1).
  int shortestWayTwoPointers(std::string_view source, std::string_view target) {
    std::size_t target_idx = 0uz;
    int subsequences = 0;

    while (target_idx < target.size()) {
      const std::size_t start_idx = target_idx;
      for (std::size_t source_idx = 0uz;
           source_idx < source.size() && target_idx < target.size();
           ++source_idx) {
        if (source[source_idx] == target[target_idx]) {
          ++target_idx;
        }
      }

      // If an entire pass over `source` made no progress, target[target_idx]
      // does not exist in `source`.
      if (target_idx == start_idx) {
        return -1;
      }
      ++subsequences;
    }

    return subsequences;
  }
};

LC_PROBLEM(1055, "Shortest Way To Form String", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.shortestWay("abc", "abcbc"), 2);
    ctx.expect_eq(sol.shortestWayTwoPointers("abc", "abcbc"), 2);
  });

  ctx.test("example_2_impossible", [&] {
    ctx.expect_eq(sol.shortestWay("abc", "acdbc"), -1);
    ctx.expect_eq(sol.shortestWayTwoPointers("abc", "acdbc"), -1);
  });

  ctx.test("example_3", [&] {
    ctx.expect_eq(sol.shortestWay("xyz", "xzyxz"), 3);
    ctx.expect_eq(sol.shortestWayTwoPointers("xyz", "xzyxz"), 3);
  });

  ctx.test("repeated_characters_exact_and_partial_passes", [&] {
    ctx.expect_eq(sol.shortestWay("aaaaa", "aaaaaaaaaaaaa"), 3);
    ctx.expect_eq(sol.shortestWayTwoPointers("aaaaa", "aaaaaaaaaaaaa"), 3);
    ctx.expect_eq(sol.shortestWay("aa", "aaaa"), 2);
    ctx.expect_eq(sol.shortestWayTwoPointers("aa", "aaaa"), 2);
  });

  ctx.test("single_subsequence_and_empty_target", [&] {
    ctx.expect_eq(sol.shortestWay("abcdef", "bdf"), 1);
    ctx.expect_eq(sol.shortestWayTwoPointers("abcdef", "bdf"), 1);
    ctx.expect_eq(sol.shortestWay("abc", ""), 0);
    ctx.expect_eq(sol.shortestWayTwoPointers("abc", ""), 0);
  });
}

}  // namespace
