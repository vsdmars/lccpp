// LeetCode 91: Decode Ways
// https://leetcode.com/problems/decode-ways/
//
// Problem:
//   Messages of uppercase English letters 'A'-'Z' are encoded as "1"-"26".
//   Given a non-empty digit string s (length 1..100), return the total number
//   of distinct ways to decode s into letters. Leading zeros (e.g., "06") are
//   invalid and cannot decode to 'F'. The answer fits in a 32-bit signed int.
//
// Algorithm:
//   Bottom-up dynamic programming with O(1) space. Let dp[k] be the number of
//   valid decodings of prefix s[0..k), with dp[0] = 1 (empty prefix). For each
//   character s[i] (corresponding to prefix length i + 1):
//     1. Single-digit decode: if s[i] != '0', it decodes to 'A'-'I', adding
//        dp[i] ways.
//     2. Two-digit decode: if the two-digit integer formed by s[i - 1..i] lies
//        in [10, 26], it decodes to 'J'-'Z', adding dp[i - 1] ways.
//   Since each state depends only on the two preceding states, two rolling
//   variables (`prev2` and `prev1`) suffice. If s is empty or starts with '0',
//   return 0 immediately.
//   Time Complexity:  O(n).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two scalar rolling DP variables (`prev2` = dp[i - 1], `prev1` = dp[i])
//     tracking prefix decoding counts without allocating an O(n) table.
//
// Real-World Applications:
//   - Ambiguous tokenization and segmentation counting in NLP (e.g., Chinese/
//     Japanese word segmentation, Viterbi lattice counting).
//   - Variable-length prefix-free vs. non-prefix-free code decoding analysis in
//     data compression and telemetry protocols.
//   - Speech recognition phoneme-to-word lattice path counting.
//
// Related LeetCode Problems:
//   - #70 Climbing Stairs [done] (unconstrained 1- or 2-step Fibonacci DP)
//   - #639 Decode Ways II [done] (adds '*' wildcard matching digits 1-9)
//   - #139 Word Break [done] (generalized dictionary-based string segmentation)
//   - #1977 Number of Ways to Separate Numbers [new] (non-decreasing partitions)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Counts the number of valid 'A'-'Z' decodings of digit string `s`.
  // Time: O(n), Space: O(1).
  int numDecodings(std::string_view s) {
    if (s.empty() || s[0] == '0') {
      return 0;
    }

    // Invariant entering iteration i (processing s[i]):
    //   prev2 == ways to decode s[0..i - 1)  (prefix of length i - 1)
    //   prev1 == ways to decode s[0..i)      (prefix of length i)
    int prev2 = 1;  // Empty prefix "" has 1 decoding.
    int prev1 = 1;  // Non-zero single char s[0] has 1 decoding.

    for (std::size_t i = 1uz; i < s.size(); ++i) {
      int cur = 0;

      // Single-digit transition: digits '1'-'9' decode independently.
      if (s[i] != '0') {
        cur += prev1;
      }

      // Two-digit transition: "10" through "26" decode as a single letter.
      const int two_digit = ((s[i - 1uz] - '0') * 10) + (s[i] - '0');
      if (two_digit >= 10 && two_digit <= 26) {
        cur += prev2;
      }

      prev2 = prev1;
      prev1 = cur;
    }

    return prev1;
  }
};

LC_PROBLEM(91, "Decode Ways", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_twelve", [&] {
    ctx.expect_eq(sol.numDecodings("12"), 2);
  });

  ctx.test("example_2_226", [&] {
    ctx.expect_eq(sol.numDecodings("226"), 3);
  });

  ctx.test("python_build_cases", [&] {
    // Matches both cases in the original Python build() helper.
    ctx.expect_eq(sol.numDecodings("012"), 0);
    ctx.expect_eq(sol.numDecodings("234"), 2);
  });

  ctx.test("zero_handling_edge_cases", [&] {
    ctx.expect_eq(sol.numDecodings("0"), 0);
    ctx.expect_eq(sol.numDecodings("06"), 0);
    ctx.expect_eq(sol.numDecodings("10"), 1);
    ctx.expect_eq(sol.numDecodings("20"), 1);
    ctx.expect_eq(sol.numDecodings("30"), 0);
    ctx.expect_eq(sol.numDecodings("100"), 0);
    ctx.expect_eq(sol.numDecodings("2101"), 1);
  });

  ctx.test("boundary_26_vs_27", [&] {
    ctx.expect_eq(sol.numDecodings("26"), 2);
    ctx.expect_eq(sol.numDecodings("27"), 1);
  });
}

}  // namespace
