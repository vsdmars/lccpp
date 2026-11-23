// LeetCode 357: Count Numbers With Unique Digits
// https://leetcode.com/problems/count-numbers-with-unique-digits/
//
// Problem:
//   Given a non-negative integer `n`, return the count of all numbers `x` with
//   unique decimal digits such that `0 <= x < 10^n`.
//   Constraints: 0 <= n <= 8 (and mathematically valid for any n >= 0 since
//   no number with more than 10 digits can have all distinct decimal digits).
//
// Algorithm:
//   1. Combinatorial Bottom-Up DP (`countNumbersWithUniqueDigits`):
//      Count valid numbers grouped by exact digit length `k`:
//        - `n == 0`: only `x = 0` lies in `[0, 1)`, so return `1`.
//        - `k == 1`: all 10 single-digit numbers `0..9` are valid -> `10`.
//        - `k == 2`: leading digit cannot be 0 (9 choices), second digit can be
//          any remaining digit including 0 (9 choices) -> `9 * 9 = 81`.
//        - `k >= 3` (up to `k = 10`): each additional digit multiplies the
//          count by `available = 11 - k` unused digits:
//          `f(k) = f(k - 1) * (11 - k)`.
//        - `k > 10`: by the Pigeonhole Principle, at least one decimal digit
//          must repeat, so `f(k) = 0`.
//      Summing `f(1) + f(2) + ... + f(min(n, 10))` yields the exact answer in
//      at most 9 loop iterations.
//   2. Bitmask Backtracking (`countNumbersWithUniqueDigitsBacktrack`):
//      Directly constructs every valid unique-digit number of length `1..min(n, 10)`
//      using a 10-bit `used_mask` (disallowing leading zeros for multi-digit
//      numbers), demonstrating the backtracking search space and validating
//      the combinatorial formula on small `n`.
//   Time Complexity:  O(min(n, 10)) = O(1) for the combinatorial DP.
//   Space Complexity: O(1) for the combinatorial DP; O(min(n, 10)) stack space
//                     for backtracking.
//
// Data Structures:
//   - Constant scalar accumulators (`total`, `unique_for_len`, `available`)
//     for O(1) space DP.
//   - 10-bit unsigned bitmask (`std::uint32_t`) tracking used decimal digits
//     during backtracking validation.
//
// Real-World Applications:
//   - PIN / OTP / license-plate keyspace sizing under non-repeating-character
//     password policies.
//   - Entropy estimation for human-readable verification codes that forbid
//     duplicate digits to reduce transcription errors.
//   - Combinatorial search-space bounding before launching brute-force
//     backtracking solvers.
//
// Related LeetCode Problems:
//   - #1012 Numbers With Repeated Digits [new] (complement problem with arbitrary upper bound N)
//   - #2376 Count Special Integers [new] (digit DP generalization for [1, N])
//   - #47 Permutations II [done] (counting/enumerating permutations without reuse)
//   - #401 Binary Watch [done] (small-space backtracking vs combinatorial enumeration)
//   - #788 Rotated Digits [done] (digit-level counting over [1, N])

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>

namespace {

class Solution {
 public:
  // Counts integers x in [0, 10^n) with all unique decimal digits using
  // combinatorial bottom-up DP.
  // Time: O(min(n, 10)) = O(1), Space: O(1).
  int countNumbersWithUniqueDigits(int n) {
    // Only x = 0 satisfies 0 <= x < 10^0 = 1.
    if (n == 0) {
      return 1;
    }

    // Start with all 10 one-digit numbers (0..9).
    int total = 10;
    // For k >= 2 digits, the leading digit has 9 choices (1..9).
    int unique_for_len = 9;
    // Remaining unused digits available for the next position (0..9 minus 1).
    int available = 9;

    // By the Pigeonhole Principle, numbers with > 10 digits cannot have all
    // unique decimal digits, so the loop naturally terminates once available == 0.
    for (int digits = 2; digits <= n && available > 0; ++digits) {
      unique_for_len *= available;
      total += unique_for_len;
      --available;
    }

    return total;
  }

  // Backtracking verification helper that enumerates all valid unique-digit
  // numbers of length <= min(n, 10) using a 10-bit used-digit bitmask.
  // Time: O(sum_{k=1..min(n,10)} P(10, k)), Space: O(min(n, 10)).
  int countNumbersWithUniqueDigitsBacktrack(int n) {
    if (n == 0) {
      return 1;
    }
    const int max_len = std::min(n, 10);

    // Count `0` directly; DFS builds all positive numbers starting with 1..9.
    int count = 1;

    auto dfs = [&](this auto&& self, int depth, std::uint32_t used_mask) -> void {
      if (depth == max_len) {
        return;
      }
      // Leading digit (depth == 0) cannot be 0 to avoid multi-digit leading zeros.
      const int start_digit = (depth == 0) ? 1 : 0;
      for (int digit = start_digit; digit <= 9; ++digit) {
        const std::uint32_t bit = 1U << static_cast<unsigned>(digit);
        if ((used_mask & bit) != 0U) {
          continue;
        }
        ++count;
        self(depth + 1, used_mask | bit);
      }
    };

    dfs(0, 0U);
    return count;
  }
};

LC_PROBLEM(357, "Count Numbers With Unique Digits", "backtracking", ctx) {
  Solution sol;

  ctx.test("standard_examples", [&] {
    ctx.expect_eq(sol.countNumbersWithUniqueDigits(0), 1);
    ctx.expect_eq(sol.countNumbersWithUniqueDigits(1), 10);
    ctx.expect_eq(sol.countNumbersWithUniqueDigits(2), 91);
    ctx.expect_eq(sol.countNumbersWithUniqueDigits(3), 739);
  });

  ctx.test("pigeonhole_cap_beyond_10_digits", [&] {
    // Mirrors the Python `build_input()` case (n = 11): adding digits beyond
    // 10 contributes 0 new unique-digit numbers.
    constexpr int kMaxUniqueDecimalNumbers = 8877691;
    ctx.expect_eq(sol.countNumbersWithUniqueDigits(10),
                  kMaxUniqueDecimalNumbers);
    ctx.expect_eq(sol.countNumbersWithUniqueDigits(11),
                  kMaxUniqueDecimalNumbers);
    ctx.expect_eq(sol.countNumbersWithUniqueDigits(20),
                  kMaxUniqueDecimalNumbers);
  });

  ctx.test("cross_check_dp_with_backtracking", [&] {
    for (int n = 0; n <= 5; ++n) {
      ctx.expect_eq(sol.countNumbersWithUniqueDigits(n),
                    sol.countNumbersWithUniqueDigitsBacktrack(n));
    }
  });
}

}  // namespace
