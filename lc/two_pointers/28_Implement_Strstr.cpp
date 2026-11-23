// LeetCode 28: Implement Strstr
// https://leetcode.com/problems/implement-strstr/
//
// Problem:
//   Given two strings `haystack` and `needle`, return the zero-based index of
//   the first occurrence of `needle` in `haystack`, or `-1` if `needle` is
//   not part of `haystack`. By convention (matching C `strstr` and classic
//   LeetCode #28), an empty `needle` matches at index `0`.
//
// Algorithm:
//   Two-pointer sliding-window substring search. First handle edge cases:
//   return `0` when `needle` is empty and `-1` when `haystack` is shorter
//   than `needle`. Then slide a starting index `start` across all valid
//   alignments `0 .. haystack.size() - needle.size()`. Whenever the first
//   character matches (`haystack[start] == needle[0]`), verify the full
//   window via zero-copy `std::string_view::substr(start, needle.size()) ==
//   needle` and return `start` on the first complete match. (For adversarial
//   repetitive inputs, KMP or Rabin-Karp reduces worst-case time to O(n + m).)
//   Time Complexity:  O((n - m + 1) * m) worst case, O(n) typical, where
//                     n = haystack.size() and m = needle.size().
//   Space Complexity: O(1), using non-owning `std::string_view` slices.
//
// Data Structures:
//   - Non-owning string slices (`std::string_view`): compare candidate
//     windows directly in memory with zero heap allocations.
//   - Alignment cursor (`start`): scans all feasible match offsets.
//
// Real-World Applications:
//   - Standard library substring search (`std::string::find`, C `strstr`,
//     HTTP header/delimiter parsing in network servers).
//   - Log-search tools, editors, and grep-like pattern matchers scanning
//     text buffers for literal tokens.
//   - Intrusion detection and packet-payload inspection matching literal
//     protocol signatures.
//
// Related LeetCode Problems:
//   - #459 Repeated Substring Pattern [new] (detects periodicity using substring search or KMP)
//   - #214 Shortest Palindrome [done] (prefix matching with KMP failure function)
//   - #686 Repeated String Match [done] (substring containment across repeated strings)
//   - #1392 Longest Happy Prefix [new] (computes KMP longest proper prefix-suffix table)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the starting index of the first occurrence of `needle` inside
  // `haystack`, `0` if `needle` is empty, or `-1` if not found.
  //
  // Time: O((n - m + 1) * m) worst case, Space: O(1).
  int strStr(std::string_view haystack, std::string_view needle) {
    if (needle.empty()) {
      return 0;
    }

    const std::size_t needle_len = needle.size();
    if (haystack.size() < needle_len) {
      return -1;
    }

    // Only starting positions up to `haystack.size() - needle_len` can hold
    // a full copy of `needle`.
    const std::size_t max_start = haystack.size() - needle_len;
    for (std::size_t start = 0uz; start <= max_start; ++start) {
      // Fast-path first-character filter before comparing the full window.
      if (haystack[start] == needle[0] &&
          haystack.substr(start, needle_len) == needle) {
        return static_cast<int>(start);
      }
    }

    return -1;
  }
};

LC_PROBLEM(28, "Implement Strstr", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_found_in_middle", [&] {
    ctx.expect_eq(sol.strStr("hello", "ll"), 2);
  });

  ctx.test("example_2_not_found", [&] {
    ctx.expect_eq(sol.strStr("aaaaa", "bba"), -1);
  });

  ctx.test("empty_needle_and_haystack", [&] {
    ctx.expect_eq(sol.strStr("", ""), 0);
    ctx.expect_eq(sol.strStr("a", ""), 0);
    ctx.expect_eq(sol.strStr("", "a"), -1);
  });

  ctx.test("match_at_start_and_end", [&] {
    ctx.expect_eq(sol.strStr("sadbutsad", "sad"), 0);
    ctx.expect_eq(sol.strStr("abc", "c"), 2);
    ctx.expect_eq(sol.strStr("abc", "abc"), 0);
  });

  ctx.test("partial_overlap_and_longer_needle", [&] {
    // Tests backtracking after a partial prefix match ("issi" vs "issip").
    ctx.expect_eq(sol.strStr("mississippi", "issip"), 4);
    ctx.expect_eq(sol.strStr("leetcode", "leeto"), -1);
    ctx.expect_eq(sol.strStr("ab", "abc"), -1);
  });
}

}  // namespace
