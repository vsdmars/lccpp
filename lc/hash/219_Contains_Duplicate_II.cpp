// LeetCode 219: Contains Duplicate II
// https://leetcode.com/problems/contains-duplicate-ii/
//
// Problem:
//   Given an integer array `nums` and an integer `k`, return true if there
//   exist two distinct indices `i` and `j` such that `nums[i] == nums[j]` and
//   `abs(i - j) <= k`, and false otherwise.
//   Constraints: 1 <= nums.length <= 10^5, -10^9 <= nums[i] <= 10^9,
//   0 <= k <= 10^5.
//
// Algorithm:
//   Scan `nums` left-to-right while maintaining a hash map `last_index` from
//   each seen value to its most recent index. Because indices strictly increase
//   during the scan, the closest prior occurrence of `nums[i]` is always the
//   most recent one stored in `last_index`:
//   - If `k <= 0`, no two distinct indices satisfy `|i - j| <= k`, so return
//     false immediately.
//   - For each element `nums[i]`, attempt `try_emplace(nums[i], i)`. If the
//     value was already present and `i - it->second <= k`, return true.
//     Otherwise update `it->second = i` so future occurrences compare against
//     this closer index.
//   Time:  O(n) average
//   Space: O(n) worst-case (one entry per distinct element)
//
// Data Structures:
//   - std::unordered_map<int, std::size_t> last_index: Records the latest index
//     at which each distinct integer appeared for O(1) expected lookup/update.
//
// Real-World Applications:
//   - Streaming event deduplication (dropping duplicate messages or telemetry
//     packets arriving within a bounded sequence-number window).
//   - Sliding-window fraud or anomaly detection (identifying repeated card
//     charges or API requests within `k` operations).
//   - Cache-locality profiling and reuse-distance measurement in memory traces.
//
// Related LeetCode Problems:
//   - #217 Contains Duplicate [done] (unbounded distance duplicate check)
//   - #220 Contains Duplicate III [done] (adds value-difference bound via balanced BST / buckets)
//   - #1 Two Sum [done] (classic value-to-index hash map lookup)
//   - #205 Isomorphic Strings [done] (index/mapping tracking over linear scans)

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `nums` contains equal elements at two distinct indices
  // separated by at most `k`.
  // Invariant: before processing index `i`, `last_index[v]` holds the largest
  // index < `i` where value `v` occurred.
  // Time: O(n) average, Space: O(n).
  bool containsNearbyDuplicate(const std::vector<int>& nums, int k) {
    // Distinct indices satisfy |i - j| >= 1, so k <= 0 can never match.
    if (k <= 0 || nums.size() < 2uz) {
      return false;
    }

    const auto max_dist = static_cast<std::size_t>(k);
    std::unordered_map<int, std::size_t> last_index;
    last_index.reserve(nums.size());

    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      // Single lookup: insert if unseen, or check distance against latest hit.
      const auto [it, inserted] = last_index.try_emplace(nums[i], i);
      if (!inserted) {
        if ((i - it->second) <= max_dist) {
          return true;
        }
        // Keep only the newest index, since earlier ones are strictly farther.
        it->second = i;
      }
    }

    return false;
  }
};

LC_PROBLEM(219, "Contains Duplicate II", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_true(sol.containsNearbyDuplicate({1, 2, 3, 1}, 3));
    ctx.expect_true(sol.containsNearbyDuplicate({1, 0, 1, 1}, 1));
    ctx.expect_false(sol.containsNearbyDuplicate({1, 2, 3, 1, 2, 3}, 2));
  });

  ctx.test("boundary_distances", [&] {
    ctx.expect_false(sol.containsNearbyDuplicate({1, 2, 3, 1}, 2));
    ctx.expect_true(sol.containsNearbyDuplicate({1, 2, 3, 1}, 10));
    ctx.expect_false(sol.containsNearbyDuplicate({1, 1}, 0));
  });

  ctx.test("edge_cases_and_negatives", [&] {
    ctx.expect_false(sol.containsNearbyDuplicate({}, 3));
    ctx.expect_false(sol.containsNearbyDuplicate({42}, 1));
    ctx.expect_true(sol.containsNearbyDuplicate({-1000000000, 5, -1000000000}, 2));
    ctx.expect_false(sol.containsNearbyDuplicate({-1, 0, 1, 2}, 5));
  });
}

}  // namespace
