// LeetCode 228: Summary Ranges
// https://leetcode.com/problems/summary-ranges/
//
// Problem:
//   Given a sorted unique integer array `nums`, return the smallest sorted
//   list of inclusive ranges that cover every element in `nums` exactly (no
//   gaps inside any range and no element covered twice). Format each range
//   `[a, b]` as `"a->b"` if `a != b`, or `"a"` if `a == b`.
//
// Examples:
//   Example 1:
//     Input:  nums = [0, 1, 2, 4, 5, 7]
//     Output: ["0->2", "4->5", "7"]
//     Explanation:
//       - Initialize range_start = 0 and range_end = 0 from nums[0].
//       - At i = 1 (nums[1] = 1 == 0 + 1) and i = 2 (nums[2] = 2 == 1 + 1), extend
//         range_end to 1 and then 2.
//       - At i = 3 (nums[3] = 4 != 2 + 1), flush "0->2" to result and reset
//         range_start = 4 and range_end = 4.
//       - At i = 4 (nums[4] = 5 == 4 + 1), extend range_end to 5; at i = 5 (nums[5]
//         = 7 != 5 + 1), flush "4->5" and reset range_start = 7 and range_end = 7.
//       - After the loop, flush the remaining run [7, 7] as "7" and return ["0->2",
//         "4->5", "7"].
//
//   Example 2:
//     Input:  nums = [0, 2, 3, 4, 6, 8, 9]
//     Output: ["0", "2->4", "6", "8->9"]
//     Explanation:
//       - Initialize range_start = 0 and range_end = 0; at i = 1 (nums[1] = 2 != 0
//         + 1), flush "0" and start [2, 2].
//       - At i = 2 (nums[2] = 3) and i = 3 (nums[3] = 4), extend range_end to 4.
//       - At i = 4 (nums[4] = 6 != 4 + 1), flush "2->4" and start [6, 6]; at i = 5
//         (nums[5] = 8 != 6 + 1), flush "6" and start [8, 8].
//       - At i = 6 (nums[6] = 9 == 8 + 1), extend range_end to 9, then flush "8->9"
//         after the loop and return ["0", "2->4", "6", "8->9"].
//
//   Example 3:
//     Input:  nums = [-2147483648, -2147483647, 2147483647]
//     Output: ["-2147483648->-2147483647", "2147483647"]
//     Explanation:
//       - Initialize range_start = -2147483648 and range_end = -2147483648.
//       - At i = 1, nums[1] = -2147483647 equals int64_t(-2147483648) + 1, so
//         extend range_end to -2147483647.
//       - At i = 2, nums[2] = 2147483647 differs from int64_t(-2147483647) + 1, so
//         flush "-2147483648->-2147483647" and start [2147483647, 2147483647].
//       - After the loop, flush "2147483647" and return
//         ["-2147483648->-2147483647", "2147483647"].
//
// Algorithm:
//   Single-Pass Contiguous Run Grouping:
//     Maintain the endpoints `[range_start, range_end]` of the active
//     contiguous run, initialized to `nums[0]`. For each subsequent element
//     `nums[i]`:
//       - Compare `nums[i]` against `range_end + 1` in 64-bit signed
//         arithmetic (preventing overflow when `range_end == INT_MAX`).
//       - If consecutive, extend the current run by setting
//         `range_end = nums[i]`.
//       - Otherwise, flush the completed run `[range_start, range_end]` to the
//         output and begin a new run at `nums[i]`.
//     After the loop finishes, flush the final active run.
//     (This refines the stack-based grouping from O(n) stack space down to
//     O(1) auxiliary space, since only the run's first and last elements are
//     ever needed.)
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space (excluding the output vector).
//
// Data Structures:
//   - Scalar endpoints (`range_start`, `range_end`): represent the active
//     contiguous integer run in constant space.
//   - std::vector<std::string> result: stores formatted range strings.
//
// Real-World Applications:
//   - Page-range formatting in print dialogs and PDF exporters
//     (e.g., `"1-3, 5, 8-12"`).
//   - Run-length / extent compaction for sorted integer IDs, inverted-index
//     posting lists, and bitmap-to-interval serialization.
//   - Firewall port-range and VLAN-ID list summarization in network
//     configuration tools.
//
// Related LeetCode Problems:
//   - #163 Missing Ranges [done] (complementary problem: emit absent intervals)
//   - #352 Data Stream as Disjoint Intervals [done] (online range merging)
//   - #56 Merge Intervals [done] (coalescing overlapping intervals)
//   - #128 Longest Consecutive Sequence [done] (consecutive runs on unsorted input)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Compresses sorted unique integers in `nums` into minimal contiguous
  // summary range strings.
  // Invariant: at the start of iteration `i`, all elements in nums[0..i-1]
  // preceding `range_start` have been emitted, and `[range_start, range_end]`
  // is the maximal contiguous run ending at `nums[i - 1]`.
  // Time: O(n), Space: O(1) auxiliary (output excluded).
  std::vector<std::string> summaryRanges(const std::vector<int>& nums) {
    std::vector<std::string> result;
    if (nums.empty()) {
      return result;
    }

    int range_start = nums[0];
    int range_end = nums[0];

    for (std::size_t i = 1uz; i < nums.size(); ++i) {
      // Promote to int64_t so range_end + 1 never overflows when
      // range_end == INT_MAX.
      if (static_cast<std::int64_t>(nums[i]) ==
          static_cast<std::int64_t>(range_end) + 1) {
        // Extend the current contiguous run.
        range_end = nums[i];
      } else {
        // Gap detected: finalize the completed run and start a new one.
        result.push_back(formatRange(range_start, range_end));
        range_start = nums[i];
        range_end = nums[i];
      }
    }

    // Flush the final active run after exiting the loop.
    result.push_back(formatRange(range_start, range_end));
    return result;
  }

 private:
  // Formats inclusive range [start, end] as "start" or "start->end".
  static std::string formatRange(int start, int end) {
    if (start == end) {
      return std::to_string(start);
    }
    return std::to_string(start) + "->" + std::to_string(end);
  }
};

LC_PROBLEM(228, "Summary Ranges", "array", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<std::string> expected{"0->2", "4->5", "7"};
    ctx.expect_eq(sol.summaryRanges({0, 1, 2, 4, 5, 7}), expected);
  });

  ctx.test("example_2_source_build", [&] {
    const std::vector<std::string> expected{"0", "2->4", "6", "8->9"};
    ctx.expect_eq(sol.summaryRanges({0, 2, 3, 4, 6, 8, 9}), expected);
  });

  ctx.test("empty_and_single_elements", [&] {
    ctx.expect_eq(sol.summaryRanges({}), std::vector<std::string>{});
    ctx.expect_eq(sol.summaryRanges({-1}), std::vector<std::string>{"-1"});
    ctx.expect_eq(sol.summaryRanges({0}), std::vector<std::string>{"0"});
  });

  ctx.test("fully_contiguous_and_fully_isolated", [&] {
    ctx.expect_eq(sol.summaryRanges({-3, -2, -1, 0, 1}),
                  std::vector<std::string>{"-3->1"});
    ctx.expect_eq(sol.summaryRanges({1, 3, 5, 7}),
                  std::vector<std::string>{"1", "3", "5", "7"});
  });

  ctx.test("extreme_int32_gap_no_overflow", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    const std::vector<std::string> expected{"-2147483648->-2147483647",
                                            "2147483647"};
    ctx.expect_eq(sol.summaryRanges({kMin, kMin + 1, kMax}), expected);
  });
}

}  // namespace
