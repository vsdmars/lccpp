// LeetCode 415: Add Strings
// https://leetcode.com/problems/add-strings/
//
// Problem:
//   Given two non-negative integers `num1` and `num2` represented as decimal
//   strings (1 <= num1.length, num2.length <= 10^4, digits '0'-'9' only, no
//   leading zeros except "0" itself), return their sum as a decimal string.
//   Built-in arbitrary-precision integer libraries (e.g. BigInteger) and
//   converting the entire inputs directly to integers are disallowed.
//
// Examples:
//   Example 1:
//     Input:  num1 = "456", num2 = "77"
//     Output: "533"
//     Explanation:
//       - Initialize `i = 3`, `j = 2`, `carry = 0`, and `result = ""`.
//       - Decrement to `i = 2` ('6') and `j = 1` ('7'), compute `sum = 0 + 6 + 7 =
//         13`, append `'3'` (`result = "3"`), and set `carry = 1`.
//       - Decrement to `i = 1` ('5') and `j = 0` ('7'), compute `sum = 1 + 5 + 7 =
//         13`, append `'3'` (`result = "33"`), and set `carry = 1`.
//       - With `j == 0`, decrement only `i = 0` ('4'), compute `sum = 1 + 4 = 5`,
//         append `'5'` (`result = "335"`), and set `carry = 0`.
//       - Exit the loop since `i == 0`, `j == 0`, and `carry == 0`, then reverse
//         `"335"` to return `"533"`.
//
//   Example 2:
//     Input:  num1 = "999", num2 = "1"
//     Output: "1000"
//     Explanation:
//       - Initialize `i = 3`, `j = 1`, `carry = 0`, and `result = ""`.
//       - Decrement to `i = 2` ('9') and `j = 0` ('1'), compute `sum = 0 + 9 + 1 =
//         10`, append `'0'` (`result = "0"`), and set `carry = 1`.
//       - With `j == 0`, process `i = 1` ('9') and then `i = 0` ('9') with `carry =
//         1`, yielding `sum = 10` each time and appending two more `'0'`s (`result
//         = "000"`, `carry = 1`).
//       - Run one final iteration because `carry != 0`, compute `sum = 1`, append
//         `'1'` (`result = "0001"`), set `carry = 0`, and reverse to return
//         `"1000"`.
//
//   Example 3:
//     Input:  num1 = "0", num2 = "0"
//     Output: "0"
//     Explanation:
//       - Initialize `i = 1`, `j = 1`, `carry = 0`, and `result = ""`.
//       - Decrement to `i = 0` ('0') and `j = 0` ('0'), compute `sum = 0 + 0 + 0 =
//         0`, append `'0'` (`result = "0"`), and set `carry = 0`.
//       - Terminate the loop and reverse `"0"` to return `"0"`.
//
// Algorithm:
//   Implements both an iterative (`addStrings`) and a recursive
//   (`addStringsRecursive`) elementary-school digit-by-digit addition from
//   least-significant to most-significant digit. Indices `i` and `j` walk
//   backward from the ends of `num1` and `num2` while maintaining an integer
//   `carry`. As long as `i > 0 || j > 0 || carry != 0`, available digits are
//   converted with `ch - '0'` and summed with `carry`; the ones digit
//   `sum % 10` is appended to `result` and `carry` becomes `sum / 10`. Passing
//   indices into the recursive lambda avoids quadratic string slicing. A
//   single in-place `std::ranges::reverse` restores most-significant-first
//   order.
//   - Iterative (`addStrings`):          Time O(max(m, n)), Space O(max(m, n))
//     for output (O(1) auxiliary).
//   - Recursive (`addStringsRecursive`): Time O(max(m, n)), Space O(max(m, n))
//     for output and recursion stack.
//
// Data Structures:
//   - `std::string_view`: Non-owning views over `num1` and `num2` for
//     zero-copy backward indexing.
//   - `std::string`: Output buffer pre-reserved to `max(m, n) + 1` bytes to
//     avoid reallocations while appending digits in reverse order.
//   - Scalar counters (`i`, `j`, `carry`) and, in the recursive variant, the
//     call stack via a C++23 deducing-`this` lambda (`dfs`).
//
// Real-World Applications:
//   - Exact arbitrary-precision decimal arithmetic in financial ledgers,
//     billing engines, and scientific calculators where 64-bit integers
//     overflow and IEEE-754 floats lose precision.
//   - Core addition kernels in BigInt libraries (GMP, OpenSSL BIGNUM, and
//     language runtimes).
//   - Serialization and telemetry systems summing unbounded numeric counters
//     encoded as ASCII text.
//
// Related LeetCode Problems:
//   - #43 Multiply Strings [done] (arbitrary-precision multiplication of decimal
//     strings)
//   - #67 Add Binary [done] (identical right-to-left carry propagation in base 2)
//   - #2 Add Two Numbers [done] (digit-by-digit addition with carry over linked lists)
//   - #66 Plus One [done] (adding 1 with carry propagation across a digit array)
//   - #989 Add to Array-Form of Integer [done] (adding an integer to a digit array)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Digit-by-digit addition from least-significant to most-significant digit
  // with carry propagation, followed by a single reversal.
  //
  // Time:  O(max(m, n))
  // Space: O(max(m, n)) for the output string
  std::string addStrings(std::string_view num1, std::string_view num2) {
    std::string result;
    result.reserve(std::max(num1.size(), num2.size()) + 1uz);

    std::size_t i = num1.size();
    std::size_t j = num2.size();
    int carry = 0;

    while (i > 0uz || j > 0uz || carry != 0) {
      int sum = carry;
      if (i > 0uz) {
        --i;
        sum += num1[i] - '0';
      }
      if (j > 0uz) {
        --j;
        sum += num2[j] - '0';
      }
      result.push_back(static_cast<char>('0' + (sum % 10)));
      carry = sum / 10;
    }

    std::ranges::reverse(result);
    return result;
  }

  // Recursive digit-by-digit addition passing indices and carry (avoids
  // quadratic string slicing).
  //
  // Time:  O(max(m, n))
  // Space: O(max(m, n)) recursion stack + output
  std::string addStringsRecursive(std::string_view num1,
                                  std::string_view num2) {
    std::string result;
    result.reserve(std::max(num1.size(), num2.size()) + 1uz);

    auto dfs = [&](this auto&& self, std::size_t i, std::size_t j,
                   int carry) -> void {
      if (i == 0uz && j == 0uz && carry == 0) {
        return;
      }
      int sum = carry;
      if (i > 0uz) {
        --i;
        sum += num1[i] - '0';
      }
      if (j > 0uz) {
        --j;
        sum += num2[j] - '0';
      }
      result.push_back(static_cast<char>('0' + (sum % 10)));
      self(i, j, sum / 10);
    };

    dfs(num1.size(), num2.size(), 0);
    std::ranges::reverse(result);
    return result;
  }
};

LC_PROBLEM(415, "Add Strings", "recursion", ctx) {
  Solution sol;

  auto check_both = [&](std::string_view a, std::string_view b,
                        std::string_view expected) {
    ctx.expect_eq(sol.addStrings(a, b), expected);
    ctx.expect_eq(sol.addStringsRecursive(a, b), expected);
  };

  ctx.test("basic_examples", [&] {
    check_both("100", "99", "199");
    check_both("123", "123", "246");
    check_both("12", "12", "24");
    check_both("11", "123", "134");
    check_both("456", "77", "533");
  });

  ctx.test("zeros_and_carry_overflow", [&] {
    check_both("0", "0", "0");
    check_both("0", "9876", "9876");
    check_both("999", "1", "1000");
    check_both("9999", "9999", "19998");
  });

  ctx.test("exceeds_64_bit_integer", [&] {
    check_both("999999999999999999999999999999", "1",
               "1000000000000000000000000000000");
    check_both("123456789012345678901234567890",
               "987654321098765432109876543210",
               "1111111110111111111011111111100");
  });
}

}  // namespace
