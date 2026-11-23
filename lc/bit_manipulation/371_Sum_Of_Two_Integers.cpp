// LeetCode 371: Sum of Two Integers
// https://leetcode.com/problems/sum-of-two-integers/
//
// Problem:
//   Given two 32-bit signed integers `a` and `b`, return their sum without
//   using the arithmetic addition (`+`) or subtraction (`-`) operators.
//   Constraints: -1000 <= a, b <= 1000 (the algorithm generalizes to any
//   32-bit two's-complement sum that does not overflow signed 32-bit int).
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
