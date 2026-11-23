// LeetCode 392: Is Subsequence
// https://leetcode.com/problems/is-subsequence/
//
// Problem:
//   Given two strings `s` and `t`, return true if `s` is a subsequence of `t`,
//   and false otherwise. A subsequence of `t` is formed by deleting zero or
//   more characters from `t` without disturbing the relative order of the
//   remaining characters.
//
// Algorithm:
//   Greedily match each character of `s` to its earliest remaining occurrence
//   in `t`. Matching as early as possible leaves the longest possible suffix of
//   `t` available for subsequent characters of `s` (exchange argument).
//   Two equivalent linear-time implementations are provided (mirroring the
//   original Python solution's `find`-cursor scan and forward-iterator scan):
//     1. `isSubsequence`: iterates over `c` in `s`, advancing a cursor `pos` in
//        `t` via `t.find(c, pos)`. If any character is not found, returns
//        false; otherwise advances `pos = found + 1`.
//     2. `isSubsequenceTwoPointers`: single forward scan over `t` advancing the
//        index in `s` whenever characters match.
//   (Follow-up note: for billions of query strings `s` against one fixed `t`,
//   precompute per-character sorted index lists in `t` in O(|t|) and answer
//   each query via binary search in O(|s| log |t|).)
//
//   Time Complexity:  O(|s| + |t|) because the cursor in `t` only moves
//                     forward across the string.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - std::string_view (non-owning views over `s` and `t`) and scalar index
//     cursors; no dynamic allocations.
//
// Real-World Applications:
//   - Fuzzy command-palette and IDE filename matching (e.g., matching "isc"
//     against "IsSubsequence.cpp").
//   - Log-event sequence verification (confirming required state transitions
//     occurred in order within a large trace).
//   - DNA/protein motif filtering as a fast pre-screen before full sequence
//     alignment.
//
// Related LeetCode Problems:
//   - #792 Number of Matching Subsequences [done] (multi-query follow-up with waiting buckets or binary search)
//   - #524 Longest Word in Dictionary through Deleting [done] (repeated subsequence check over a dictionary)
//   - #1055 Shortest Way to Form String [done] (minimum concatenations of source subsequences)
//   - #115 Distinct Subsequences [done] (DP counting all ways s appears as a subsequence of t)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Checks whether `s` is a subsequence of `t` by greedily advancing a search
  // cursor through `t` with std::string_view::find.
  // Time: O(|s| + |t|), Space: O(1).
  bool isSubsequence(std::string_view s, std::string_view t) {
    std::size_t pos = 0uz;
    for (const char ch : s) {
      // Find the earliest occurrence of `ch` at or after `pos`.
      const std::size_t found = t.find(ch, pos);
      if (found == std::string_view::npos) {
        return false;
      }
      // Advance strictly past the matched character to preserve order.
      pos = found + 1uz;
    }
    return true;
  }

  // Equivalent two-pointer forward-scan implementation (paralleling Python's
  // forward-iterator approach).
  // Time: O(|s| + |t|), Space: O(1).
  bool isSubsequenceTwoPointers(std::string_view s, std::string_view t) {
    std::size_t si = 0uz;
    for (std::size_t ti = 0uz; ti < t.size() && si < s.size(); ++ti) {
      if (t[ti] == s[si]) {
        ++si;
      }
    }
    return si == s.size();
  }
};

LC_PROBLEM(392, "Is Subsequence", "greedy", ctx) {
  ctx.test("example_1_true", [&] {
    Solution sol;
    ctx.expect_true(sol.isSubsequence("abc", "ahbgdc"));
    ctx.expect_true(sol.isSubsequenceTwoPointers("abc", "ahbgdc"));
  });

  ctx.test("example_2_false", [&] {
    Solution sol;
    ctx.expect_false(sol.isSubsequence("axc", "ahbgdc"));
    ctx.expect_false(sol.isSubsequenceTwoPointers("axc", "ahbgdc"));
  });

  ctx.test("repeated_characters_insufficient", [&] {
    Solution sol;
    ctx.expect_false(sol.isSubsequence("aaaaaa", "bbaaaa"));
    ctx.expect_false(sol.isSubsequenceTwoPointers("aaaaaa", "bbaaaa"));
  });

  ctx.test("order_matters", [&] {
    Solution sol;
    ctx.expect_true(sol.isSubsequence("ace", "abcde"));
    ctx.expect_false(sol.isSubsequence("aec", "abcde"));
    ctx.expect_false(sol.isSubsequenceTwoPointers("aec", "abcde"));
  });

  ctx.test("empty_edge_cases", [&] {
    Solution sol;
    ctx.expect_true(sol.isSubsequence("", "ahbgdc"));
    ctx.expect_true(sol.isSubsequence("", ""));
    ctx.expect_false(sol.isSubsequence("a", ""));
    ctx.expect_true(sol.isSubsequenceTwoPointers("", "ahbgdc"));
    ctx.expect_false(sol.isSubsequenceTwoPointers("a", ""));
  });
}

}  // namespace
