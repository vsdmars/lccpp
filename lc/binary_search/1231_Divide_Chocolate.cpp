// LeetCode 1231: Divide Chocolate
// https://leetcode.com/problems/divide-chocolate/
//
// Problem:
//   You have a chocolate bar represented as an array `sweetness` of positive
//   integers. You want to share the bar with `k` friends by making `k` cuts to
//   partition it into `k + 1` non-empty contiguous pieces. You will eat the
//   piece with the minimum total sweetness and give the remaining `k` pieces to
//   your friends. Return the maximum possible total sweetness of the piece you
//   get when cutting optimally.
//   Constraints: 0 <= k < sweetness.length <= 10^4, 1 <= sweetness[i] <= 10^5.
//
// Algorithm:
//   Binary search on the answer (the target minimum piece sweetness `target`):
//   - Feasibility is monotonic: if we can partition `sweetness` into at least
//     `k + 1` contiguous pieces each of sweetness `>= target`, then every
//     smaller threshold `< target` is also feasible.
//   - Tight bounds:
//     - `left = min(sweetness)`: every single chunk has sweetness >= `left`,
//       and `sweetness.size() >= k + 1`, so `left` is always feasible.
//     - `right = sum(sweetness) / (k + 1)`: by the pigeonhole principle, the
//       minimum of `k + 1` pieces cannot exceed the average piece sweetness.
//   - Greedy check `canDivide(sweetness, k + 1, target)`: scan chunks from left
//     to right accumulating `current_sum`; as soon as `current_sum >= target`,
//     close that piece (`++pieces`) and reset `current_sum = 0`. Closing each
//     piece as early as possible leaves the largest suffix for remaining pieces.
//   - Because we maximize `target`, use upper-mid `mid = right - (right - left) / 2`
//     so setting `left = mid` always makes progress.
//   Time Complexity:  O(n log(S / (k + 1))), where n = sweetness.size() and
//                     S = sum(sweetness).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - `std::vector<int>` (input array of chunk sweetness values).
//   - 64-bit accumulators (`std::int64_t`) for overflow-free sum/bound math.
//
// Real-World Applications:
//   - Fair resource/workload partitioning (max-min fairness) when dividing an
//     ordered timeline, bandwidth budget, or linear territory among `k + 1`
//     agents so the least-provisioned agent gets as much as possible.
//   - Cutting stock / timber / fiber-optic cable into contiguous segments
//     meeting minimum quality or yield thresholds.
//
// Related LeetCode Problems:
//   - #1011 Capacity To Ship Packages Within D Days [done] (dual: minimize maximum subarray sum)
//   - #875 Koko Eating Bananas [done] (binary search on answer with greedy check)
//   - #1552 Magnetic Force Between Two Balls [done] (maximize minimum spacing via upper-mid BS)
//   - #1891 Cutting Ribbons [done] (maximize feasible length via binary search)
//   - #410 Split Array Largest Sum [done] (minimax dual of this maximin partition problem)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum achievable minimum sweetness among `k + 1` contiguous
  // pieces of `sweetness`.
  // Time: O(n log(sum / (k + 1))), Space: O(1).
  int maximizeSweetness(const std::vector<int>& sweetness, int k) {
    if (sweetness.empty() || k < 0) {
      return 0;
    }

    const int required_pieces = k + 1;
    std::int64_t left = sweetness.front();
    std::int64_t total_sweetness = 0;
    for (int chunk : sweetness) {
      left = std::min<std::int64_t>(left, chunk);
      total_sweetness += chunk;
    }

    // By the pigeonhole principle, the minimum piece cannot exceed the mean.
    std::int64_t right = total_sweetness / required_pieces;

    // Upper-mid binary search for the largest feasible threshold in [left, right].
    // Invariant: `left` is feasible; every value > `right` is infeasible.
    while (left < right) {
      const std::int64_t mid = right - ((right - left) / 2);
      if (canDivide(sweetness, required_pieces, mid)) {
        left = mid;
      } else {
        right = mid - 1;
      }
    }

    return static_cast<int>(left);
  }

 private:
  // Checks greedily whether `sweetness` can be split into at least
  // `required_pieces` contiguous segments each having sum >= `min_sweetness`.
  // Time: O(n), Space: O(1).
  static bool canDivide(const std::vector<int>& sweetness, int required_pieces,
                        std::int64_t min_sweetness) {
    int pieces = 0;
    std::int64_t current_sum = 0;
    for (int chunk : sweetness) {
      current_sum += chunk;
      if (current_sum >= min_sweetness) {
        ++pieces;
        current_sum = 0;
        if (pieces >= required_pieces) {
          return true;
        }
      }
    }
    return false;
  }
};

LC_PROBLEM(1231, "Divide Chocolate", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.maximizeSweetness({1, 2, 3, 4, 5, 6, 7, 8, 9}, 5), 6);
  });

  ctx.test("example_2_cut_every_chunk", [&] {
    ctx.expect_eq(sol.maximizeSweetness({5, 6, 7, 8, 9, 1, 2, 3, 4}, 8), 1);
  });

  ctx.test("example_3_equal_partitions", [&] {
    ctx.expect_eq(sol.maximizeSweetness({1, 2, 2, 1, 2, 2, 1, 2, 2}, 2), 5);
  });

  ctx.test("zero_friends_whole_bar", [&] {
    ctx.expect_eq(sol.maximizeSweetness({1, 2, 3, 4}, 0), 10);
  });

  ctx.test("uniform_chunks", [&] {
    ctx.expect_eq(sol.maximizeSweetness({5, 5, 5, 5, 5}, 1), 10);
    ctx.expect_eq(sol.maximizeSweetness({5, 5, 5, 5, 5}, 4), 5);
  });
}

}  // namespace
