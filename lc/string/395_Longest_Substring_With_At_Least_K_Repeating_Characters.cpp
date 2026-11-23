// LeetCode 395: Longest Substring With At Least K Repeating Characters
// https://leetcode.com/problems/longest-substring-with-at-least-k-repeating-characters/
//
// Problem:
//   Given a lowercase English string `s` and an integer `k`, return the length
//   of the longest contiguous substring `t` of `s` such that every distinct
//   character appearing in `t` occurs at least `k` times.
//
// Algorithm:
//   Divide-and-Conquer on Invalid Split Characters:
//   In any candidate substring `sub`:
//     1. If `sub.size() < k`, no valid non-empty substring can fit -> return 0.
//     2. Count the frequency of all 26 lowercase letters within `sub`.
//     3. If every present letter appears at least `k` times, `sub` itself is
//        valid and maximal -> return `sub.size()`.
//     4. Otherwise, any character whose total count in `sub` is strictly less
//        than `k` can NEVER belong to a valid substring of `sub`. Such
//        characters therefore partition `sub` into maximal contiguous segments
//        containing only potentially valid letters. Recurse on each segment and
//        return the maximum length found.
//   Because every recursion level eliminates at least one distinct character
//   from the 26-letter alphabet, recursion depth is bounded by 26.
//   Time Complexity:  O(|Sigma| * n) = O(26 * n) = O(n)
//   Space Complexity: O(|Sigma|) = O(26) = O(1) recursion stack (zero-copy
//                     `std::string_view` slices).
//
// Data Structures:
//   - `std::array<int, 26>`: per-level lowercase letter frequency table.
//   - `std::string_view`: zero-copy sub-range views passed down recursion.
//
// Real-World Applications:
//   - Genomic sequence filtering to isolate contiguous motif regions where
//     every observed nucleotide/codon exceeds a minimum support threshold.
//   - Telemetry and log-stream segmentation to extract steady-state windows
//     free of rare/transient fault events.
//   - Data-quality windowing in time-series analytics where every category in
//     a cohort window must satisfy a minimum sample-size requirement.
//
// Related LeetCode Problems:
//   - #3 Longest Substring Without Repeating Characters [done] (classic sliding-window substring constraint)
//   - #340 Longest Substring with At Most K Distinct Characters [done] (sliding window bounded by distinct character count)
//   - #424 Longest Repeating Character Replacement [done] (longest valid window under frequency constraint)
//   - #76 Minimum Window Substring [done] (character frequency satisfaction across substrings)
//   - #2067 Number of Equal Count Substrings [new] (fixed-frequency substring counting over a 26-letter alphabet)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the length of the longest substring of `s` in which every present
  // character occurs at least `k` times.
  // Time: O(n), Space: O(1) (bounded by the 26-letter alphabet depth).
  int longestSubstring(std::string_view s, int k) {
    if (k <= 1) {
      return static_cast<int>(s.size());
    }
    return divide_and_conquer(s, k);
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;

  // Recursively splits `sub` around characters whose frequency in `sub` is < k.
  // Invariant: each recursive call receives a substring strictly missing at
  // least one distinct character present in its parent, bounding depth <= 26.
  static int divide_and_conquer(std::string_view sub, int k) {
    if (sub.size() < static_cast<std::size_t>(k)) {
      return 0;
    }

    std::array<int, kAlphabetSize> freq{};
    for (const char ch : sub) {
      ++freq[static_cast<std::size_t>(ch - 'a')];
    }

    // If no character violates the frequency threshold, the entire slice wins.
    bool all_valid = true;
    for (const int count : freq) {
      if (count > 0 && count < k) {
        all_valid = false;
        break;
      }
    }
    if (all_valid) {
      return static_cast<int>(sub.size());
    }

    // Partition `sub` at every invalid character (count < k) in a single pass
    // and take the best valid substring across all resulting candidate runs.
    int best = 0;
    std::size_t seg_start = 0uz;
    for (std::size_t i = 0uz; i <= sub.size(); ++i) {
      const bool is_boundary =
          (i == sub.size()) ||
          (freq[static_cast<std::size_t>(sub[i] - 'a')] < k);
      if (is_boundary) {
        if (i > seg_start) {
          const int candidate =
              divide_and_conquer(sub.substr(seg_start, i - seg_start), k);
          best = std::max(best, candidate);
        }
        seg_start = i + 1uz;
      }
    }
    return best;
  }
};

LC_PROBLEM(395, "Longest Substring With At Least K Repeating Characters",
           "string", ctx) {
  Solution sol;

  ctx.test("example_1_aaabb",
           [&] { ctx.expect_eq(sol.longestSubstring("aaabb", 3), 3); });

  ctx.test("example_2_ababbc",
           [&] { ctx.expect_eq(sol.longestSubstring("ababbc", 2), 5); });

  ctx.test("build_and_docstring_cases", [&] {
    ctx.expect_eq(sol.longestSubstring("abbbcdddeff", 2), 3);
    ctx.expect_eq(sol.longestSubstring("caabbbdef", 2), 5);
  });

  ctx.test("nested_split_cascading_invalidation", [&] {
    // Splitting on 'c' leaves "ababa" where 'b' now drops to count 2 < 3,
    // forcing a second split that leaves no valid substring.
    ctx.expect_eq(sol.longestSubstring("ababacb", 3), 0);
  });

  ctx.test("edge_cases_k1_empty_and_impossible", [&] {
    ctx.expect_eq(sol.longestSubstring("", 2), 0);
    ctx.expect_eq(sol.longestSubstring("abcdef", 1), 6);
    ctx.expect_eq(sol.longestSubstring("abc", 2), 0);
    ctx.expect_eq(sol.longestSubstring("aabbcc", 2), 6);
  });
}

}  // namespace
