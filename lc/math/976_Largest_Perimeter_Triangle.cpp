// LeetCode 976: Largest Perimeter Triangle
// https://leetcode.com/problems/largest-perimeter-triangle/
//
// Problem:
//   Given an integer array `nums` representing segment lengths, return the
//   largest perimeter of a non-degenerate triangle (strictly positive area)
//   that can be formed by choosing three of these lengths. If no three lengths
//   can form a valid triangle, return 0.
//   Constraints: 3 <= nums.length <= 10^4, 1 <= nums[i] <= 10^6.
//
// Algorithm:
//   Greedy selection after descending sort.
//   For three side lengths c >= b >= a > 0, the triangle inequality holds
//   (giving positive area) iff c < b + a (the other two inequalities hold
//   automatically since c >= b and c >= a).
//   Key invariant: once `nums` is sorted in non-increasing order, if we fix
//   `nums[i]` as the longest side `c`, the best possible choices for the other
//   two sides to satisfy `c < b + a` AND maximize the perimeter are the two
//   immediately following elements `nums[i + 1]` and `nums[i + 2]`. If even
//   those two largest remaining values fail `nums[i] < nums[i + 1] + nums[i + 2]`,
//   then NO pair of smaller elements can work with `nums[i]`, so `nums[i]` can
//   never be the longest side of any valid triangle and we safely advance `i`.
//   The first triple satisfying the inequality yields the global maximum
//   perimeter.
//   Time Complexity:  O(N log N) for sorting, followed by an O(N) scan.
//   Space Complexity: O(N) for a local sorted copy (O(1) auxiliary if sorting
//                     the caller's array in place).
//
// Data Structures:
//   - std::vector<int>: contiguous array sorted in descending order so each
//     candidate longest side and its two largest neighbors are adjacent.
//
// Real-World Applications:
//   - Structural engineering & truss design: selecting optimal-span rigid
//     triangular bracing members from a stock inventory of beam lengths.
//   - Computational geometry & mesh generation: filtering degenerate
//     zero-area triangles when constructing valid simplicial complexes.
//   - Wireless localization & trilateration: validating whether measured
//     inter-node distances satisfy metric triangle inequalities before solving
//     for 2D positions.
//
// Related LeetCode Problems:
//   - #611 Valid Triangle Number [done] (counting all valid triangle triples using two pointers after sorting)
//   - #812 Largest Triangle Area [new] (maximizing triangle area over 2D point triples)
//   - #2971 Find Polygon With the Largest Perimeter [new] (generalization of the greedy triangle inequality to k-gons)
//   - #977 Squares of a Sorted Array [done] (sorted-order greedy reasoning on integer arrays)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum perimeter of any non-degenerate triangle formed by
  // three elements of `nums`, or 0 if none exists.
  // Time: O(N log N), Space: O(N).
  [[nodiscard]] int largestPerimeter(const std::vector<int>& nums) const {
    if (nums.size() < 3uz) {
      return 0;
    }

    // Sort a local copy descending so we test largest possible perimeters first.
    std::vector<int> sorted = nums;
    std::ranges::sort(sorted, std::greater<>{});

    // For each candidate longest side sorted[i], only the two immediately
    // following elements can maximize b + a to satisfy c < b + a.
    for (std::size_t i = 0uz; (i + 2uz) < sorted.size(); ++i) {
      const int longest = sorted[i];
      const int mid = sorted[i + 1uz];
      const int shortest = sorted[i + 2uz];

      if (longest < (mid + shortest)) {
        return (longest + mid) + shortest;
      }
    }

    return 0;
  }
};

LC_PROBLEM(976, "Largest Perimeter Triangle", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_isosceles", [&] {
    ctx.expect_eq(sol.largestPerimeter({2, 1, 2}), 5);
  });

  ctx.test("example_2_degenerate_line", [&] {
    // 2 == 1 + 1 has zero area, so no valid triangle exists.
    ctx.expect_eq(sol.largestPerimeter({1, 2, 1}), 0);
  });

  ctx.test("example_3_four_sides", [&] {
    ctx.expect_eq(sol.largestPerimeter({3, 2, 3, 4}), 10);
  });

  ctx.test("example_4_skip_outlier_longest_side", [&] {
    // 6 cannot pair with (3, 3) since 6 == 3 + 3; next triple (3, 3, 2) => 8.
    ctx.expect_eq(sol.largestPerimeter({3, 6, 2, 3}), 8);
  });

  ctx.test("impossible_fibonacci_like_growth", [&] {
    ctx.expect_eq(sol.largestPerimeter({1, 2, 3, 5, 8, 13}), 0);
  });
}

}  // namespace
