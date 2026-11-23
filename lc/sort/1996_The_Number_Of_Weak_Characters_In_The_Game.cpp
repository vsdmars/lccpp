// LeetCode 1996: The Number Of Weak Characters In The Game
// https://leetcode.com/problems/the-number-of-weak-characters-in-the-game/
//
// Problem:
//   You are given a 2D integer array `properties` where
//   `properties[i] = [attack_i, defense_i]` describes the stats of the i-th
//   character. A character `i` is considered "weak" if there exists another
//   character `j` with strictly greater attack AND strictly greater defense
//   (`attack_j > attack_i` and `defense_j > defense_i`). Return the total
//   number of weak characters.
//   Constraints: 2 <= properties.length <= 10^5,
//   1 <= attack_i, defense_i <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  properties = [[5, 5], [6, 3], [3, 6]]
//     Output: 0
//     Explanation:
//       - Sort by attack ascending and defense descending to [[3, 6], [5, 5], [6,
//         3]], starting with max_defense = INT_MIN and weak_count = 0.
//       - Reverse-sweep visits [6, 3] first: 3 >= INT_MIN, so max_defense updates
//         to 3.
//       - Next visits [5, 5]: 5 >= 3 updates max_defense to 5; then [3, 6]: 6 >= 5
//         updates max_defense to 6.
//       - No defense is strictly below max_defense, so return weak_count = 0.
//
//   Example 2:
//     Input:  properties = [[1, 5], [10, 4], [4, 3]]
//     Output: 1
//     Explanation:
//       - Sort by attack ascending and defense descending to [[1, 5], [4, 3], [10,
//         4]].
//       - Reverse-sweep starts at [10, 4] and sets max_defense = 4 with weak_count
//         = 0.
//       - At [4, 3], defense 3 < max_defense (4), so weak_count increments to 1.
//       - At [1, 5], defense 5 >= 4 updates max_defense to 5, and the function
//         returns 1.
//
//   Example 3:
//     Input:  properties = [[3, 6], [5, 7], [5, 5], [6, 3]]
//     Output: 1
//     Explanation:
//       - Sort by attack ascending and defense descending to [[3, 6], [5, 7], [5,
//         5], [6, 3]] so [5, 7] precedes [5, 5].
//       - Reverse-sweep visits [6, 3] first, setting max_defense = 3.
//       - Next visits [5, 5] before [5, 7], raising max_defense to 5 without
//         falsely marking [5, 5] weak, and then [5, 7] raises max_defense to 7.
//       - Finally [3, 6] has defense 6 < 7, incrementing weak_count to 1 and
//         returning 1.
//
// Algorithm:
//   This is a classic 2D Pareto dominance counting problem solved by sorting
//   and sweeping one dimension while tracking a running maximum in the other:
//   1. Sort `properties` by `attack` ascending, and for equal `attack`, sort by
//      `defense` descending.
//   2. Sweep from right to left (highest attack to lowest attack), maintaining
//      `max_defense`, the maximum defense seen so far.
//   Why tie-breaking `defense` descending is critical:
//     Within a group sharing the same `attack` (e.g., `[5, 7]` before `[5, 5]`),
//     the right-to-left sweep visits `[5, 5]` BEFORE `[5, 7]`. Therefore:
//       - When inspecting `[5, 5]`, `max_defense` only reflects characters with
//         STRICTLY greater attack (`attack > 5`), never `[5, 7]`.
//       - When subsequently inspecting `[5, 7]`, any update contributed by
//         `[5, 5]` satisfies `5 <= 7`, so `7 < max_defense` can only hold if
//         some character with strictly greater attack had defense `> 7`.
//     Hence `defense < max_defense` holds if and only if there exists a
//     character strictly dominating the current one in both dimensions.
//   Time Complexity:  O(n log n) for sorting, O(n) for the reverse sweep.
//   Space Complexity: O(log n) auxiliary stack space for introsort.
//
// Data Structures:
//   - `std::vector<std::vector<int>>`: sorted in place using a two-key
//     comparator, then scanned in reverse via `std::views::reverse`.
//   - Scalar running maximum `max_defense` tracking the Pareto frontier.
//
// Real-World Applications:
//   - Multi-objective optimization / skyline queries in databases (filtering
//     out dominated offers, e.g., hotels or flights strictly worse in both
//     price and rating).
//   - Cloud instance & hardware SKU selection (eliminating configurations
//     dominated in both throughput and cost-efficiency).
//   - Game-balance auditing (detecting strictly outclassed units, cards, or
//     equipment items).
//
// Related LeetCode Problems:
//   - #354 Russian Doll Envelopes [done] (2D strict dominance using the exact same asc/desc tie-break sort)
//   - #1288 Remove Covered Intervals [new] (interval dominance via asc start, desc end sorting)
//   - #1691 Maximum Height By Stacking Cuboids [new] (3D dominance sorting + DP)
//   - #2736 Maximum Sum Queries [new] (2D offline dominance queries via sorting and monotonic stack)

#include "framework/lc_framework.h"

#include <algorithm>
#include <limits>
#include <ranges>
#include <vector>

namespace {

class Solution {
 public:
  // Counts characters strictly dominated in both attack and defense by at least
  // one other character.
  // Time: O(n log n), Space: O(log n).
  int numberOfWeakCharacters(std::vector<std::vector<int>> properties) {
    if (properties.empty()) {
      return 0;
    }

    // Sort by attack ascending, and defense descending on ties so that during
    // the right-to-left sweep, equal-attack characters are visited in
    // non-decreasing defense order and can never falsely dominate each other.
    std::ranges::sort(properties,
                      [](const std::vector<int>& lhs,
                         const std::vector<int>& rhs) {
                        if (lhs[0] != rhs[0]) {
                          return lhs[0] < rhs[0];
                        }
                        return lhs[1] > rhs[1];
                      });

    int max_defense = std::numeric_limits<int>::min();
    int weak_count = 0;

    // Sweep from highest attack down to lowest attack.
    for (const auto& character : properties | std::views::reverse) {
      const int defense = character[1];
      if (defense < max_defense) {
        // Strictly smaller than a defense achieved at a strictly greater
        // attack level.
        ++weak_count;
      } else {
        max_defense = defense;
      }
    }

    return weak_count;
  }
};

LC_PROBLEM(1996, "The Number Of Weak Characters In The Game", "sort", ctx) {
  Solution sol;

  ctx.test("example1_none_weak", [&] {
    ctx.expect_eq(sol.numberOfWeakCharacters({{5, 5}, {6, 3}, {3, 6}}), 0);
  });

  ctx.test("example2_one_strictly_dominated", [&] {
    ctx.expect_eq(sol.numberOfWeakCharacters({{2, 2}, {3, 3}}), 1);
  });

  ctx.test("example3_non_adjacent_domination", [&] {
    ctx.expect_eq(sol.numberOfWeakCharacters({{1, 5}, {10, 4}, {4, 3}}), 1);
  });

  ctx.test("equal_attack_tie_break_critical_case", [&] {
    // [5, 5] must NOT be marked weak by [5, 7]; only [3, 6] is weak vs [5, 7].
    ctx.expect_eq(
        sol.numberOfWeakCharacters({{3, 6}, {5, 7}, {5, 5}, {6, 3}}), 1);
  });

  ctx.test("all_same_attack_and_empty", [&] {
    ctx.expect_eq(sol.numberOfWeakCharacters({{5, 5}, {5, 5}, {5, 5}}), 0);
    ctx.expect_eq(sol.numberOfWeakCharacters({{5, 1}, {5, 2}, {5, 3}}), 0);
    ctx.expect_eq(sol.numberOfWeakCharacters({}), 0);
  });
}

}  // namespace
