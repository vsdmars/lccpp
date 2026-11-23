// LeetCode 38: Count And Say
// https://leetcode.com/problems/count-and-say/
//
// Problem:
//   The count-and-say sequence is defined recursively on positive integers:
//   - `countAndSay(1) = "1"`
//   - `countAndSay(n)` is the run-length encoding (RLE) of `countAndSay(n - 1)`,
//     where each maximal contiguous run of identical digits is replaced by
//     `<count><digit>` (e.g., `"1"` -> `"11"` -> `"21"` -> `"1211"` ->
//     `"111221"`).
//   Given `n`, return the `n`-th term as a string.
//
//   Constraints:
//   - 1 <= n <= 30
//
// Examples:
//   Example 1:
//     Input:  n = 4
//     Output: "1211"
//     Explanation:
//       - Initialize term = "1" for step 1 of the sequence.
//       - At step = 1, describeNext("1") flushes the single run of length 1 of '1',
//         producing "11".
//       - At step = 2, describeNext("11") matches '1' at index 1 to reach run_len =
//         2 and flushes "21".
//       - At step = 3, describeNext("21") sees '1' != '2' at index 1, emits "12",
//         resets run_len = 1, and flushes the trailing '1' to return "1211".
//
//   Example 2:
//     Input:  n = 6
//     Output: "312211"
//     Explanation:
//       - Iterate step = 1..4 to advance term from "1" through "11", "21", and
//         "1211" to "111221".
//       - At step = 5, describeNext("111221") counts three consecutive '1's at
//         indices 0..2 and emits "31" when index 3 switches to '2'.
//       - Scan '2' at indices 3..4 with run_len = 2 and emit "22" when index 5
//         switches to '1'.
//       - Flush the trailing run of one '1' after the loop to append "11", yielding
//         "312211".
//
//   Example 3:
//     Input:  n = 1
//     Output: "1"
//     Explanation:
//       - Initialize term = "1" as the base sequence value.
//       - Skip the loop for step = 1; step < 1 and return "1" immediately.
//
// Algorithm:
//   Iterative run-length encoding:
//   Start with `term = "1"` and apply `describeNext(term)` exactly `n - 1`
//   times. Inside `describeNext`, scan `current` left to right while tracking
//   the length `run_len` of the current contiguous digit run. Whenever a new
//   digit appears (and once at the end of the string), emit the decimal count
//   followed by the run's character into a pre-reserved output buffer.
//
//   Time Complexity:  O(L_n), where L_n is the length of the n-th term. By
//                     Conway's cosmological theorem, L_n grows as
//                     Theta(lambda^n) with lambda ~= 1.303577, so the geometric
//                     sum of all intermediate term lengths is O(L_n).
//   Space Complexity: O(L_n) to store the current and next terms.
//
// Data Structures:
//   - `std::string` buffers (`term` and `next`) swapped across iterations, with
//     `std::string_view` passed to the RLE step helper.
//
// Real-World Applications:
//   - Run-length encoding (RLE) compression in bitmap image formats (BMP, TIFF,
//     PCX), fax transmission (CCITT Group 3/4), and columnar database blocks
//     (Parquet, ORC).
//   - Compact representation of sparse boolean masks and repetitive telemetry
//     streams.
//
// Related LeetCode Problems:
//   - #443 String Compression [done] (in-place run-length encoding of chars)
//   - #271 Encode and Decode Strings [done] (length-prefixed string framing)
//   - #394 Decode String [done] (expanding count-prefixed repeated substrings)
//   - #604 Design Compressed String Iterator [new] (streaming RLE decoder)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Generates the `n`-th term of the count-and-say sequence iteratively.
  // Invariant: after `step` iterations, `term` equals `countAndSay(step)`.
  // Time: O(L_n), Space: O(L_n), where L_n = Theta(1.3036^n).
  std::string countAndSay(int n) {
    if (n <= 0) {
      return {};
    }

    std::string term = "1";
    for (int step = 1; step < n; ++step) {
      term = describeNext(term);
    }
    return term;
  }

 private:
  // Produces the run-length encoding of a non-empty digit string `current`.
  // Invariant: `run_len` is the number of consecutive copies of `current[i - 1]`
  // seen in the active run ending at index `i - 1`.
  static std::string describeNext(std::string_view current) {
    std::string next;
    // Every character expands to at most 2 characters when all runs have
    // length 1, so `2 * current.size()` prevents reallocations.
    next.reserve(current.size() * 2uz);

    std::size_t run_len = 1uz;
    for (std::size_t i = 1uz; i < current.size(); ++i) {
      if (current[i] == current[i - 1uz]) {
        ++run_len;
      } else {
        next.append(std::to_string(run_len));
        next.push_back(current[i - 1uz]);
        run_len = 1uz;
      }
    }

    // Flush the trailing run after the loop exits.
    next.append(std::to_string(run_len));
    next.push_back(current.back());
    return next;
  }
};

LC_PROBLEM(38, "Count And Say", "string", ctx) {
  Solution sol;

  ctx.test("first_six_terms", [&] {
    ctx.expect_eq(sol.countAndSay(1), "1");
    ctx.expect_eq(sol.countAndSay(2), "11");
    ctx.expect_eq(sol.countAndSay(3), "21");
    ctx.expect_eq(sol.countAndSay(4), "1211");
    ctx.expect_eq(sol.countAndSay(5), "111221");
    ctx.expect_eq(sol.countAndSay(6), "312211");
  });

  ctx.test("ninth_term_from_build", [&] {
    ctx.expect_eq(sol.countAndSay(9), "31131211131221");
  });
}

}  // namespace
