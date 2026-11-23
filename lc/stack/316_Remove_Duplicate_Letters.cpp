// LeetCode 316: Remove Duplicate Letters
// https://leetcode.com/problems/remove-duplicate-letters/
//
// Problem:
//   Given a string `s` consisting of lowercase English letters, remove
//   duplicate letters so that every distinct letter appears once and only once.
//   Among all valid deduplicated subsequences, return the one that is
//   lexicographically smallest.
//
// Examples:
//   Example 1:
//     Input:  s = "bcabc"
//     Output: "abc"
//     Explanation:
//       - Precompute last_occurrence['a'] = 2, last_occurrence['b'] = 3, and
//         last_occurrence['c'] = 4.
//       - At i = 0 ('b') and i = 1 ('c'), neither character is in the stack and 'c'
//         > 'b', so stack becomes "bc".
//       - At i = 2 ('a'), pop 'c' (last_occurrence 4 > 2) and pop 'b'
//         (last_occurrence 3 > 2) because both appear later, then push 'a' leaving
//         stack "a".
//       - At i = 3 ('b') and i = 4 ('c'), neither is in_stack, so both are pushed
//         onto stack in order to return "abc".
//
//   Example 2:
//     Input:  s = "cbacdcbc"
//     Output: "acdb"
//     Explanation:
//       - Precompute last_occurrence: 'a' -> 2, 'b' -> 6, 'c' -> 7, 'd' -> 4.
//       - At i = 0 ('c'), push 'c'; at i = 1 ('b'), pop 'c' (last_occurrence 7 > 1)
//         and push 'b'; at i = 2 ('a'), pop 'b' (last_occurrence 6 > 2) and push
//         'a' (stack "a").
//       - At i = 3 ('c') and i = 4 ('d'), push both in increasing order so stack
//         becomes "acd", then skip i = 5 ('c') since in_stack['c'] is true.
//       - At i = 6 ('b'), 'd' > 'b' but last_occurrence['d'] = 4 <= 6 so 'd' cannot
//         be popped; push 'b' ("acdb") and skip i = 7 ('c') to return "acdb".
//
//   Example 3:
//     Input:  s = "ecbacdcbc"
//     Output: "eacdb"
//     Explanation:
//       - Precompute last_occurrence: 'e' -> 0, 'a' -> 3, 'd' -> 5, 'b' -> 7, 'c'
//         -> 8.
//       - At i = 0 push 'e'; at i = 1 ('c'), 'e' > 'c' but last_occurrence['e'] = 0
//         <= 1, so 'e' cannot be popped and stack becomes "ec".
//       - At i = 2 ('b') pop 'c' and push 'b' ("eb"), then at i = 3 ('a') pop 'b'
//         (last_occurrence 7 > 3) while 'e' stays fixed, leaving stack "ea".
//       - Push 'c' (i = 4) and 'd' (i = 5) to form "eacd", skip 'c' at i = 6, push
//         'b' at i = 7 (stopping at 'd' since last_occurrence['d'] = 5 <= 7), and
//         skip 'c' at i = 8 to return "eacdb".
//
// Algorithm:
//   Greedy monotonic increasing stack with last-occurrence lookahead:
//   1. Precompute the last index at which each letter `'a'..'z'` appears in `s`.
//   2. Scan `s` left to right while maintaining a result string as a stack and
//      a boolean membership table `in_stack`:
//      - If the current character `c` is already in the stack, skip it (the
//        earlier placement was already made optimal relative to preceding
//        characters).
//      - Otherwise, while the stack is non-empty, its top character is greater
//        than `c`, AND that top character still appears later in `s`
//        (`last_occurrence[top] > i`), pop it and mark it not in the stack so
//        `c` can take an earlier position and the popped letter can be added
//        later.
//      - Push `c` onto the stack and mark it present.
//   Invariant: At every index `i`, the stack holds the lexicographically
//   smallest valid prefix over the characters already committed.
//   Time Complexity: O(N), since each character is pushed and popped at most
//   once.
//   Space Complexity: O(1) auxiliary space (fixed 26-element lookup arrays and
//   an output stack of length at most 26).
//
// Data Structures:
//   - `std::string` used directly as a monotonic character stack and output
//     buffer.
//   - `std::array<std::size_t, 26>` storing each letter's last index in `s`.
//   - `std::array<bool, 26>` tracking O(1) membership in the current stack.
//
// Real-World Applications:
//   - Canonicalizing deduplicated token streams or permission sets while
//     preserving relative ordering constraints.
//   - Generating deterministic, minimal-lexicographic keys from ordered event
//     or dependency sequences in distributed logs.
//   - String compression and subsequence optimization in bioinformatics and
//     compiler optimization passes.
//
// Related LeetCode Problems:
//   - #1081 Smallest Subsequence of Distinct Characters [new] (identical problem statement and greedy stack algorithm)
//   - #402 Remove K Digits [done] (lexicographically smallest subsequence using a monotonic increasing stack)
//   - #321 Create Maximum Number [done] (lexicographically optimal subsequence selection with monotonic stacks)
//   - #1673 Find the Most Competitive Subsequence [new] (fixed-length smallest subsequence via monotonic stack)
//   - #496 Next Greater Element I [done] (foundational monotonic stack pattern)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the lexicographically smallest subsequence containing every
  // distinct lowercase letter from `s` exactly once.
  // Time: O(N), Space: O(1) auxiliary (at most 26 distinct letters).
  std::string removeDuplicateLetters(std::string_view s) {
    // Pass 1: record the final index where each character appears so we know
    // whether popping a character from the stack can safely be recovered later.
    std::array<std::size_t, kAlphabetSize> last_occurrence{};
    for (std::size_t i = 0uz; i < s.size(); ++i) {
      last_occurrence[char_index(s[i])] = i;
    }

    std::array<bool, kAlphabetSize> in_stack{};
    std::string stack;
    stack.reserve(kAlphabetSize);

    // Pass 2: greedily build the lexicographically smallest result.
    for (std::size_t i = 0uz; i < s.size(); ++i) {
      const char c = s[i];
      const std::size_t c_idx = char_index(c);

      // Skip letters already committed to the stack (e.g., repeated runs like
      // "aaaaa"); keeping the earlier copy preserves optimal lexicographic
      // order.
      if (in_stack[c_idx]) {
        continue;
      }

      // Pop larger letters that will appear again later so the smaller current
      // letter `c` can precede them.
      while (!stack.empty() && stack.back() > c &&
             last_occurrence[char_index(stack.back())] > i) {
        in_stack[char_index(stack.back())] = false;
        stack.pop_back();
      }

      stack.push_back(c);
      in_stack[c_idx] = true;
    }

    return stack;
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;

  // Maps lowercase ASCII letter `'a'..'z'` to index `0..25`.
  static std::size_t char_index(char c) {
    return static_cast<std::size_t>(c - 'a');
  }
};

LC_PROBLEM(316, "Remove Duplicate Letters", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.removeDuplicateLetters("bcabc"), "abc");
    ctx.expect_eq(sol.removeDuplicateLetters("cbacdcbc"), "acdb");
  });

  ctx.test("source_build_case", [&] {
    // 'e' occurs only at index 0, so it cannot be popped despite being larger
    // than subsequent letters.
    ctx.expect_eq(sol.removeDuplicateLetters("ecbacdcbc"), "eacdb");
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.removeDuplicateLetters(""), "");
    ctx.expect_eq(sol.removeDuplicateLetters("a"), "a");
    ctx.expect_eq(sol.removeDuplicateLetters("aaaaa"), "a");
    ctx.expect_eq(sol.removeDuplicateLetters("zyxwvutsrqponmlkjihgfedcba"),
                  "zyxwvutsrqponmlkjihgfedcba");
    ctx.expect_eq(sol.removeDuplicateLetters("abacb"), "abc");
    ctx.expect_eq(sol.removeDuplicateLetters("bbcaac"), "bac");
  });
}

}  // namespace
