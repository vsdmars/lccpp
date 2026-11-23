// LeetCode 835: Image Overlap
// https://leetcode.com/problems/image-overlap/
//
// Problem:
//   Given two `n x n` binary matrices `img1` and `img2` (entries are `0` or
//   `1`), we may translate `img1` by any integer offset `(dr, dc)` (left,
//   right, up, or down, without rotation; bits shifted outside the `n x n`
//   bounds are discarded). The overlap of a translation is the number of
//   positions that contain a `1` in both the translated `img1` and `img2`.
//   Return the largest possible overlap over all translations.
//   Constraints: 1 <= n <= 30; img1[r][c], img2[r][c] in {0, 1}.
//
// Examples:
//   Example 1:
//     Input:  img1 = [[1, 1, 0], [0, 1, 0], [0, 1, 0]], img2 = [[0, 0, 0], [0, 1,
//             1], [0, 0, 1]]
//     Output: 3
//     Explanation:
//       - With n = 3, offset = 2, and span = 5, collectOnes extracts ones1 =
//         [(0,0), (0,1), (1,1), (2,1)] and ones2 = [(1,1), (1,2), (2,2)].
//       - Pairs (0,0)->(1,1), (0,1)->(1,2), and (1,1)->(2,2) all share displacement
//         (dr, dc) = (1, 1), mapping to shifted coordinates (3, 3) and bucket 3 * 5
//         + 3 = 18.
//       - Incrementing shift_counts[18] three times raises max_overlap to 3 while
//         all other shift buckets reach at most 2.
//       - Return max_overlap = 3.
//
//   Example 2:
//     Input:  img1 = [[0, 1], [1, 1]], img2 = [[1, 1], [1, 0]]
//     Output: 2
//     Explanation:
//       - With n = 2, offset = 1, and span = 3, collectOnes extracts ones1 =
//         [(0,1), (1,0), (1,1)] and ones2 = [(0,0), (0,1), (1,0)].
//       - Pairs (0,1)->(0,1) and (1,0)->(1,0) produce shift (0, 0) at bucket 1 * 3
//         + 1 = 4 with count 2.
//       - Shifts (0, -1) (bucket 3) and (-1, 0) (bucket 1) also reach count 2, and
//         no shift aligns all three 1s, so return max_overlap = 2.
//
//   Example 3:
//     Input:  img1 = [[1, 0, 0], [0, 0, 0], [0, 0, 0]], img2 = [[0, 0, 0], [0, 0,
//             0], [0, 0, 1]]
//     Output: 1
//     Explanation:
//       - With n = 3, offset = 2, and span = 5, collectOnes extracts ones1 =
//         [(0,0)] and ones2 = [(2,2)].
//       - The sole pair yields shifted_dr = (2 + 2) - 0 = 4 and shifted_dc = (2 +
//         2) - 0 = 4, mapping to bucket 4 * 5 + 4 = 24.
//       - Incrementing shift_counts[24] updates max_overlap to 1, and the function
//         returns 1.
//
// Algorithm:
//   Instead of testing all `(2n - 1)^2` translations against every grid cell
//   (including zeros), extract the sparse coordinate lists of `1`-cells in
//   `img1` (`ones1`) and `img2` (`ones2`). Every pair of `1`-cells
//   `(r1, c1) in ones1` and `(r2, c2) in ones2` aligns under the unique
//   translation vector `(dr, dc) = (r2 - r1, c2 - c1)`. Since `dr` and `dc`
//   each lie in `[-(n - 1), +(n - 1)]` (a span of `2n - 1` values), shifting
//   both by `n - 1` maps each 2D vector injectively into a flat count table
//   index `((dr + n - 1) * (2n - 1)) + (dc + n - 1)`. Incrementing this bucket
//   for every pair `(p1, p2)` counts how many `1`s overlap for every
//   translation simultaneously; the maximum bucket value is the answer.
//   Time Complexity:  O(n^2 + |ones1| * |ones2|), which is O(n^4) worst case
//                     for dense matrices and much faster for sparse images.
//   Space Complexity: O(n^2) for the coordinate lists and `(2n - 1)^2` table.
//
// Data Structures:
//   - Sparse Coordinate Lists (`std::vector<std::pair<std::size_t, std::size_t>>`):
//     store only the `(row, col)` coordinates of active `1` pixels in each binary
//     image.
//   - Flat 2D Shift Histogram (`std::vector<int>`): `(2n - 1) x (2n - 1)`
//     table mapping each translation vector `(dr, dc)` to its overlap count in
//     O(1) with no hashing overhead.
//
// Real-World Applications:
//   - Computer vision & image registration: aligning two binary edge maps,
//     star fields, or medical scans by discrete 2D cross-correlation.
//   - Semiconductor & PCB inspection: detecting translational misalignment
//     between a reference mask and an optical capture.
//   - Robotics & LiDAR scan matching: finding the 2D grid displacement that
//     maximizes occupied-cell agreement between consecutive occupancy grids.
//
// Related LeetCode Problems:
//   - #48 Rotate Image [done] (2D matrix geometric transformations)
//   - #311 Sparse Matrix Multiplication [done] (exploiting sparse non-zero coordinates)
//   - #221 Maximal Square [done] (binary 2D matrix pattern analysis)
//   - #661 Image Smoother [new] (2D sliding-window / kernel operations on images)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum number of overlapping `1`s over all 2D translations
  // by counting displacement vectors between active cells in `img1` and `img2`.
  // Time: O(n^2 + |ones1| * |ones2|), Space: O(n^2).
  int largestOverlap(const std::vector<std::vector<int>>& img1,
                     const std::vector<std::vector<int>>& img2) {
    const std::size_t n = img1.size();
    if (n == 0uz) {
      return 0;
    }

    const std::vector<std::pair<std::size_t, std::size_t>> ones1 =
        collectOnes(img1);
    const std::vector<std::pair<std::size_t, std::size_t>> ones2 =
        collectOnes(img2);
    if (ones1.empty() || ones2.empty()) {
      return 0;
    }

    // Both row shift `dr = r2 - r1` and column shift `dc = c2 - c1` fall in
    // `[-(n - 1), +(n - 1)]`, giving `span = 2n - 1` distinct values per axis.
    // Adding `offset = n - 1` before subtracting keeps unsigned arithmetic
    // strictly non-negative in `[0, 2n - 2]`.
    const std::size_t offset = n - 1uz;
    const std::size_t span = (2uz * n) - 1uz;
    std::vector<int> shift_counts(span * span, 0);

    int max_overlap = 0;
    for (const auto& [r1, c1] : ones1) {
      for (const auto& [r2, c2] : ones2) {
        const std::size_t shifted_dr = (r2 + offset) - r1;
        const std::size_t shifted_dc = (c2 + offset) - c1;
        const std::size_t bucket = (shifted_dr * span) + shifted_dc;

        ++shift_counts[bucket];
        max_overlap = std::max(max_overlap, shift_counts[bucket]);
      }
    }

    return max_overlap;
  }

 private:
  // Extracts the `(row, col)` coordinates of every `1` cell in a square binary
  // image so zero pixels are skipped during translation matching.
  static std::vector<std::pair<std::size_t, std::size_t>> collectOnes(
      const std::vector<std::vector<int>>& img) {
    const std::size_t n = img.size();
    std::vector<std::pair<std::size_t, std::size_t>> coords;
    for (std::size_t r = 0uz; r < n; ++r) {
      for (std::size_t c = 0uz; c < n; ++c) {
        if (img[r][c] == 1) {
          coords.emplace_back(r, c);
        }
      }
    }
    return coords;
  }
};

LC_PROBLEM(835, "Image Overlap", "array", ctx) {
  Solution sol;

  ctx.test("example_1_shift_right_1_down_1", [&] {
    const std::vector<std::vector<int>> img1 = {
        {1, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
    };
    const std::vector<std::vector<int>> img2 = {
        {0, 0, 0},
        {0, 1, 1},
        {0, 0, 1},
    };
    ctx.expect_eq(sol.largestOverlap(img1, img2), 3);
  });

  ctx.test("python_build_2x2_case", [&] {
    const std::vector<std::vector<int>> img1 = {
        {0, 1},
        {1, 1},
    };
    const std::vector<std::vector<int>> img2 = {
        {1, 1},
        {1, 0},
    };
    ctx.expect_eq(sol.largestOverlap(img1, img2), 2);
  });

  ctx.test("single_cell_matrices", [&] {
    ctx.expect_eq(sol.largestOverlap({{1}}, {{1}}), 1);
    ctx.expect_eq(sol.largestOverlap({{0}}, {{0}}), 0);
    ctx.expect_eq(sol.largestOverlap({{1}}, {{0}}), 0);
  });

  ctx.test("opposite_corners_align_with_max_shift", [&] {
    const std::vector<std::vector<int>> img1 = {
        {1, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };
    const std::vector<std::vector<int>> img2 = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 1},
    };
    ctx.expect_eq(sol.largestOverlap(img1, img2), 1);
  });

  ctx.test("all_ones_full_overlap", [&] {
    const std::vector<std::vector<int>> ones = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1},
    };
    ctx.expect_eq(sol.largestOverlap(ones, ones), 9);
  });
}

}  // namespace
