// LeetCode 1868: Product Of Two Run Length Encoded Arrays
// https://leetcode.com/problems/product-of-two-run-length-encoded-arrays/
//
// Problem:
//   Run-length encoding (RLE) compresses an integer array into segments
//   `[val, freq]`, meaning `val` repeated `freq` consecutive times.
//   Given two RLE arrays `encoded1` and `encoded2` whose expanded arrays have
//   the same total length, compute the element-wise product of the expanded
//   arrays and return it compressed into the shortest possible RLE array
//   (adjacent segments with the same product value must be merged).
//   Constraints:
//     - 1 <= encoded1.length, encoded2.length <= 10^5
//     - 1 <= val_i, freq_i <= 10^4
//     - sum(freq in encoded1) == sum(freq in encoded2)
//
// Examples:
//   Example 1:
//     Input:  encoded1 = [[1, 3], [2, 3]], encoded2 = [[6, 3], [3, 3]]
//     Output: [[6, 6]]
//     Explanation:
//       - At idx1 = 0 ([1, 3], rem1 = 3) and idx2 = 0 ([6, 3], rem2 = 3), overlap =
//         min(3, 3) = 3 and prod = 1 * 6 = 6, appending [6, 3] and advancing both
//         pointers.
//       - At idx1 = 1 ([2, 3], rem1 = 3) and idx2 = 1 ([3, 3], rem2 = 3), overlap =
//         3 and prod = 2 * 3 = 6; since result.back()[0] == 6, it merges into [6, 3
//         + 3] = [6, 6].
//       - Both streams reach the end simultaneously, returning [[6, 6]].
//
//   Example 2:
//     Input:  encoded1 = [[1, 3], [2, 1], [3, 2]], encoded2 = [[2, 3], [3, 3]]
//     Output: [[2, 3], [6, 1], [9, 2]]
//     Explanation:
//       - At idx1 = 0 ([1, 3]) and idx2 = 0 ([2, 3]), overlap = 3 and prod = 1 * 2
//         = 2 append [2, 3] and advance to idx1 = 1 (rem1 = 1) and idx2 = 1 (rem2 =
//         3).
//       - At idx1 = 1 ([2, 1]) and idx2 = 1 ([3, 3]), overlap = min(1, 3) = 1 and
//         prod = 2 * 3 = 6 append [6, 1]; rem1 reaches 0 so idx1 advances to 2
//         (rem1 = 2) while rem2 becomes 2.
//       - At idx1 = 2 ([3, 2]) and idx2 = 1 ([3, 3]), overlap = min(2, 2) = 2 and
//         prod = 3 * 3 = 9 append [9, 2], exhausting both inputs and returning [[2,
//         3], [6, 1], [9, 2]].
//
//   Example 3:
//     Input:  encoded1 = [[1, 2], [2, 4]], encoded2 = [[6, 3], [3, 3]]
//     Output: [[6, 2], [12, 1], [6, 3]]
//     Explanation:
//       - At idx1 = 0 ([1, 2], rem1 = 2) and idx2 = 0 ([6, 3], rem2 = 3), overlap =
//         2 and prod = 6 append [6, 2]; idx1 advances to 1 ([2, 4], rem1 = 4) while
//         rem2 drops to 1.
//       - At idx1 = 1 and idx2 = 0, overlap = min(4, 1) = 1 and prod = 2 * 6 = 12
//         append [12, 1]; rem1 drops to 3 and idx2 advances to 1 ([3, 3], rem2 =
//         3).
//       - At idx1 = 1 and idx2 = 1, overlap = min(3, 3) = 3 and prod = 2 * 3 = 6
//         append [6, 3] (kept separate from [6, 2] by [12, 1]), returning [[6, 2],
//         [12, 1], [6, 3]].
//
// Algorithm:
//   Expanding either array can take up to 10^9 elements, so we operate
//   directly on the compressed segments using two pointers `idx1` and `idx2`
//   along with local remaining-count variables `rem1` and `rem2` (avoiding
//   any mutation of the caller's input arrays).
//   At each step:
//     1. The current active segments overlap for `overlap = min(rem1, rem2)`
//        positions, all sharing product `prod = val1 * val2`.
//     2. If the last emitted segment in `result` already has value `prod`,
//        merge by adding `overlap` to its frequency; otherwise append
//        `{prod, overlap}`.
//     3. Subtract `overlap` from both `rem1` and `rem2`. Whenever a remaining
//        count reaches 0, advance that array's pointer and load the next
//        segment's frequency.
//   Each iteration exhausts at least one segment, so the loop executes at most
//   `encoded1.size() + encoded2.size()` times.
//   Time Complexity:  O(M + N), where M = encoded1.size(), N = encoded2.size().
//   Space Complexity: O(1) auxiliary space (excluding the output RLE vector).
//
// Data Structures:
//   - Two `std::size_t` segment pointers and two `int` remaining-frequency
//     counters tracking the partially consumed head of each RLE stream.
//   - `std::vector<std::vector<int>>` accumulating the merged RLE output.
//
// Real-World Applications:
//   - Columnar analytical databases (Apache Parquet / DuckDB / ClickHouse)
//     performing vectorized arithmetic directly on RLE-compressed columns
//     without decompression.
//   - Time-series / telemetry stream alignment and scaling over piecewise-
//     constant signals.
//   - Sparse bitmap and fax/image scanline compositing in RLE form.
//
// Related LeetCode Problems:
//   - #1570 Dot Product of Two Sparse Vectors [done] (two-pointer product over compressed vector representations)
//   - #986 Interval List Intersections [done] (two-pointer overlap consumption across aligned intervals)
//   - #443 String Compression [done] (run-length encoding construction)
//   - #900 RLE Iterator [new] (consuming elements from a run-length encoded stream)
//   - #1213 Intersection of Three Sorted Arrays [done] (synchronized multi-pointer traversal)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimal run-length encoded element-wise product of `encoded1`
  // and `encoded2` without expanding either array or mutating the inputs.
  //
  // Invariant: The total consumed length from `encoded1` equals the total
  // consumed length from `encoded2`, and `result` contains no two adjacent
  // segments with the same value.
  // Time: O(M + N), Space: O(1) auxiliary.
  std::vector<std::vector<int>> findRLEArray(
      const std::vector<std::vector<int>>& encoded1,
      const std::vector<std::vector<int>>& encoded2) {
    if (encoded1.empty() || encoded2.empty()) {
      return {};
    }

    std::size_t idx1 = 0uz;
    std::size_t idx2 = 0uz;
    int rem1 = encoded1[0][1];
    int rem2 = encoded2[0][1];
    std::vector<std::vector<int>> result;

    while (idx1 < encoded1.size() && idx2 < encoded2.size()) {
      const int prod = encoded1[idx1][0] * encoded2[idx2][0];
      const int overlap = std::min(rem1, rem2);

      // Coalesce consecutive segments that produce the same value so the
      // returned RLE representation has minimum possible length.
      if (!result.empty() && result.back()[0] == prod) {
        result.back()[1] += overlap;
      } else {
        result.push_back({prod, overlap});
      }

      rem1 -= overlap;
      rem2 -= overlap;

      // Advance whichever segment(s) just ran out of remaining elements.
      if (rem1 == 0) {
        ++idx1;
        if (idx1 < encoded1.size()) {
          rem1 = encoded1[idx1][1];
        }
      }
      if (rem2 == 0) {
        ++idx2;
        if (idx2 < encoded2.size()) {
          rem2 = encoded2[idx2][1];
        }
      }
    }

    return result;
  }
};

LC_PROBLEM(1868, "Product Of Two Run Length Encoded Arrays", "two_pointers",
           ctx) {
  Solution sol;

  ctx.test("example_1_full_merge", [&] {
    const std::vector<std::vector<int>> encoded1 = {{1, 3}, {2, 3}};
    const std::vector<std::vector<int>> encoded2 = {{6, 3}, {3, 3}};
    const std::vector<std::vector<int>> expected = {{6, 6}};
    ctx.expect_eq(sol.findRLEArray(encoded1, encoded2), expected);
  });

  ctx.test("example_2_partial_splits", [&] {
    const std::vector<std::vector<int>> encoded1 = {{1, 3}, {2, 1}, {3, 2}};
    const std::vector<std::vector<int>> encoded2 = {{2, 3}, {3, 3}};
    const std::vector<std::vector<int>> expected = {{2, 3}, {6, 1}, {9, 2}};
    ctx.expect_eq(sol.findRLEArray(encoded1, encoded2), expected);
  });

  ctx.test("python_build_case", [&] {
    const std::vector<std::vector<int>> encoded1 = {{1, 2}, {2, 4}};
    const std::vector<std::vector<int>> encoded2 = {{6, 3}, {3, 3}};
    const std::vector<std::vector<int>> expected = {{6, 2}, {12, 1}, {6, 3}};
    ctx.expect_eq(sol.findRLEArray(encoded1, encoded2), expected);
  });

  ctx.test("multi_way_merge_across_boundaries", [&] {
    const std::vector<std::vector<int>> encoded1 = {{2, 2}, {4, 2}, {1, 2}};
    const std::vector<std::vector<int>> encoded2 = {{4, 2}, {2, 2}, {8, 2}};
    const std::vector<std::vector<int>> expected = {{8, 6}};
    ctx.expect_eq(sol.findRLEArray(encoded1, encoded2), expected);
  });

  ctx.test("single_segment_each", [&] {
    const std::vector<std::vector<int>> encoded1 = {{7, 5}};
    const std::vector<std::vector<int>> encoded2 = {{3, 5}};
    const std::vector<std::vector<int>> expected = {{21, 5}};
    ctx.expect_eq(sol.findRLEArray(encoded1, encoded2), expected);
  });
}

}  // namespace
