// LeetCode 228: Summary Ranges
// https://leetcode.com/problems/summary-ranges/
//
// Problem:
//   Given a sorted unique integer array `nums`, return the smallest sorted
//   list of inclusive ranges that cover every element in `nums` exactly (no
//   gaps inside any range and no element covered twice). Format each range
//   `[a, b]` as `"a->b"` if `a != b`, or `"a"` if `a == b`.
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
