// LeetCode 397: Integer Replacement
// https://leetcode.com/problems/integer-replacement/
//
// Problem:
//   Given a positive integer `n` (1 <= n <= 2^31 - 1), repeatedly apply one of:
//     - If `n` is even, replace `n` with `n / 2`.
//     - If `n` is odd, replace `n` with either `n + 1` or `n - 1`.
//   Return the minimum number of operations required for `n` to become `1`.
//
// Examples:
//   Example 1:
//     Input:  n = 8
//     Output: 3
//     Explanation:
//       - Initialize `val = 8` (`1000_2`) and `steps = 0`.
//       - Each iteration sees `(val & 1) == 0` and halves `val`: `8 -> 4` (`steps =
//         1`), `4 -> 2` (`steps = 2`), and `2 -> 1` (`steps = 3`).
//       - The loop exits when `val == 1` and returns `3`.
//
//   Example 2:
//     Input:  n = 7
//     Output: 4
//     Explanation:
//       - Initialize `val = 7` (`111_2`) and `steps = 0`; since `val != 3` and `(7
//         & 3) == 3`, increment to `val = 8` (`steps = 1`).
//       - Now `val = 8` is even, so three consecutive halvings (`8 -> 4 -> 2 -> 1`)
//         advance `steps` from `1` to `4`.
//       - The loop exits when `val == 1` and returns `4`.
//
//   Example 3:
//     Input:  n = 2147483647
//     Output: 32
//     Explanation:
//       - Widen `n = 2147483647` (`2^31 - 1`) to 64-bit `val = 2147483647ULL` with
//         `steps = 0`.
//       - Because `val != 3` and `(val & 3) == 3`, increment `val` to `2147483648`
//         (`2^31`, `steps = 1`) without signed 32-bit overflow.
//       - Repeatedly halve the even power of two (`2^31 -> 2^30 -> ... -> 1`)
//         across 31 shifts, reaching `val == 1` with `steps = 32`.
//
// Algorithm:
//   Greedy bit-level reduction using `std::uint64_t` (preventing signed 32-bit
//   overflow when `n == INT_MAX` and we increment `n + 1`).
//   - Whenever `val` is even (`(val & 1ULL) == 0ULL`), halving (`val >>= 1U`)
//     is the only legal move and immediately removes one bit of length.
//   - Whenever `val` is odd, we want the next number to contain as many
//     trailing zero bits as possible (at least two, i.e., divisible by 4) so
//     subsequent halvings shrink `val` maximally:
//       * If `val == 3ULL` (`11_2`), decrementing gives `3 -> 2 -> 1` (2 ops),
//         strictly beating `3 -> 4 -> 2 -> 1` (3 ops).
//       * If `(val & 3ULL) == 1ULL` (binary ends in `01`), decrementing yields
//         `...00` (divisible by 4) without creating higher carry bits.
//       * Otherwise `(val & 3ULL) == 3ULL` with `val > 3` (binary ends in
//         `11`), so incrementing clears the entire trailing run of 1-bits via
//         carry propagation, strictly reducing or matching the remaining bits.
//   Time Complexity:  O(log n) — each odd step is followed by at least two
//                     halvings (except the terminal `3 -> 2 -> 1`), so total
//                     iterations are at most ~1.5 * bit_length(n).
//   Space Complexity: O(1) — scalar loop variables only.
//
// Data Structures:
//   Scalar 64-bit unsigned integer (`std::uint64_t`) holding the current value
//   safely above `INT_MAX`, plus an integer step counter.
//
// Real-World Applications:
//   Non-Adjacent Form (NAF) signed-digit recoding in elliptic-curve and RSA
//   scalar multiplication (minimizing point additions by turning runs of 1-bits
//   into `+1` / `-1` operations), and strength reduction in compiler constant
//   multiplication.
//
// Related LeetCode Problems:
//   - #191 Number of 1 Bits [done] (bit-level popcount and trailing-bit analysis)
//   - #231 Power of Two [done] (halving and power-of-two bit properties)
//   - #1342 Number of Steps to Reduce a Number to Zero [new] (even/odd step count)
//   - #1404 Number of Steps to Reduce a Number in Binary Representation to One [done] (binary +1 and >>1 reduction)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>

namespace {

class Solution {
 public:
  // Computes the minimum number of `/2`, `+1`, or `-1` operations to reduce
  // positive integer `n` to `1`.
  // Invariant: at every odd value `val > 3`, choosing the direction that makes
  // `val` a multiple of 4 minimizes the remaining bit-reduction cost.
  // Time: O(log n), Space: O(1).
  int integerReplacement(int n) {
    // Widen to uint64_t so incrementing `n == INT_MAX` (2^31 - 1 -> 2^31) never
    // overflows signed 32-bit int.
    auto val = static_cast<std::uint64_t>(n);
    int steps = 0;

    while (val > 1ULL) {
      if ((val & 1ULL) == 0ULL) {
        // Even: divide by 2 via right shift.
        val >>= 1U;
      } else if (val == 3ULL || (val & 3ULL) == 1ULL) {
        // Special case 3 (3 -> 2 -> 1 beats 3 -> 4 -> 2 -> 1), or binary ends
        // in `01` where subtracting 1 produces at least two trailing zeros.
        --val;
      } else {
        // Binary ends in `11` (and val > 3): adding 1 collapses the trailing
        // run of 1-bits into zeros via carry propagation.
        ++val;
      }
      ++steps;
    }

    return steps;
  }
};

LC_PROBLEM(397, "Integer Replacement", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_power_of_two",
           [&] { ctx.expect_eq(sol.integerReplacement(8), 3); });

  ctx.test("example_2_odd_seven",
           [&] { ctx.expect_eq(sol.integerReplacement(7), 4); });

  ctx.test("small_base_cases", [&] {
    ctx.expect_eq(sol.integerReplacement(1), 0);
    ctx.expect_eq(sol.integerReplacement(2), 1);
    ctx.expect_eq(sol.integerReplacement(3), 2);
    ctx.expect_eq(sol.integerReplacement(4), 2);
    ctx.expect_eq(sol.integerReplacement(5), 3);
    ctx.expect_eq(sol.integerReplacement(6), 3);
  });

  ctx.test("all_ones_and_int_max_no_overflow", [&] {
    // 65535 = 2^16 - 1 -> +1 to 2^16, then 16 halvings = 17 steps.
    ctx.expect_eq(sol.integerReplacement(65535), 17);
    // INT_MAX = 2^31 - 1 -> +1 to 2^31, then 31 halvings = 32 steps.
    ctx.expect_eq(sol.integerReplacement(std::numeric_limits<int>::max()), 32);
  });
}

}  // namespace
