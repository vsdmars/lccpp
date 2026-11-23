// LeetCode 167: Two Sum II Input Array Is Sorted
// https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/
//
// Problem:
//   Given a 1-indexed array of integers `numbers` sorted in non-decreasing
//   order, find two distinct elements that add up to a specific `target`.
//   Return their 1-based indices `[index1, index2]` with
//   `1 <= index1 < index2 <= numbers.size()`. Exactly one solution is
//   guaranteed to exist, and the same element may not be used twice.
//
// Examples:
//   Example 1:
//     Input:  numbers = [2, 7, 11, 15], target = 9
//     Output: [1, 2]
//     Explanation:
//       - Initialize left = 0 (numbers[0] = 2) and right = 3 (numbers[3] = 15) with
//         target64 = 9.
//       - Compute sum = 2 + 15 = 17 > 9, so decrement right to 2 (numbers[2] = 11).
//       - Compute sum = 2 + 11 = 13 > 9, so decrement right to 1 (numbers[1] = 7).
//       - Compute sum = 2 + 7 = 9 == 9 and return 1-based indices [0 + 1, 1 + 1] =
//         [1, 2].
//
//   Example 2:
//     Input:  numbers = [1, 3, 3, 8], target = 6
//     Output: [2, 3]
//     Explanation:
//       - Initialize left = 0 (numbers[0] = 1) and right = 3 (numbers[3] = 8) with
//         target64 = 6.
//       - Compute sum = 1 + 8 = 9 > 6, so decrement right to 2 (numbers[2] = 3).
//       - Compute sum = 1 + 3 = 4 < 6, so increment left to 1 (numbers[1] = 3).
//       - Compute sum = 3 + 3 = 6 == 6 with left < right, returning 1-based indices
//         [1 + 1, 2 + 1] = [2, 3].
//
//   Example 3:
//     Input:  numbers = [-2147483648, -1, 0, 1, 2147483647], target = -1
//     Output: [1, 5]
//     Explanation:
//       - Initialize left = 0 (numbers[0] = -2147483648) and right = 4 (numbers[4]
//         = 2147483647) with target64 = -1.
//       - Widen both operands to int64_t and compute sum = -2147483648 + 2147483647
//         = -1 without 32-bit signed overflow.
//       - Since sum == target64, immediately return 1-based indices [0 + 1, 4 + 1]
//         = [1, 5].
//
// Algorithm:
//   Converging Two Pointers:
//     Place `left` at the first element (`0`) and `right` at the last element
//     (`n - 1`). At each step, compute `sum = numbers[left] + numbers[right]`
//     in 64-bit arithmetic to avoid signed overflow:
//       - If `sum == target`, return `{left + 1, right + 1}`.
//       - If `sum < target`, `numbers[left]` paired with even the largest
//         remaining value `numbers[right]` is still too small, so `left` can
//         never participate in the answer; increment `left`.
//       - If `sum > target`, `numbers[right]` paired with even the smallest
//         remaining value `numbers[left]` is still too large, so `right` can
//         never participate in the answer; decrement `right`.
//     Each step eliminates one candidate index, guaranteeing convergence to
//     the unique solution in at most n - 1 iterations.
//   Time Complexity:  O(n), where n = numbers.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two index pointers (`left`, `right`) operating in-place over the sorted
//     input `std::vector<int>`.
//
// Real-World Applications:
//   - Sort-merge join predicates and range-pair matching in relational database
//     query engines.
//   - Matching complementary buy/sell orders or budget-constrained item pairs
//     in financial and e-commerce systems.
//   - Pairwise mass-spectrometry peak matching over sorted m/z spectra.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (unsorted variant requiring an O(n) hash map)
//   - #15 3Sum [done] (outer loop reducing to sorted two-pointer Two Sum II)
//   - #16 3Sum Closest [done] (two-pointer closest-sum tracking on sorted array)
//   - #18 4Sum [done] (nested reduction to sorted two-pointer scan)
//   - #653 Two Sum IV [done] - Input is a BST (in-order iterator two-pointer analog)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the 1-based indices [index1, index2] of the two distinct elements
  // in sorted `numbers` whose sum equals `target`.
  // Invariant: if a valid pair (i, j) exists, it always satisfies
  // left <= i < j <= right.
  // Time: O(n), Space: O(1).
  std::vector<int> twoSum(const std::vector<int>& numbers, int target) {
    if (numbers.size() < 2uz) {
      return {};
    }

    std::size_t left = 0uz;
    std::size_t right = numbers.size() - 1;
    const auto target64 = static_cast<std::int64_t>(target);

    // Strictly `left < right` guarantees the same element is never reused.
    while (left < right) {
      const std::int64_t sum = static_cast<std::int64_t>(numbers[left]) +
                               static_cast<std::int64_t>(numbers[right]);

      if (sum == target64) {
        return {static_cast<int>(left + 1), static_cast<int>(right + 1)};
      }

      if (sum < target64) {
        // Current sum is too small; move left forward to increase the sum.
        ++left;
      } else {
        // Current sum is too large; move right backward to decrease the sum.
        --right;
      }
    }

    // Unreachable for valid LeetCode inputs (which guarantee a unique pair).
    return {};
  }
};

LC_PROBLEM(167, "Two Sum II Input Array Is Sorted", "array", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    ctx.expect_eq(sol.twoSum({2, 7, 11, 15}, 9), std::vector<int>{1, 2});
  });

  ctx.test("example_2_outer_pair", [&] {
    ctx.expect_eq(sol.twoSum({2, 3, 4}, 6), std::vector<int>{1, 3});
  });

  ctx.test("example_3_negatives", [&] {
    ctx.expect_eq(sol.twoSum({-1, 0}, -1), std::vector<int>{1, 2});
  });

  ctx.test("duplicate_elements_forming_target", [&] {
    ctx.expect_eq(sol.twoSum({1, 3, 3, 8}, 6), std::vector<int>{2, 3});
  });

  ctx.test("mixed_signs_and_wide_range", [&] {
    ctx.expect_eq(sol.twoSum({-10, -3, 0, 5, 9, 12}, 2),
                  std::vector<int>{1, 6});
  });

  ctx.test("extreme_values_no_overflow", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    ctx.expect_eq(sol.twoSum({kMin, -1, 0, 1, kMax}, -1),
                  std::vector<int>{1, 5});
  });
}

}  // namespace
