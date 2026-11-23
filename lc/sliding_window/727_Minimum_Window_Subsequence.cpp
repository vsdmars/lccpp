// LeetCode 727: Minimum Window Subsequence
// https://leetcode.com/problems/minimum-window-subsequence/
//
// Problem:
//   Given two lowercase strings `s1` and `s2`, find the shortest contiguous
//   substring `w` of `s1` such that `s2` is a subsequence of `w` (characters
//   of `s2` appear in `w` in the same relative order, not necessarily
//   adjacent). If no such substring exists, return the empty string `""`. If
//   multiple substrings share the minimum length, return the left-most one.
//
// Algorithm:
//   Two-pointer forward-and-backward ("pinch / squeeze") search:
//     1. Forward pass: starting from `search_from`, scan greedily through `s1`
//        with `find` to locate the earliest index where all characters of `s2`
//        match in order. If any character cannot be found, no further valid
//        window exists and we stop.
//     2. Backward pass: from the matched right endpoint `window_end`, scan
//        backward through `s2` in reverse using `rfind` to tighten the left
//        boundary `window_start` to the rightmost starting position that still
//        yields `s2` ending at `window_end - 1`.
//     3. Candidate update: if `[window_start, window_end)` is strictly shorter
//        than the best window so far (preserving left-most on ties), record it.
//     4. Advance `search_from = window_start + 1` (NOT `window_end`) so we
//        never miss overlapping shorter windows that begin inside the previous
//        window.
//   Time Complexity: O(|s1| * |s2|) in the worst case (often much faster in
//     practice thanks to vectorized character search).
//   Space Complexity: O(1) auxiliary space (excluding the returned substring).
//
// Data Structures:
//   - `std::string_view` slices over `s1` and `s2` for zero-copy forward and
//     reverse character lookups.
//   - Scalar window bounds (`best_start`, `best_len`) so the result string is
//     materialized at most once at the end.
//
// Real-World Applications:
//   - Log correlation and distributed tracing: finding the tightest time
//     window in an event log where a specific ordered sequence of milestones
//     occurred.
//   - Bioinformatics motif localization: locating the shortest genomic segment
//     containing an ordered series of conserved residue markers.
//   - Fuzzy command-palette matching (IDE quick-open): highlighting the
//     compactest span of a file path that matches a user's typed subsequence.
//
// Related LeetCode Problems:
//   - #76 Minimum Window Substring [done] (minimum window covering a multiset of characters without ordering)
//   - #392 Is Subsequence [done] (decision version checking whether a subsequence exists)
//   - #792 Number of Matching Subsequences [done] (matching many query subsequences against one text)
//   - #1055 Shortest Way to Form String [done] (greedy subsequence matching to cover a target string)
//   - #209 Minimum Size Subarray Sum [done] (minimum-length contiguous window satisfying a predicate)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Finds the left-most shortest substring of `s1` containing `s2` as a
  // subsequence using forward greedy matching and backward tightening.
  // Time: O(|s1| * |s2|) worst-case, Space: O(1) auxiliary.
  std::string minWindow(std::string_view s1, std::string_view s2) {
    if (s2.empty() || s2.size() > s1.size()) {
      return "";
    }

    std::size_t best_start = 0uz;
    std::size_t best_len = 0uz;
    std::size_t search_from = 0uz;

    while (search_from < s1.size()) {
      // Phase 1 (Forward pass): match every character of `s2` in order as
      // early as possible starting at `search_from`.
      std::size_t cursor = search_from;
      bool matched = true;
      for (const char ch : s2) {
        const std::size_t found = s1.find(ch, cursor);
        if (found == std::string_view::npos) {
          matched = false;
          break;
        }
        cursor = found + 1uz;
      }

      // If no match starts at or after `search_from`, no more windows exist.
      if (!matched) {
        break;
      }

      // `window_end` is the exclusive right boundary of this candidate window.
      const std::size_t window_end = cursor;

      // Phase 2 (Backward pass): walk `s2` in reverse from `window_end - 1`
      // to pull the left boundary as far right as possible while preserving
      // `s2` as a subsequence. Because the forward pass confirmed a valid
      // match inside `s1[0..window_end)`, `back_pos` stays >= 1 before each
      // decrement and `rfind` always succeeds.
      std::size_t back_pos = window_end;
      for (std::size_t j = s2.size(); j > 0uz; --j) {
        back_pos = s1.rfind(s2[j - 1uz], back_pos - 1uz);
      }

      const std::size_t window_len = window_end - back_pos;
      if (best_len == 0uz || window_len < best_len) {
        best_start = back_pos;
        best_len = window_len;
      }

      // Phase 3: advance one step past the tightened left boundary rather than
      // `window_end`, so shorter overlapping windows are not skipped.
      search_from = back_pos + 1uz;
    }

    return best_len == 0uz ? "" : std::string(s1.substr(best_start, best_len));
  }
};

LC_PROBLEM(727, "Minimum Window Subsequence", "sliding_window", ctx) {
  ctx.test("leetcode example 1 left-most tie break", [&] {
    Solution sol;
    ctx.expect_eq(sol.minWindow("abcdebdde", "bde"), std::string("bcde"));
  });

  ctx.test("backward tightening required", [&] {
    Solution sol;
    ctx.expect_eq(sol.minWindow("cnhczmccqouqadqtmjjzl", "cm"),
                  std::string("czm"));
  });

  ctx.test("python build test case", [&] {
    Solution sol;
    ctx.expect_eq(sol.minWindow("fgrqsqsnodwmxzkzxwqegkndaa", "fnok"),
                  std::string("fgrqsqsnodwmxzk"));
  });

  ctx.test("no valid subsequence", [&] {
    Solution sol;
    ctx.expect_eq(sol.minWindow("jmeqksfrsdcmsiwvaovztaqenprpvnbstl", "u"),
                  std::string(""));
  });

  ctx.test("single character exact match", [&] {
    Solution sol;
    ctx.expect_eq(sol.minWindow("a", "a"), std::string("a"));
  });

  ctx.test("overlapping shorter window after first match", [&] {
    Solution sol;
    ctx.expect_eq(sol.minWindow("abbbbcabbc", "abc"), std::string("abbc"));
  });
}

}  // namespace
