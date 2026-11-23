// LeetCode 564: Find The Closest Palindrome
// https://leetcode.com/problems/find-the-closest-palindrome/
//
// Problem:
//   Given a string `n` representing a positive base-10 integer with no leading
//   zeros, return the string representation of the closest palindromic integer
//   (strictly excluding `n` itself), where closeness is measured by minimum
//   absolute difference. If two palindromes tie in distance, return the
//   smaller one.
//   Constraints: 1 <= n.size() <= 18, 1 <= int(n) <= 10^18 - 1.
//
// Examples:
//   Example 1:
//     Input:  n = "123"
//     Output: "121"
//     Explanation:
//       - Record cross-length boundary candidates 10^2 - 1 = 99 and 10^3 + 1 = 1001
//         alongside num = 123.
//       - With len = 3, half_len = 2, and is_odd = true, shift prefix = 12 by {-1,
//         0, 1} to get 11, 12, and 13 (all within [10, 99]) and mirror them into
//         111, 121, and 131.
//       - Compare absolute differences from 123: |99 - 123| = 24, |1001 - 123| =
//         878, |111 - 123| = 12, |121 - 123| = 2, and |131 - 123| = 8.
//       - Select 121 as the candidate with minimum distance 2 and return "121".
//
//   Example 2:
//     Input:  n = "100"
//     Output: "99"
//     Explanation:
//       - Generate boundary candidates 10^2 - 1 = 99 and 10^3 + 1 = 1001 for len =
//         3 and num = 100.
//       - Mirror valid shifted prefixes from prefix = 10 (skipping 9 since it is
//         below min_half = 10): 10 mirrors to 101 and 11 mirrors to 111.
//       - Evaluate distances: |99 - 100| = 1, |1001 - 100| = 901, |101 - 100| = 1,
//         and |111 - 100| = 11.
//       - Break the distance-1 tie between 99 and 101 in favor of the smaller
//         candidate, returning "99".
//
//   Example 3:
//     Input:  n = "999999999999999999"
//     Output: "1000000000000000001"
//     Explanation:
//       - For len = 18, add boundary candidates 10^17 - 1 = 99999999999999999 and
//         10^18 + 1 = 1000000000000000001.
//       - With half_len = 9, is_odd = false, and prefix = 999999999, prefix - 1 =
//         999999998 mirrors to 999999998899999999, prefix mirrors to num itself
//         (excluded), and prefix + 1 = 1000000000 exceeds max_half = 999999999 so
//         it is skipped to avoid 20-digit overflow.
//       - Among valid candidates, 1000000000000000001 has the smallest distance
//         |1000000000000000001 - 999999999999999999| = 2 and is returned.
//
// Algorithm:
//   Candidate generation around prefix mirroring and digit-length boundaries.
//   Let L = n.size() and let `prefix` be the integer formed by the first
//   half_len = (L + 1) / 2 digits of `n`.
//   1. Different digit length:
//      - The closest palindrome with fewer than L digits is 10^(L - 1) - 1
//        (e.g., 99...9, or 0 when L == 1).
//      - The closest palindrome with more than L digits is 10^L + 1
//        (e.g., 100...001).
//   2. Same digit length L:
//      - Because L-digit palindromes are strictly monotonic in their first
//        `half_len` digits, the closest L-digit palindrome(s) must be formed
//        by mirroring `prefix - 1`, `prefix`, or `prefix + 1` (skipping any
//        shifted prefix whose digit length crosses a power-of-10 boundary,
//        since those boundaries are already covered by step 1 and skipping
//        prevents 20-digit overflow when L == 18).
//   Evaluate at most 5 candidates in 64-bit integer arithmetic (10^18 + 1 fits
//   comfortably inside `std::int64_t`), exclude `num` itself, and select the
//   candidate minimizing |candidate - num| with ties broken by smaller value.
//   Time Complexity:  O(L) where L <= 18 (effectively O(1)).
//   Space Complexity: O(L) (effectively O(1)).
//
// Data Structures:
//   - `std::vector<std::int64_t>` (at most 5 elements): stores candidate
//     palindromes for deterministic closest-match selection.
//
// Real-World Applications:
//   - Numeric rounding and snapping to constrained symmetric codes (e.g.,
//     checksummed serial numbers, symmetric memory/tag identifiers).
//   - Boundary-value test generation for decimal parsers and arbitrary-
//     precision arithmetic libraries.
//   - Nearest structured-pattern search over lexicographically ordered
//     numeric spaces without brute-force enumeration.
//
// Related LeetCode Problems:
//   - #9 Palindrome Number [done] (checking whether an integer is a palindrome)
//   - #2396 Strictly Palindromic Number [new] (palindrome properties across bases)
//   - #2967 Minimum Cost to Make Array Equalindromic [new] (uses closest palindrome around the median)
//   - #866 Prime Palindrome [new] (generating palindromes by mirroring prefixes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the closest palindromic integer to `n` (excluding `n` itself),
  // breaking ties in favor of the smaller palindrome.
  //
  // Time: O(L) with L <= 18 (O(1)), Space: O(1).
  std::string nearestPalindromic(std::string_view n) {
    if (n.empty()) {
      return {};
    }

    const std::size_t len = n.size();
    const std::int64_t num = parse_int64(n);

    // Collect the up to 5 candidates that can possibly be optimal:
    //   1) 10^(len - 1) - 1  (largest palindrome with fewer digits)
    //   2) 10^len + 1        (smallest palindrome with more digits)
    //   3..5) L-digit palindromes mirrored from prefix + {-1, 0, +1}.
    std::vector<std::int64_t> candidates;
    candidates.reserve(5uz);
    candidates.push_back(pow10(len - 1uz) - 1);
    candidates.push_back(pow10(len) + 1);

    const std::size_t half_len = (len + 1uz) / 2uz;
    const bool is_odd = (len % 2uz) != 0uz;
    const std::int64_t prefix = parse_int64(n.substr(0uz, half_len));

    // Keep mirrored prefixes within the exact `half_len`-digit range so that
    // mirrored values always have length `len` and never overflow int64_t
    // (e.g. when n == "999999999999999999", prefix + 1 would mirror to 20
    // digits, while 10^18 + 1 is already in `candidates`).
    const std::int64_t min_half =
        (len == 1uz) ? 0 : pow10(half_len - 1uz);
    const std::int64_t max_half = pow10(half_len) - 1;

    for (const std::int64_t delta : {-1, 0, 1}) {
      const std::int64_t shifted = prefix + delta;
      if (shifted >= min_half && shifted <= max_half) {
        candidates.push_back(mirror_half(shifted, is_odd));
      }
    }

    // Select the closest candidate != num, breaking ties by smaller value.
    std::int64_t best = -1;
    std::int64_t best_diff = std::numeric_limits<std::int64_t>::max();
    for (const std::int64_t cand : candidates) {
      if (cand == num) {
        continue;
      }
      const std::int64_t diff = abs_diff(cand, num);
      if (diff < best_diff || (diff == best_diff && cand < best)) {
        best_diff = diff;
        best = cand;
      }
    }

    return std::to_string(best);
  }

 private:
  // Parses a base-10 non-negative integer string into `std::int64_t`.
  static std::int64_t parse_int64(std::string_view sv) {
    std::int64_t value = 0;
    for (const char ch : sv) {
      value = (value * 10) + static_cast<std::int64_t>(ch - '0');
    }
    return value;
  }

  // Computes 10^exp for 0 <= exp <= 18 without floating-point inaccuracy.
  static std::int64_t pow10(std::size_t exp) {
    std::int64_t result = 1;
    for (std::size_t i = 0uz; i < exp; ++i) {
      result *= 10;
    }
    return result;
  }

  // Mirrors `half` into a palindrome of length 2*digits (even) or
  // 2*digits - 1 (odd, omitting the repeated middle digit).
  static std::int64_t mirror_half(std::int64_t half, bool is_odd) {
    std::int64_t pal = half;
    std::int64_t tail = is_odd ? (half / 10) : half;
    while (tail > 0) {
      pal = (pal * 10) + (tail % 10);
      tail /= 10;
    }
    return pal;
  }

  // Computes |a - b| safely for non-negative 64-bit integers.
  static std::int64_t abs_diff(std::int64_t a, std::int64_t b) {
    return (a >= b) ? (a - b) : (b - a);
  }
};

LC_PROBLEM(564, "Find The Closest Palindrome", "math", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    ctx.expect_eq(sol.nearestPalindromic("123"), std::string("121"));
  });

  ctx.test("example_2_single_digit_one", [&] {
    ctx.expect_eq(sol.nearestPalindromic("1"), std::string("0"));
  });

  ctx.test("python_build_power_of_ten_tie", [&] {
    // Both 99 and 101 are at distance 1; tie breaks to smaller (99).
    ctx.expect_eq(sol.nearestPalindromic("100"), std::string("99"));
  });

  ctx.test("all_nines_crosses_digit_boundary", [&] {
    ctx.expect_eq(sol.nearestPalindromic("99"), std::string("101"));
    ctx.expect_eq(sol.nearestPalindromic("999"), std::string("1001"));
  });

  ctx.test("already_palindrome_even_and_odd", [&] {
    ctx.expect_eq(sol.nearestPalindromic("1221"), std::string("1111"));
    ctx.expect_eq(sol.nearestPalindromic("1001"), std::string("999"));
    ctx.expect_eq(sol.nearestPalindromic("12321"), std::string("12221"));
  });

  ctx.test("ten_and_eleven", [&] {
    ctx.expect_eq(sol.nearestPalindromic("10"), std::string("9"));
    ctx.expect_eq(sol.nearestPalindromic("11"), std::string("9"));
  });

  ctx.test("max_18_digit_inputs_no_overflow", [&] {
    ctx.expect_eq(sol.nearestPalindromic("999999999999999999"),
                  std::string("1000000000000000001"));
    ctx.expect_eq(sol.nearestPalindromic("100000000000000000"),
                  std::string("99999999999999999"));
    ctx.expect_eq(sol.nearestPalindromic("807045053224792883"),
                  std::string("807045053350540708"));
  });
}

}  // namespace
