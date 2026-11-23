// LeetCode 1891: Cutting Ribbons
// https://leetcode.com/problems/cutting-ribbons/
//
// Problem:
//   Given an integer array `ribbons` where `ribbons[i]` is the length of the
//   i-th ribbon, and a positive integer `k`, you may cut any ribbon into any
//   number of positive integer-length segments (discarding leftovers) or leave
//   it uncut. Return the maximum positive integer length `L` such that you can
//   obtain at least `k` ribbons of length `L`, or `0` if it is impossible even
//   for `L = 1`.
//   Constraints: 1 <= ribbons.length <= 10^5, 1 <= ribbons[i] <= 10^5,
//   1 <= k <= 10^9.
//
// Algorithm:
//   Binary search on the target ribbon length `L`:
//   - At length `L >= 1`, the i-th ribbon yields `floor(ribbons[i] / L)` usable
//     segments, so the total count `sum(ribbons[i] / L)` is monotonically
//     non-increasing in `L`: if length `L` yields `>= k` pieces, every length
//     `1 <= L' < L` also yields `>= k` pieces.
//   - Search range: `[0, max(ribbons)]`, where `0` acts as the sentinel meaning
//     no positive length can produce `k` ribbons.
//   - Use upper-mid `mid = right - (right - left) / 2` (guaranteed `mid >= 1`
//     whenever `left < right`, avoiding division by zero). If `canCut(mid)` is
//     true, keep `mid` with `left = mid`; otherwise discard `[mid, right]` with
//     `right = mid - 1`.
//   Time Complexity:  O(n log M), where n = ribbons.size() and M = max(ribbons).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - `std::vector<int>` (input ribbon lengths).
//   - 64-bit piece accumulator (`std::int64_t`) because the sum of pieces at
//     `L = 1` can reach `10^5 * 10^5 = 10^{10}`, exceeding 32-bit `int`.
//
// Real-World Applications:
//   - One-dimensional cutting-stock / manufacturing optimization (pipes, steel
//     bars, wood planks, fabric rolls) into equal-length parts.
//   - Splitting heterogeneous disk/memory blocks into the largest uniform chunk
//     size that still satisfies a target shard count.
//   - Allocating uniform-duration video/audio clips from raw recordings.
//
// Related LeetCode Problems:
//   - #875 Koko Eating Bananas [done] (binary search on answer with division sum)
//   - #1011 Capacity To Ship Packages Within D Days [done] (binary search on feasible bound)
//   - #1231 Divide Chocolate [done] (maximize minimum feasible threshold via BS)
//   - #1552 Magnetic Force Between Two Balls [done] (upper-mid binary search maximization)
//   - #2226 Maximum Candies Allocated to K Children [new] (identical floor-division predicate)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum positive integer ribbon length `L` that yields at least
  // `k` equal ribbons, or `0` if fewer than `k` unit-length pieces exist.
  // Time: O(n log M), Space: O(1), where M = max(ribbons).
  int maxLength(const std::vector<int>& ribbons, int k) {
    if (ribbons.empty() || k <= 0) {
      return 0;
    }

    const int max_ribbon = std::ranges::max(ribbons);
    if (max_ribbon <= 0) {
      return 0;
    }

    // `left = 0` serves as the fallback sentinel when even length 1 cannot
    // yield `k` pieces; `right = max_ribbon` is the largest possible piece.
    int left = 0;
    int right = max_ribbon;

    // Upper-mid binary search ensures `mid >= 1` while `left < right` and
    // converges to the largest feasible length in [0, max_ribbon].
    while (left < right) {
      const int mid = right - ((right - left) / 2);
      if (canCut(ribbons, k, mid)) {
        left = mid;
      } else {
        right = mid - 1;
      }
    }

    return left;
  }

 private:
  // Returns true if cutting each ribbon into segments of length `length`
  // produces at least `k` total segments.
  // Time: O(n), Space: O(1).
  static bool canCut(const std::vector<int>& ribbons, int k, int length) {
    std::int64_t pieces = 0;
    for (int ribbon : ribbons) {
      pieces += ribbon / length;
      if (pieces >= k) {
        return true;
      }
    }
    return false;
  }
};

LC_PROBLEM(1891, "Cutting Ribbons", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.maxLength({9, 7, 5}, 3), 5);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.maxLength({7, 5, 9}, 4), 4);
  });

  ctx.test("example_3_impossible", [&] {
    ctx.expect_eq(sol.maxLength({5, 7, 9}, 22), 0);
  });

  ctx.test("exact_unit_length_boundary", [&] {
    ctx.expect_eq(sol.maxLength({5, 7, 9}, 21), 1);
  });

  ctx.test("single_ribbon_and_k_equals_1", [&] {
    ctx.expect_eq(sol.maxLength({10}, 1), 10);
    ctx.expect_eq(sol.maxLength({10}, 3), 3);
    ctx.expect_eq(sol.maxLength({1, 2, 100}, 1), 100);
  });

  ctx.test("overflow_guard_many_large_ribbons", [&] {
    // 30,000 ribbons of length 100,000 -> total length 3e9 (> INT_MAX).
    // At length 3 each ribbon yields floor(100,000 / 3) = 33,333 pieces,
    // totaling 999,990,000 pieces (< 1,000,000,000), whereas length 2 yields
    // 1,500,000,000 pieces.
    const std::vector<int> ribbons(30'000, 100'000);
    ctx.expect_eq(sol.maxLength(ribbons, 1'000'000'000), 2);
    ctx.expect_eq(sol.maxLength(ribbons, 999'990'000), 3);
  });
}

}  // namespace
