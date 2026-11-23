// LeetCode 1088: Confusing Number II
// https://leetcode.com/problems/confusing-number-ii/
//
// Problem:
//   A confusing number is a positive integer that, when rotated 180 degrees,
//   remains a valid number (every digit is in `{0, 1, 6, 8, 9}`, mapping to
//   `{0, 1, 9, 8, 6}` respectively; digits `2, 3, 4, 5, 7` become invalid) AND
//   differs from the original number (leading zeros in the rotated result are
//   dropped, e.g., `10` rotates to `01` = `1`). Given a positive integer `n`
//   (1 <= n <= 10^9), return the count of confusing numbers in `[1, n]`.
//
// Examples:
//   Example 1:
//     Input:  n = 20
//     Output: 6
//     Explanation:
//       - Seed the DFS with non-zero rotatable digits `<= 20`: `(1, 1, 10)`, `(6,
//         9, 10)`, `(8, 8, 10)`, and `(9, 6, 10)`.
//       - From `(1, 1, 10)`, `1 == 1` does not increment `count`, and extending by
//         `{0, 1, 6, 8, 9}` produces `10` (rotated `1`), `11` (rotated `11`), `16`
//         (rotated `91`), `18` (rotated `81`), and `19` (rotated `61`).
//       - Among those two-digit numbers, `10`, `16`, `18`, and `19` satisfy `num !=
//         rotated` (adding `4` to `count`), and all their extensions (`>= 100`)
//         exceed `limit = 20` and break.
//       - Visiting seeds `(6, 9, 10)` and `(9, 6, 10)` increments `count` by `2`
//         (`6 != 9` and `9 != 6`) while `(8, 8, 10)` adds `0`, and their children
//         (`>= 60`) exceed `20` and break.
//       - Return `count = 6` for `{6, 9, 10, 16, 18, 19}`.
//
//   Example 2:
//     Input:  n = 5
//     Output: 0
//     Explanation:
//       - Check the early-exit guard `n < 6` (`5 < 6`).
//       - Return `0` immediately because digits `2..5` are non-rotatable and `1`
//         rotates to itself.
//
//   Example 3:
//     Input:  n = 10
//     Output: 3
//     Explanation:
//       - Seed the DFS with `(1, 1, 10)`, `(6, 9, 10)`, `(8, 8, 10)`, and `(9, 6,
//         10)` since all four are `<= 10`.
//       - From `(1, 1, 10)`, appending `digit = 0` visits `(10, 1, 100)` (`10 !=
//         1`, `count = 1`), and `digit = 1` yields `11 > 10` so the loop breaks.
//       - Visiting `(6, 9, 10)` raises `count` to `2`, `(8, 8, 10)` leaves `count =
//         2`, and `(9, 6, 10)` raises `count` to `3`, while all extensions `>= 60`
//         exceed `10`.
//       - Return `count = 3` for `{6, 9, 10}`.
//
// Algorithm:
//   Constructive DFS over the 5-ary tree of valid rotatable numbers instead of
//   scanning all `1..n` integers.
//   - Every confusing number consists exclusively of digits `{0, 1, 6, 8, 9}`
//     and cannot start with `0`.
//   - Starting from each non-zero rotatable seed `{1, 6, 8, 9}` that is <= `n`,
//     we append rotatable digits one at a time while maintaining three values
//     in O(1) per transition:
//       * `num`:     the constructed number (`(num * 10) + digit`),
//       * `rotated`: its exact 180-degree rotation (`(rot_digit * place) + rotated`),
//       * `place`:   the next power of 10 (`10^(digits_in_num)`).
//   - Whenever `num != rotated`, increment `count`. Because digits are iterated
//     in ascending order `{0, 1, 6, 8, 9}`, we `break` as soon as
//     `next_num > n`.
//   - All arithmetic uses `std::int64_t` because `next_num` and `place` can
//     temporarily exceed 32-bit `int` when `n` is close to `10^9`.
//
//   Time Complexity:  O(5^(log10 n)) — at most ~2 * 10^6 nodes for n = 10^9
//                     (compared to O(n) = 10^9 for brute force).
//   Space Complexity: O(log10 n) for the DFS recursion stack (depth <= 10).
//
// Data Structures:
//   - Fixed compile-time table `std::array<std::pair<std::int64_t, std::int64_t>, 5>`
//     (`kDigitRotations`) mapping each rotatable digit to its 180-degree image
//     in ascending order.
//   - Call stack frames storing `(num, rotated, place)` in O(1) per depth.
//
// Real-World Applications:
//   - Ambiguity detection on 7-segment displays, serial labels, shipping tags,
//     and physical asset IDs that can be read upside down.
//   - Restricted-alphabet numeric ID generation (e.g., excluding visually
//     ambiguous or orientation-sensitive glyphs in manufacturing/logistics).
//   - Pruned digit-tree search for counting bounded numbers over constrained
//     digit alphabets in compiler/hardware verification.
//
// Related LeetCode Problems:
//   - #1056 Confusing Number [new] (testing a single integer for confusing property)
//   - #246 Strobogrammatic Number [done] (checking if a number equals its 180-degree rotation)
//   - #247 Strobogrammatic Number II [done] (generating all length-n strobogrammatic numbers)
//   - #248 Strobogrammatic Number III [new] (counting strobogrammatic numbers in a range)
//   - #788 Rotated Digits [done] (similar digit-rotation counting problem without reversal)

#include "framework/lc_framework.h"

#include <array>
#include <cstdint>
#include <utility>

namespace {

// Sorted table of `(digit, rotated_digit)` pairs for 180-degree rotation.
// Ascending digit order enables early termination once `(num * 10) + digit > n`.
constexpr std::array<std::pair<std::int64_t, std::int64_t>, 5>
    kDigitRotations = {{
        {0, 0},
        {1, 1},
        {6, 9},
        {8, 8},
        {9, 6},
    }};

class Solution {
 public:
  // Counts all confusing numbers in the inclusive range `[1, n]`.
  //
  // Invariant: Every DFS call visits a unique positive integer `num <= n` made
  // solely of digits in `{0, 1, 6, 8, 9}`, paired with its exact 180-degree
  // rotation `rotated` and `place == 10^(digit_count(num))`.
  // Time: O(5^(log10 n)), Space: O(log10 n).
  int confusingNumberII(int n) {
    if (n < 6) {
      // The smallest confusing number is 6 (1 rotates to 1; 2..5 are invalid).
      return 0;
    }

    const auto limit = static_cast<std::int64_t>(n);
    int count = 0;

    auto dfs = [&](this auto&& self, std::int64_t num, std::int64_t rotated,
                   std::int64_t place) -> void {
      if (num != rotated) {
        ++count;
      }

      for (const auto& [digit, rot_digit] : kDigitRotations) {
        const std::int64_t next_num = (num * 10) + digit;
        if (next_num > limit) {
          // Since `kDigitRotations` is sorted by `digit`, all subsequent
          // candidates at this node will also exceed `limit`.
          break;
        }
        // Appending `digit` to the least-significant end of `num` prepends
        // `rot_digit` to the most-significant end of `rotated`.
        const std::int64_t next_rotated = (rot_digit * place) + rotated;
        self(next_num, next_rotated, place * 10);
      }
    };

    // Seed DFS with every non-zero rotatable digit `<= limit` to prevent
    // generating numbers with leading zeros (fixes the small-n boundary).
    for (const auto& [digit, rot_digit] : kDigitRotations) {
      if (digit != 0 && digit <= limit) {
        dfs(digit, rot_digit, 10);
      }
    }

    return count;
  }
};

LC_PROBLEM(1088, "Confusing Number II", "backtracking", ctx) {
  ctx.test("example_1_n_20", [&] {
    Solution sol;
    // Confusing numbers in [1, 20]: 6, 9, 10, 16, 18, 19.
    ctx.expect_eq(sol.confusingNumberII(20), 6);
  });

  ctx.test("example_2_n_100", [&] {
    Solution sol;
    ctx.expect_eq(sol.confusingNumberII(100), 19);
  });

  ctx.test("small_boundary_values", [&] {
    Solution sol;
    ctx.expect_eq(sol.confusingNumberII(1), 0);
    ctx.expect_eq(sol.confusingNumberII(5), 0);
    ctx.expect_eq(sol.confusingNumberII(6), 1);
    ctx.expect_eq(sol.confusingNumberII(8), 1);
    ctx.expect_eq(sol.confusingNumberII(9), 2);
    ctx.expect_eq(sol.confusingNumberII(10), 3);
  });

  ctx.test("larger_powers_of_ten", [&] {
    Solution sol;
    // Python build() includes 10000: 2 + 16 + 88 + 480 + 1 = 587.
    ctx.expect_eq(sol.confusingNumberII(10000), 587);
    // 1..10^6: 2 + 16 + 88 + 480 + 2440 + 12400 + 1 = 15427.
    ctx.expect_eq(sol.confusingNumberII(1000000), 15427);
  });
}

}  // namespace
