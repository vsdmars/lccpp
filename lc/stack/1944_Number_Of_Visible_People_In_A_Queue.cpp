// LeetCode 1944: Number Of Visible People In A Queue
// https://leetcode.com/problems/number-of-visible-people-in-a-queue/
//
// Problem:
//   There are `n` people standing left-to-right in a queue with distinct
//   heights `heights[0..n-1]`. Person `i` can see person `j` (`i < j`) if
//   every person strictly between them is shorter than both `heights[i]` and
//   `heights[j]`. Return an array `answer` of length `n` where `answer[i]` is
//   the number of people to the right that person `i` can see.
//
// Algorithm:
//   Left-to-Right Monotonic Decreasing Stack:
//     Maintain a stack of indices of people whose rightward view has not yet
//     been blocked by a taller person. Because any person shorter than a
//     subsequent person is popped immediately, the heights of indices on the
//     stack remain strictly decreasing from bottom to top.
//     When processing person `i` with height `heights[i]`:
//       1. While the stack top is shorter than or equal to `heights[i]`, that
//          earlier person can see person `i`, AND person `i` permanently blocks
//          their view of everyone further right — increment their visible count
//          and pop them.
//       2. If the stack is still non-empty after the loop, the remaining top
//          person is strictly taller than person `i` (and everyone between them
//          was already popped because they were shorter than `heights[i]`).
//          Therefore, the stack top can also see person `i` — increment their
//          visible count without popping them.
//       3. Push `i` onto the stack.
//   Time Complexity:  O(n) — each person is pushed once and popped at most once.
//   Space Complexity: O(n) for the monotonic stack and output array.
//
// Data Structures:
//   - Strictly decreasing monotonic stack (`std::vector<std::size_t>`): holds
//     indices of unresolved people awaiting a blocking taller person to their
//     right.
//   - Result vector (`std::vector<int>`): accumulates visibility counts.
//
// Real-World Applications:
//   - Wireless line-of-sight and antenna / relay visibility planning along a
//     linear elevation profile.
//   - Visibility graphs in computational geometry and 2D robot motion planning.
//   - Stock price span and financial technical-indicator breakout analysis.
//
// Related LeetCode Problems:
//   - #1762 Buildings With An Ocean View [done] (unobstructed view to right end)
//   - #739 Daily Temperatures [done] (next greater element via monotonic stack)
//   - #84 Largest Rectangle in Histogram [done] (monotonic stack span bounds)
//   - #901 Online Stock Span [new] (consecutive shorter elements via monotonic stack)
//   - #2282 Number of People That Can Be Seen in a Grid [new] (2D grid extension)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes how many people to the right each person in the queue can see.
  // Time: O(n), Space: O(n).
  std::vector<int> canSeePersonsCount(const std::vector<int>& heights) {
    const std::size_t n = heights.size();
    std::vector<int> visible(n, 0);

    // Monotonic decreasing stack storing indices of people whose rightward view
    // is still open.
    std::vector<std::size_t> stack;
    stack.reserve(n);

    for (std::size_t i = 0; i < n; ++i) {
      // Every stacked person shorter than (or equal to) heights[i] can see
      // person i, and is thereafter blocked from seeing anyone beyond i.
      while (!stack.empty() && heights[stack.back()] <= heights[i]) {
        ++visible[stack.back()];
        stack.pop_back();
      }

      // If a taller person remains on the stack, nobody between them and i was
      // taller than heights[i], so that person can also see person i.
      if (!stack.empty()) {
        ++visible[stack.back()];
      }

      stack.push_back(i);
    }

    return visible;
  }
};

LC_PROBLEM(1944, "Number Of Visible People In A Queue", "stack", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.canSeePersonsCount({10, 6, 8, 5, 11, 9}),
                  std::vector<int>({3, 1, 2, 1, 1, 0}));
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.canSeePersonsCount({5, 1, 2, 3, 10}),
                  std::vector<int>({4, 1, 1, 1, 0}));
  });

  ctx.test("python_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.canSeePersonsCount({7, 6, 5, 4, 9}),
                  std::vector<int>({2, 2, 2, 1, 0}));
  });

  ctx.test("strictly_increasing_and_decreasing", [&] {
    Solution sol;
    // In a strictly increasing queue, each person is immediately blocked by
    // their right neighbor.
    ctx.expect_eq(sol.canSeePersonsCount({1, 2, 3, 4}),
                  std::vector<int>({1, 1, 1, 0}));
    // In a strictly decreasing queue, each person only sees their immediate
    // neighbor (who blocks everyone shorter behind them).
    ctx.expect_eq(sol.canSeePersonsCount({4, 3, 2, 1}),
                  std::vector<int>({1, 1, 1, 0}));
  });

  ctx.test("single_person", [&] {
    Solution sol;
    ctx.expect_eq(sol.canSeePersonsCount({42}), std::vector<int>({0}));
  });
}

}  // namespace
