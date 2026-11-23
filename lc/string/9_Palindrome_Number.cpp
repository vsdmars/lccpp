// LeetCode 9: Palindrome Number
// https://leetcode.com/problems/palindrome-number/
//
// Problem:
//   Given a 32-bit signed integer `x`, return `true` if `x` reads the same
//   backward as forward in base 10, and `false` otherwise.
//
//   Constraints:
//   - -2^31 <= x <= 2^31 - 1
//   - Follow-up: solve without converting the integer into a string.
//
// Algorithm:
//   Half-number arithmetic reversal (satisfies the O(1)-space follow-up without
//   string conversion or overflow risk):
//   1. Reject negative integers immediately (the leading `'-'` cannot match the
//      rightmost digit) and reject non-zero multiples of 10 (a trailing `'0'`
//      would require a matching leading `'0'`).
//   2. Repeatedly pop the rightmost digit from `x` and append it to
//      `reversed_half` until `x <= reversed_half`. Because the loop stops at
//      the midpoint of `x`, `reversed_half` never exceeds 6 decimal digits for
//      any 32-bit `int`, guaranteeing zero overflow.
//   3. For an even number of digits, the halves must match exactly
//      (`x == reversed_half`). For an odd number of digits, the middle digit
//      sits at the end of `reversed_half` and is discarded via
//      `x == reversed_half / 10`.
//
//   A secondary `isPalindromeViaString` helper is also included to mirror the
//   direct string two-pointer approach.
//
//   Time Complexity:  O(log10(|x|)) — processes half of the decimal digits.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two scalar `int` accumulators (`x` and `reversed_half`) for the optimal
//     arithmetic solution; `std::string` with two indices for the string
//     variant.
//
// Real-World Applications:
//   - Checksum and symmetric identifier validation in embedded/low-memory
//     firmware where heap allocations are forbidden.
//   - Base-conversion and digit-extraction primitives in financial terminals
//     and formatting libraries.
//   - Symmetry detection in signal/sequence identifiers and coding theory.
//
// Related LeetCode Problems:
//   - #7 Reverse Integer [done] (digit-by-digit decimal integer reversal)
//   - #125 Valid Palindrome [done] (two-pointer palindrome verification on text)
//   - #234 Palindrome Linked List [done] (half-reversal palindrome check on lists)
//   - #564 Find the Closest Palindrome [done] (constructing nearest palindromic integer)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <string>

namespace {

class Solution {
 public:
  // Checks whether `x` is a base-10 palindrome by reversing only the lower half
  // of its digits, avoiding both heap allocations and integer overflow.
  // Time: O(log10(|x|)), Space: O(1).
  bool isPalindrome(int x) {
    // Negative numbers fail due to the minus sign; positive multiples of 10
    // fail because non-zero integers never have a leading zero.
    if (x < 0 || (x % 10 == 0 && x != 0)) {
      return false;
    }

    int reversed_half = 0;
    // Invariant: `reversed_half` holds the reversed suffix of digits removed
    // from `x`. Stopping once `x <= reversed_half` guarantees we have crossed
    // the decimal midpoint.
    while (x > reversed_half) {
      reversed_half = (reversed_half * 10) + (x % 10);
      x /= 10;
    }

    // Even digit count: `x == reversed_half`.
    // Odd digit count:  drop the shared center digit via `reversed_half / 10`.
    return x == reversed_half || x == reversed_half / 10;
  }

  // Alternative string-based two-pointer check mirroring the Python baseline.
  // Time: O(log10(|x|)), Space: O(log10(|x|)).
  bool isPalindromeViaString(int x) {
    if (x < 0) {
      return false;
    }

    const std::string digits = std::to_string(x);
    std::size_t left = 0uz;
    std::size_t right = digits.size();
    while (left + 1uz < right) {
      --right;
      if (digits[left] != digits[right]) {
        return false;
      }
      ++left;
    }
    return true;
  }
};

LC_PROBLEM(9, "Palindrome Number", "string", ctx) {
  Solution sol;

  ctx.test("examples_from_statement", [&] {
    ctx.expect_true(sol.isPalindrome(121));
    ctx.expect_false(sol.isPalindrome(-121));
    ctx.expect_false(sol.isPalindrome(10));
    ctx.expect_true(sol.isPalindrome(101));
  });

  ctx.test("zero_and_single_digits", [&] {
    ctx.expect_true(sol.isPalindrome(0));
    ctx.expect_true(sol.isPalindrome(7));
    ctx.expect_false(sol.isPalindrome(-1));
  });

  ctx.test("even_odd_and_trailing_zeros", [&] {
    ctx.expect_true(sol.isPalindrome(1221));
    ctx.expect_false(sol.isPalindrome(1231));
    ctx.expect_false(sol.isPalindrome(100));
    ctx.expect_false(sol.isPalindrome(1000021));
  });

  ctx.test("int32_boundaries_and_string_equivalence", [&] {
    constexpr int kMax = std::numeric_limits<int>::max();
    constexpr int kMin = std::numeric_limits<int>::min();
    ctx.expect_false(sol.isPalindrome(kMax));
    ctx.expect_false(sol.isPalindrome(kMin));
    ctx.expect_true(sol.isPalindrome(2147447412));

    for (const int val : {-121, -1, 0, 7, 10, 101, 1221, 12321, 12345, kMax}) {
      ctx.expect_eq(sol.isPalindrome(val), sol.isPalindromeViaString(val));
    }
  });
}

}  // namespace
