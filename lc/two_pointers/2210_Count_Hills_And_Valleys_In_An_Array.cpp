// LeetCode 2210: Count Hills And Valleys In An Array
// https://leetcode.com/problems/count-hills-and-valleys-in-an-array/
//
// Problem:
//   Given a 0-indexed integer array `nums`, an index `i` is part of a hill if
//   its closest non-equal left and right neighbors are both strictly smaller
//   than `nums[i]`, or part of a valley if both are strictly larger than
//   `nums[i]`. Adjacent indices with equal values belong to the same hill or
//   valley and must only be counted once. Return the total number of hills
//   and valleys in `nums`.
//   Constraints:
//     - 3 <= nums.length <= 100
//     - 1 <= nums[i] <= 100
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 4, 1, 1, 6, 5]
//     Output: 3
//     Explanation:
//       - Initialize left = 0 (nums[0] = 2) and count = 0; at i = 1 (nums[1] = 4),
//         4 > nums[0] (2) and 4 > nums[2] (1) detects a hill, setting count = 1 and
//         left = 1.
//       - At i = 2 (nums[2] = 1), nums[2] == nums[3] (1) fails both strict
//         comparisons, deferring the flat plateau without changing left or count.
//       - At i = 3 (nums[3] = 1), 1 < nums[1] (4) and 1 < nums[4] (6) detects the
//         end of the valley plateau, incrementing count to 2 and setting left = 3.
//       - At i = 4 (nums[4] = 6), 6 > nums[3] (1) and 6 > nums[5] (5) detects a
//         hill, incrementing count to 3 and returning 3.
//
//   Example 2:
//     Input:  nums = [6, 6, 5, 5, 4, 1]
//     Output: 0
//     Explanation:
//       - Initialize left = 0 (nums[0] = 6) and count = 0.
//       - Across i = 1 to 4, the sequence 6, 5, 5, 4 is non-increasing, so nums[i]
//         <= nums[left] and nums[i] >= nums[i + 1] at every index.
//       - Neither is_hill nor is_valley ever becomes true, so left stays 0 and
//         count = 0 is returned.
//
//   Example 3:
//     Input:  nums = [1, 2, 2, 3, 2]
//     Output: 1
//     Explanation:
//       - Initialize left = 0 (nums[0] = 1) and count = 0; at i = 1 (nums[1] = 2)
//         and i = 2 (nums[2] = 2), nums[i] <= nums[i + 1] holds, so left remains 0.
//       - At i = 3 (nums[3] = 3), 3 > nums[0] (1) and 3 > nums[4] (2) identifies a
//         single hill after the stepped ascent.
//       - Increment count to 1, update left = 3, and return 1.
//
// Algorithm:
//   Primary (Single-Pass Two Pointers):
//     Maintain a pointer `left` recording the index of the most recent
//     confirmed hill/valley (initially 0). Sweep `i` from 1 to n - 2:
//       - While no new extremum has occurred since `left`, the subarray
//         `nums[left..i]` is weakly monotonic (non-decreasing or
//         non-increasing).
//       - Plateaus (`nums[i] == nums[i + 1]`) are automatically deferred until
//         their final index `i` where `nums[i] != nums[i + 1]`, guaranteeing
//         each flat-topped hill or flat-bottomed valley is counted once.
//       - If `nums[i] > nums[left] && nums[i] > nums[i + 1]` (hill) or
//         `nums[i] < nums[left] && nums[i] < nums[i + 1]` (valley), increment
//         `count` and advance `left = i`.
//     Time Complexity:  O(n), where n = nums.size().
//     Space Complexity: O(1).
//
//   Alternative (Run Deduplication):
//     Compress consecutive duplicates into a strictly alternating neighbor
//     vector `unique_vals`, then count interior local maxima and minima where
//     `(prev < cur && cur > next) || (prev > cur && cur < next)`.
//     Time Complexity:  O(n).
//     Space Complexity: O(n).
//
// Data Structures:
//   - Two `std::size_t` pointers (`left` anchor and `i` scanner) over `nums`
//     for O(1)-space plateau-aware extremum detection.
//   - `std::vector<int>` (in the deduplication alternative) storing collapsed
//     runs of consecutive equal values.
//
// Real-World Applications:
//   - Financial technical analysis (detecting local swing highs and swing lows
//     while collapsing flat price plateaus).
//   - Signal processing and ECG/sensor waveform peak/trough counting with
//     quantization noise plateaus.
//   - Terrain elevation profiling to count distinct summits and depressions
//     along a 1D hiking trail.
//
// Related LeetCode Problems:
//   - #845 Longest Mountain in Array [done] (identifying hill structures in an array)
//   - #941 Valid Mountain Array [done] (strict single-peak mountain validation)
//   - #162 Find Peak Element [done] (finding a local maximum in O(log n))
//   - #376 Wiggle Subsequence [new] (counting alternating peaks and valleys)
//   - #26 Remove Duplicates from Sorted Array [done] (two-pointer adjacent duplicate deduplication)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Counts hills and valleys in a single O(1)-space pass by tracking the
  // anchor index `left` of the previous extremum (or index 0 initially).
  //
  // Invariant: `nums[left..i]` is weakly monotonic, so `nums[left]` has the
  // same ordering relation to `nums[i]` as `nums[i]`'s closest non-equal left
  // neighbor whenever `nums[i] != nums[left]`.
  // Time: O(n), Space: O(1).
  int countHillValley(const std::vector<int>& nums) {
    // At least three elements are required to have both left and right
    // neighbors.
    if (nums.size() < 3uz) {
      return 0;
    }

    std::size_t left = 0uz;
    int count = 0;

    // Index 0 and index n - 1 cannot be hills or valleys because they lack a
    // neighbor on one side.
    for (std::size_t i = 1uz; i < (nums.size() - 1uz); ++i) {
      const bool is_hill = nums[i] > nums[left] && nums[i] > nums[i + 1uz];
      const bool is_valley = nums[i] < nums[left] && nums[i] < nums[i + 1uz];

      if (is_hill || is_valley) {
        ++count;
        left = i;
      }
    }

    return count;
  }

  // Deduplication alternative: collapses consecutive equal values first so
  // every adjacent pair is strictly distinct, then counts interior extrema.
  // Time: O(n), Space: O(n).
  int countHillValleyDedup(const std::vector<int>& nums) {
    if (nums.size() < 3uz) {
      return 0;
    }

    std::vector<int> compressed;
    compressed.reserve(nums.size());
    for (const int val : nums) {
      if (compressed.empty() || compressed.back() != val) {
        compressed.push_back(val);
      }
    }

    if (compressed.size() < 3uz) {
      return 0;
    }

    int count = 0;
    for (std::size_t i = 1uz; i < (compressed.size() - 1uz); ++i) {
      const bool is_hill =
          compressed[i] > compressed[i - 1uz] &&
          compressed[i] > compressed[i + 1uz];
      const bool is_valley =
          compressed[i] < compressed[i - 1uz] &&
          compressed[i] < compressed[i + 1uz];
      if (is_hill || is_valley) {
        ++count;
      }
    }

    return count;
  }
};

LC_PROBLEM(2210, "Count Hills And Valleys In An Array", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_with_valley_plateau", [&] {
    const std::vector<int> nums = {2, 4, 1, 1, 6, 5};
    ctx.expect_eq(sol.countHillValley(nums), 3);
    ctx.expect_eq(sol.countHillValleyDedup(nums), 3);
  });

  ctx.test("example_2_monotonic_with_plateaus", [&] {
    const std::vector<int> nums = {6, 6, 5, 5, 4, 1};
    ctx.expect_eq(sol.countHillValley(nums), 0);
    ctx.expect_eq(sol.countHillValleyDedup(nums), 0);
  });

  ctx.test("all_equal_elements", [&] {
    const std::vector<int> nums = {5, 5, 5, 5};
    ctx.expect_eq(sol.countHillValley(nums), 0);
    ctx.expect_eq(sol.countHillValleyDedup(nums), 0);
  });

  ctx.test("flat_hill_plateau", [&] {
    const std::vector<int> nums = {1, 3, 3, 3, 1};
    ctx.expect_eq(sol.countHillValley(nums), 1);
    ctx.expect_eq(sol.countHillValleyDedup(nums), 1);
  });

  ctx.test("alternating_zigzag", [&] {
    const std::vector<int> nums = {1, 5, 2, 6, 3, 7};
    ctx.expect_eq(sol.countHillValley(nums), 4);
    ctx.expect_eq(sol.countHillValleyDedup(nums), 4);
  });

  ctx.test("stepped_ascent_then_drop", [&] {
    const std::vector<int> nums = {1, 2, 2, 3, 2};
    ctx.expect_eq(sol.countHillValley(nums), 1);
    ctx.expect_eq(sol.countHillValleyDedup(nums), 1);
  });
}

}  // namespace
