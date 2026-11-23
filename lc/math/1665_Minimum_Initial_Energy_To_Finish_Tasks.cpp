// LeetCode 1665: Minimum Initial Energy To Finish Tasks
// https://leetcode.com/problems/minimum-initial-energy-to-finish-tasks/
//
// Problem:
//   Given an array `tasks` where `tasks[i] = [actual_i, minimum_i]`, starting
//   task `i` requires having at least `minimum_i` current energy, and finishing
//   it consumes `actual_i` energy (where `actual_i <= minimum_i`). Tasks may be
//   completed in any order. Return the minimum initial energy needed to finish
//   all tasks.
//   Constraints: 1 <= tasks.length <= 10^5, 1 <= actual_i <= minimum_i <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  tasks = [[1, 2], [2, 4], [4, 8]]
//     Output: 8
//     Explanation:
//       - Compute buffers (minimum - actual) as 1 for [1, 2], 2 for [2, 4], and 4
//         for [4, 8], then sort ascending into [[1, 2], [2, 4], [4, 8]].
//       - Initialize required = 0 and process [1, 2]: required = max(0 + 1, 2) = 2.
//       - Process [2, 4]: required = max(2 + 2, 4) = 4.
//       - Process [4, 8]: required = max(4 + 4, 8) = 8, so the function returns 8.
//
//   Example 2:
//     Input:  tasks = [[1, 3], [2, 4], [10, 11], [10, 12], [8, 9]]
//     Output: 32
//     Explanation:
//       - Compute buffers (minimum - actual): [10, 11] and [8, 9] have buffer 1,
//         while [1, 3], [2, 4], and [10, 12] have buffer 2, so buffer-1 tasks are
//         sorted before buffer-2 tasks.
//       - Process the two buffer-1 tasks from required = 0: regardless of tie
//         order, required becomes max(max(0 + 10, 11) + 8, 9) = 19 (or max(max(0 +
//         8, 9) + 10, 11) = 19).
//       - Process the three buffer-2 tasks [1, 3], [2, 4], and [10, 12] in any tie
//         order: since required >= 19 exceeds every task's minimum, each update
//         adds actual (19 + 1 + 2 + 10 = 32).
//       - Return the final accumulated required energy of 32.
//
//   Example 3:
//     Input:  tasks = [[3, 3], [4, 4], [5, 5]]
//     Output: 12
//     Explanation:
//       - All three tasks have buffer (minimum - actual) = 0, so sorting leaves
//         them with equal key 0.
//       - Starting from required = 0, processing [3, 3], [4, 4], and [5, 5] updates
//         required via max(0 + 3, 3) = 3, max(3 + 4, 4) = 7, and max(7 + 5, 5) =
//         12.
//       - Return 12, which equals the sum of actual energy costs when every task
//         has zero buffer.
//
// Algorithm:
//   Greedy ordering via an exchange argument. Suppose we execute task A before
//   task B. Starting energy required for (A, B) is `max(mA, aA + mB)`, whereas
//   for (B, A) it is `max(mB, aB + mA)`. Because `mA >= aA` and `mB >= aB`,
//   (A, B) is no worse than (B, A) iff `aA + mB <= aB + mA`, i.e.,
//   `mA - aA >= mB - aB`. Thus we should execute tasks in descending order of
//   their surplus/buffer `minimum - actual`.
//   Equivalently, we can sort tasks ascending by `minimum - actual` (reverse
//   execution order) and accumulate the minimum energy `required` needed right
//   before each task: to complete all subsequent tasks needing `required`
//   energy after spending `actual`, we need at least `required + actual`, and
//   to start this task we also need at least `minimum`, giving
//   `required = max(required + actual, minimum)`.
//   Time Complexity:  O(n log n) for sorting.
//   Space Complexity: O(n) for the compact copy sorted in place.
//
// Data Structures:
//   - `std::vector<Task>`: flat contiguous array of `{actual, minimum}` structs
//     avoiding nested vector indirection during sorting.
//
// Real-World Applications:
//   - Job scheduling with peak memory/credit preconditions and post-execution
//     consumption (e.g., cloud batch pipelines and CI/CD runners).
//   - Capital / cash-flow sequencing where projects require a minimum balance
//     threshold before commitment and expend a smaller direct cost.
//   - Battery-budgeted robotics and UAV mission planning with safety reserves.
//
// Related LeetCode Problems:
//   - #134 Gas Station [done] (greedy circular energy feasibility check)
//   - #630 Course Schedule III [new] (greedy task ordering with deadline constraints)
//   - #1029 Two City Scheduling [done] (greedy sorting by cost-difference key)
//   - #1834 Single-Threaded CPU [done] (task scheduling by processing criteria)

#include "framework/lc_framework.h"

#include <algorithm>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum starting energy needed to complete all tasks in an
  // optimal order.
  // Sorts tasks ascending by `(minimum - actual)` (reverse execution order)
  // and propagates the required pre-task energy backward.
  // Time: O(n log n), Space: O(n).
  int minimumEffort(const std::vector<std::vector<int>>& tasks) {
    struct Task {
      int actual = 0;
      int minimum = 0;
    };

    // Copy into a flat struct array so sorting avoids nested vector moves.
    std::vector<Task> ordered;
    ordered.reserve(tasks.size());
    for (const auto& task : tasks) {
      ordered.push_back({.actual = task[0], .minimum = task[1]});
    }

    // Sort ascending by buffer (minimum - actual), which corresponds to
    // walking the optimal execution schedule from the last task to the first.
    std::ranges::sort(ordered, [](const Task& lhs, const Task& rhs) {
      return (lhs.minimum - lhs.actual) < (rhs.minimum - rhs.actual);
    });

    // `required` is the minimum energy needed to complete all tasks processed
    // so far (the suffix of the actual execution order).
    int required = 0;
    for (const Task& task : ordered) {
      // To execute this task before the suffix, we must enter with enough
      // energy to pay `actual` and still have `required` left, while also
      // satisfying this task's entry threshold `minimum`.
      required = std::max(required + task.actual, task.minimum);
    }

    return required;
  }
};

LC_PROBLEM(1665, "Minimum Initial Energy To Finish Tasks", "math", ctx) {
  ctx.test("example_1_build", [&] {
    Solution sol;
    ctx.expect_eq(sol.minimumEffort({{1, 2}, {2, 4}, {4, 8}}), 8);
  });

  ctx.test("example_2_mixed_buffers", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.minimumEffort({{1, 3}, {2, 4}, {10, 11}, {10, 12}, {8, 9}}), 32);
  });

  ctx.test("example_3_equal_buffers", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.minimumEffort(
            {{1, 7}, {2, 8}, {3, 9}, {4, 10}, {5, 11}, {6, 12}}),
        27);
  });

  ctx.test("single_task", [&] {
    Solution sol;
    ctx.expect_eq(sol.minimumEffort({{5, 10}}), 10);
  });

  ctx.test("zero_buffer_tasks", [&] {
    Solution sol;
    // When actual == minimum for every task, initial energy is just the sum.
    ctx.expect_eq(sol.minimumEffort({{3, 3}, {4, 4}, {5, 5}}), 12);
  });
}

}  // namespace
