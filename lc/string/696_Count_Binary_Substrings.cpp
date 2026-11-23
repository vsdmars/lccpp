// LeetCode 696: Count Binary Substrings
// https://leetcode.com/problems/count-binary-substrings/
//
// Problem:
//   Given a binary string `s` consisting only of '0' and '1', return the number
//   of non-empty contiguous substrings that contain an equal number of '0's and
//   '1's where all '0's are grouped consecutively and all '1's are grouped
//   consecutively. Identical substrings occurring at different positions are
//   counted separately.
//
// Algorithm:
//   Any valid substring must straddle exactly one transition boundary between a
//   maximal run of identical bits and the immediately following maximal run of
//   the opposite bit. If two adjacent runs have lengths `prev_run` and
//   `curr_run`, they yield exactly `min(prev_run, curr_run)` valid substrings
//   (one for each half-length k in [1, min(prev_run, curr_run)]).
//   We scan `s` once, maintaining `prev_run` (length of the previous maximal
//   run) and `curr_run` (length of the active run). At each character:
//     - If `s[i] == s[i - 1]`, extend `curr_run`.
//     - Otherwise, a transition occurs: set `prev_run = curr_run` and reset
//       `curr_run = 1`.
//     - Whenever `curr_run <= prev_run`, the newly extended character completes
//       exactly one new balanced substring of length `2 * curr_run`, so we
//       increment `count`.
//   Time Complexity:  O(n), where n = |s| (single linear pass).
//   Space Complexity: O(1) auxiliary state (two integer run counters).
//
// Data Structures:
//   - Scalar run-length counters (`prev_run`, `curr_run`) replacing an explicit
//     run-length array by observing that only the two most recent runs interact.
//
// Real-World Applications:
//   - Run-length transition analysis in digital signal processing, Manchester
//     encoding, and pulse-width modulation (PWM) bitstreams.
//   - Detecting balanced toggle windows in binary telemetry and hardware
//     debouncing pipelines.
//   - Streaming run-length encoding (RLE) feature extraction in compression
//     engines.
//
// Related LeetCode Problems:
//   - #485 Max Consecutive Ones [done] (single-pass consecutive run-length tracking)
//   - #525 Contiguous Array [done] (equal 0s and 1s without consecutive-grouping constraint)
//   - #443 String Compression [done] (run-length encoding of consecutive characters)
//   - #1759 Count Number of Homogenous Substrings [new] (counting substrings from run lengths)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Counts all non-empty substrings with equal, consecutively grouped 0s and 1s.
  // Time: O(n), Space: O(1).
  int countBinarySubstrings(std::string_view s) {
    if (s.size() < 2uz) {
      return 0;
    }

    int count = 0;
    int prev_run = 0;  // Length of the immediately preceding maximal bit run.
    int curr_run = 1;  // Length of the current maximal bit run ending at i.

    for (std::size_t i = 1uz; i < s.size(); ++i) {
      if (s[i] == s[i - 1uz]) {
        // Still inside the same run of identical characters.
        ++curr_run;
      } else {
        // Transition between '0' and '1': current run becomes previous run.
        prev_run = curr_run;
        curr_run = 1;
      }

      // Every step while curr_run <= prev_run uniquely forms one valid
      // substring of length 2 * curr_run centered at the last transition.
      if (curr_run <= prev_run) {
        ++count;
      }
    }

    return count;
  }
};

LC_PROBLEM(696, "Count Binary Substrings", "string", ctx) {
  Solution sol;

  ctx.test("example_1_pairs_of_two", [&] {
    ctx.expect_eq(sol.countBinarySubstrings("00110011"), 6);
  });

  ctx.test("example_2_alternating", [&] {
    ctx.expect_eq(sol.countBinarySubstrings("10101"), 4);
  });

  ctx.test("single_character", [&] {
    ctx.expect_eq(sol.countBinarySubstrings("0"), 0);
  });

  ctx.test("all_identical_bits", [&] {
    ctx.expect_eq(sol.countBinarySubstrings("11111"), 0);
  });

  ctx.test("asymmetric_run_lengths", [&] {
    ctx.expect_eq(sol.countBinarySubstrings("00011"), 2);
  });

  ctx.test("multiple_varied_runs", [&] {
    // Runs: 3, 1, 2, 4 -> min(3,1) + min(1,2) + min(2,4) = 1 + 1 + 2 = 4.
    ctx.expect_eq(sol.countBinarySubstrings("0001001111"), 4);
  });
}

}  // namespace
