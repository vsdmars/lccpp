// LeetCode 977: Squares Of A Sorted Array
// https://leetcode.com/problems/squares-of-a-sorted-array/
//
// Problem:
//   Given an integer array `nums` sorted in non-decreasing order (may contain
//   negative, zero, and positive values), return an array of the squares of
//   each element, also sorted in non-decreasing order.
//   Constraints: 1 <= nums.length <= 10^4, -10^4 <= nums[i] <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  nums = [-4, -1, 0, 3, 10]
//     Output: [0, 1, 9, 16, 100]
//     Explanation:
//       - Allocate result of size 5 and initialize left = 0, right = 5, and write =
//         5.
//       - At write = 4, compare (-4)^2 = 16 and 10^2 = 100; place 100 at result[4]
//         and decrement right to 4.
//       - At write = 3, compare (-4)^2 = 16 and 3^2 = 9; place 16 at result[3] and
//         increment left to 1.
//       - At write = 2, compare (-1)^2 = 1 and 3^2 = 9; place 9 at result[2] and
//         decrement right to 3.
//       - At write = 1, compare (-1)^2 = 1 and 0^2 = 0; place 1 at result[1] and
//         increment left to 2.
//       - At write = 0 (left = 2, right = 3), 0 > 0 is false, so write 0 at
//         result[0], decrement right to 2, and return [0, 1, 9, 16, 100].
//
//   Example 2:
//     Input:  nums = [-5, -3, -2, -1]
//     Output: [1, 4, 9, 25]
//     Explanation:
//       - Allocate result of size 4 and initialize left = 0, right = 4, and write =
//         4.
//       - For write = 3, 2, and 1, left_sq exceeds right_sq (25 > 1, 9 > 1, and 4 >
//         1), so 25, 9, and 4 are written to result[3..1] as left advances to 3.
//       - At write = 0 (left = 3, right = 4), left_sq == right_sq == 1 so (-1)^2 =
//         1 is placed at result[0], right becomes 3, and [1, 4, 9, 25] is returned.
//
//   Example 3:
//     Input:  nums = [-10000]
//     Output: [100000000]
//     Explanation:
//       - Allocate result of size 1 with left = 0, right = 1, and write = 1.
//       - On the single iteration (write = 0), left_sq and right_sq both equal
//         (-10000)^2 = 100000000; left_sq > right_sq is false, so result[0]
//         receives 100000000 and right decrements to 0.
//       - The loop ends with left == right == 0 and returns [100000000].
//
// Algorithm:
//   Converging two pointers filling the output from back to front:
//   - Because `nums` is sorted, the squares form a V-shaped (unimodal) sequence
//     whose largest values always sit at the two extreme ends (`left = 0` and
//     `right - 1 = n - 1`).
//   - Maintain half-open active range `[left, right)` and a write cursor
//     `write` starting at `n`. At each step, decrement `write`, compare
//     `nums[left]^2` against `nums[right - 1]^2`, place the larger square into
//     `result[write]`, and advance the chosen pointer inward.
//   - Filling from index `n - 1` down to `0` yields the sorted output directly
//     without needing an extra reversal pass.
//   Time Complexity:  O(N) — single linear pass.
//   Space Complexity: O(1) auxiliary space (O(N) for the returned result).
//
// Data Structures:
//   - Output `std::vector<int>` (`result`) pre-sized to `nums.size()`, filled
//     from back to front via converging two pointers (`left`, `right`).
//
// Real-World Applications:
//   - Merging sorted negative and non-negative error/residual streams by
//     squared magnitude (e.g., L2 loss / variance ranking in analytics pipelines).
//   - Signal processing pipelines sorting signed sensor deviations by energy
//     (`amplitude^2`) without an O(N log N) re-sort.
//   - Database query execution optimizing `ORDER BY x*x` over an existing
//     B-tree index on `x` via bidirectional index scan.
//
// Related LeetCode Problems:
//   - #360 Sort Transformed Array [done] (generalizes f(x) = x^2 to any quadratic a*x^2 + b*x + c)
//   - #88 Merge Sorted Array [done] (filling a merged sorted array from the back)
//   - #905 Sort Array By Parity [done] (converging two-pointer traversal over an array)
//   - #658 Find K Closest Elements [done] (exploiting sorted order around a pivot value)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the squares of all elements in non-decreasing `nums`, sorted in
  // non-decreasing order.
  // Invariant: `result[write .. n - 1]` contains the `n - write` largest
  // squares in sorted order, and all remaining elements lie in `[left, right)`.
  // Time: O(N), Space: O(1) extra (O(N) output).
  std::vector<int> sortedSquares(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    std::vector<int> result(n);

    // Half-open range [left, right) prevents unsigned underflow when right hits 0.
    std::size_t left = 0uz;
    std::size_t right = n;
    std::size_t write = n;

    while (left < right) {
      --write;
      const int left_sq = nums[left] * nums[left];
      const int right_sq = nums[right - 1uz] * nums[right - 1uz];

      // The larger square belongs at the highest unfilled position `write`.
      if (left_sq > right_sq) {
        result[write] = left_sq;
        ++left;
      } else {
        result[write] = right_sq;
        --right;
      }
    }

    return result;
  }
};

LC_PROBLEM(977, "Squares Of A Sorted Array", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_mixed_with_zero", [&] {
    ctx.expect_eq(sol.sortedSquares({-4, -1, 0, 3, 10}),
                  std::vector<int>{0, 1, 9, 16, 100});
  });

  ctx.test("example_2_mixed_no_zero", [&] {
    ctx.expect_eq(sol.sortedSquares({-7, -3, 2, 3, 11}),
                  std::vector<int>{4, 9, 9, 49, 121});
  });

  ctx.test("python_build_cases", [&] {
    ctx.expect_eq(sol.sortedSquares({0, 2}), std::vector<int>{0, 4});
    ctx.expect_eq(sol.sortedSquares({-1, 1}), std::vector<int>{1, 1});
  });

  ctx.test("all_negative_and_all_positive", [&] {
    ctx.expect_eq(sol.sortedSquares({-5, -3, -2, -1}),
                  std::vector<int>{1, 4, 9, 25});
    ctx.expect_eq(sol.sortedSquares({1, 2, 3, 5}),
                  std::vector<int>{1, 4, 9, 25});
  });

  ctx.test("single_element_and_empty", [&] {
    ctx.expect_eq(sol.sortedSquares({-10000}), std::vector<int>{100000000});
    ctx.expect_eq(sol.sortedSquares({}), std::vector<int>{});
  });
}

}  // namespace
