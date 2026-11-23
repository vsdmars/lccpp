// LeetCode 1762: Buildings With An Ocean View
// https://leetcode.com/problems/buildings-with-an-ocean-view/
//
// Problem:
//   Given an integer array `heights` of `n` buildings standing in a line with
//   the ocean to the right, a building at index `i` has an ocean view if every
//   building to its right (`j > i`) is strictly shorter (`heights[j] <
//   heights[i]`). Return the 0-indexed list of all buildings with an ocean view
//   in strictly increasing index order.
//
// Examples:
//   Example 1:
//     Input:  heights = [4, 2, 3, 1]
//     Output: [0, 2, 3]
//     Explanation:
//       - Scan right-to-left starting with max_height_right = INT_MIN: index 3
//         (height 1 > INT_MIN) appends 3 and sets max_height_right = 1.
//       - At index 2 (height 3 > 1), append 2 and update max_height_right = 3; at
//         index 1 (height 2 <= 3), skip because building 2 blocks its view.
//       - At index 0 (height 4 > 3), append 0 and update max_height_right = 4, then
//         reverse result from [3, 2, 0] to [0, 2, 3].
//
//   Example 2:
//     Input:  heights = [1, 3, 2, 4]
//     Output: [3]
//     Explanation:
//       - At the rightmost index 3 (height 4 > INT_MIN), append 3 to result and set
//         max_height_right = 4.
//       - Scanning indices 2 (height 2), 1 (height 3), and 0 (height 1), none
//         satisfies heights[idx] > 4, so all are skipped.
//       - Reverse result ([3]) and return [3].
//
//   Example 3:
//     Input:  heights = [2, 2, 2, 2]
//     Output: [3]
//     Explanation:
//       - At index 3 (height 2 > INT_MIN), append 3 and set max_height_right = 2.
//       - At indices 2, 1, and 0, each building has height 2, which is not strictly
//         greater than max_height_right (2), so none is added.
//       - Return the reversed result [3].
//
// Algorithm:
//   Primary (Right-to-Left Running Maximum):
//     Scan from right to left while maintaining `max_height_right`, the tallest
//     building encountered so far to the right. Any building strictly taller
//     than `max_height_right` has an unobstructed ocean view: record its index
//     and update `max_height_right`. Because indices are discovered in
//     descending order, reverse the collected result at the end.
//     Time Complexity:  O(n).
//     Space Complexity: O(1) auxiliary space (excluding the output vector).
//
//   Alternative (Left-to-Right Monotonic Decreasing Stack):
//     Scan from left to right maintaining a stack of building indices whose
//     heights are strictly decreasing. Before pushing index `i`, pop any
//     earlier building whose height is `<= heights[i]` (its view is now
//     blocked). At the end, the stack directly holds all ocean-view indices in
//     increasing order without needing a reversal.
//     Time Complexity:  O(n).
//     Space Complexity: O(n).
//
// Data Structures:
//   - Running suffix maximum scalar (`max_height_right`) + output vector
//     (`std::vector<int>`).
//   - Strictly decreasing monotonic stack (`std::vector<int>`) in the
//     left-to-right streaming variant.
//
// Real-World Applications:
//   - Skyline / Pareto frontier filtering in multi-criteria optimization and
//     database skyline queries.
//   - Line-of-sight and terrain visibility analysis in GIS, radar placement,
//     and 2D/3D graphics occlusion culling.
//   - Streaming leader / record-high detection in time-series monitoring.
//
// Related LeetCode Problems:
//   - #1944 Number Of Visible People In A Queue [done] (monotonic stack visibility counts)
//   - #739 Daily Temperatures [done] (next greater element via monotonic stack)
//   - #496 Next Greater Element I [done] (finding first blocking element to the right)
//   - #42 Trapping Rain Water [done] (prefix/suffix max height envelopes)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all buildings with an ocean view via a right-to-left suffix maximum
  // scan.
  // Time: O(n), Auxiliary Space: O(1) (excluding output).
  std::vector<int> findBuildings(const std::vector<int>& heights) {
    std::vector<int> result;
    int max_height_right = std::numeric_limits<int>::min();

    // Traverse from the ocean (rightmost building) toward the left.
    for (std::size_t i = heights.size(); i > 0; --i) {
      const std::size_t idx = i - 1uz;
      if (heights[idx] > max_height_right) {
        max_height_right = heights[idx];
        result.push_back(static_cast<int>(idx));
      }
    }

    // Indices were appended from right to left; reverse into ascending order.
    std::ranges::reverse(result);
    return result;
  }

  // Left-to-right monotonic decreasing stack alternative (ideal when heights
  // arrive as an online stream).
  // Time: O(n), Space: O(n).
  std::vector<int> findBuildingsMonotonicStack(
      const std::vector<int>& heights) {
    std::vector<int> stack;
    for (std::size_t i = 0; i < heights.size(); ++i) {
      // Any earlier building shorter than or equal to heights[i] is blocked.
      while (!stack.empty() &&
             heights[static_cast<std::size_t>(stack.back())] <= heights[i]) {
        stack.pop_back();
      }
      stack.push_back(static_cast<int>(i));
    }
    return stack;
  }
};

LC_PROBLEM(1762, "Buildings With An Ocean View", "stack", ctx) {
  auto verify_both = [&](const std::vector<int>& heights,
                         const std::vector<int>& expected) {
    Solution sol;
    ctx.expect_eq(sol.findBuildings(heights), expected);
    ctx.expect_eq(sol.findBuildingsMonotonicStack(heights), expected);
  };

  ctx.test("example_1", [&] { verify_both({4, 2, 3, 1}, {0, 2, 3}); });

  ctx.test("example_2_strictly_decreasing",
           [&] { verify_both({4, 3, 2, 1}, {0, 1, 2, 3}); });

  ctx.test("example_3_tallest_at_end", [&] { verify_both({1, 3, 2, 4}, {3}); });

  ctx.test("example_4_all_equal", [&] { verify_both({2, 2, 2, 2}, {3}); });

  ctx.test("python_build_case", [&] { verify_both({8, 4, 7, 1}, {0, 2, 3}); });

  ctx.test("single_and_empty", [&] {
    verify_both({5}, {0});
    verify_both({}, {});
  });
}

}  // namespace
