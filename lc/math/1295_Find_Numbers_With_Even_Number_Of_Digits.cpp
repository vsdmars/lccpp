// LeetCode 1295: Find Numbers With Even Number Of Digits
// https://leetcode.com/problems/find-numbers-with-even-number-of-digits/
//
// Problem:
//   Given an array `nums` of positive integers, return how many elements in
//   `nums` contain an even number of base-10 digits (e.g., 12 has 2 digits and
//   7896 has 4 digits, while 345 has 3 digits).
//   Constraints: 1 <= nums.length <= 500, 1 <= nums[i] <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  nums = [12, 345, 2, 6, 7896]
//     Output: 2
//     Explanation:
//       - For 12, countDigits divides by 10 twice (12 -> 1 -> 0) and returns 2
//         (even), incrementing even_digit_count to 1.
//       - For 345, 2, and 6, countDigits returns 3, 1, and 1 (all odd), leaving
//         even_digit_count at 1.
//       - For 7896, countDigits runs 4 divisions (7896 -> 789 -> 78 -> 7 -> 0) to
//         return 4 (even), raising even_digit_count to 2.
//
//   Example 2:
//     Input:  nums = [555, 901, 482, 1771]
//     Output: 1
//     Explanation:
//       - For 555, 901, and 482, countDigits performs 3 divisions each and returns
//         3 (odd), keeping even_digit_count at 0.
//       - For 1771, countDigits performs 4 divisions (1771 -> 177 -> 17 -> 1 -> 0)
//         and returns 4 (even).
//       - Increment even_digit_count to 1 and return 1.
//
//   Example 3:
//     Input:  nums = [1, 9, 10, 99, 100, 999, 1000, 9999, 10000, 99999, 100000]
//     Output: 5
//     Explanation:
//       - Values 1 and 9 have 1 digit (odd), while 10 and 99 take 2 divisions each
//         (even), setting even_digit_count to 2.
//       - Values 100 and 999 have 3 digits (odd), while 1000 and 9999 have 4 digits
//         (even), advancing even_digit_count to 4.
//       - Values 10000 and 99999 have 5 digits (odd), and 100000 takes 6 divisions
//         (even), returning even_digit_count = 5.
//
// Algorithm:
//   Iterate once over `nums` and count the decimal digits of each value by
//   repeatedly dividing by 10 until it reaches 0 (treating 0 as 1 digit).
//   Increment the counter whenever the digit count is even (`digits % 2 == 0`).
//   Unlike string-conversion approaches (`std::to_string(num).size()`),
//   arithmetic digit counting avoids heap allocations and works identically for
//   arbitrary `int` ranges without hardcoding the problem's 10^5 upper bound.
//   Time Complexity:  O(n * d), where n = nums.size() and d = floor(log_10(max)) + 1
//                     (bounded by 6 under constraints, so effectively O(n)).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - None beyond scalar loop variables and the input `std::vector<int>`.
//
// Real-World Applications:
//   - Tabular data formatting and terminal/report column width calculation:
//     determining printed decimal widths of numeric fields without allocating
//     temporary formatted strings.
//   - Data validation and ETL pipelines: filtering fixed-width numeric codes
//     (e.g., 2-, 4-, or 6-digit identifiers) at high throughput.
//
// Related LeetCode Problems:
//   - #1281 Subtract The Product And Sum Of Digits Of An Integer [done] (base-10 digit extraction via division/modulo)
//   - #2180 Count Integers With Even Digit Sum [new] (counting integers matching a digit-parity predicate)
//   - #2520 Count the Digits That Divide a Number [new] (iterating over base-10 digits of an integer)
//   - #9 Palindrome Number [done] (decimal digit manipulation without string conversion)

#include "framework/lc_framework.h"

#include <vector>

namespace {

class Solution {
 public:
  // Returns the count of elements in `nums` that have an even number of
  // decimal digits.
  // Time: O(n * log_10(max_val)), Space: O(1).
  int findNumbers(const std::vector<int>& nums) {
    int even_digit_count = 0;
    for (const int num : nums) {
      if ((countDigits(num) % 2) == 0) {
        ++even_digit_count;
      }
    }
    return even_digit_count;
  }

 private:
  // Counts the number of base-10 digits in `num` without allocating a string.
  // Handles 0 (1 digit) and negative integers cleanly.
  static int countDigits(int num) {
    if (num == 0) {
      return 1;
    }

    int digits = 0;
    while (num != 0) {
      num /= 10;
      ++digits;
    }
    return digits;
  }
};

LC_PROBLEM(1295, "Find Numbers With Even Number Of Digits", "math", ctx) {
  Solution sol;

  ctx.test("example_1_mixed_lengths", [&] {
    // 12 (2 digits) and 7896 (4 digits) have an even number of digits.
    ctx.expect_eq(sol.findNumbers({12, 345, 2, 6, 7896}), 2);
  });

  ctx.test("example_2_single_match", [&] {
    // Only 1771 (4 digits) has an even digit count.
    ctx.expect_eq(sol.findNumbers({555, 901, 482, 1771}), 1);
  });

  ctx.test("power_of_ten_boundaries", [&] {
    // Even-digit values: 10 (2), 99 (2), 1000 (4), 9999 (4), 100000 (6).
    ctx.expect_eq(
        sol.findNumbers({1, 9, 10, 99, 100, 999, 1000, 9999, 10000, 99999,
                         100000}),
        5);
  });

  ctx.test("all_odd_all_even_and_empty", [&] {
    ctx.expect_eq(sol.findNumbers({1, 3, 555, 10000}), 0);
    ctx.expect_eq(sol.findNumbers({10, 42, 1234, 100000}), 4);
    ctx.expect_eq(sol.findNumbers({}), 0);
  });
}

}  // namespace
