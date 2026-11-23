// LeetCode 639: Decode Ways II
// https://leetcode.com/problems/decode-ways-ii/
//
// Problem:
//   A message containing uppercase letters 'A'-'Z' is encoded to numbers via
//   'A' -> "1", 'B' -> "2", ..., 'Z' -> "26". The encoded string `s` contains
//   digits '0'-'9' and wildcard characters '*', where each '*' independently
//   represents any single digit from '1' to '9' (never '0'). Return the total
//   number of valid ways to decode `s`, modulo 10^9 + 7.
//
// Algorithm:
//   Constant-space bottom-up dynamic programming scanning `s` left to right.
//   Let dp[k] be the number of ways to decode the prefix `s[0..k)` modulo
//   10^9 + 7.
//   - Base cases: dp[0] = 1 (one empty decoding), dp[1] = waysSingle(s[0]).
//   - Transition for index i (1 <= i < n), corresponding to prefix length i + 1:
//       dp[i + 1] = (waysSingle(s[i]) * dp[i]
//                    + waysPair(s[i - 1], s[i]) * dp[i - 1]) mod (10^9 + 7),
//     where:
//       * waysSingle(c) is 9 for '*', 0 for '0', and 1 for '1'..'9'.
//       * waysPair(a, b) counts how many assignments of '*' in (a, b) form a
//         two-digit number in [10, 26]:
//           - "**" -> 15 (11..19 and 21..26)
//           - "*b" -> 2 if b in '0'..'6', else 1 (only '1'b is <= 26)
//           - "1*" -> 9, "2*" -> 6, other digit + '*' -> 0
//           - two digits -> 1 if 10 <= (10*a + b) <= 26, else 0.
//   Since each state depends only on the previous two states, two rolling
//   64-bit scalars (`prev2`, `prev1`) suffice.
//   Time Complexity: O(n), where n = s.size().
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two rolling scalar DP registers (`std::int64_t` `prev2`, `prev1`):
//     track dp[i - 1] and dp[i] without allocating a DP array or lookup maps.
//
// Real-World Applications:
//   - Ambiguous tokenization / lattice path counting in natural language
//     processing, speech recognition, and IME (input method editor) decoders.
//   - Counting valid parses of variable-length prefix-free / non-prefix-free
//     wire encodings with masked or erased bytes.
//
// Related LeetCode Problems:
//   - #91 Decode Ways [done] (digit-only predecessor without '*' wildcards)
//   - #70 Climbing Stairs [done] (base two-step recurrence pattern)
//   - #1416 Restore The Array [new] (generalized range-bounded digit segmentation DP)
//   - #1977 Number of Ways to Separate Numbers [new] (non-decreasing numeric segmentation DP)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace {

constexpr std::int64_t kMod = 1'000'000'007;

class Solution {
 public:
  // Returns the number of valid decodings of `s` modulo 10^9 + 7 in O(n) time
  // and O(1) space.
  int numDecodings(std::string_view s) {
    if (s.empty()) {
      return 0;
    }

    // Rolling DP states:
    //   prev2 = ways to decode prefix s[0..i - 1)
    //   prev1 = ways to decode prefix s[0..i)
    std::int64_t prev2 = 1;
    std::int64_t prev1 = waysSingle(s[0]);

    for (std::size_t i = 1uz; i < s.size(); ++i) {
      const std::int64_t single_ways = (waysSingle(s[i]) * prev1) % kMod;
      const std::int64_t pair_ways =
          (waysPair(s[i - 1uz], s[i]) * prev2) % kMod;
      const std::int64_t curr = (single_ways + pair_ways) % kMod;

      prev2 = prev1;
      prev1 = curr;
    }

    return static_cast<int>(prev1);
  }

 private:
  // Number of valid 1-character encodings ('1'..'9') represented by `ch`.
  static constexpr std::int64_t waysSingle(char ch) noexcept {
    if (ch == '*') {
      return 9;
    }
    if (ch >= '1' && ch <= '9') {
      return 1;
    }
    return 0;
  }

  // Number of valid 2-character encodings ("10".."26") represented by
  // `(first, second)`, given that '*' can only match '1'..'9'.
  static constexpr std::int64_t waysPair(char first, char second) noexcept {
    if (first == '*' && second == '*') {
      // 11..19 (9 ways) + 21..26 (6 ways) = 15 ways.
      return 15;
    }
    if (first == '*') {
      // If second is '0'..'6', first can be '1' or '2'; for '7'..'9', only '1'.
      if (second >= '0' && second <= '6') {
        return 2;
      }
      if (second >= '7' && second <= '9') {
        return 1;
      }
      return 0;
    }
    if (second == '*') {
      if (first == '1') {
        return 9;  // 11..19
      }
      if (first == '2') {
        return 6;  // 21..26
      }
      return 0;
    }
    if (first < '1' || first > '2' || second < '0' || second > '9') {
      return 0;
    }
    const int value = ((first - '0') * 10) + (second - '0');
    return (value >= 10 && value <= 26) ? 1 : 0;
  }
};

LC_PROBLEM(639, "Decode Ways II", "dp", ctx) {
  ctx.test("examples_from_prompt", [&] {
    Solution sol;
    ctx.expect_eq(sol.numDecodings("*"), 9);
    ctx.expect_eq(sol.numDecodings("1*"), 18);
    ctx.expect_eq(sol.numDecodings("2*"), 15);
  });

  ctx.test("python_build_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.numDecodings("**"), 96);
    ctx.expect_eq(sol.numDecodings("**129*212"), 6510);
  });

  ctx.test("zeros_and_invalid_sequences", [&] {
    Solution sol;
    ctx.expect_eq(sol.numDecodings("0"), 0);
    ctx.expect_eq(sol.numDecodings("0*"), 0);
    ctx.expect_eq(sol.numDecodings("*0"), 2);
    ctx.expect_eq(sol.numDecodings("10"), 1);
    ctx.expect_eq(sol.numDecodings("30"), 0);
    ctx.expect_eq(sol.numDecodings("*7"), 10);
  });

  ctx.test("wildcard_growth_and_modulo", [&] {
    Solution sol;
    // Recurrence for k stars: a_0 = 1, a_1 = 9, a_k = 9*a_{k-1} + 15*a_{k-2}.
    ctx.expect_eq(sol.numDecodings("***"), 999);
    ctx.expect_eq(sol.numDecodings("****"), 10431);
    ctx.expect_eq(sol.numDecodings("*****"), 108864);

    // Ensure large inputs stay properly reduced modulo 10^9 + 7 without
    // signed overflow.
    const std::string many_stars(100uz, '*');
    const int res = sol.numDecodings(many_stars);
    ctx.expect_true(res >= 0 && res < static_cast<int>(kMod));
  });
}

}  // namespace
