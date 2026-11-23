// LeetCode 1578: Minimum Deletion Cost To Avoid Repeating Letters
// https://leetcode.com/problems/minimum-deletion-cost-to-avoid-repeating-letters/
//
// Problem:
//   Given a string `s` of lowercase English letters and an integer array `cost`
//   of the same length where `cost[i]` is the cost of deleting `s[i]`, return
//   the minimum total cost to delete characters so that no two adjacent
//   characters in the remaining string are identical.
//   Constraints: s.length == cost.length, 1 <= s.length <= 10^5,
//   1 <= cost[i] <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  s = "abaac", cost = [1, 2, 3, 4, 5]
//     Output: 3
//     Explanation:
//       - Initialize total_cost = 0 and max_in_run = cost[0] = 1, then reset
//         max_in_run = 2 at i = 1 ('b') and max_in_run = 3 at i = 2 ('a') because
//         s[i] != s[i - 1].
//       - At i = 3, s[3] == s[2] == 'a', so pay min(3, 4) = 3 (total_cost = 3) and
//         update max_in_run = max(3, 4) = 4.
//       - At i = 4 ('c'), reset max_in_run = 5 and return total_cost = 3.
//
//   Example 2:
//     Input:  s = "aabaa", cost = [1, 2, 3, 4, 1]
//     Output: 2
//     Explanation:
//       - Start with total_cost = 0 and max_in_run = cost[0] = 1; at i = 1 ('a' ==
//         'a'), add min(1, 2) = 1 to total_cost (1) and set max_in_run = 2.
//       - Reset max_in_run = 3 at i = 2 ('b') and max_in_run = 4 at i = 3 ('a') as
//         each starts a new character run.
//       - At i = 4 ('a' == 'a'), add min(4, 1) = 1 to total_cost (2), keep
//         max_in_run = 4, and return 2.
//
//   Example 3:
//     Input:  s = "aaaaa", cost = [5, 1, 4, 2, 3]
//     Output: 10
//     Explanation:
//       - Initialize total_cost = 0 and max_in_run = cost[0] = 5 for the first 'a'.
//       - At i = 1 and i = 2, s[i] == 'a', so add min(5, 1) = 1 and min(5, 4) = 4
//         (total_cost = 5) while max_in_run remains 5.
//       - At i = 3 and i = 4, add min(5, 2) = 2 and min(5, 3) = 3, returning
//         total_cost = 10 while retaining cost[0] = 5.
//
// Algorithm:
//   Partition `s` conceptually into maximal contiguous runs of identical
//   characters. Within any run of length k, at most one character may survive
//   (otherwise two identical characters remain adjacent), and keeping exactly
//   one character is always optimal because deleting all of them only adds
//   non-negative cost without helping neighboring runs (which hold different
//   letters). Thus, for each run we keep the single character with maximum
//   deletion cost and delete all others.
//   This runs in a single greedy O(1)-space pass: track `max_in_run`, the
//   highest cost seen in the current run. When `s[i] == s[i - 1]`, pay
//   `min(max_in_run, cost[i])` to delete the cheaper of the current survivor
//   and `s[i]`, and keep `max(max_in_run, cost[i])` as the new survivor. When
//   the character changes, reset `max_in_run = cost[i]`.
//   Time Complexity:  O(n), single linear scan.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two scalar accumulators (`total_cost` and `max_in_run`) operating directly
//     over `std::string_view` and `const std::vector<int>&`.
//
// Real-World Applications:
//   - Deduplicating consecutive telemetry / log alerts while preserving the
//     highest-priority or highest-confidence event in each burst.
//   - Video frame deduplication / keyframe selection (retaining the sharpest
//     frame within each static scene run).
//   - De-bouncing consecutive duplicate network packets or UI events while
//     retaining the highest-value payload.
//
// Related LeetCode Problems:
//   - #1047 Remove All Adjacent Duplicates In String [done] (removes adjacent duplicates iteratively using a stack)
//   - #1209 Remove All Adjacent Duplicates In String II [done] (removes k consecutive identical characters)
//   - #1758 Minimum Changes To Make Alternating Binary String [new] (minimum edits to avoid adjacent equal characters)
//   - #1839 Longest Substring Of All Vowels In Order [new] (single-pass run-length grouping over strings)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum deletion cost so no two adjacent characters in `s` are
  // identical. Keeps the highest-cost character in every contiguous run.
  // Time: O(n), Space: O(1).
  int minCost(std::string_view s, const std::vector<int>& cost) {
    if (s.size() <= 1uz) {
      return 0;
    }

    int total_cost = 0;
    // Highest deletion cost among characters seen in the current identical run
    // (the candidate we retain so far).
    int max_in_run = cost[0];

    for (std::size_t i = 1uz; i < s.size(); ++i) {
      if (s[i] == s[i - 1uz]) {
        // One of the current survivor and s[i] must be deleted; greedily delete
        // the cheaper one and preserve the costlier one for future comparisons.
        total_cost += std::min(max_in_run, cost[i]);
        max_in_run = std::max(max_in_run, cost[i]);
      } else {
        // Start of a new distinct-character run.
        max_in_run = cost[i];
      }
    }

    return total_cost;
  }
};

LC_PROBLEM(1578, "Minimum Deletion Cost To Avoid Repeating Letters", "sort",
           ctx) {
  Solution sol;

  ctx.test("example1_single_duplicate_pair", [&] {
    ctx.expect_eq(sol.minCost("abaac", {1, 2, 3, 4, 5}), 3);
  });

  ctx.test("example2_no_adjacent_repeats", [&] {
    ctx.expect_eq(sol.minCost("abc", {1, 2, 3}), 0);
  });

  ctx.test("example3_multiple_runs", [&] {
    ctx.expect_eq(sol.minCost("aabaa", {1, 2, 3, 4, 1}), 2);
  });

  ctx.test("all_identical_characters", [&] {
    // Keep cost 5, delete 1 + 4 + 2 + 3 = 10.
    ctx.expect_eq(sol.minCost("aaaaa", {5, 1, 4, 2, 3}), 10);
  });

  ctx.test("equal_costs_and_single_char", [&] {
    ctx.expect_eq(sol.minCost("bbbb", {3, 3, 3, 3}), 9);
    ctx.expect_eq(sol.minCost("z", {10}), 0);
  });
}

}  // namespace
