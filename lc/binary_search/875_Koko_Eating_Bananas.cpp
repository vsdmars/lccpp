// LeetCode 875: Koko Eating Bananas
// https://leetcode.com/problems/koko-eating-bananas/
//
// Problem:
//   Given `n` piles of bananas where `piles[i]` is the number of bananas in the
//   i-th pile, and an integer `h` representing the total number of hours before
//   the guards return, Koko chooses a constant integer eating speed `k` bananas
//   per hour. Each hour she picks one pile and eats up to `k` bananas from it
//   (if the pile has fewer than `k` bananas, she finishes the pile and waits
//   until the next hour). Return the minimum integer `k >= 1` such that she can
//   eat all bananas within `h` hours.
//   Constraints: 1 <= piles.length <= 10^4, piles.length <= h <= 10^9,
//   1 <= piles[i] <= 10^9.
//
// Examples:
//   Example 1:
//     Input:  piles = [3, 6, 7, 11], h = 8
//     Output: 4
//     Explanation:
//       - Sum total_bananas = 27 with max_pile = 11, setting left = (27 - 1) / 8 +
//         1 = 4 and right = 11.
//       - Iteration 1 tests mid = 7: hoursNeeded gives 1 + 1 + 1 + 2 = 5 <= 8,
//         narrowing the range to [4, 7].
//       - Iteration 2 tests mid = 5: hoursNeeded gives 1 + 2 + 2 + 3 = 8 <= 8,
//         narrowing the range to [4, 5].
//       - Iteration 3 tests mid = 4: hoursNeeded gives 1 + 2 + 2 + 3 = 8 <= 8,
//         setting right = 4 so left == right == 4 is returned.
//
//   Example 2:
//     Input:  piles = [30, 11, 23, 4, 20], h = 5
//     Output: 30
//     Explanation:
//       - Compute total_bananas = 88 and max_pile = 30, initializing left = (88 -
//         1) / 5 + 1 = 18 and right = 30.
//       - On mid = 24, hoursNeeded is 2 + 1 + 1 + 1 + 1 = 6 > 5, raising left to
//         25.
//       - On mid = 27 and then mid = 29, pile 30 still takes 2 hours for a total of
//         6 > 5, advancing left to 30.
//       - At left == right == 30, every pile finishes in 1 hour (5 total hours <=
//         5), returning 30.
//
//   Example 3:
//     Input:  piles = [30, 11, 23, 4, 20], h = 6
//     Output: 23
//     Explanation:
//       - With total_bananas = 88 and max_pile = 30, initialize left = (88 - 1) / 6
//         + 1 = 15 and right = 30.
//       - Iteration 1 tests mid = 22: hoursNeeded is 2 + 1 + 2 + 1 + 1 = 7 > 6,
//         raising left to 23.
//       - Iterations 2 and 3 test mid = 26 and mid = 24, both needing 2 + 1 + 1 + 1
//         + 1 = 6 <= 6 hours and lowering right to 24.
//       - Iteration 4 tests mid = 23 (6 <= 6 hours), setting right = 23 and
//         returning left == right == 23.
//
// Algorithm:
//   Binary search on the answer (eating speed `k`). The total hours required at
//   speed `k` is `sum(ceil(pile / k))`, which is monotonically non-increasing
//   as `k` increases: if speed `k` finishes within `h` hours, every speed > `k`
//   also finishes within `h` hours.
//   - Lower bound `left`: `max(1, ceil(total_bananas / h))`, because even
//     without per-pile rounding waste, Koko must eat at least that many bananas
//     per hour on average.
//   - Upper bound `right`: `max(piles)`, because at speed `max(piles)` every
//     pile takes exactly 1 hour, totaling `piles.size() <= h` hours.
//   We maintain the invariant that the minimum feasible speed lies in
//   `[left, right]` and shrink the half-interval with `mid = left + (right - left) / 2`
//   until `left == right`.
//   Time Complexity:  O(n log M), where n = piles.size() and M = max(piles).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - `std::vector<int>` (input array of pile sizes, scanned in O(n) per probe).
//   - Scalar 64-bit accumulators (`std::int64_t`) to sum total bananas and
//     required hours without 32-bit signed integer overflow.
//
// Real-World Applications:
//   - Provisioning minimum compute/worker throughput (e.g., batch jobs per
//     second) needed to drain variable-sized queues before an SLA deadline.
//   - Rate-limiting and bandwidth allocation where each task occupies an
//     integer number of fixed time quanta (e.g., network frame scheduling).
//   - Video transcoding or ETL pipeline sizing to hit fixed completion windows.
//
// Related LeetCode Problems:
//   - #1011 Capacity To Ship Packages Within D Days [done] (binary search on minimum capacity)
//   - #1231 Divide Chocolate [done] (binary search on answer with greedy feasibility)
//   - #1552 Magnetic Force Between Two Balls [done] (binary search on maximum minimum distance)
//   - #1891 Cutting Ribbons [done] (binary search on maximum feasible piece length)
//   - #2064 Minimized Maximum of Products Distributed to Any Store [new] (same ceil-sum predicate)
//   - #2187 Minimum Time to Complete Trips [done] (dual formulation over total time)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the minimum integer eating speed `k` so all piles are finished in
  // at most `h` hours.
  // Time: O(n log M), Space: O(1), where M = max(piles).
  int minEatingSpeed(const std::vector<int>& piles, int h) {
    if (piles.empty() || h <= 0) {
      return 0;
    }

    // Compute total bananas in 64-bit to avoid 32-bit overflow (up to 10^13),
    // and track the largest pile as the tight upper bound on eating speed.
    std::int64_t total_bananas = 0;
    int max_pile = 0;
    for (int pile : piles) {
      total_bananas += pile;
      max_pile = std::max(max_pile, pile);
    }

    // Even with zero rounding waste, Koko needs at least ceil(total / h).
    // Clamp into [1, max_pile] to stay well-formed even on degenerate inputs.
    const std::int64_t min_avg_speed = ((total_bananas - 1) / h) + 1;
    int left = static_cast<int>(
        std::clamp<std::int64_t>(min_avg_speed, 1, max_pile));
    int right = max_pile;

    // Invariant: every speed < left is infeasible; `right` is feasible
    // (assuming piles.size() <= h). Converges to the smallest feasible speed.
    while (left < right) {
      const int mid = left + ((right - left) / 2);
      if (hoursNeeded(piles, mid) > h) {
        left = mid + 1;
      } else {
        right = mid;
      }
    }

    return left;
  }

 private:
  // Computes total hours needed to finish all piles at a given positive `speed`.
  // Uses `((pile - 1) / speed) + 1` for overflow-free integer ceiling division.
  // Time: O(n), Space: O(1).
  static std::int64_t hoursNeeded(const std::vector<int>& piles, int speed) {
    std::int64_t hours = 0;
    for (int pile : piles) {
      hours += static_cast<std::int64_t>((pile - 1) / speed) + 1;
    }
    return hours;
  }
};

LC_PROBLEM(875, "Koko Eating Bananas", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.minEatingSpeed({3, 6, 7, 11}, 8), 4);
  });

  ctx.test("example_2_exact_pile_count_hours", [&] {
    ctx.expect_eq(sol.minEatingSpeed({30, 11, 23, 4, 20}, 5), 30);
  });

  ctx.test("example_3", [&] {
    ctx.expect_eq(sol.minEatingSpeed({30, 11, 23, 4, 20}, 6), 23);
  });

  ctx.test("build_cases_and_large_hours", [&] {
    ctx.expect_eq(sol.minEatingSpeed({312884470}, 968709470), 1);
    ctx.expect_eq(sol.minEatingSpeed({3, 6, 7, 16}, 8), 6);
  });

  ctx.test("overflow_guard_large_piles", [&] {
    ctx.expect_eq(sol.minEatingSpeed({1'000'000'000, 1'000'000'000}, 3),
                  1'000'000'000);
    ctx.expect_eq(sol.minEatingSpeed({1'000'000'000, 1'000'000'000}, 4),
                  500'000'000);
  });

  ctx.test("single_pile_exact_and_remainder", [&] {
    ctx.expect_eq(sol.minEatingSpeed({10}, 2), 5);
    ctx.expect_eq(sol.minEatingSpeed({10}, 3), 4);
    ctx.expect_eq(sol.minEatingSpeed({1}, 1), 1);
  });
}

}  // namespace
