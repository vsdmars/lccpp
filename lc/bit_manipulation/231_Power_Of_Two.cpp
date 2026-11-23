// LeetCode 231: Power Of Two
// https://leetcode.com/problems/power-of-two/
//
// Problem:
//   Given a signed 32-bit integer `n`, return `true` if `n` is a power of two
//   (that is, there exists an integer `x >= 0` such that `n == 2^x`); otherwise
//   return `false`. Negative numbers and zero are never powers of two.
//
// Examples:
//   Example 1:
//     Input:  n = 16
//     Output: true
//     Explanation:
//       - Check the guard n <= 0; since 16 > 0, execution continues.
//       - Compute n - 1 = 15 (0b01111), which clears bit 4 of 16 (0b10000) and sets
//         bits 0 through 3 to 1.
//       - Evaluate (16 & 15) == 0 (0b10000 & 0b01111 == 0), confirming 16 has
//         exactly one set bit and returning true.
//
//   Example 2:
//     Input:  n = 18
//     Output: false
//     Explanation:
//       - Check the guard n <= 0; since 18 > 0, execution continues.
//       - Compute n - 1 = 17 (0b10001), which clears only the lowest set bit (bit
//         1) of 18 (0b10010) while leaving bit 4 intact.
//       - Evaluate 18 & 17 = 16 (0b10000), which is nonzero, so isPowerOfTwo
//         returns false.
//
//   Example 3:
//     Input:  n = -2147483648 (INT_MIN)
//     Output: false
//     Explanation:
//       - Although INT_MIN has a single 1-bit in two's complement (0x80000000), it
//         is negative and not a power of two.
//       - The entry guard if (n <= 0) immediately returns false, preventing signed
//         underflow on n - 1.
//
// Algorithm:
//   Single-Bit Clearing Check:
//   A positive integer `n` is a power of two if and only if its binary
//   representation contains exactly one `'1'` bit. Clearing the lowest set bit
//   via `n & (n - 1)` produces 0 if and only if no other `'1'` bits exist.
//   Crucially, we must first guard `n <= 0`:
//   - `0` has zero set bits (not a power of two).
//   - Negative integers are never powers of two, and evaluating `INT_MIN - 1`
//     on a signed 32-bit `int` would invoke undefined behavior (fixing the
//     missing negative check in the original Python snippet).
//   Time: O(1), Space: O(1).
//
// Data Structures:
//   - None (pure constant-time scalar bitwise arithmetic).
//
// Real-World Applications:
//   - Memory allocators (buddy allocators, slab/page alignment) and hardware
//     DMA/cache-line alignment validation (`size` must be a power of two so
//     modulo wraps via fast bitmask `idx & (cap - 1)`).
//   - Ring buffers, lock-free queues, and hash tables whose capacities are
//     restricted to powers of two for branchless index masking.
//   - Radix-2 FFT input-size validation and GPU texture mipmap dimension checks.
//
// Related LeetCode Problems:
//   - #191 Number Of 1 Bits [done] (generalizes `n & (n - 1)` into a full set-bit counter)
//   - #338 Counting Bits [done] (uses binary bit structure to count 1-bits across 0..n)
//   - #342 Power of Four [new] (adds an even-bit-position mask `0x55555555` to the power-of-two test)
//   - #326 Power of Three [new] (checks powers of a non-binary base without bit tricks)

#include "framework/lc_framework.h"

#include <limits>

namespace {

class Solution {
 public:
  // Returns true iff `n == 2^k` for some integer k >= 0.
  // Invariant: once `n > 0` is verified, `n - 1` cannot underflow signed int,
  // and `(n & (n - 1)) == 0` holds iff `n` has a single set bit.
  // Time: O(1), Space: O(1).
  bool isPowerOfTwo(int n) {
    // Reject zero and all negative values (also prevents signed overflow on
    // `INT_MIN - 1`).
    if (n <= 0) {
      return false;
    }
    // Clearing the sole set bit of a power of two yields exactly 0.
    return (n & (n - 1)) == 0;
  }
};

LC_PROBLEM(231, "Power Of Two", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("powers_of_two", [&] {
    ctx.expect_true(sol.isPowerOfTwo(1));
    ctx.expect_true(sol.isPowerOfTwo(2));
    ctx.expect_true(sol.isPowerOfTwo(16));
    ctx.expect_true(sol.isPowerOfTwo(1024));
    // Largest power of two representable in signed 32-bit int (2^30).
    ctx.expect_true(sol.isPowerOfTwo(1 << 30));
  });

  ctx.test("non_powers_of_two", [&] {
    ctx.expect_false(sol.isPowerOfTwo(3));
    ctx.expect_false(sol.isPowerOfTwo(5));
    ctx.expect_false(sol.isPowerOfTwo(6));
    ctx.expect_false(sol.isPowerOfTwo(18));
    ctx.expect_false(sol.isPowerOfTwo(std::numeric_limits<int>::max()));
  });

  ctx.test("zero_and_negatives", [&] {
    ctx.expect_false(sol.isPowerOfTwo(0));
    ctx.expect_false(sol.isPowerOfTwo(-1));
    ctx.expect_false(sol.isPowerOfTwo(-2));
    ctx.expect_false(sol.isPowerOfTwo(-16));
    // INT_MIN has a single 1-bit in two's complement (0x80000000), but is
    // negative and must safely return false without signed overflow.
    ctx.expect_false(sol.isPowerOfTwo(std::numeric_limits<int>::min()));
  });
}

}  // namespace
