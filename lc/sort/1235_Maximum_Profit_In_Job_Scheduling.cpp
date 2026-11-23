// LeetCode 1235: Maximum Profit In Job Scheduling
// https://leetcode.com/problems/maximum-profit-in-job-scheduling/
//
// Problem:
//   We have `n` jobs where job `i` runs over the half-open time interval
//   `[startTime[i], endTime[i])` and yields `profit[i]`. Return the maximum
//   total profit achievable from a subset of non-overlapping jobs. A job that
//   starts at time `X` is allowed to follow a job that ends at time `X`.
//   Constraints: `1 <= n <= 5 * 10^4`, `1 <= startTime[i] < endTime[i] <= 10^9`.
//
// Algorithm:
//   Weighted Interval Scheduling via End-Time Sorting + Binary-Search DP:
//   1. Pair each job into `(start, end, profit)` and sort ascending by `end`.
//   2. Maintain a compressed DP table `dp` of `(end_time, max_profit)` entries
//      initialized with sentinel `(0, 0)`.
//      Invariant: `dp` is strictly increasing in both `end_time` and
//      `max_profit`, storing the Pareto-optimal profit achievable on or before
//      each recorded completion time.
//   3. For each job `(s, e, p)`:
//      - Binary-search `dp` with `std::ranges::upper_bound` for the first entry
//        whose `end_time > s`. Its immediate predecessor gives the maximum
//        profit achievable by time `s` (compatible with starting at `s`).
//      - Let `candidate = prev_profit + p`. Only if `candidate` strictly beats
//        `dp.back().max_profit` do we record it (either updating `dp.back()` in
//        place when `dp.back().end_time == e`, or appending `(e, candidate)`).
//   4. The answer is `dp.back().max_profit`.
//   Time Complexity:  O(n log n) — O(n log n) sort + `n` binary searches.
//   Space Complexity: O(n) for the sorted jobs list and compressed DP table.
//
// Data Structures:
//   - Job array (`std::vector<Job>`): bundles `(start, end, profit)` for
//     cache-friendly end-time sorting.
//   - Monotonic DP vector (`std::vector<DpEntry>`): stores strictly increasing
//     `(end_time, max_profit)` checkpoints supporting O(log n) `upper_bound`.
//
// Real-World Applications:
//   - Cloud compute / batch GPU cluster scheduling to maximize revenue or SLA
//     utility across fixed-window reservation bids on a single machine.
//   - Ad-slot / broadcast airtime auction allocation maximizing total ad value
//     over non-overlapping time intervals.
//   - Autonomous vehicle / satellite downlink task scheduling where contact
//     windows carry distinct scientific priority weights.
//
// Related LeetCode Problems:
//   - #2008 Maximum Earnings From Taxi [new] (weighted interval scheduling on discrete stops)
//   - #1751 Maximum Number of Events That Can Be Attended II [new] (weighted interval scheduling with a count limit k)
//   - #435 Non-overlapping Intervals [done] (unweighted interval scheduling via end-time greedy)
//   - #1353 Maximum Number of Events That Can Be Attended [done] (greedy interval scheduling with min-heap)
//   - #729 My Calendar I [done] (online non-overlapping interval management)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace {

struct Job {
  int start = 0;
  int end = 0;
  int profit = 0;
};

struct DpEntry {
  int end_time = 0;
  int max_profit = 0;
};

class Solution {
 public:
  // Computes the maximum profit from any non-overlapping subset of jobs.
  //
  // Time: O(n log n), Space: O(n).
  int jobScheduling(const std::vector<int>& start_time,
                    const std::vector<int>& end_time,
                    const std::vector<int>& profit) {
    const std::size_t n = start_time.size();
    if (n == 0) {
      return 0;
    }

    // Phase 1: Bundle job attributes and sort ascending by completion time so
    // optimal prefixes are built chronologically.
    std::vector<Job> jobs;
    jobs.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      jobs.push_back(Job{
          .start = start_time[i],
          .end = end_time[i],
          .profit = profit[i],
      });
    }
    std::ranges::sort(jobs, {}, &Job::end);

    // Phase 2: Monotonic DP table storing `(end_time, max_profit)`.
    // Sentinel `(0, 0)` represents taking no jobs before time 0.
    std::vector<DpEntry> dp;
    dp.reserve(n + 1);
    dp.push_back(DpEntry{.end_time = 0, .max_profit = 0});

    for (const auto& job : jobs) {
      // Find the first DP checkpoint ending strictly after `job.start`;
      // its predecessor is the best state finishing at or before `job.start`.
      const auto it = std::ranges::upper_bound(dp, job.start, {},
                                               &DpEntry::end_time);
      const int prev_profit =
          (it == dp.begin()) ? 0 : std::prev(it)->max_profit;
      const int candidate_profit = prev_profit + job.profit;

      // Only record transitions that strictly improve upon the current best
      // profit, maintaining strict monotonicity in `max_profit`.
      if (candidate_profit > dp.back().max_profit) {
        if (dp.back().end_time == job.end) {
          dp.back().max_profit = candidate_profit;
        } else {
          dp.push_back(
              DpEntry{.end_time = job.end, .max_profit = candidate_profit});
        }
      }
    }

    return dp.back().max_profit;
  }
};

LC_PROBLEM(1235, "Maximum Profit In Job Scheduling", "sort", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(
        sol.jobScheduling({1, 2, 3, 3}, {3, 4, 5, 6}, {50, 10, 40, 70}), 120);
  });

  ctx.test("python_build_case", [&] {
    ctx.expect_eq(
        sol.jobScheduling({1, 2, 3, 3}, {3, 4, 5, 6}, {50, 90, 40, 70}), 120);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.jobScheduling({1, 2, 3, 4, 6}, {3, 5, 10, 6, 9},
                                    {20, 20, 100, 70, 60}),
                  150);
  });

  ctx.test("example_3_all_overlapping", [&] {
    ctx.expect_eq(sol.jobScheduling({1, 1, 1}, {2, 3, 4}, {5, 6, 4}), 6);
  });

  ctx.test("same_end_time_jobs", [&] {
    ctx.expect_eq(sol.jobScheduling({1, 2, 5}, {5, 5, 8}, {10, 25, 30}), 55);
  });

  ctx.test("single_job", [&] {
    ctx.expect_eq(sol.jobScheduling({4}, {10}, {42}), 42);
  });
}

}  // namespace
