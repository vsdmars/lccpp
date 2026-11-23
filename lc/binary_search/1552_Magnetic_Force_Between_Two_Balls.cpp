// LeetCode 1552: Magnetic Force Between Two Balls
// https://leetcode.com/problems/magnetic-force-between-two-balls/
//
// Problem:
//   Given `n` empty baskets located at distinct integer coordinates in
//   `position` and an integer `m` (`2 <= m <= n`), place `m` balls into `m`
//   distinct baskets so that the minimum magnetic force `|x - y|` between any
//   two placed balls is as large as possible. Return that maximum achievable
//   minimum distance.
//   Constraints: 2 <= position.length <= 10^5, 1 <= position[i] <= 10^9,
//   all positions are distinct, 2 <= m <= position.length.
//
// Algorithm:
//   Sort `position` ascending, then binary search on the minimum separation
//   distance `d`:
//   - Feasibility is monotonic: if we can place `m` balls with pairwise
//     separation `>= d`, then any smaller separation `< d` is also feasible.
//   - Tight bounds:
//     - `left = 1` (positions are distinct, so distance 1 is always feasible).
//     - `right = (sorted.back() - sorted.front()) / (m - 1)` (maximum possible
//       spacing if all `m` balls were spaced completely evenly across the span).
//   - Greedy placement check `canPlaceBalls(sorted, m, d)`: always place the
//     first ball at `sorted[0]`, then scan left to right placing the next ball
//     at the earliest basket `>= last_pos + d`. By a standard exchange argument,
//     choosing the leftmost valid basket never hurts the remaining placements.
//   - Use upper-mid `mid = right - (right - left) / 2` to converge on the
//     maximum feasible distance.
//   Time Complexity:  O(n log n + n log(D / (m - 1))), where n = position.size()
//                     and D = max(position) - min(position).
//   Space Complexity: O(n) for the non-mutating sorted copy (O(log n) if
//                     sorting in place).
//
// Data Structures:
//   - `std::vector<int>` (sorted basket coordinates scanned sequentially).
//
// Real-World Applications:
//   - Placing cell towers, Wi-Fi access points, or sensors along a corridor
//     to maximize minimum separation and reduce interference.
//   - Scheduling periodic jobs or sampling points from discrete candidate
//     timestamps with maximum minimum spacing (aggressive-cows facility layout).
//   - Spreading distributed cache replicas across ordered rack/ring positions.
//
// Related LeetCode Problems:
//   - #875 Koko Eating Bananas [done] (binary search on answer with linear check)
//   - #1011 Capacity To Ship Packages Within D Days [done] (binary search + greedy validation)
//   - #1231 Divide Chocolate [done] (maximize minimum objective via upper-mid binary search)
//   - #1891 Cutting Ribbons [done] (maximize feasible integer length via binary search)
//   - #2517 Maximum Tastiness of Candy Basket [new] (identical formulation)
//   - #1870 Minimum Speed to Arrive on Time [new] (binary search on feasible parameter)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum possible minimum distance between any two of `m` balls
  // placed in the baskets at `position`.
  // Time: O(n log n + n log(D / (m - 1))), Space: O(n).
  int maxDistance(const std::vector<int>& position, int m) {
    if (m < 2 || position.size() < static_cast<std::size_t>(m)) {
      return 0;
    }

    // Sort a local copy so the caller's input vector remains unmodified.
    std::vector<int> sorted = position;
    std::ranges::sort(sorted);

    // Since all basket positions are distinct, a spacing of 1 is always
    // achievable, while (span / (m - 1)) is the theoretical upper limit.
    int left = 1;
    int right = (sorted.back() - sorted.front()) / (m - 1);

    // Upper-mid binary search for the largest feasible minimum spacing.
    // Invariant: `left` is feasible; every distance > `right` is infeasible.
    while (left < right) {
      const int mid = right - ((right - left) / 2);
      if (canPlaceBalls(sorted, m, mid)) {
        left = mid;
      } else {
        right = mid - 1;
      }
    }

    return left;
  }

 private:
  // Greedily tests whether at least `m` balls can be placed in `sorted` such
  // that consecutive placed balls are at least `min_dist` apart.
  // Time: O(n), Space: O(1).
  static bool canPlaceBalls(const std::vector<int>& sorted, int m,
                            int min_dist) {
    int placed = 1;
    int last_pos = sorted.front();
    for (std::size_t i = 1uz; i < sorted.size(); ++i) {
      if (sorted[i] - last_pos >= min_dist) {
        ++placed;
        last_pos = sorted[i];
        if (placed >= m) {
          return true;
        }
      }
    }
    return false;
  }
};

LC_PROBLEM(1552, "Magnetic Force Between Two Balls", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.maxDistance({1, 2, 3, 4, 7}, 3), 3);
  });

  ctx.test("example_2_two_balls_wide_span", [&] {
    ctx.expect_eq(sol.maxDistance({5, 4, 3, 2, 1, 1'000'000'000}, 2),
                  999'999'999);
  });

  ctx.test("unsorted_input_all_baskets_used", [&] {
    ctx.expect_eq(sol.maxDistance({79, 74, 57, 22}, 4), 5);
  });

  ctx.test("consecutive_baskets_all_filled", [&] {
    ctx.expect_eq(sol.maxDistance({1, 2, 3, 4, 5}, 5), 1);
  });

  ctx.test("unsorted_example_1_permutation", [&] {
    ctx.expect_eq(sol.maxDistance({7, 1, 4, 2, 3}, 3), 3);
  });
}

}  // namespace
