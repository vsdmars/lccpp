// LeetCode 670: Maximum Swap
// https://leetcode.com/problems/maximum-swap/
//
// Problem:
//   Given a non-negative integer `num`, you may swap two digits at most once
//   to obtain the maximum possible integer value. Return the maximum integer
//   achievable.
//
// Algorithm:
//   Single-Pass Right-to-Left Suffix-Maximum Scan (O(d) time, O(d) space,
//   where `d` is the number of base-10 digits, `d <= 9` for 32-bit `int`):
//     1. Convert `num` to its decimal string `digits`.
//     2. Scan from the least significant digit (`d - 1`) leftward to `0`,
//        tracking the rightmost occurrence (`max_idx`) of the largest digit
//        (`max_digit`) seen so far in the suffix:
//          - If `digits[i] > max_digit` (strictly greater so ties preserve the
//            *rightmost* index, which minimizes the value lost at the lower
//            place value), update `max_digit` and `max_idx`.
//          - Else if `digits[i] < max_digit`, then swapping `i` with `max_idx`
//            strictly increases the number. Record `swap_left = i` and
//            `swap_right = max_idx`. Continuing leftward automatically prefers
//            the most significant improvable position, while freezing
//            `swap_right` to the suffix maximum valid for that position (even
//            if an even larger digit later appears further left).
//     3. If a valid swap pair was recorded, swap `digits[swap_left]` and
//        `digits[swap_right]` and parse the string back to `int`.
//
//   Time Complexity: O(d) = O(log10(num)).
//   Space Complexity: O(d) for the decimal digit string.
//
// Data Structures:
//   - `std::string` storing the decimal digits of `num`.
//   - Four scalar tracking variables (`max_digit`, `max_idx`, `swap_left`,
//     `swap_right`) plus a `has_swap` flag.
//
// Real-World Applications:
//   - Greedy lexicographical optimization in resource allocation and priority
//     scheduling where a single pairwise swap improves the most significant
//     bottleneck metric.
//   - Financial order-book and auction tie-breaking heuristics prioritizing the
//     highest-weight slot first while selecting the cheapest rightmost donor.
//   - Local-search neighborhood operators (1-swap moves) in combinatorial
//     optimization solvers.
//
// Related LeetCode Problems:
//   - #31 Next Permutation [done] (lexicographically next digit/array permutation via suffix scan)
//   - #321 Create Maximum Number [done] (greedy lexicographically maximal digit selection)
//   - #402 Remove K Digits [done] (greedy monotonic digit manipulation)
//   - #556 Next Greater Element III [done] (single-number digit rearrangement with 32-bit bounds)
//   - #1323 Maximum 69 Number [new] (simpler single-digit greedy maximization)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <utility>

namespace {

class Solution {
 public:
  // Returns the maximum integer obtainable by swapping at most one pair of
  // digits in `num`.
  //
  // Invariant:
  //   During the right-to-left scan at index `i`, `max_idx` holds the rightmost
  //   index in `[i + 1, n - 1]` achieving the suffix maximum `max_digit`, and
  //   `(swap_left, swap_right)` (when `has_swap` is true) holds the optimal
  //   swap pair for the suffix `[i + 1, n - 1]`.
  //
  // Time Complexity: O(d), where `d` is the number of decimal digits in `num`.
  // Space Complexity: O(d).
  int maximumSwap(int num) {
    std::string digits = std::to_string(num);
    if (digits.size() <= 1) {
      return num;
    }

    char max_digit = digits.back();
    std::size_t max_idx = digits.size() - 1;

    bool has_swap = false;
    std::size_t swap_left = 0;
    std::size_t swap_right = 0;

    // Scan from second-to-last digit down to index 0 using underflow-safe
    // post-decrement on `std::size_t`.
    for (std::size_t i = digits.size() - 1; i-- > 0;) {
      if (digits[i] > max_digit) {
        // Strictly greater preserves the rightmost occurrence on ties (e.g.,
        // in 1993, swapping index 0 with index 2 yields 9913 > 9193).
        max_digit = digits[i];
        max_idx = i;
      } else if (digits[i] < max_digit) {
        // Record both indices together: a larger digit encountered even further
        // left later must not overwrite `swap_right` for this pair (e.g., in
        // 98368, the leading 9 updates `max_idx` without clobbering (2, 4)).
        has_swap = true;
        swap_left = i;
        swap_right = max_idx;
      }
    }

    if (has_swap) {
      std::swap(digits[swap_left], digits[swap_right]);
    }
    return std::stoi(digits);
  }
};

LC_PROBLEM(670, "Maximum Swap", "array", ctx) {
  Solution sol;

  ctx.test("example_1_basic_swap", [&] {
    ctx.expect_eq(sol.maximumSwap(2736), 7236);
  });

  ctx.test("example_2_already_maximal", [&] {
    ctx.expect_eq(sol.maximumSwap(9973), 9973);
  });

  ctx.test("python_build_duplicate_max_digits", [&] {
    // Must swap leading '1' with the *rightmost* '9' to maximize the result.
    ctx.expect_eq(sol.maximumSwap(10909091), 90909011);
  });

  ctx.test("leading_max_does_not_clobber_best_pair", [&] {
    // Leading '9' is larger than '8', but cannot swap leftward; best swap is
    // index 2 ('3') with index 4 ('8') -> 98863.
    ctx.expect_eq(sol.maximumSwap(98368), 98863);
  });

  ctx.test("duplicate_max_in_middle", [&] {
    ctx.expect_eq(sol.maximumSwap(1993), 9913);
    ctx.expect_eq(sol.maximumSwap(72636), 76632);
  });

  ctx.test("single_digit_and_zero", [&] {
    ctx.expect_eq(sol.maximumSwap(0), 0);
    ctx.expect_eq(sol.maximumSwap(5), 5);
  });
}

}  // namespace
