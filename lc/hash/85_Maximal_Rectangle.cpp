// LeetCode 85: Maximal Rectangle
// https://leetcode.com/problems/maximal-rectangle/
//
// Problem:
//   Given a 2D binary matrix of dimensions `rows x cols` filled with '0' and
//   '1' characters, find the largest axis-aligned rectangle containing only
//   '1's and return its area.
//   Constraints: 1 <= rows, cols <= 200; matrix[r][c] is '0' or '1'.
//
// Examples:
//   Example 1:
//     Input:  matrix = [["1", "0", "1", "0", "0"], ["1", "0", "1", "1", "1"], ["1",
//             "1", "1", "1", "1"], ["1", "0", "0", "1", "0"]]
//     Output: 6
//     Explanation:
//       - Row 0 builds heights = [1, 0, 1, 0, 0] (plus sentinel 0 at index 5) with
//         best area 1; row 1 updates heights = [2, 0, 2, 1, 1] with best area 3
//         across columns 2..4.
//       - Row 2 updates heights = [3, 1, 3, 2, 2]; when the sentinel c = 5 (height
//         0) is reached, popping column 3 (height 2) with stack top 1 gives width =
//         5 - 1 - 1 = 3 and area = 2 * 3 = 6.
//       - Row 3 updates heights = [4, 0, 0, 3, 0], whose largest rectangle area is
//         4, so max_area = 6 is returned.
//
//   Example 2:
//     Input:  matrix = [["1", "1", "0", "1", "1", "1"]]
//     Output: 3
//     Explanation:
//       - Processing the single row sets heights = [1, 1, 0, 1, 1, 1] followed by
//         the sentinel 0 at index 6.
//       - At c = 2 (height 0), the stack pops columns 1 and 0 of height 1 to record
//         area 2; at sentinel c = 6, popping column 3 above stack top 2 gives width
//         = 6 - 2 - 1 = 3 and best area = 1 * 3 = 3.
//
//   Example 3:
//     Input:  matrix = []
//     Output: 0
//     Explanation:
//       - Check the initial empty-matrix guard matrix.empty() || matrix[0].empty(),
//         which evaluates to true.
//       - Return 0 immediately without allocating heights or stack.
//
// Algorithm:
//   Reduce the 2D problem to `rows` independent instances of Largest Rectangle
//   in Histogram (LeetCode #84). Maintain a 1D array `heights` of length
//   `cols + 1`, where `heights[c]` stores the number of consecutive '1's ending
//   at the current row in column `c` (resetting to 0 on '0'), and the trailing
//   entry `heights[cols]` stays 0 as a sentinel to flush the stack.
//
//   For each row, sweep left-to-right with a monotonic strictly-increasing
//   stack of column indices:
//   - Whenever `heights[c] < heights[stack.back()]`, the popped bar at index
//     `top` has found its first strictly shorter bar to the right (at `c`) and
//     its first strictly shorter bar to the left (the new `stack.back()`, or
//     before column 0 if the stack is now empty).
//   - Its maximal rectangle width is `c` if the stack is empty, or
//     `c - stack.back() - 1` otherwise, giving candidate area `heights[top] * w`.
//   Each column index is pushed and popped at most once per row.
//   Time:  O(rows * cols)
//   Space: O(cols)
//
// Data Structures:
//   - std::vector<int> heights: Running histogram bar heights per column plus a
//     trailing 0 sentinel slot.
//   - std::vector<std::size_t> stack: Monotonic increasing stack of column
//     indices for O(cols) largest-rectangle-in-histogram computation.
//
// Real-World Applications:
//   - Document layout analysis and OCR: locating the largest whitespace or
//     content bounding boxes on a binarized page grid.
//   - VLSI floorplanning and 2D bin-packing: finding the largest contiguous
//     free rectangular region on a chip die or warehouse map.
//   - Computer vision & medical imaging: extracting maximal homogeneous
//     rectangular regions of interest from binary segmentation masks.
//
// Related LeetCode Problems:
//   - #84 Largest Rectangle in Histogram [done] (the 1D monotonic-stack core)
//   - #221 Maximal Square [done] (2D DP variant restricted to square submatrices)
//   - #1504 Count Submatrices With All Ones [new] (counting all 1-rectangles via histogram heights)
//   - #363 Max Sum of Rectangle No Larger Than K [done] (weighted 2D rectangle optimization)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum area of an all-'1' rectangle in `matrix`.
  // Time: O(rows * cols), Space: O(cols).
  int maximalRectangle(const std::vector<std::vector<char>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return 0;
    }

    const std::size_t cols = matrix[0].size();
    // Extra trailing slot stays 0 to force-flush all bars at the end of a row.
    std::vector<int> heights(cols + 1uz, 0);
    std::vector<std::size_t> stack;
    stack.reserve(cols + 1uz);

    int max_area = 0;
    for (const auto& row : matrix) {
      // Update cumulative vertical run of '1's ending at this row.
      for (std::size_t c = 0uz; c < cols; ++c) {
        heights[c] = (row[c] == '1') ? (heights[c] + 1) : 0;
      }
      max_area = std::max(max_area, largestRectangleInRow(heights, stack));
    }

    return max_area;
  }

 private:
  // Evaluates the largest rectangle in a single histogram row using a monotonic
  // increasing stack of column indices. `heights.back()` must be 0 (sentinel).
  // Invariant: for all k in `stack`, heights[stack[k]] is non-decreasing.
  // Time: O(cols), Space: O(cols) reused across rows.
  static int largestRectangleInRow(const std::vector<int>& heights,
                                   std::vector<std::size_t>& stack) {
    stack.clear();
    int best = 0;

    for (std::size_t c = 0uz; c < heights.size(); ++c) {
      // Any bar taller than heights[c] cannot extend past column c - 1.
      while (!stack.empty() && heights[c] < heights[stack.back()]) {
        const int height = heights[stack.back()];
        stack.pop_back();
        // Left boundary is just after the previous bar on the stack (or 0 if
        // the stack is now empty).
        const std::size_t width =
            stack.empty() ? c : ((c - stack.back()) - 1uz);
        const int area = height * static_cast<int>(width);
        best = std::max(best, area);
      }
      stack.push_back(c);
    }

    return best;
  }
};

LC_PROBLEM(85, "Maximal Rectangle", "hash", ctx) {
  Solution sol;

  ctx.test("classic_4x5_example", [&] {
    const std::vector<std::vector<char>> matrix = {
        {'1', '0', '1', '0', '0'},
        {'1', '0', '1', '1', '1'},
        {'1', '1', '1', '1', '1'},
        {'1', '0', '0', '1', '0'},
    };
    ctx.expect_eq(sol.maximalRectangle(matrix), 6);
  });

  ctx.test("single_cells_and_empty", [&] {
    ctx.expect_eq(sol.maximalRectangle({}), 0);
    ctx.expect_eq(sol.maximalRectangle({{}}), 0);
    ctx.expect_eq(sol.maximalRectangle({{'0'}}), 0);
    ctx.expect_eq(sol.maximalRectangle({{'1'}}), 1);
  });

  ctx.test("all_ones_and_all_zeros", [&] {
    const std::vector<std::vector<char>> all_ones = {
        {'1', '1', '1'},
        {'1', '1', '1'},
    };
    ctx.expect_eq(sol.maximalRectangle(all_ones), 6);

    const std::vector<std::vector<char>> all_zeros = {
        {'0', '0'},
        {'0', '0'},
    };
    ctx.expect_eq(sol.maximalRectangle(all_zeros), 0);
  });

  ctx.test("single_row_and_single_col", [&] {
    const std::vector<std::vector<char>> single_row = {
        {'1', '1', '0', '1', '1', '1'}};
    ctx.expect_eq(sol.maximalRectangle(single_row), 3);

    const std::vector<std::vector<char>> single_col = {
        {'1'}, {'1'}, {'1'}, {'0'}, {'1'}};
    ctx.expect_eq(sol.maximalRectangle(single_col), 3);
  });
}

}  // namespace
