// LeetCode 66: Plus One
// https://leetcode.com/problems/plus-one/
//
// Problem:
//   Given a non-empty array `digits` representing a non-negative base-10
//   integer (most significant digit at index 0, each entry in [0, 9], no
//   leading zeros except for 0 itself), increment the integer by one and
//   return the resulting array of digits.
//
// Algorithm:
//   Schoolbook base-10 ripple-carry addition from least significant digit to
//   most significant digit:
//   1. Copy `digits` into `result` and initialize `carry = 1` (representing
//      the "+ 1" increment).
//   2. Traverse `result` in reverse (`std::views::reverse`): add `carry` to
//      the current digit, write `sum % 10` back, and set `carry = sum / 10`.
//      As soon as `carry == 0`, stop early because all higher digits remain
//      unchanged.
//   3. If a carry still remains after the most significant digit (e.g., all
//      digits were 9, such as [9, 9] -> [1, 0, 0]), prepend `carry` at index 0.
//   Because addition operates directly on digit arrays rather than converting
//   to a fixed-width integer, it handles arbitrarily large numbers without
//   overflow.
//   Time complexity: O(n) worst case (e.g., all 9s), O(1) work beyond copy
//     when the trailing digit is < 9.
//   Space complexity: O(n) for the returned digit vector (O(1) extra space).
//
// Data Structures:
//   - `std::vector<int>`: stores the big-endian base-10 digit sequence.
//   - Scalar `carry`: propagates overflow between adjacent decimal places.
//
// Real-World Applications:
//   - Arbitrary-precision (BigInt / decimal) arithmetic libraries used in
//     cryptography, financial accounting, and scientific calculators.
//   - Lexicographic counter / version-number / odometer incrementers in
//     distributed key generators and protocol sequence numbers.
//   - Hardware ripple-carry adder simulation and BCD (Binary-Coded Decimal)
//     arithmetic units.
//
// Related LeetCode Problems:
//   - #2 Add Two Numbers [done] (base-10 carry propagation on linked lists)
//   - #67 Add Binary [done] (base-2 carry propagation on strings)
//   - #369 Plus One Linked List [new] (same problem with singly linked list representation)
//   - #415 Add Strings [done] (general big-integer addition on decimal strings)
//   - #989 Add to Array-Form of Integer [done] (adds an arbitrary integer k instead of 1)

#include "framework/lc_framework.h"

#include <cstddef>
#include <ranges>
#include <vector>

namespace {

class Solution {
 public:
  // Increments the big-endian base-10 integer represented by `digits` by 1.
  //
  // Time: O(n) worst case, Space: O(n) for the output vector.
  std::vector<int> plusOne(const std::vector<int>& digits) {
    std::vector<int> result = digits;

    // Seed carry with 1 to represent adding one at the least significant digit.
    int carry = 1;
    for (int& digit : result | std::views::reverse) {
      const int sum = digit + carry;
      digit = sum % 10;
      carry = sum / 10;
      if (carry == 0) {
        // No further carry propagation needed for more significant digits.
        break;
      }
    }

    // All digits overflowed (e.g. 99 -> 100): prepend the final carry digit.
    if (carry != 0) {
      result.insert(result.begin(), carry);
    }

    return result;
  }
};

LC_PROBLEM(66, "Plus One", "array", ctx) {
  Solution sol;

  ctx.test("example_1_no_carry", [&] {
    ctx.expect_eq(sol.plusOne({1, 2, 3}), std::vector<int>{1, 2, 4});
  });

  ctx.test("example_2_four_digits", [&] {
    ctx.expect_eq(sol.plusOne({4, 3, 2, 1}), std::vector<int>{4, 3, 2, 2});
  });

  ctx.test("all_nines_expands_length", [&] {
    ctx.expect_eq(sol.plusOne({9}), std::vector<int>{1, 0});
    ctx.expect_eq(sol.plusOne({9, 9}), std::vector<int>{1, 0, 0});
    ctx.expect_eq(sol.plusOne({9, 9, 9, 9}), std::vector<int>{1, 0, 0, 0, 0});
  });

  ctx.test("partial_carry_and_zero", [&] {
    ctx.expect_eq(sol.plusOne({0}), std::vector<int>{1});
    ctx.expect_eq(sol.plusOne({1, 9, 9}), std::vector<int>{2, 0, 0});
    ctx.expect_eq(sol.plusOne({4, 3, 2, 2}), std::vector<int>{4, 3, 2, 3});
  });

  ctx.test("exceeds_64_bit_integer", [&] {
    // 25 nines exceeds uint64_t max (~1.84e19), verifying arbitrary precision.
    const std::vector<int> big(25uz, 9);
    std::vector<int> expected(26uz, 0);
    expected[0] = 1;
    ctx.expect_eq(sol.plusOne(big), expected);
  });
}

}  // namespace
