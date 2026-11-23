// LeetCode 163: Missing Ranges
// https://leetcode.com/problems/missing-ranges/
//
// Problem:
//   You are given an inclusive integer range `[lower, upper]` and a sorted
//   unique integer array `nums`, where all elements of `nums` lie within
//   `[lower, upper]`. Return the shortest sorted list of missing ranges that
//   cover every integer in `[lower, upper]` absent from `nums`. Each missing
//   range `[a, b]` is formatted as `"a"` if `a == b` and `"a->b"` if `a < b`.
//
// Algorithm:
//   Virtual Sentinel Linear Scan:
//     Treat the stream as starting at a virtual sentinel `prev = lower - 1`
//     and ending at a virtual sentinel `upper + 1` (computed in 64-bit signed
//     arithmetic so `INT_MIN - 1` and `INT_MAX + 1` never overflow).
//     Walk through each element `curr` of `nums`, followed by `upper + 1`:
//       - If `curr - prev >= 2`, all integers in `[prev + 1, curr - 1]` are
//         missing; format and append this interval.
//       - Advance `prev = curr`.
//     Because `nums` is read-only and the trailing sentinel is synthesized by
//     the loop bound `i <= nums.size()`, the caller's input vector is never
//     mutated or copied.
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space (excluding the output list).
//
// Data Structures:
//   - std::vector<std::string> result: accumulates formatted missing ranges in
//     ascending order.
//   - 64-bit signed scalars (`std::int64_t`) for overflow-safe sentinel math.
//
// Real-World Applications:
//   - TCP/UDP packet sequence-number gap detection (generating SACK blocks for
//     missing byte ranges).
//   - Log / time-series ingestion audits to report missing sequence IDs,
//     timestamps, or shard partitions.
//   - Free-space extent tracking in filesystems, memory allocators, and
//     database primary-key gap analysis.
//
// Related LeetCode Problems:
//   - #228 Summary Ranges [done] (inverse task: compress present consecutive runs)
//   - #56 Merge Intervals [done] (interval coalescing in sorted order)
//   - #57 Insert Interval [done] (linear interval scan and splicing)
//   - #352 Data Stream as Disjoint Intervals [done] (dynamic range maintenance)
//   - #268 Missing Number [done] (single missing value in [0, n])

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all maximal missing integer ranges within [lower, upper] not present
  // in the sorted array `nums`.
  // Invariant: after processing index `i`, all missing integers in
  // [lower, prev] have been emitted in sorted order.
  // Time: O(n), Space: O(1) auxiliary (output excluded).
  std::vector<std::string> findMissingRanges(const std::vector<int>& nums,
                                             int lower, int upper) {
    std::vector<std::string> result;

    // Promote bounds to int64_t so lower - 1 and upper + 1 cannot overflow
    // when lower == INT_MIN or upper == INT_MAX.
    std::int64_t prev = static_cast<std::int64_t>(lower) - 1;
    const std::int64_t upper_sentinel = static_cast<std::int64_t>(upper) + 1;

    // Iterate one step past nums.size() to handle the trailing gap [last+1,
    // upper] identically to internal gaps without mutating `nums`.
    for (std::size_t i = 0uz; i <= nums.size(); ++i) {
      const std::int64_t curr = (i < nums.size())
                                    ? static_cast<std::int64_t>(nums[i])
                                    : upper_sentinel;

      // A gap exists iff at least one integer lies strictly between prev and
      // curr (i.e., prev + 1 <= curr - 1).
      if (curr - prev >= 2) {
        result.push_back(formatRange(prev + 1, curr - 1));
      }
      prev = curr;
    }

    return result;
  }

 private:
  // Formats inclusive range [start, end] as "start" or "start->end".
  static std::string formatRange(std::int64_t start, std::int64_t end) {
    if (start == end) {
      return std::to_string(start);
    }
    return std::to_string(start) + "->" + std::to_string(end);
  }
};

LC_PROBLEM(163, "Missing Ranges", "array", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    const std::vector<std::string> expected{"2", "4->49", "51->74", "76->99"};
    ctx.expect_eq(sol.findMissingRanges({0, 1, 3, 50, 75}, 0, 99), expected);
  });

  ctx.test("leading_gap_only", [&] {
    const std::vector<std::string> expected{"0->1"};
    ctx.expect_eq(sol.findMissingRanges({2, 3, 4}, 0, 4), expected);
  });

  ctx.test("empty_input_single_and_multi", [&] {
    ctx.expect_eq(sol.findMissingRanges({}, 1, 1),
                  std::vector<std::string>{"1"});
    ctx.expect_eq(sol.findMissingRanges({}, -3, -1),
                  std::vector<std::string>{"-3->-1"});
  });

  ctx.test("no_missing_values", [&] {
    ctx.expect_eq(sol.findMissingRanges({-1}, -1, -1),
                  std::vector<std::string>{});
    ctx.expect_eq(sol.findMissingRanges({0, 1, 2, 3}, 0, 3),
                  std::vector<std::string>{});
  });

  ctx.test("extreme_int32_boundaries", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();

    ctx.expect_eq(sol.findMissingRanges({}, kMin, kMax),
                  std::vector<std::string>{"-2147483648->2147483647"});
    ctx.expect_eq(sol.findMissingRanges({kMin, kMax}, kMin, kMax),
                  std::vector<std::string>{"-2147483647->2147483646"});
  });
}

}  // namespace
