// LeetCode 774: Minimize Max Distance To Gas Station
// https://leetcode.com/problems/minimize-max-distance-to-gas-station/
//
// Problem:
//   You are given an integer array `stations` representing positions of
//   existing gas stations along the x-axis (sorted in ascending order) and an
//   integer `k` representing the number of additional gas stations you may add
//   at any real-valued coordinates. Return the minimum possible value of `D`,
//   the maximum distance between any two adjacent gas stations after adding
//   the `k` new stations. Answers within `10^-6` of the true value are accepted.
//   Constraints: 10 <= stations.length <= 2000, 0 <= stations[i] <= 10^8,
//   1 <= k <= 10^6.
//
// Algorithm:
//   Binary search on the continuous answer `D` ("binary search on the answer").
//   Feasibility is monotonic: if a maximum gap `D` is achievable with `<= k`
//   added stations, then every larger `D' > D` is also achievable.
//   For a candidate threshold `mid`, each existing gap `g = stations[i] -
//   stations[i - 1]` strictly greater than `mid` must be split into
//   `ceil(g / mid)` sub-intervals, requiring `ceil(g / mid) - 1` newly inserted
//   stations. Summing over all `n - 1` gaps gives the minimum stations needed.
//   - If `needed > k`, `mid` is too small -> raise `left = mid`.
//   - Otherwise `mid` is feasible -> lower `right = mid`.
//   Iterate until `right - left <= 1e-7` (10x tighter than the 1e-6 tolerance).
//   Time Complexity: O(n log(max_gap / eps)), where `n = stations.size()`.
//   Space Complexity: O(1).
//
// Data Structures:
//   - Read-only `std::vector<int>` of station coordinates scanned linearly per
//     binary-search iteration, plus scalar `double` bounds `[left, right]`.
//
// Real-World Applications:
//   - EV fast-charging corridor and telecom cell-tower placement minimizing
//     worst-case coverage gap under a fixed infrastructure budget.
//   - Sensor-network repeater deployment along pipelines, railways, or subsea
//     fiber links.
//   - Continuous minimax resource allocation (e.g., placing checkpoints to
//     bound worst-case recovery time).
//
// Related LeetCode Problems:
//   - #410 Split Array Largest Sum [done] (discrete binary search on minimax answer)
//   - #875 Koko Eating Bananas [done] (binary search on minimum feasible rate)
//   - #1011 Capacity To Ship Packages Within D Days [done] (binary search on capacity threshold)
//   - #1231 Divide Chocolate [done] (binary search on maximin subarray sum)
//   - #1552 Magnetic Force Between Two Balls [done] (binary search on maximin spacing)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum possible maximum distance between adjacent stations
  // after inserting `k` additional stations.
  // Time: O(n log(max_gap / eps)), Space: O(1).
  double minmaxGasDist(const std::vector<int>& stations, int k) {
    if (stations.size() < 2uz) {
      return 0.0;
    }

    // Tight upper bound is the largest existing gap between consecutive
    // stations (adding k >= 0 stations can never increase the max gap).
    double max_gap = 0.0;
    for (std::size_t i = 1uz; i < stations.size(); ++i) {
      const auto gap = static_cast<double>(stations[i] - stations[i - 1uz]);
      max_gap = std::max(max_gap, gap);
    }

    // Converge to 1e-7 precision, safely stricter than the 1e-6 requirement.
    static constexpr double kEps = 1e-7;
    double left = 0.0;
    double right = max_gap;

    // Invariant: `left` is infeasible (or 0), `right` is always feasible.
    while ((right - left) > kEps) {
      const double mid = left + ((right - left) / 2.0);
      if (canAchieveMaxDist(stations, k, mid)) {
        right = mid;
      } else {
        left = mid;
      }
    }

    return right;
  }

 private:
  // Returns true iff every adjacent gap can be reduced to `<= max_dist` using
  // at most `k` new stations.
  static bool canAchieveMaxDist(const std::vector<int>& stations, int k,
                                double max_dist) {
    std::int64_t needed = 0;
    const auto budget = static_cast<std::int64_t>(k);

    for (std::size_t i = 1uz; i < stations.size(); ++i) {
      const auto gap = static_cast<double>(stations[i] - stations[i - 1uz]);
      if (gap > max_dist) {
        // Splitting `gap` into sub-segments of length <= `max_dist` needs
        // `ceil(gap / max_dist)` segments, i.e., `segments - 1` new stations.
        needed += static_cast<std::int64_t>(std::ceil(gap / max_dist)) - 1;
        if (needed > budget) {
          return false;
        }
      }
    }
    return true;
  }
};

LC_PROBLEM(774, "Minimize Max Distance To Gas Station", "binary_search", ctx) {
  constexpr double kTol = 1e-5;

  ctx.test("example_1_uniform_gaps", [&] {
    Solution sol;
    const double actual = sol.minmaxGasDist({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 9);
    ctx.expect_true(std::fabs(actual - 0.5) <= kTol);
  });

  ctx.test("example_2_single_extra_station", [&] {
    Solution sol;
    // Largest gap is 84 - 65 = 19; splitting it yields 9.5, leaving 98 - 84 = 14.
    const double actual =
        sol.minmaxGasDist({23, 24, 36, 39, 46, 56, 57, 65, 84, 98}, 1);
    ctx.expect_true(std::fabs(actual - 14.0) <= kTol);
  });

  ctx.test("two_stations_even_split", [&] {
    Solution sol;
    const double actual = sol.minmaxGasDist({0, 100}, 3);
    ctx.expect_true(std::fabs(actual - 25.0) <= kTol);
  });

  ctx.test("uneven_gaps_multiple_splits_in_one_gap", [&] {
    Solution sol;
    const double actual = sol.minmaxGasDist({0, 10, 11}, 1);
    ctx.expect_true(std::fabs(actual - 5.0) <= kTol);
    const double actual2 = sol.minmaxGasDist({0, 10, 11}, 3);
    ctx.expect_true(std::fabs(actual2 - 2.5) <= kTol);
  });

  ctx.test("large_k_small_distance", [&] {
    Solution sol;
    const double actual = sol.minmaxGasDist({0, 1}, 1000);
    ctx.expect_true(std::fabs(actual - (1.0 / 1001.0)) <= kTol);
  });
}

}  // namespace
