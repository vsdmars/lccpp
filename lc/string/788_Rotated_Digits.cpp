// LeetCode 788: Rotated Digits
// https://leetcode.com/problems/rotated-digits/
//
// Problem:
//   An integer x is called a "good" number if, after rotating every decimal
//   digit individually by 180 degrees (without reversing the digit order), the
//   resulting string forms a valid number that is strictly different from x.
//   Specifically:
//     - 0, 1, 8 rotate to themselves (valid, unchanged).
//     - 2 <-> 5 and 6 <-> 9 rotate to each other (valid, changed).
//     - 3, 4, 7 do not form valid digits when rotated (invalid).
//   Given a positive integer `n`, return how many integers in [1, n] are good.
//
// Examples:
//   Example 1:
//     Input:  n = 10
//     Output: 4
//     Explanation:
//       - Iterate x from 1 to 10 and inspect each decimal digit via kDigitState[x %
//         10].
//       - Reject x = 3, 4, and 7 immediately upon seeing state -1, and reject x =
//         1, 8, and 10 because all their digits have state 0.
//       - Accept x = 2, 5, 6, and 9 because each has only valid digits with at
//         least one state 1 digit, returning good_count = 4.
//
//   Example 2:
//     Input:  n = 20
//     Output: 9
//     Explanation:
//       - Count 4 good numbers in [1, 10] from {2, 5, 6, 9}.
//       - For x in [11, 19], the tens digit 1 has state 0, so 12, 15, 16, and 19
//         pass via their units digit (increasing good_count to 8), while 13, 14, 17
//         fail on -1 and 11, 18 stay unchanged.
//       - For x = 20, digit 0 has state 0 and digit 2 has state 1, so is_good(20)
//         returns true and yields good_count = 9.
//
//   Example 3:
//     Input:  n = 1
//     Output: 0
//     Explanation:
//       - Run the loop once for x = 1 and look up kDigitState[1] = 0.
//       - Finish the digit loop with has_changing_digit still false, so is_good(1)
//         returns false and good_count remains 0.
//
// Algorithm:
//   Classify each decimal digit 0..9 in a compile-time lookup table as:
//     - -1 (invalid: 3, 4, 7),
//     -  0 (valid but unchanged: 0, 1, 8),
//     - +1 (valid and changed: 2, 5, 6, 9).
//   For every integer x in [1, n], inspect its decimal digits via repeated
//   modulo/division by 10 (avoiding any string allocation). If any digit has
//   state -1, reject x immediately. Otherwise x is good if and only if at least
//   one digit has state +1.
//   Time Complexity:  O(n log10 n) — at most 5 digits inspected per integer up
//                     to n <= 10^4.
//   Space Complexity: O(1) — fixed 10-element compile-time lookup array.
//
// Data Structures:
//   - std::array<int, 10>: compile-time digit classification table mapping each
//     decimal digit to its rotation category.
//
// Real-World Applications:
//   - Validating ambigram / upside-down-readable serial numbers, parking
//     badges, and PCB component markings.
//   - Input validation and character-set filtering for seven-segment LED/LCD
//     displays that can be mounted inverted.
//   - Constraint-based enumeration over decimal digit sets in numeric puzzle
//     and code-generation tools.
//
// Related LeetCode Problems:
//   - #246 Strobogrammatic Number [done] (full-number 180-degree rotation equality)
//   - #247 Strobogrammatic Number II [done] (generating all n-digit strobogrammatic numbers)
//   - #1056 Confusing Number [new] (180-degree rotation of the whole number differs)
//   - #1088 Confusing Number II [done] (counting confusing numbers in [1, n])

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>

namespace {

class Solution {
 public:
  // Counts how many integers in [1, n] remain valid and strictly change when
  // each decimal digit is individually rotated by 180 degrees.
  // Time: O(n log10 n), Space: O(1).
  int rotatedDigits(int n) {
    int good_count = 0;
    for (int x = 1; x <= n; ++x) {
      if (is_good(x)) {
        ++good_count;
      }
    }
    return good_count;
  }

 private:
  // Digit classification under 180-degree individual rotation:
  //   -1 -> invalid (3, 4, 7)
  //    0 -> valid, rotates to itself (0, 1, 8)
  //    1 -> valid, rotates to a different digit (2 <-> 5, 6 <-> 9)
  static constexpr std::array<int, 10> kDigitState = {
      0,   // 0 -> 0
      0,   // 1 -> 1
      1,   // 2 -> 5
      -1,  // 3 -> invalid
      -1,  // 4 -> invalid
      1,   // 5 -> 2
      1,   // 6 -> 9
      -1,  // 7 -> invalid
      0,   // 8 -> 8
      1,   // 9 -> 6
  };

  // Returns true iff every digit of `x` is rotatable and at least one digit
  // changes under rotation.
  static bool is_good(int x) {
    bool has_changing_digit = false;
    while (x > 0) {
      const int state = kDigitState[static_cast<std::size_t>(x % 10)];
      if (state == -1) {
        return false;
      }
      if (state == 1) {
        has_changing_digit = true;
      }
      x /= 10;
    }
    return has_changing_digit;
  }
};

LC_PROBLEM(788, "Rotated Digits", "string", ctx) {
  Solution sol;

  ctx.test("example_1_n_10", [&] {
    // Good numbers in [1, 10]: 2, 5, 6, 9.
    ctx.expect_eq(sol.rotatedDigits(10), 4);
  });

  ctx.test("source_build_case_n_1", [&] {
    ctx.expect_eq(sol.rotatedDigits(1), 0);
  });

  ctx.test("n_2_first_good_number", [&] {
    ctx.expect_eq(sol.rotatedDigits(2), 1);
  });

  ctx.test("n_20_multi_digit", [&] {
    // 2, 5, 6, 9, 12, 15, 16, 19, 20.
    ctx.expect_eq(sol.rotatedDigits(20), 9);
  });

  ctx.test("n_100_combinatorial_check", [&] {
    // In 00..99: 7^2 valid - 3^2 unchanged = 49 - 9 = 40; 100 is unchanged.
    ctx.expect_eq(sol.rotatedDigits(100), 40);
  });

  ctx.test("n_1000_combinatorial_check", [&] {
    // In 000..999: 7^3 - 3^3 = 343 - 27 = 316; 1000 is unchanged.
    ctx.expect_eq(sol.rotatedDigits(1000), 316);
  });
}

}  // namespace
