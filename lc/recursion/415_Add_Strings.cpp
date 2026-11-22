// https://leetcode.com/problems/add-strings/
//
// Given two non-negative integers `num1` and `num2` represented as strings,
// return their sum as a string without using any built-in arbitrary-precision
// integer library or converting the inputs directly to integers.

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
