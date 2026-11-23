// LeetCode 128: Longest Consecutive Sequence
// https://leetcode.com/problems/longest-consecutive-sequence/
//
// Problem:
//   Given an unsorted integer array `nums` (which may contain duplicates and
//   negative numbers), return the length of the longest sequence of
//   consecutive integers (`x, x+1, ..., x+k-1`) whose values appear anywhere
//   in `nums`. The algorithm must run in O(n) time.
//
// Examples:
//   Example 1:
//     Input:  nums = [100, 4, 200, 1, 3, 2]
//     Output: 4
//     Explanation:
//       - Insert all elements into remaining = {100, 4, 200, 1, 3, 2} and
//         initialize best = 0.
//       - At num = 100, erase 100 (length = 1); neither 101 nor 99 exists in
//         remaining, so best becomes 1.
//       - At num = 4, erase 4 (length = 1); 5 is absent, while downward expansion
//         erases 3, 2, and 1 to reach length = 4, updating best = 4.
//       - At num = 200, erase 200 (length = 1); subsequent values 1, 3, and 2 fail
//         remaining.erase and are skipped, returning best = 4.
//
//   Example 2:
//     Input:  nums = [0, 3, 7, 2, 5, 8, 4, 6, 0, 1]
//     Output: 9
//     Explanation:
//       - Insert nums into remaining = {0, 1, 2, 3, 4, 5, 6, 7, 8} (deduplicating
//         the repeated 0) with best = 0.
//       - At num = 0, erase 0 (length = 1) and expand upward erasing 1 through 8
//         (length = 9); -1 is absent, so best becomes 9.
//       - All remaining elements (3, 7, 2, 5, 8, 4, 6, 0, 1) are already erased
//         from remaining and skipped, returning 9.
//
//   Example 3:
//     Input:  nums = [-3, 10, -1, 0, -2, 1, 50]
//     Output: 5
//     Explanation:
//       - Populate remaining = {-3, -2, -1, 0, 1, 10, 50} and initialize best = 0.
//       - At num = -3, erase -3 and expand upward through -2, -1, 0, and 1 (length
//         = 5); -4 is absent, updating best = 5.
//       - At num = 10 and num = 50, each forms an isolated length-1 component while
//         -1, 0, -2, and 1 are skipped, returning 5.
//
// Algorithm:
//   Hash-set bidirectional expansion with eager erasure:
//   1. Insert all elements of `nums` into an `std::unordered_set<int>` called
//      `remaining` (automatically deduplicating values).
//   2. Iterate over `nums` (not `remaining`, so erasing from `remaining` never
//      invalidates our loop iterator). For each `num`, attempt to erase `num`
//      from `remaining`; if it was already consumed as part of an earlier
//      component, skip it.
//   3. Otherwise, expand strictly upward (`high + 1`) and downward (`low - 1`)
//      while guarding against `int` min/max overflow, erasing each neighbor as
//      it is counted.
//   4. Update `best = max(best, length)`.
//   Because every distinct element is inserted once and erased exactly once,
//   the total number of hash-table operations across the entire run is O(n).
//   Time complexity: O(n) expected.
//   Space complexity: O(n) for the hash set.
//
// Data Structures:
//   - `std::unordered_set<int>`: provides O(1) expected-time membership test
//     and deletion so each connected component of consecutive integers is
//     discovered and removed exactly once.
//
// Real-World Applications:
//   - Log and telemetry analytics: detecting the longest streak of consecutive
//     active days, consecutive heartbeat IDs, or uninterrupted packet sequence
//     numbers arriving out of order.
//   - Database indexing and storage compaction: coalescing unordered block IDs
//     or page numbers into contiguous extents for sequential I/O.
//   - Fraud / gamification systems: computing longest consecutive daily login
//     or transaction streaks from unsorted event streams.
//
// Related LeetCode Problems:
//   - #298 Binary Tree Longest Consecutive Sequence [done] (consecutive runs along tree paths)
//   - #228 Summary Ranges [done] (coalesces consecutive runs on already-sorted input)
//   - #549 Binary Tree Longest Consecutive Sequence II [done] (bidirectional consecutive paths in trees)
//   - #2154 Keep Multiplying Found Values by Two [new] (hash-set chain expansion)
//   - #2501 Longest Square Streak in an Array [new] (hash-set geometric streak instead of +1)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest run of consecutive integers present in
  // `nums` using hash-set bidirectional expansion with eager erasure.
  //
  // Invariant: once any member of a maximal consecutive component [L, R] is
  // visited, the entire interval [L, R] is erased from `remaining`, ensuring
  // every distinct value is processed at most once.
  //
  // Time: O(n) expected, Space: O(n).
  int longestConsecutive(const std::vector<int>& nums) {
    std::unordered_set<int> remaining(nums.begin(), nums.end());
    int best = 0;

    // Iterate over `nums` (not `remaining`) so erasing consumed values from
    // `remaining` never invalidates the outer loop iterator.
    for (const int num : nums) {
      if (remaining.erase(num) == 0uz) {
        continue;
      }

      int length = 1;

      // Expand upward while guarding against signed int overflow at INT_MAX.
      int high = num;
      while (high < std::numeric_limits<int>::max() &&
             remaining.erase(high + 1) > 0uz) {
        ++high;
        ++length;
      }

      // Expand downward while guarding against signed int underflow at INT_MIN.
      int low = num;
      while (low > std::numeric_limits<int>::min() &&
             remaining.erase(low - 1) > 0uz) {
        --low;
        ++length;
      }

      best = std::max(best, length);
    }

    return best;
  }
};

LC_PROBLEM(128, "Longest Consecutive Sequence", "array", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    ctx.expect_eq(sol.longestConsecutive({100, 4, 200, 1, 3, 2}), 4);
  });

  ctx.test("example_2_with_duplicates_and_zero", [&] {
    ctx.expect_eq(sol.longestConsecutive({0, 3, 7, 2, 5, 8, 4, 6, 0, 1}), 9);
  });

  ctx.test("empty_single_and_all_equal", [&] {
    ctx.expect_eq(sol.longestConsecutive({}), 0);
    ctx.expect_eq(sol.longestConsecutive({42}), 1);
    ctx.expect_eq(sol.longestConsecutive({5, 5, 5, 5}), 1);
  });

  ctx.test("negative_values_crossing_zero", [&] {
    ctx.expect_eq(sol.longestConsecutive({-3, 10, -1, 0, -2, 1, 50}), 5);
    ctx.expect_eq(sol.longestConsecutive({1, 2, 0, 1}), 3);
  });

  ctx.test("extreme_int_boundaries", [&] {
    const int kMin = std::numeric_limits<int>::min();
    const int kMax = std::numeric_limits<int>::max();
    ctx.expect_eq(sol.longestConsecutive({kMin, kMin + 1, kMax - 1, kMax}), 2);
  });
}

}  // namespace
