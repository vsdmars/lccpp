// LeetCode 371: Sum of Two Integers
// https://leetcode.com/problems/sum-of-two-integers/
//
// Problem:
//   Given two 32-bit signed integers `a` and `b`, return their sum without
//   using the arithmetic addition (`+`) or subtraction (`-`) operators.
//   Constraints: -1000 <= a, b <= 1000 (the algorithm generalizes to any
//   32-bit two's-complement sum that does not overflow signed 32-bit int).
//
// Examples:
//   Example 1:
//     Input:  a = 1, b = 2
//     Output: 3
//     Explanation:
//       - Convert operands to 32-bit unsigned integers `ua = 1` (`0001_2`) and `ub
//         = 2` (`0010_2`).
//       - Iteration 1 (`ub != 0`): compute `carry = (1 & 2) << 1 = 0`, update `ua =
//         1 ^ 2 = 3` (`0011_2`), and set `ub = 0`.
//       - The loop exits with `ub == 0` and casts `ua = 3` back to `int`, returning
//         `3`.
//
//   Example 2:
//     Input:  a = -2, b = 3
//     Output: 1
//     Explanation:
//       - Reinterpret both signed inputs as 32-bit unsigned values: `ua =
//         0xFFFFFFFE` (`-2`) and `ub = 0x00000003` (`3`).
//       - Iteration 1 computes `carry = (0xFFFFFFFE & 0x00000003) << 1 = 4`,
//         updates `ua = 0xFFFFFFFE ^ 0x00000003 = 0xFFFFFFFD` (`-3`), and sets `ub
//         = 4`.
//       - Iteration 2 computes `carry = (0xFFFFFFFD & 4) << 1 = 8`, updates `ua =
//         0xFFFFFFF9` (`-7`), and sets `ub = 8`.
//       - Subsequent iterations shift the carry bit left (`8 -> 16 -> ... ->
//         0x80000000 -> 0` modulo 2^32) while `ua` updates to `1` (`0x00000001`),
//         terminating when `ub == 0` and returning `1`.
//
//   Example 3:
//     Input:  a = 1023, b = 1
//     Output: 1024
//     Explanation:
//       - Initialize `ua = 1023` (`0011_1111_1111_2`) and `ub = 1` (`0001_2`).
//       - Iteration 1 computes `carry = (1023 & 1) << 1 = 2`, `ua = 1023 ^ 1 =
//         1022`, and `ub = 2`.
//       - Iterations 2 through 10 ripple the single carry bit across the remaining
//         nine 1-bits (`ub` becomes `4, 8, ..., 1024` as `ua` clears down to `0`).
//       - Iteration 11 sees `ua = 0` and `ub = 1024`, yielding `carry = 0`, `ua = 0
//         ^ 1024 = 1024`, and `ub = 0`, so the loop stops and returns `1024`.
//
// Algorithm:
//   Simulate binary ripple-carry addition using bitwise operations on unsigned
//   32-bit integers (`std::uint32_t`). At each step:
//     - `ua ^ ub` computes the sum of every bit column without carry.
//     - `(ua & ub) << 1U` computes the carry bits generated where both operands
//       had a `1`, shifted one position left to their target bit column.
//   Repeating `ua = ua ^ ub; ub = carry;` propagates carries across bit
//   positions until `ub == 0U`. Operating in `std::uint32_t` gives well-defined
//   modulo-2^32 two's-complement arithmetic for negative inputs without signed
//   left-shift UB, and casting back to `int` recovers the signed 32-bit result.
//   Time Complexity:  O(1) — at most 32 iterations (each shift moves the lowest
//                     set carry bit left by at least one position).
//   Space Complexity: O(1) — constant number of scalar bitmasks.
//
// Data Structures:
//   Scalar 32-bit unsigned integers (`std::uint32_t`) representing the running
//   carry-less partial sum and pending carry mask.
//
// Real-World Applications:
//   Hardware ALU half-adder / carry-save adder circuits in CPUs and FPGAs,
//   constant-time cryptographic arithmetic (avoiding carry-dependent branches),
//   and SIMD / SWAR (SIMD Within A Register) parallel bit-field addition.
//
// Related LeetCode Problems:
//   - #2 Add Two Numbers [done] (digit-by-digit ripple-carry addition on lists)
//   - #67 Add Binary [done] (binary string addition with explicit carry bit)
//   - #415 Add Strings [done] (base-10 manual addition with carry propagation)
//   - #989 Add to Array-Form of Integer [done] (carry propagation over digits)
//   - #191 Number of 1 Bits [done] (foundational 32-bit bit manipulation)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>

namespace {

class Solution {
 public:
  // Computes `a + b` using only bitwise XOR, AND, and left shift.
  // Invariant: after each iteration, `(ua + ub) mod 2^32 == (a + b) mod 2^32`,
  // and the trailing-zero count of `ub` strictly increases until `ub == 0`.
  // Time: O(1) (<= 32 iterations), Space: O(1).
  int getSum(int a, int b) {
    // Reinterpret both signed operands in unsigned 32-bit two's complement so
    // left-shifting a high carry bit wraps modulo 2^32 cleanly without UB.
    auto ua = static_cast<std::uint32_t>(a);
    auto ub = static_cast<std::uint32_t>(b);

    while (ub != 0U) {
      // Columns where both bits are 1 generate a carry into the next column.
      const std::uint32_t carry = (ua & ub) << 1U;
      // XOR adds each bit column modulo 2 (sum without carries).
      ua ^= ub;
      ub = carry;
    }

    // In C++20/23, converting `std::uint32_t` to 32-bit `int` preserves the
    // exact two's-complement bit pattern modulo 2^32.
    return static_cast<int>(ua);
  }
};

LC_PROBLEM(371, "Sum Of Two Integers", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_positive", [&] { ctx.expect_eq(sol.getSum(1, 2), 3); });

  ctx.test("example_2_mixed_signs",
           [&] { ctx.expect_eq(sol.getSum(-2, 3), 1); });

  ctx.test("both_negative", [&] {
    ctx.expect_eq(sol.getSum(-5, -7), -12);
    ctx.expect_eq(sol.getSum(-1000, -1000), -2000);
  });

  ctx.test("cancel_to_zero_and_identity", [&] {
    ctx.expect_eq(sol.getSum(0, 0), 0);
    ctx.expect_eq(sol.getSum(0, -15), -15);
    ctx.expect_eq(sol.getSum(15, 0), 15);
    ctx.expect_eq(sol.getSum(-42, 42), 0);
    ctx.expect_eq(sol.getSum(-1, 1), 0);
  });

  ctx.test("long_ripple_carry_and_bounds", [&] {
    ctx.expect_eq(sol.getSum(1023, 1), 1024);
    ctx.expect_eq(sol.getSum(1000, 1000), 2000);
    ctx.expect_eq(sol.getSum(std::numeric_limits<int>::max(), 0),
                  std::numeric_limits<int>::max());
    ctx.expect_eq(sol.getSum(std::numeric_limits<int>::min(), 1),
                  std::numeric_limits<int>::min() + 1);
  });
}

}  // namespace
