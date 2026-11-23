// LeetCode 611: Valid Triangle Number
// https://leetcode.com/problems/valid-triangle-number/
//
// Problem:
//   Given an integer array `nums` of non-negative side lengths, return the
//   number of index triplets `(i, j, k)` with `i < j < k` that can form a
//   non-degenerate triangle (each side strictly smaller than the sum of the
//   other two).
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 3, 5, 4]
//     Output: 3
//     Explanation:
//       - Sort a local copy to sorted = [2, 3, 4, 5].
//       - Fix k = 3 (sorted[3] = 5): with left = 0 and right = 2, 2 + 4 > 5 adds
//         right - left = 2 and sets right = 1; then 2 + 3 <= 5 advances left to 1
//         (count = 2).
//       - Fix k = 2 (sorted[2] = 4): with left = 0 and right = 1, 2 + 3 > 4 adds
//         right - left = 1 and sets right = 0, returning count = 3.
//
//   Example 2:
//     Input:  nums = [2, 2, 3, 4]
//     Output: 3
//     Explanation:
//       - Use the already sorted copy sorted = [2, 2, 3, 4].
//       - Fix k = 3 (sorted[3] = 4): left = 0, right = 2 satisfies 2 + 3 > 4 to add
//         2 triplets and decrement right to 1, then 2 + 2 <= 4 increments left to 1
//         (count = 2).
//       - Fix k = 2 (sorted[2] = 3): left = 0, right = 1 satisfies 2 + 2 > 3 to add
//         1 triplet and decrement right to 0, returning count = 3.
//
//   Example 3:
//     Input:  nums = [1, 2, 3, 6, 10]
//     Output: 0
//     Explanation:
//       - Keep the sorted copy sorted = [1, 2, 3, 6, 10] and start with count = 0.
//       - For k = 4 (10), even the largest pair 3 + 6 = 9 <= 10 advances left to
//         right = 3 without adding any triplets.
//       - For k = 3 (6) with max pair 2 + 3 <= 6 and k = 2 (3) with 1 + 2 <= 3,
//         every check fails strict inequality, returning 0.
//
// Algorithm:
//   Sort a copy of `nums` in non-decreasing order. When three sides satisfy
//   `a <= b <= c`, the three triangle inequalities reduce to the single check
//   `a + b > c` (since `a + c > b` and `b + c > a` automatically hold for
//   positive `a, b, c`, and `a + b > c` fails whenever `a == 0` because
//   `0 + b <= c`).
//   Fix the largest side `sorted[k]` by iterating `k` from `n - 1` down to `2`,
//   and run a converging two-pointer scan with `left = 0` and `right = k - 1`:
//     - If `sorted[left] + sorted[right] > sorted[k]`, then every index `idx`
//       in `[left, right - 1]` also satisfies `sorted[idx] + sorted[right] >
//       sorted[k]` because the array is sorted. Add `right - left` to the
//       count and decrement `right`.
//     - Otherwise, `sorted[left]` is too small even with the largest remaining
//       partner `sorted[right]`, so increment `left`.
//
//   Time Complexity: O(n^2) — sorting takes O(n log n), and each of the O(n)
//   choices of the largest side runs a linear O(n) two-pointer pass.
//   Space Complexity: O(n) for the non-mutating sorted copy (O(log n) auxiliary
//   if sorting in place).
//
// Data Structures:
//   - Sorted `std::vector<int>` enabling monotonic two-pointer pruning.
//   - Two index pointers (`left`, `right`) per fixed largest side `k`.
//
// Real-World Applications:
//   - Computational geometry and 3D mesh validation (filtering degenerate
//     triangulations and checking metric-space triangle inequalities).
//   - Robotics kinematics and linkage feasibility checks (verifying whether
//     three rigid arm segments can form a closed triangular loop).
//   - Network latency anomaly detection (counting triples of nodes obeying
//     or violating triangle inequality routing bounds).
//
// Related LeetCode Problems:
//   - #15 3Sum [done] (canonical sort + converging two-pointer triplet enumeration)
//   - #16 3Sum Closest [done] (sort + two pointers minimizing triplet sum distance)
//   - #259 3Sum Smaller [done] (exact same batch-counting `(right - left)` trick)
//   - #976 Largest Perimeter Triangle [done] (greedy largest valid triangle after sorting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Counts the number of index triplets in `nums` that can form a valid
  // triangle with strictly positive area.
  //
  // Invariant:
  //   For each fixed largest side `sorted[k]`, all valid pairs `(i, j)` with
  //   `i < j < k` outside the current `[left, right]` window have already been
  //   accounted for, and every remaining candidate lies within `[left, right]`.
  //
  // Time Complexity: O(n^2).
  // Space Complexity: O(n) to avoid mutating the caller's input vector.
  int triangleNumber(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n < 3) {
      return 0;
    }

    // Sort a local copy so `sorted[left] <= sorted[right] <= sorted[k]`.
    std::vector<int> sorted = nums;
    std::ranges::sort(sorted);

    int count = 0;

    // Fix the longest side `sorted[k]` from largest to smallest (k >= 2).
    for (std::size_t k = n - 1; k >= 2; --k) {
      std::size_t left = 0;
      std::size_t right = k - 1;

      while (left < right) {
        if (sorted[left] + sorted[right] > sorted[k]) {
          // Because `sorted` is non-decreasing, every index in `[left, right - 1]`
          // paired with `right` also exceeds `sorted[k]`.
          count += static_cast<int>(right - left);
          --right;
        } else {
          // `sorted[left]` is too small even with the largest available partner.
          ++left;
        }
      }
    }

    return count;
  }
};

LC_PROBLEM(611, "Valid Triangle Number", "array", ctx) {
  Solution sol;

  ctx.test("example_1_duplicates", [&] {
    // Valid triplets: (2, 3, 4) twice and (2, 2, 3).
    ctx.expect_eq(sol.triangleNumber({2, 2, 3, 4}), 3);
  });

  ctx.test("example_2_distinct_sides", [&] {
    // Valid triplets: (2, 3, 4), (2, 4, 5), (3, 4, 5).
    ctx.expect_eq(sol.triangleNumber({2, 3, 5, 4}), 3);
  });

  ctx.test("example_3_four_sides", [&] {
    ctx.expect_eq(sol.triangleNumber({4, 2, 3, 4}), 4);
  });

  ctx.test("zeros_cannot_form_triangles", [&] {
    ctx.expect_eq(sol.triangleNumber({0, 0, 0}), 0);
    ctx.expect_eq(sol.triangleNumber({0, 1, 1, 1}), 1);
  });

  ctx.test("fewer_than_three_elements", [&] {
    ctx.expect_eq(sol.triangleNumber({}), 0);
    ctx.expect_eq(sol.triangleNumber({3}), 0);
    ctx.expect_eq(sol.triangleNumber({3, 4}), 0);
  });

  ctx.test("degenerate_triangles_excluded", [&] {
    // 1 + 2 == 3 is degenerate, not a valid triangle.
    ctx.expect_eq(sol.triangleNumber({1, 2, 3, 6, 10}), 0);
  });
}

}  // namespace
