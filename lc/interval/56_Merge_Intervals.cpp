// https://leetcode.com/problems/merge-intervals/description/
//
// Given an array of `intervals` where `intervals[i] = [start_i, end_i]`, merge
// all overlapping intervals and return an array of the non-overlapping
// intervals that cover all the intervals in the input.

#include "framework/lc_framework.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Sort intervals by start (then end), then greedily merge overlapping
  // intervals onto a result stack.
  // Time: O(N log N), Space: O(N) for the output.
  std::vector<std::vector<int>> merge(
      std::vector<std::vector<int>> intervals) {
    std::ranges::sort(intervals);

    std::vector<std::vector<int>> merged;
    merged.reserve(intervals.size());

    for (auto& interval : intervals) {
      if (!merged.empty() && interval[0] <= merged.back()[1]) {
        merged.back()[1] = std::max(merged.back()[1], interval[1]);
        continue;
      }
      merged.push_back(std::move(interval));
    }

    return merged;
  }
};

LC_PROBLEM(56, "Merge Intervals", "interval", ctx) {
  ctx.test("leetcode_example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {
        {1, 6}, {8, 10}, {15, 18}};
    ctx.expect_eq(sol.merge({{1, 3}, {2, 6}, {8, 10}, {15, 18}}), expected);
  });

  ctx.test("touching_boundaries", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{1, 5}};
    ctx.expect_eq(sol.merge({{1, 4}, {4, 5}}), expected);
  });

  ctx.test("unsorted_and_nested", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{1, 10}};
    ctx.expect_eq(sol.merge({{4, 8}, {1, 10}, {2, 3}}), expected);
  });

  ctx.test("empty_and_single", [&] {
    Solution sol;
    ctx.expect_eq(sol.merge({}), std::vector<std::vector<int>>{});
    ctx.expect_eq(sol.merge({{3, 7}}), std::vector<std::vector<int>>{{3, 7}});
  });

  ctx.test("negative_coordinates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{-3, 2}, {5, 6}};
    ctx.expect_eq(sol.merge({{5, 6}, {-1, 2}, {-3, 0}}), expected);
  });
}

}  // namespace
