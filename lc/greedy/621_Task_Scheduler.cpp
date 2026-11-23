// LeetCode 621: Task Scheduler
// https://leetcode.com/problems/task-scheduler/
//
// Problem:
//   Given a list of CPU tasks labeled with uppercase English letters 'A'-'Z'
//   and a non-negative cooldown interval `n` (meaning any two executions of the
//   same task must be separated by at least `n` intervals, during which the CPU
//   may execute other tasks or stay idle), return the minimum total number of
//   intervals required to complete all tasks.
//
// Algorithm:
//   Use the greedy frame-construction counting formula:
//     1. Tally the frequency of each task letter 'A'-'Z' in a 26-element array.
//     2. Find the maximum frequency `max_freq` and the number of distinct tasks
//        `max_count` that occur `max_freq` times.
//     3. Anchor the most frequent tasks into `max_freq - 1` full frames of
//        width `n + 1` (each starting with those `max_count` tasks followed by
//        remaining slots), plus one final partial frame holding the last
//        occurrence of those `max_count` tasks:
//            frame_intervals = ((max_freq - 1) * (n + 1)) + max_count.
//     4. All less-frequent tasks can be distributed round-robin into the open
//        frame slots without ever violating the cooldown `n`. If the total
//        number of tasks exceeds `frame_intervals`, the frames simply expand
//        with zero idle time, so the answer is max(tasks.size(),
//        frame_intervals).
//
//   Time Complexity:  O(m + Sigma) = O(m), where m = tasks.size() and
//                     Sigma = 26.
//   Space Complexity: O(Sigma) = O(1) for the fixed 26-slot frequency table.
//
// Data Structures:
//   - std::array<int, 26> fixed-size frequency histogram for uppercase task
//     labels.
//
// Real-World Applications:
//   - Operating-system CPU and GPU warp scheduling under per-resource cooldown
//     or thermal-throttling constraints.
//   - Rate-limited API job dispatchers where repeated calls to the same
//     endpoint or tenant require a minimum spacing window.
//   - Instruction scheduling in pipelined compilers to hide memory/ALU
//     latencies by interleaving independent instructions.
//
// Related LeetCode Problems:
//   - #358 Rearrange String k Distance Apart [done] (builds an explicit valid schedule via max-heap + cooldown queue)
//   - #767 Reorganize String [done] (special case of #358 with k = 2)
//   - #2365 Task Scheduler II [new] (fixed task order with per-task cooldown tracking via hash map)
//   - #1953 Maximum Number of Weeks for Which You Can Work [new] (max-frequency bottleneck analysis without idle slots)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Returns the minimum number of CPU intervals needed to execute all `tasks`
  // with cooldown `n` between identical tasks.
  // Time: O(tasks.size()), Space: O(1).
  int leastInterval(const std::vector<char>& tasks, int n) {
    if (tasks.empty()) {
      return 0;
    }

    // Count occurrences of each uppercase task label 'A'..'Z'.
    std::array<int, kAlphabetSize> freq{};
    for (const char task : tasks) {
      const auto idx = static_cast<std::size_t>(
          static_cast<unsigned char>(task) - static_cast<unsigned char>('A'));
      ++freq[idx];
    }

    // Determine the bottleneck frequency and how many tasks tie for it.
    const int max_freq = std::ranges::max(freq);
    const auto max_count = static_cast<int>(std::ranges::count(freq, max_freq));

    // (max_freq - 1) full frames of length (n + 1), plus the final tail of
    // tasks that appear `max_freq` times.
    const int frame_intervals = ((max_freq - 1) * (n + 1)) + max_count;

    // If total tasks exceed the frame skeleton, all idle slots are eliminated.
    return std::max(static_cast<int>(tasks.size()), frame_intervals);
  }
};

LC_PROBLEM(621, "Task Scheduler", "greedy", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<char> tasks = {'A', 'A', 'A', 'B', 'B', 'B'};
    ctx.expect_eq(sol.leastInterval(tasks, 2), 8);
  });

  ctx.test("zero_cooldown", [&] {
    Solution sol;
    const std::vector<char> tasks = {'A', 'A', 'A', 'B', 'B', 'B'};
    ctx.expect_eq(sol.leastInterval(tasks, 0), 6);
  });

  ctx.test("idle_slots_completely_filled", [&] {
    Solution sol;
    // AAAA BBBB CCCC DDD EEE FF GG HH JJ KK (28 tasks), n = 6 ->
    // frame = 3 * 7 + 3 = 24 < 28, so total task count dominates.
    const std::vector<char> tasks = {
        'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D',
        'D', 'E', 'E', 'E', 'F', 'F', 'G', 'G', 'H', 'H', 'J', 'J', 'K', 'K'};
    ctx.expect_eq(sol.leastInterval(tasks, 6), 28);
  });

  ctx.test("single_dominant_task", [&] {
    Solution sol;
    const std::vector<char> tasks = {'A', 'A', 'A', 'A', 'A', 'A',
                                     'B', 'C', 'D', 'E', 'F', 'G'};
    ctx.expect_eq(sol.leastInterval(tasks, 2), 16);
  });

  ctx.test("single_task", [&] {
    Solution sol;
    ctx.expect_eq(sol.leastInterval({'Z'}, 100), 1);
  });
}

}  // namespace
