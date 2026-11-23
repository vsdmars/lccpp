// LeetCode 1834: Single Threaded Cpu
// https://leetcode.com/problems/single-threaded-cpu/
//
// Problem:
//   Given `n` tasks (1 <= n <= 10^5) where `tasks[i] = [enqueueTime_i,
//   processingTime_i]` (1 <= enqueueTime_i, processingTime_i <= 10^9), a
//   single-threaded non-preemptive CPU processes at most one task at a time:
//   - If idle with no available tasks, it stays idle until a task arrives.
//   - If idle with available tasks, it chooses the one with the shortest
//     `processingTime`, breaking ties by smallest original task index `i`.
//   - Once started, a task runs to completion uninterrupted.
//   Return the order of original task indices in which the CPU executes them.
//
// Examples:
//   Example 1:
//     Input:  tasks = [[1, 2], [2, 4], [3, 2], [4, 1]]
//     Output: [0, 2, 3, 1]
//     Explanation:
//       - Sort tasks with original indices by enqueue_time: (1, 2, 0), (2, 4, 1),
//         (3, 2, 2), (4, 1, 3).
//       - Advance current_time from 0 to 1, admit task 0, and execute it:
//         current_time becomes 1 + 2 = 3 and order = [0].
//       - At current_time = 3, admit tasks 1 (4, 1) and 2 (2, 2); pop shortest task
//         (2, 2), advancing current_time to 5 and order = [0, 2].
//       - At current_time = 5, admit task 3 (1, 3) alongside (4, 1); pop (1, 3)
//         (current_time = 6, order = [0, 2, 3]), then pop (4, 1) for [0, 2, 3, 1].
//
//   Example 2:
//     Input:  tasks = [[7, 10], [7, 12], [7, 5], [7, 4], [7, 2]]
//     Output: [4, 3, 2, 0, 1]
//     Explanation:
//       - All five tasks share enqueue_time = 7, so current_time fast-forwards from
//         0 to 7 and admits all five into ready_heap.
//       - ready_heap orders them by processing_time ascending: (2, 4), (4, 3), (5,
//         2), (10, 0), (12, 1).
//       - Popping each shortest available task in turn executes indices 4, 3, 2, 0,
//         1, advancing current_time to 9, 13, 18, 28, and 40.
//
//   Example 3:
//     Input:  tasks = [[1, 2], [100, 3], [100, 3], [100, 1]]
//     Output: [0, 3, 1, 2]
//     Explanation:
//       - Advance current_time to 1, admit and run task 0 (duration 2), leaving
//         ready_heap empty at current_time = 3 with order = [0].
//       - Because ready_heap is empty and current_time = 3 < 100, fast-forward
//         current_time across the idle gap to 100 and admit tasks 1, 2, and 3.
//       - Execute shortest task 3 (duration 1) first for order = [0, 3], then break
//         the processing_time = 3 tie between tasks 1 and 2 by smaller index to
//         produce [0, 3, 1, 2].
//
// Algorithm:
//   Event-Driven Simulation with Release-Time Sort + Shortest-Job-First Heap:
//   1. Pair each task with its original index and sort ascending by
//      `enqueue_time`.
//   2. Track the simulated clock `current_time` as a 64-bit integer
//      (`std::int64_t`) to prevent 32-bit overflow when summing up to 10^5
//      tasks of duration 10^9.
//   3. Repeat until all `n` tasks are scheduled:
//      - If the available min-heap is empty and `current_time` is earlier than
//        the next arriving task, fast-forward `current_time` directly to that
//        task's `enqueue_time`.
//      - Push every task with `enqueue_time <= current_time` into the min-heap
//        ordered by `(processing_time, original_index)`.
//      - Pop the minimum task, advance `current_time += processing_time`, and
//        append its original index to the result.
//   Invariant: Whenever the CPU picks a task at `current_time`, the min-heap
//   contains every unstarted task whose `enqueue_time <= current_time`, so the
//   heap top is always the exact SJF choice with deterministic tie-breaking.
//   Time Complexity:  O(N log N) for sorting and N heap pushes/pops.
//   Space Complexity: O(N) for the sorted task array, min-heap, and result.
//
// Data Structures:
//   - Sorted `std::vector<ScheduledTask>`: delivers tasks in arrival order
//     without modifying the caller's input.
//   - Min-heap (`std::priority_queue<ReadyTask, ..., std::greater<>>`):
//     implements Shortest Job First (SJF) selection ordered by
//     `(processing_time, index)` in O(log N) per operation.
//
// Real-World Applications:
//   - Operating system Shortest-Job-First (SJF) / Shortest-Remaining-Time CPU
//     and I/O job schedulers minimizing average turnaround time.
//   - Single-worker CI/CD build runners and database query queues prioritizing
//     short jobs to prevent head-of-line blocking.
//   - Discrete-event simulation engines advancing simulation clocks across
//     idle periods.
//
// Related LeetCode Problems:
//   - #1882 Process Tasks Using Servers [new] (multi-server event simulation with two min-heaps)
//   - #1353 Maximum Number Of Events That Can Be Attended [done] (release-time sweep + min-heap scheduling)
//   - #2402 Meeting Rooms III [new] (event-driven resource assignment using free/busy heaps)
//   - #621 Task Scheduler [done] (CPU task scheduling with cooldown constraints)
//   - #1057 Campus Bikes [done] (greedy priority-queue dispatch with deterministic tie-breaking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <queue>
#include <vector>

namespace {

// Full task metadata used while sorting by arrival (`enqueue_time`).
struct ScheduledTask {
  int enqueue_time = 0;
  int processing_time = 0;
  int index = 0;

  auto operator<=>(const ScheduledTask&) const = default;
};

// Ready-queue entry ordered by `processing_time` ascending, then `index`.
struct ReadyTask {
  int processing_time = 0;
  int index = 0;

  auto operator<=>(const ReadyTask&) const = default;
};

class Solution {
 public:
  // Simulates the non-preemptive Shortest-Job-First single-threaded CPU and
  // returns the execution order of original task indices.
  //
  // Time Complexity:  O(N log N)
  // Space Complexity: O(N)
  [[nodiscard]] std::vector<int> getOrder(
      const std::vector<std::vector<int>>& tasks) {
    const std::size_t n = tasks.size();
    if (n == 0uz) {
      return {};
    }

    // Phase 1: Attach original indices and sort by enqueue time ascending.
    std::vector<ScheduledTask> sorted_tasks;
    sorted_tasks.reserve(n);
    for (std::size_t i = 0uz; i < n; ++i) {
      sorted_tasks.push_back(ScheduledTask{
          .enqueue_time = tasks[i][0],
          .processing_time = tasks[i][1],
          .index = static_cast<int>(i),
      });
    }
    std::ranges::sort(sorted_tasks);

    // Phase 2: Event-driven SJF simulation with a 64-bit clock to prevent
    // signed 32-bit overflow across up to 10^5 tasks of length 10^9.
    std::priority_queue<ReadyTask, std::vector<ReadyTask>, std::greater<>>
        ready_heap;
    std::vector<int> order;
    order.reserve(n);

    std::size_t next_idx = 0uz;
    std::int64_t current_time = 0;

    while (order.size() < n) {
      // If the CPU is idle and no tasks are waiting, jump directly to the
      // next task's arrival time rather than stepping tick by tick.
      if (ready_heap.empty() && next_idx < n &&
          current_time < sorted_tasks[next_idx].enqueue_time) {
        current_time = sorted_tasks[next_idx].enqueue_time;
      }

      // Admitting all tasks that have arrived by `current_time`.
      while (next_idx < n &&
             sorted_tasks[next_idx].enqueue_time <= current_time) {
        ready_heap.push(ReadyTask{
            .processing_time = sorted_tasks[next_idx].processing_time,
            .index = sorted_tasks[next_idx].index,
        });
        ++next_idx;
      }

      // Execute the shortest available job (tie-broken by smallest index).
      const ReadyTask chosen = ready_heap.top();
      ready_heap.pop();

      current_time += static_cast<std::int64_t>(chosen.processing_time);
      order.push_back(chosen.index);
    }

    return order;
  }
};

LC_PROBLEM(1834, "Single Threaded Cpu", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> tasks = {
        {1, 2}, {2, 4}, {3, 2}, {4, 1}};
    ctx.expect_eq(sol.getOrder(tasks), std::vector<int>{0, 2, 3, 1});
  });

  ctx.test("example_2_simultaneous_arrival", [&] {
    Solution sol;
    const std::vector<std::vector<int>> tasks = {
        {7, 10}, {7, 12}, {7, 5}, {7, 4}, {7, 2}};
    ctx.expect_eq(sol.getOrder(tasks), std::vector<int>{4, 3, 2, 0, 1});
  });

  ctx.test("idle_gap_and_index_tie_break", [&] {
    Solution sol;
    const std::vector<std::vector<int>> tasks = {
        {1, 2}, {100, 3}, {100, 3}, {100, 1}};
    ctx.expect_eq(sol.getOrder(tasks), std::vector<int>{0, 3, 1, 2});
  });

  ctx.test("large_timestamps_require_64bit_clock", [&] {
    Solution sol;
    // Running task 0 advances clock to 1.8e9, admitting tasks 1, 2, 3;
    // subsequent tasks push clock past INT32_MAX (2.14e9) to 3.8e9 + 1.
    const std::vector<std::vector<int>> tasks = {
        {800'000'000, 1'000'000'000},
        {800'000'000, 1'000'000'000},
        {900'000'000, 1'000'000'000},
        {1'000'000'000, 1},
    };
    ctx.expect_eq(sol.getOrder(tasks), std::vector<int>{0, 3, 1, 2});
  });

  ctx.test("single_task_and_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.getOrder({{5, 10}}), std::vector<int>{0});
    ctx.expect_eq(sol.getOrder({}), std::vector<int>{});
  });
}

}  // namespace
