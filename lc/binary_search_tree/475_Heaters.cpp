// LeetCode 475: Heaters
// https://leetcode.com/problems/heaters/
//
// Problem:
//   Given integer coordinates of `houses` and `heaters` on a 1D horizontal
//   line, all heaters share the same uniform heating radius `r`. A house at
//   position `x` is warmed if there exists at least one heater at position `h`
//   with |x - h| <= r. Return the minimum non-negative radius `r` required so
//   that every house is covered by at least one heater.
//
// Examples:
//   Example 1:
//     Input:  houses = [1, 2, 3, 4], heaters = [1, 4]
//     Output: 1
//     Explanation:
//       - Both houses and heaters are already sorted; initialize heater_idx = 0
//         (heater 1) and min_radius = 0.
//       - For house = 1 and house = 2, 2 * house (2 and 4) is less than 1 + 4 = 5,
//         so heater_idx stays at 0 (heater 1), yielding distances 0 and 1 and
//         min_radius = 1.
//       - For house = 3, 2 * 3 = 6 >= 5 advances heater_idx to 1 (heater 4), giving
//         distance |4 - 3| = 1.
//       - For house = 4, heater_idx = 1 gives distance |4 - 4| = 0, so min_radius
//         remains 1.
//
//   Example 2:
//     Input:  houses = [1, 5], heaters = [2]
//     Output: 3
//     Explanation:
//       - Sort houses = [1, 5] and heaters = [2], and initialize heater_idx = 0
//         with min_radius = 0.
//       - Because only one heater exists, heater_idx stays at 0 (heater 2) for all
//         houses.
//       - House 1 gives distance |2 - 1| = 1 (min_radius = 1), and house 5 gives
//         distance |2 - 5| = 3, updating min_radius to 3.
//
//   Example 3:
//     Input:  houses = [10, 2, 7, 1], heaters = [5, 5, 1, 5]
//     Output: 5
//     Explanation:
//       - Sort houses to [1, 2, 7, 10] and heaters to [1, 5, 5, 5], starting at
//         heater_idx = 0 (heater 1).
//       - For house = 1 and house = 2, 2 * house < 1 + 5 = 6 keeps heater_idx = 0,
//         updating min_radius to max(0, 1) = 1.
//       - For house = 7, 2 * 7 = 14 >= 1 + 5 advances heater_idx to 1 (heater 5)
//         and then 14 >= 5 + 5 advances past duplicate 5s to heater_idx = 3 (heater
//         5), giving distance |5 - 7| = 2.
//       - For house = 10, heater_idx = 3 (heater 5) gives distance |5 - 10| = 5,
//         updating min_radius to 5.
//
// Algorithm:
//   Sort both `houses` and `heaters` in ascending order, then sweep through
//   the houses with a monotonic two-pointer index `heater_idx` into `heaters`:
//   - Because both arrays are sorted, as house coordinate `x` increases, its
//     nearest heater never moves to the left of the previous house's nearest
//     heater.
//   - While `heater_idx + 1 < heaters.size()` and `heaters[heater_idx + 1]` is
//     at least as close to `x` as `heaters[heater_idx]` — equivalently, using
//     64-bit arithmetic to avoid overflow and floating-point rounding,
//     `2 * x >= heaters[heater_idx] + heaters[heater_idx + 1]` — advance
//     `heater_idx`.
//   - Update `min_radius = max(min_radius, |heaters[heater_idx] - x|)`.
//   (An alternative O((m + n) log n) approach sorts only `heaters` and uses
//   `std::ranges::lower_bound` per house; the two-pointer scan avoids repeated
//   binary searches and matches the source Python algorithm.)
//   Time Complexity: O(m log m + n log n), where m = houses.size() and
//   n = heaters.size().
//   Space Complexity: O(1) auxiliary space beyond sorting the input copies.
//
// Data Structures:
//   - Two sorted `std::vector<int>` arrays (`houses`, `heaters`).
//   - A single monotonic index pointer (`heater_idx`) tracking the closest
//     heater as we scan houses from left to right.
//
// Real-World Applications:
//   - Wireless cell-tower / Wi-Fi access-point transmission power calibration
//     to guarantee full coverage along a highway or corridor.
//   - CDN edge-server and emergency-service (fire station / ambulance)
//     worst-case latency analysis along a linear route.
//   - Sensor-network range sizing in industrial pipelines and rail monitoring.
//
// Related LeetCode Problems:
//   - #849 Maximize Distance to Closest Person [new] (nearest-occupied distance on 1D line)
//   - #2055 Plates Between Candles [new] (nearest left/right landmark queries)
//   - #1011 Capacity To Ship Packages Within D Days [done] (minimax feasibility / radius optimization)
//   - #34 Find First and Last Position of Element in Sorted Array [done] (sorted binary search lookup)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum uniform heater radius needed to cover every house.
  // Time: O(m log m + n log n), Space: O(1) auxiliary.
  int findRadius(std::vector<int> houses, std::vector<int> heaters) {
    if (houses.empty() || heaters.empty()) {
      return 0;
    }

    std::ranges::sort(houses);
    std::ranges::sort(heaters);

    std::size_t heater_idx = 0uz;
    int min_radius = 0;

    for (const int house : houses) {
      // Advance `heater_idx` while the next heater is closer (or equally close)
      // to `house`. Comparing `2 * house >= heaters[i] + heaters[i + 1]` in
      // int64_t avoids both floating-point division and 32-bit signed overflow.
      while (heater_idx + 1uz < heaters.size()) {
        const std::int64_t twice_house = 2 * static_cast<std::int64_t>(house);
        const std::int64_t midpoint_twice =
            static_cast<std::int64_t>(heaters[heater_idx]) +
            static_cast<std::int64_t>(heaters[heater_idx + 1uz]);
        if (twice_house < midpoint_twice) {
          break;
        }
        ++heater_idx;
      }

      const int dist = std::abs(heaters[heater_idx] - house);
      min_radius = std::max(min_radius, dist);
    }

    return min_radius;
  }
};

LC_PROBLEM(475, "Heaters", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    ctx.expect_eq(sol.findRadius({1, 2, 3}, {2}), 1);
  });

  ctx.test("leetcode_example_2", [&] {
    ctx.expect_eq(sol.findRadius({1, 2, 3, 4}, {1, 4}), 1);
  });

  ctx.test("leetcode_example_3", [&] {
    ctx.expect_eq(sol.findRadius({1, 5}, {2}), 3);
  });

  ctx.test("python_build_pos_cases", [&] {
    ctx.expect_eq(sol.findRadius({1, 2, 3, 4, 5, 6}, {4, 6}), 3);
    ctx.expect_eq(
        sol.findRadius({1, 1, 1, 1, 1, 1, 999, 999, 999, 999, 999},
                       {499, 500, 501}),
        498);
  });

  ctx.test("unsorted_and_duplicate_heaters", [&] {
    ctx.expect_eq(sol.findRadius({10, 2, 7, 1}, {5, 5, 1, 5}), 5);
  });

  ctx.test("large_coordinates_near_1e9", [&] {
    // Verifies 64-bit midpoint comparison does not overflow near 10^9.
    ctx.expect_eq(sol.findRadius({1, 1000000000}, {500000000, 999999999}),
                  499999999);
  });

  ctx.test("all_houses_on_heaters_zero_radius", [&] {
    ctx.expect_eq(sol.findRadius({3, 1, 5}, {5, 3, 1}), 0);
  });
}

}  // namespace
