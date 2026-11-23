// LeetCode 989: Add to Array-Form of Integer
// https://leetcode.com/problems/add-to-array-form-of-integer/
//
// Problem:
//   The array-form of a non-negative integer `num` is an array of its base-10
//   digits in left-to-right (most-significant to least-significant) order.
//   Given the array-form `num` and a non-negative integer `k`, return the
//   array-form of the integer `num + k`.
//   Constraints: 1 <= num.length <= 10^4, 0 <= num[i] <= 9, no leading zeros
//   except for 0 itself, 1 <= k <= 10^4.
//
// Algorithm:
//   Schoolbook right-to-left addition with carry propagation.
//   Unlike Python's arbitrary-precision `int`, C++ fixed-width integers cannot
//   hold a 10^4-digit number directly. Instead, initialize `carry = k` and
//   iterate over `num` from the least-significant digit to the most-significant
//   digit. At each position:
//     1. Add `num[i]` into `carry`.
//     2. Emit `carry % 10` as the current result digit.
//     3. Update `carry /= 10` to propagate the carry to the next higher place.
//   After exhausting `num`, continue emitting `carry % 10` and dividing by 10
//   while `carry > 0` (which handles both carry-out like 999...9 + 1 and cases
//   where `k` has more digits than `num`). Finally, reverse the collected
//   digits to restore most-significant-first order.
//   Time Complexity:  O(max(N, log10(k))) where N = num.size().
//   Space Complexity: O(max(N, log10(k))) for the output digit vector.
//
// Data Structures:
//   - std::vector<int>: accumulates output digits in reverse (O(1) amortized
//     push_back) before a single in-place reversal.
//
// Real-World Applications:
//   - Arbitrary-precision arithmetic libraries (BigInt / GMP / cryptography):
//     limb-by-limb addition with carry propagation for numbers far exceeding
//     64-bit hardware registers.
//   - Financial & decimal billing systems: exact fixed-point / BCD addition
//     without IEEE-754 floating-point rounding error.
//   - Hardware digital logic: ripple-carry and carry-propagate adders in ALUs.
//
// Related LeetCode Problems:
//   - #66 Plus One [done] (special case where k = 1)
//   - #67 Add Binary [done] (base-2 digit-by-digit addition with carry)
//   - #415 Add Strings [done] (adding two arbitrary-length decimal strings)
//   - #2 Add Two Numbers [done] (digit-by-digit addition on reversed linked lists)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the base-10 array-form of `num + k` using right-to-left carry
  // propagation so arbitrarily long digit arrays never overflow.
  // Time: O(max(N, log10(k))), Space: O(max(N, log10(k))).
  [[nodiscard]] std::vector<int> addToArrayForm(const std::vector<int>& num,
                                                int k) const {
    constexpr int kBase = 10;
    // Reserve space for all existing digits plus up to 6 extra carry digits.
    std::vector<int> result;
    result.reserve(num.size() + 6uz);

    int carry = k;

    // Process digits from least-significant to most-significant, folding the
    // entire remaining `carry` (initially k) into each place value.
    for (std::size_t i = num.size(); i > 0uz; --i) {
      carry += num[i - 1uz];
      result.push_back(carry % kBase);
      carry /= kBase;
    }

    // Drain any leftover carry digits when k exceeds num's digit width or
    // causes a carry out of the most-significant digit.
    while (carry > 0) {
      result.push_back(carry % kBase);
      carry /= kBase;
    }

    // Digits were appended least-significant first; reverse to restore order.
    std::ranges::reverse(result);
    return result;
  }
};

LC_PROBLEM(989, "Add to Array-Form of Integer", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_no_carry_overflow", [&] {
    ctx.expect_eq(sol.addToArrayForm({1, 2, 0, 0}, 34),
                  std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("example_2_internal_carry", [&] {
    ctx.expect_eq(sol.addToArrayForm({2, 7, 4}, 181),
                  std::vector<int>{4, 5, 5});
  });

  ctx.test("example_3_new_leading_digit", [&] {
    ctx.expect_eq(sol.addToArrayForm({2, 1, 5}, 806),
                  std::vector<int>{1, 0, 2, 1});
  });

  ctx.test("example_4_all_nines_from_python_build", [&] {
    ctx.expect_eq(sol.addToArrayForm({9, 9, 9, 9, 9, 9, 9, 9, 9, 9}, 1),
                  std::vector<int>{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  });

  ctx.test("k_wider_than_num", [&] {
    ctx.expect_eq(sol.addToArrayForm({0}, 10000),
                  std::vector<int>{1, 0, 0, 0, 0});
  });

  ctx.test("beyond_uint64_length", [&] {
    // 25 digits of 9 + 1 exceeds 64-bit integers and tests arbitrary length.
    const std::vector<int> twenty_five_nines(25uz, 9);
    std::vector<int> expected(26uz, 0);
    expected[0uz] = 1;
    ctx.expect_eq(sol.addToArrayForm(twenty_five_nines, 1), expected);
  });
}

}  // namespace
