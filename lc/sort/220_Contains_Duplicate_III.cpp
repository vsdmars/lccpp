// LeetCode 220: Contains Duplicate III
// https://leetcode.com/problems/contains-duplicate-iii/
//
// Problem:
//   Given an integer array nums and two integers index_diff (k) and
//   value_diff (t), return true if there exist two distinct indices i and j
//   such that abs(i - j) <= index_diff and abs(nums[i] - nums[j]) <=
//   value_diff; otherwise return false.
//
// Algorithm:
//   Sliding-window bucket sort (pigeonhole principle):
//   - Partition the integer number line into contiguous half-open buckets of
//     width w = value_diff + 1: ..., [-w, -1], [0, w - 1], [w, 2w - 1], ...
//     using true mathematical floor division so negative numbers never fold
//     into bucket 0.
//   - Maintain a hash map `buckets` holding only elements from the active
//     index window [i - index_diff, i - 1]. By the pigeonhole principle:
//       1. If bucket `bid` is already occupied, those two elements differ by
//          at most w - 1 = value_diff -> immediately return true.
//       2. Otherwise, any qualifying neighbor MUST lie in `bid - 1` or
//          `bid + 1`; check whether abs(val - neighbor_val) <= value_diff.
//   - Insert `buckets[bid] = val`. Because we return immediately on any
//     same-bucket hit, every active bucket holds at most one element, making
//     eviction of `nums[i - index_diff]` exact and collision-free.
//   - All arithmetic uses 64-bit integers (std::int64_t) to avoid signed
//     32-bit overflow when value_diff == INT_MAX or nums contains INT_MIN /
//     INT_MAX.
//   Time Complexity: O(n) average time (O(1) hash operations per element).
//   Space Complexity: O(min(n, index_diff)).
//
// Data Structures:
//   - std::unordered_map<std::int64_t, std::int64_t>: maps active bucket ID
//     to the sole element currently residing in that bucket within the last
//     index_diff positions.
//
// Real-World Applications:
//   - Real-time financial fraud / duplicate-transaction detection within a
//     sliding time window and amount tolerance.
//   - Spatial hashing and broad-phase collision detection in physics engines
//     and sensor-fusion deduplication (LiDAR / IoT telemetry streams).
//   - Approximate near-duplicate detection in streaming ETL pipelines.
//
// Related LeetCode Problems:
//   - #217 Contains Duplicate [done] (unbounded exact duplicate detection)
//   - #219 Contains Duplicate II [done] (sliding-window exact duplicates)
//   - #164 Maximum Gap [new] (pigeonhole bucketing on integer ranges)
//   - #480 Sliding Window Median [done] (ordered sliding-window queries)
//   - #1438 Longest Continuous Subarray With Absolute Diff Less Than or Equal to Limit [done] (bounded value diff in a window)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Checks whether two distinct indices within distance index_diff have values
  // within distance value_diff using sliding-window pigeonhole bucketing.
  // Time: O(n) average, Space: O(min(n, index_diff)).
  bool containsNearbyAlmostDuplicate(const std::vector<int>& nums,
                                     int index_diff, int value_diff) {
    // Distinct indices require index_diff >= 1; absolute value difference
    // cannot be negative.
    if (index_diff <= 0 || value_diff < 0 || nums.size() < 2uz) {
      return false;
    }

    // Widen to 64-bit so width = value_diff + 1 and value differences never
    // overflow 32-bit signed int.
    const std::int64_t width = static_cast<std::int64_t>(value_diff) + 1;
    const auto window = static_cast<std::size_t>(index_diff);

    // Maps bucket_id -> value currently inside the active sliding window.
    std::unordered_map<std::int64_t, std::int64_t> buckets;
    buckets.reserve(std::min(nums.size(), window + 1uz));

    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      const auto val = static_cast<std::int64_t>(nums[i]);
      const std::int64_t bid = bucketId(val, width);

      // 1. Same bucket: two values in [bid*w, (bid+1)*w - 1] differ by <= t.
      if (buckets.contains(bid)) {
        return true;
      }

      // 2. Previous bucket: check if the stored value is within value_diff.
      if (const auto it = buckets.find(bid - 1);
          it != buckets.end() && (val - it->second) <= value_diff) {
        return true;
      }

      // 3. Next bucket: check if the stored value is within value_diff.
      if (const auto it = buckets.find(bid + 1);
          it != buckets.end() && (it->second - val) <= value_diff) {
        return true;
      }

      buckets[bid] = val;

      // Evict the element that falls out of the window for the next index.
      if (i >= window) {
        const auto expired = static_cast<std::int64_t>(nums[i - window]);
        buckets.erase(bucketId(expired, width));
      }
    }

    return false;
  }

 private:
  // Computes floor(value / width) for width > 0. Standard C++ integer division
  // truncates toward zero, which would wrongly place [-width+1, -1] into
  // bucket 0 alongside [0, width-1].
  static std::int64_t bucketId(std::int64_t value, std::int64_t width) {
    if (value >= 0) {
      return value / width;
    }
    return ((value + 1) / width) - 1;
  }
};

LC_PROBLEM(220, "Contains Duplicate III", "sort", ctx) {
  Solution sol;

  ctx.test("example_1_exact_match_in_window", [&] {
    ctx.expect_true(sol.containsNearbyAlmostDuplicate({1, 2, 3, 1}, 3, 0));
  });

  ctx.test("example_2_no_pair_within_bounds", [&] {
    ctx.expect_false(
        sol.containsNearbyAlmostDuplicate({1, 5, 9, 1, 5, 9}, 2, 3));
  });

  ctx.test("python_build_negative_values", [&] {
    // nums[0] = -3 and nums[2] = -6 have |idx| = 2 <= 2 and |val| = 3 <= 3.
    ctx.expect_true(sol.containsNearbyAlmostDuplicate({-3, 3, -6}, 2, 3));
  });

  ctx.test("negative_boundary_floor_division", [&] {
    // -1 and 3 are distance 4 apart (> 3); must not collide in bucket 0.
    ctx.expect_false(sol.containsNearbyAlmostDuplicate({-1, 3}, 1, 3));
    // -1 and 2 are distance 3 apart (<= 3); adjacent buckets -1 and 0 match.
    ctx.expect_true(sol.containsNearbyAlmostDuplicate({-1, 2}, 1, 3));
  });

  ctx.test("edge_cases_window_eviction_and_int_limits", [&] {
    // Match exists only if expired element is NOT evicted properly.
    ctx.expect_false(sol.containsNearbyAlmostDuplicate({1, 10, 20, 1}, 2, 0));
    ctx.expect_false(sol.containsNearbyAlmostDuplicate({1, 1}, 0, 0));
    ctx.expect_false(sol.containsNearbyAlmostDuplicate({1, 1}, 1, -1));

    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    // |INT_MIN - INT_MAX| = 2^32 - 1 > INT_MAX -> false without overflow.
    ctx.expect_false(sol.containsNearbyAlmostDuplicate({kMin, kMax}, 1, kMax));
    // |(INT_MIN + 1) - 0| = 2^31 - 1 == INT_MAX -> true.
    ctx.expect_true(
        sol.containsNearbyAlmostDuplicate({kMin + 1, 0}, 1, kMax));
  });
}

}  // namespace
