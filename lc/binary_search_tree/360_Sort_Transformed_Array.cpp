// LeetCode 360: Sort Transformed Array
// https://leetcode.com/problems/sort-transformed-array/
//
// Problem:
//   Given a sorted integer array `nums` in non-decreasing order and three
//   integer coefficients `a`, `b`, and `c`, apply the quadratic function
//   f(x) = a*x^2 + b*x + c to every element `x` in `nums` and return the
//   transformed values in non-decreasing sorted order. The target runtime is
//   O(n).
//
// Algorithm:
//   Exploit the unimodal shape of a parabola evaluated on a sorted domain:
//   - When `a >= 0`, f(x) opens upward (or is a monotone line when `a == 0`),
//     meaning the largest values always lie at the two outer ends of the
//     remaining interval `[left, right]`. Compare f(nums[left]) and
//     f(nums[right]), place the LARGER value at the back of the output array
//     (filling from index n - 1 down to 0), and advance the chosen pointer
//     inward.
//   - When `a < 0`, f(x) opens downward, meaning the smallest values always
//     lie at the two outer ends `[left, right]`. Compare f(nums[left]) and
//     f(nums[right]), place the SMALLER value at the front of the output array
//     (filling from index 0 up to n - 1), and advance the chosen pointer
//     inward.
//   Using `>=` (for `a >= 0`) and `<=` (for `a < 0`) ensures that on the final
//   step when `left == right`, the `left` pointer advances and `right` is never
//   decremented below 0.
//   Time Complexity: O(n), a single two-pointer pass over `nums`.
//   Space Complexity: O(1) auxiliary space (excluding the O(n) output vector).
//
// Data Structures:
//   - Pre-sized output `std::vector<int>` filled from the back (`a >= 0`) or
//     front (`a < 0`).
//   - Two index pointers (`left`, `right`) converging from the array endpoints.
//
// Real-World Applications:
//   - Streaming / real-time signal processing where sorted timestamps or
//     sensor readings pass through convex/concave quadratic calibration curves.
//   - Numerical optimization and computational geometry when merging sorted
//     parabolic evaluations without paying an O(n log n) re-sort cost.
//   - Database query execution: preserving sort order across unimodal
//     projections in linear time.
//
// Related LeetCode Problems:
//   - #977 Squares of a Sorted Array [done] (special case a = 1, b = 0, c = 0)
//   - #88 Merge Sorted Array [done] (two-pointer merge filling from the back)
//   - #167 Two Sum II [done] - Input Array Is Sorted (converging two pointers on sorted input)
//   - #11 Container With Most Water [done] (endpoint elimination with two pointers)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Applies f(x) = a*x^2 + b*x + c to each element of sorted `nums` and
  // returns the results in non-decreasing order in O(n) time.
  // Time: O(n), Space: O(1) extra (O(n) output).
  std::vector<int> sortTransformedArray(const std::vector<int>& nums, int a,
                                        int b, int c) {
    if (nums.empty()) {
      return {};
    }

    const std::size_t n = nums.size();
    std::vector<int> result(n);
    std::size_t left = 0uz;
    std::size_t right = n - 1uz;

    // Evaluates the quadratic polynomial f(x) = a*x^2 + b*x + c.
    auto eval = [a, b, c](int x) -> int { return (a * x * x) + (b * x) + c; };

    for (std::size_t step = 0uz; step < n; ++step) {
      const int left_val = eval(nums[left]);
      const int right_val = eval(nums[right]);

      if (a >= 0) {
        // Upward-opening parabola (or linear when a == 0): outer endpoints
        // hold the current maximum, so fill `result` from back to front.
        const std::size_t write_idx = n - 1uz - step;
        if (left_val >= right_val) {
          result[write_idx] = left_val;
          ++left;
        } else {
          result[write_idx] = right_val;
          --right;
        }
      } else {
        // Downward-opening parabola: outer endpoints hold the current minimum,
        // so fill `result` from front to back.
        const std::size_t write_idx = step;
        if (left_val <= right_val) {
          result[write_idx] = left_val;
          ++left;
        } else {
          result[write_idx] = right_val;
          --right;
        }
      }
    }

    return result;
  }
};

LC_PROBLEM(360, "Sort Transformed Array", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1_upward_parabola", [&] {
    ctx.expect_eq(sol.sortTransformedArray({-4, -2, 2, 4}, 1, 3, 5),
                  std::vector<int>{3, 9, 15, 33});
  });

  ctx.test("leetcode_example_2_downward_parabola", [&] {
    ctx.expect_eq(sol.sortTransformedArray({-4, -2, 2, 4}, -1, 3, 5),
                  std::vector<int>{-23, -5, 1, 7});
  });

  ctx.test("linear_positive_and_negative_slope", [&] {
    // a == 0, b > 0 preserves order.
    ctx.expect_eq(sol.sortTransformedArray({-3, -1, 0, 2, 5}, 0, 2, 1),
                  std::vector<int>{-5, -1, 1, 5, 11});
    // a == 0, b < 0 reverses order.
    ctx.expect_eq(sol.sortTransformedArray({-3, -1, 0, 2, 5}, 0, -2, 1),
                  std::vector<int>{-9, -3, 1, 3, 7});
    // Constant function (a == 0, b == 0).
    ctx.expect_eq(sol.sortTransformedArray({-2, 0, 3}, 0, 0, 7),
                  std::vector<int>{7, 7, 7});
  });

  ctx.test("empty_and_single_element", [&] {
    ctx.expect_eq(sol.sortTransformedArray({}, 1, 2, 3), std::vector<int>{});
    ctx.expect_eq(sol.sortTransformedArray({-5}, -2, 3, 1),
                  std::vector<int>{-64});
  });

  ctx.test("duplicates_and_symmetric_values", [&] {
    ctx.expect_eq(sol.sortTransformedArray({-3, -3, 0, 3, 3}, 1, 0, 0),
                  std::vector<int>{0, 9, 9, 9, 9});
  });
}

}  // namespace
