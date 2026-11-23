// LeetCode 43: Multiply Strings
// https://leetcode.com/problems/multiply-strings/
//
// Problem:
//   Given two non-negative integers `num1` and `num2` represented as decimal
//   strings, return their product as a decimal string without using any
//   built-in arbitrary-precision integer library or converting the entire
//   inputs directly into integer types.
//
//   Constraints:
//   - 1 <= num1.length, num2.length <= 200
//   - num1 and num2 consist of digits `'0'`-`'9'` only
//   - Neither input contains leading zeros, except the number `"0"` itself
//
// Algorithm:
//   Grade-school long multiplication into a fixed-size positional digit array:
//   1. Fast-path `"0"` operands to return `"0"` immediately.
//   2. Allocate `digits` of size `num1.size() + num2.size()`, initialized to 0,
//      since the product of an m-digit and an n-digit number has at most m + n
//      digits.
//   3. Iterate `idx1` from `m - 1` down to `0` and `idx2` from `n - 1` down to
//      `0`. The partial product `num1[idx1] * num2[idx2]` contributes directly
//      to positions `low = idx1 + idx2 + 1` (ones digit) and
//      `high = idx1 + idx2` (carry). Add `digits[low]` into the partial sum,
//      store `sum % 10` back into `digits[low]`, and propagate `sum / 10` into
//      `digits[high]`.
//   4. Skip any leading zeros (leaving at least one digit) and convert the
//      remaining digits to ASCII characters.
//
//   Time Complexity:  O(m * n), where m = num1.size() and n = num2.size().
//   Space Complexity: O(m + n) for the positional accumulator and output string.
//
// Data Structures:
//   - `std::vector<int>` of length `m + n`: positional base-10 accumulator
//     storing partial sums and propagating carries in place.
//   - `std::string`: final ASCII representation assembled after stripping
//     leading zeros.
//
// Real-World Applications:
//   - Arbitrary-precision (BigInt) arithmetic libraries (GMP, OpenSSL BIGNUM)
//     used in RSA/ECC public-key cryptography and digital signatures.
//   - Exact fixed-point and decimal financial engines avoiding IEEE-754
//     floating-point rounding drift.
//   - Symbolic algebra systems and polynomial convolution kernels.
//
// Related LeetCode Problems:
//   - #2 Add Two Numbers [done] (carry propagation on digit lists)
//   - #66 Plus One [done] (base-10 carry propagation in a digit array)
//   - #67 Add Binary [done] (string-based base-2 arithmetic with carry)
//   - #415 Add Strings [done] (string-based base-10 addition)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Multiplies two non-negative decimal integer strings `num1` and `num2`
  // using positional digit accumulation.
  // Invariant: after all pairs `(idx1, idx2)` contributing to position `p`
  // have been processed, `digits[p]` is normalized to `[0, 9]`, and `digits[0]`
  // is guaranteed to lie in `[0, 9]` because `num1 * num2 < 10^(m + n)`.
  // Time: O(m * n), Space: O(m + n).
  std::string multiply(std::string_view num1, std::string_view num2) {
    if (num1.empty() || num2.empty() || num1 == "0" || num2 == "0") {
      return "0";
    }

    const std::size_t m = num1.size();
    const std::size_t n = num2.size();
    std::vector<int> digits(m + n, 0);

    // Traverse both factors from least-significant to most-significant digit
    // so carries accumulate naturally into higher positions before those
    // positions are finalized.
    for (std::size_t i = m; i > 0uz; --i) {
      const std::size_t idx1 = i - 1uz;
      const int d1 = static_cast<unsigned char>(num1[idx1]) -
                     static_cast<unsigned char>('0');
      for (std::size_t j = n; j > 0uz; --j) {
        const std::size_t idx2 = j - 1uz;
        const int d2 = static_cast<unsigned char>(num2[idx2]) -
                       static_cast<unsigned char>('0');

        const std::size_t low = idx1 + idx2 + 1uz;
        const std::size_t high = idx1 + idx2;
        const int sum = (d1 * d2) + digits[low];
        digits[low] = sum % 10;
        digits[high] += sum / 10;
      }
    }

    // Advance past leading zeros while preserving at least one trailing digit.
    std::size_t first = 0uz;
    while (first + 1uz < digits.size() && digits[first] == 0) {
      ++first;
    }

    std::string product;
    product.reserve(digits.size() - first);
    for (std::size_t k = first; k < digits.size(); ++k) {
      product.push_back(static_cast<char>('0' + digits[k]));
    }
    return product;
  }
};

LC_PROBLEM(43, "Multiply Strings", "string", ctx) {
  Solution sol;

  ctx.test("examples_and_build_cases", [&] {
    ctx.expect_eq(sol.multiply("2", "3"), "6");
    ctx.expect_eq(sol.multiply("123", "456"), "56088");
    ctx.expect_eq(sol.multiply("0", "0"), "0");
    ctx.expect_eq(sol.multiply("6", "4"), "24");
    ctx.expect_eq(sol.multiply("123", "34"), "4182");
    ctx.expect_eq(sol.multiply("9", "9"), "81");
    ctx.expect_eq(sol.multiply("4", "5"), "20");
    ctx.expect_eq(sol.multiply("9999", "87"), "869913");
  });

  ctx.test("zero_operand_fast_path", [&] {
    ctx.expect_eq(sol.multiply("12345", "0"), "0");
    ctx.expect_eq(sol.multiply("0", "98765"), "0");
  });

  ctx.test("beyond_64bit_integer_precision", [&] {
    // 10^20 * (10^20 - 1)
    ctx.expect_eq(
        sol.multiply("100000000000000000000", "99999999999999999999"),
        "9999999999999999999900000000000000000000");
    // (10^20 - 1)^2 = 10^40 - 2*10^20 + 1
    ctx.expect_eq(
        sol.multiply("99999999999999999999", "99999999999999999999"),
        "9999999999999999999800000000000000000001");
  });
}

}  // namespace
