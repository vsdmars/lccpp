// LeetCode 400: Nth Digit
// https://leetcode.com/problems/nth-digit/
//
// Problem:
//   Given a positive 32-bit signed integer `n` (1 <= n <= 2^31 - 1), return the
//   n-th digit (1-indexed) of the infinite integer sequence formed by
//   concatenating all positive integers in order: "12345678910111213...".
//
// Algorithm:
//   Partition the positive integers by their decimal digit length d = 1, 2, ...:
//     - d = 1: 9 numbers (1..9), contributing 9 * 1 = 9 digits.
//     - d = 2: 90 numbers (10..99), contributing 90 * 2 = 180 digits.
//     - In general, digit-length block d starts at `start = 10^(d - 1)`,
//       contains `count = 9 * 10^(d - 1)` numbers, and spans `count * d` digits.
//   Subtract whole blocks while `remaining > count * d` using 64-bit arithmetic
//   (necessary because `9 * 10^8 * 9 = 8.1 * 10^9` overflows 32-bit int).
//   Once inside the target block, convert `remaining` to a 0-based offset:
//     - `target = start + (remaining - 1) / d` is the integer containing the
//       answer digit.
//     - `(remaining - 1) % d` is the 0-based digit index inside `target`.
//   Time Complexity: O(log10 n) — at most 9 block iterations for 32-bit `n`,
//     plus O(log10 n) string conversion of the target number.
//   Space Complexity: O(log10 n) for the decimal string of the target integer.
//
// Data Structures:
//   64-bit integer counters for block bounds (`digits`, `count`, `start`) and a
//   short `std::string` holding the decimal representation of the target number.
//
// Real-World Applications:
//   Random-access indexing into variable-width serialized records or
//   prefix-grouped log blocks without materializing the full stream, and
//   unranking elements in positional numeral / combinatorial coding systems.
//
// Related LeetCode Problems:
//   - #233 Number of Digit One [new] (counting digits across positional decimal blocks)
//   - #172 Factorial Trailing Zeroes [new] (base-10 positional counting math)
//   - #273 Integer to English Words [done] (decomposing integers by decimal place-value groups)
//   - #60 Permutation Sequence [new] (unranking the k-th element directly by block sizes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

namespace {

class Solution {
 public:
  // Finds the 1-indexed n-th digit in the infinite sequence "1234567891011...".
  // Time: O(log10 n), Space: O(log10 n).
  int findNthDigit(int n) {
    // Use 64-bit integers because `count * digits` exceeds INT32_MAX at d = 9.
    auto remaining = static_cast<std::int64_t>(n);
    std::int64_t digits = 1;
    std::int64_t count = 9;
    std::int64_t start = 1;

    // Phase 1: skip complete digit-length blocks until `remaining` falls inside
    // the block of `digits`-digit integers starting at `start`.
    while (remaining > (count * digits)) {
      remaining -= (count * digits);
      ++digits;
      count *= 10;
      start *= 10;
    }

    // Phase 2: map the 1-indexed position within the block to a 0-based index
    // to locate both the exact integer and the digit offset inside it.
    const std::int64_t zero_based = remaining - 1;
    const std::int64_t target_number = start + (zero_based / digits);
    const auto digit_index = static_cast<std::size_t>(zero_based % digits);

    const std::string number_str = std::to_string(target_number);
    return number_str[digit_index] - '0';
  }
};

LC_PROBLEM(400, "Nth Digit", "math", ctx) {
  ctx.test("example_1_single_digit", [&] {
    Solution sol;
    ctx.expect_eq(sol.findNthDigit(3), 3);
  });

  ctx.test("example_2_inside_10", [&] {
    Solution sol;
    ctx.expect_eq(sol.findNthDigit(11), 0);
  });

  ctx.test("build_22nd_digit", [&] {
    Solution sol;
    // 1..9 (9), 10..15 (12 more -> 21), 22nd digit is '1' from 16.
    ctx.expect_eq(sol.findNthDigit(22), 1);
  });

  ctx.test("block_boundaries", [&] {
    Solution sol;
    ctx.expect_eq(sol.findNthDigit(9), 9);    // last 1-digit number: 9
    ctx.expect_eq(sol.findNthDigit(10), 1);   // first digit of 10
    ctx.expect_eq(sol.findNthDigit(189), 9);  // last digit of 99
    ctx.expect_eq(sol.findNthDigit(190), 1);  // first digit of 100
    ctx.expect_eq(sol.findNthDigit(191), 0);  // second digit of 100
  });

  ctx.test("max_int32_no_overflow", [&] {
    Solution sol;
    // At INT32_MAX, remaining inside 9-digit block is 1,358,594,758 ->
    // 0-based index 1,358,594,757 = 150,954,973 * 9 + 0 -> first digit of
    // 250,954,973, which is 2.
    ctx.expect_eq(sol.findNthDigit(std::numeric_limits<int>::max()), 2);
  });
}

}  // namespace
