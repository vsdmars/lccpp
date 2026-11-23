// LeetCode 405: Convert a Number to Hexadecimal
// https://leetcode.com/problems/convert-a-number-to-hexadecimal/
//
// Problem:
//   Given a 32-bit signed integer `num`, return a string representing its
//   lowercase hexadecimal encoding (`0-9`, `a-f`) without leading zeros (with
//   `0` represented as `"0"`). Negative numbers must be encoded using their
//   32-bit two's-complement representation, and built-in hex formatting library
//   functions must not be used.
//   Constraints: -2^31 <= num <= 2^31 - 1.
//
// Examples:
//   Example 1:
//     Input:  num = 26
//     Output: "1a"
//     Explanation:
//       - Since num != 0, convert 26 to std::uint32_t giving value = 26U (0x1a).
//       - Iteration 1 extracts nibble = 26U & 0xFU = 10, appends kHexDigits[10] =
//         'a' to result ("a"), and shifts value >>= 4U to 1U.
//       - Iteration 2 extracts nibble = 1U & 0xFU = 1, appends kHexDigits[1] = '1'
//         to result ("a1"), and shifts value >>= 4U to 0U.
//       - The loop ends when value == 0U, and std::ranges::reverse(result) reverses
//         "a1" to return "1a".
//
//   Example 2:
//     Input:  num = -1
//     Output: "ffffffff"
//     Explanation:
//       - Since num != 0, casting -1 to std::uint32_t yields its 32-bit
//         two's-complement bit pattern value = 0xFFFFFFFFU.
//       - Each of the 8 loop iterations masks nibble = value & 0xFU = 15, appends
//         kHexDigits[15] = 'f', and performs a logical shift value >>= 4U.
//       - After 8 shifts value reaches 0U with result = "ffffffff", which reverses
//         to "ffffffff".
//
//   Example 3:
//     Input:  num = 0
//     Output: "0"
//     Explanation:
//       - The initial guard if (num == 0) matches before any unsigned conversion or
//         bit-shifting loop executes.
//       - It returns "0" directly so zero is not emitted as an empty string.
//
// Algorithm:
//   Handle `num == 0` directly by returning `"0"`. Otherwise, convert `num` to
//   `std::uint32_t`, which preserves the exact 32-bit two's-complement bit
//   pattern for negative values and guarantees logical (zero-filling) right
//   shifts. Repeatedly extract the lowest 4-bit nibble (`value & 0xFU`), map it
//   via a 16-character lookup table `"0123456789abcdef"`, and shift right by 4
//   bits (`value >>= 4U`) until `value == 0U`. Finally reverse the collected
//   digits so the most significant nibble appears first.
//   Time Complexity:  O(1) — at most 8 loop iterations (32 bits / 4 bits).
//   Space Complexity: O(1) — output string of at most 8 characters.
//
// Data Structures:
//   `std::string_view` lookup table mapping 4-bit nibble values `0..15` to
//   hex characters, and a `std::string` output buffer reserved to 8 bytes.
//
// Real-World Applications:
//   Memory-address / register dump formatting in debuggers and kernel loggers,
//   UUID and cryptographic digest (SHA-256 / MD5) hex encoding, and low-level
//   protocol/wire-format serializers.
//
// Related LeetCode Problems:
//   - #168 Excel Sheet Column Title [done] (base-26 digit extraction and reversal)
//   - #371 Sum of Two Integers [done] (32-bit two's-complement unsigned bit ops)
//   - #504 Base 7 [new] (positional radix conversion without library helpers)
//   - #190 Reverse Bits [done] (bit-field extraction from 32-bit unsigned words)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Converts 32-bit signed `num` to lowercase hexadecimal using two's
  // complement for negative values.
  // Invariant: each iteration appends the next least significant hex nibble
  // of `value` and shifts it out logically until all remaining high bits are 0.
  // Time: O(1) (<= 8 nibbles), Space: O(1).
  std::string toHex(int num) {
    if (num == 0) {
      return "0";
    }

    static constexpr std::string_view kHexDigits = "0123456789abcdef";
    static constexpr std::size_t kMaxHexDigits = 8uz;

    // Unsigned 32-bit conversion yields the exact 32-bit two's-complement bits
    // for negative numbers and ensures `>>= 4U` fills with zeros on the left.
    auto value = static_cast<std::uint32_t>(num);
    std::string result;
    result.reserve(kMaxHexDigits);

    while (value != 0U) {
      const auto nibble = static_cast<std::size_t>(value & 0xFU);
      result.push_back(kHexDigits[nibble]);
      value >>= 4U;
    }

    // Digits were collected least-significant first; reverse to canonical order.
    std::ranges::reverse(result);
    return result;
  }
};

LC_PROBLEM(405, "Convert A Number To Hexadecimal", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_positive",
           [&] { ctx.expect_eq(sol.toHex(26), std::string("1a")); });

  ctx.test("example_2_negative_one",
           [&] { ctx.expect_eq(sol.toHex(-1), std::string("ffffffff")); });

  ctx.test("zero_and_small_values", [&] {
    ctx.expect_eq(sol.toHex(0), std::string("0"));
    ctx.expect_eq(sol.toHex(16), std::string("10"));
    ctx.expect_eq(sol.toHex(48), std::string("30"));
    ctx.expect_eq(sol.toHex(-2), std::string("fffffffe"));
  });

  ctx.test("larger_and_boundary_values", [&] {
    ctx.expect_eq(sol.toHex(111111), std::string("1b207"));
    ctx.expect_eq(sol.toHex(262626), std::string("401e2"));
    ctx.expect_eq(sol.toHex(1000000000), std::string("3b9aca00"));
    ctx.expect_eq(sol.toHex(std::numeric_limits<int>::max()),
                  std::string("7fffffff"));
    ctx.expect_eq(sol.toHex(std::numeric_limits<int>::min()),
                  std::string("80000000"));
  });
}

}  // namespace
