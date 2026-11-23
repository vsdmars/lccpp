// LeetCode 179: Largest Number
// https://leetcode.com/problems/largest-number/
//
// Problem:
//   Given a list of non-negative integers nums (1 <= nums.length <= 100,
//   0 <= nums[i] <= 10^9), arrange them so that their decimal concatenation
//   forms the largest possible number, and return it as a string (without
//   spurious leading zeros, e.g., [0, 0] returns "0").
//
// Examples:
//   Example 1:
//     Input:  nums = [10, 2]
//     Output: "210"
//     Explanation:
//       - Convert nums to decimal strings parts = ["10", "2"] with total_length =
//         3.
//       - Sort parts with (a + b) > (b + a): since "210" > "102", "2" precedes "10"
//         to give parts = ["2", "10"].
//       - Verify parts[0] != "0" and concatenate parts into "210".
//
//   Example 2:
//     Input:  nums = [3, 30, 34, 5, 9]
//     Output: "9534330"
//     Explanation:
//       - Convert all elements into parts = ["3", "30", "34", "5", "9"].
//       - Sort using (a + b) > (b + a): "9" and "5" lead ("95" > "59"), "34"
//         precedes "3" ("343" > "334"), and "3" precedes "30" ("330" > "303"),
//         yielding ["9", "5", "34", "3", "30"].
//       - Confirm parts[0] != "0" and concatenate the sorted strings into
//         "9534330".
//
//   Example 3:
//     Input:  nums = [0, 0]
//     Output: "0"
//     Explanation:
//       - Convert nums to parts = ["0", "0"] and run std::ranges::sort (leaving
//         parts as ["0", "0"]).
//       - Observe parts[0] == "0" and return "0" directly instead of concatenating
//         "00".
//
// Algorithm:
//   Convert every integer to its base-10 string representation and sort the
//   strings using a custom strict-weak-ordering comparator:
//     a precedes b  iff  (a + b) > (b + a)  lexicographically.
//   Correctness intuition: Because (a + b) and (b + a) always have equal
//   length, lexicographical comparison matches numerical comparison. Comparing
//   a + b vs b + a is equivalent to comparing the repeating fractions
//   a / (10^|a| - 1) vs b / (10^|b| - 1), which proves transitivity and thus
//   guarantees a globally optimal total order. After sorting, if the largest
//   element is "0", all elements must be "0", so return "0" directly (avoiding
//   integer parsing that would overflow 64-bit integers on long outputs).
//   Time Complexity: O(n log n * d), where d <= 10 is the max digit count.
//   Space Complexity: O(n * d) to hold the decimal strings and output buffer.
//
// Data Structures:
//   - std::vector<std::string>: stores decimal representations for sorting.
//   - std::string: pre-reserved output buffer for linear concatenation.
//
// Real-World Applications:
//   - Custom comparator design and exchange-argument scheduling (e.g., Smith's
//     rule / job ordering minimizing weighted completion time).
//   - Distributed key serialization and lexicographical composite-key ordering
//     in LSM-tree storage engines (RocksDB, Bigtable).
//   - Big-integer formatting where outputs exceed native 64-bit word widths.
//
// Related LeetCode Problems:
//   - #165 Compare Version Numbers [done] (custom string-based numeric ordering)
//   - #321 Create Maximum Number [done] (maximizing digit sequence via greedy)
//   - #402 Remove K Digits [done] (lexicographically optimal digit arrangement)
//   - #937 Reorder Data in Log Files [new] (custom string comparator sorting)
//   - #1665 Minimum Initial Energy to Finish Tasks [done] (exchange-argument sort)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Arranges non-negative integers so their decimal concatenation is maximal.
  // Time: O(n log n * d), Space: O(n * d).
  std::string largestNumber(const std::vector<int>& nums) {
    if (nums.empty()) {
      return "0";
    }

    // Convert integers to strings once so the comparator avoids repeated
    // integer-to-string conversions during sorting.
    std::vector<std::string> parts;
    parts.reserve(nums.size());
    std::size_t total_length = 0uz;
    for (int num : nums) {
      parts.push_back(std::to_string(num));
      total_length += parts.back().size();
    }

    // Place a before b iff concatenating a then b yields a larger string than
    // concatenating b then a (e.g., "9" before "90" since "990" > "909").
    std::ranges::sort(parts, [](const std::string& a, const std::string& b) {
      return (a + b) > (b + a);
    });

    // If the greatest element is "0", every element is zero; collapse "00..0"
    // to a single "0".
    if (parts[0] == "0") {
      return "0";
    }

    std::string result;
    result.reserve(total_length);
    for (const std::string& part : parts) {
      result += part;
    }
    return result;
  }
};

LC_PROBLEM(179, "Largest Number", "sort", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    ctx.expect_eq(sol.largestNumber({10, 2}), "210");
  });

  ctx.test("example_2_prefix_tiebreak", [&] {
    ctx.expect_eq(sol.largestNumber({3, 30, 34, 5, 9}), "9534330");
  });

  ctx.test("example_3_and_4_single_element", [&] {
    ctx.expect_eq(sol.largestNumber({1}), "1");
    ctx.expect_eq(sol.largestNumber({10}), "10");
  });

  ctx.test("edge_cases_zeros_and_prefix_90_9", [&] {
    ctx.expect_eq(sol.largestNumber({90, 9}), "990");
    ctx.expect_eq(sol.largestNumber({0, 0}), "0");
    ctx.expect_eq(sol.largestNumber({0, 0, 0}), "0");
  });

  ctx.test("overflow_beyond_64bit_integer", [&] {
    // Concatenation has 28 digits, exceeding uint64_t range.
    ctx.expect_eq(sol.largestNumber({999999991, 9, 999999999, 999999998}),
                  "9999999999999999998999999991");
  });
}

}  // namespace
