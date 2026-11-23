// LeetCode 552: Student Attendance Record II
// https://leetcode.com/problems/student-attendance-record-ii/
//
// Problem:
//   Given a positive integer n (1 <= n <= 100,000), return the number of
//   possible attendance records of length n that qualify for an attendance
//   award, modulo 10^9 + 7. Each character in a record is one of:
//     - 'A' (Absent)
//     - 'L' (Late)
//     - 'P' (Present)
//   A record is rewardable if and only if it contains strictly fewer than
//   two 'A's in total AND never contains three or more consecutive 'L's.
//
// Examples:
//   Example 1:
//     Input:  n = 2
//     Output: 8
//     Explanation:
//       - Allocate dp of size 3 with 'A'-free base counts dp[0] = 1, dp[1] = 2, and
//         dp[2] = 4 ("PP", "PL", "LP", "LL").
//       - Skip the i = 3..length recurrence loop because length is 2, and
//         initialize total = dp[2] = 4 for zero-'A' records.
//       - Add single-'A' splits: pos = 0 contributes dp[0] * dp[1] = 1 * 2 = 2 and
//         pos = 1 contributes dp[1] * dp[0] = 2 * 1 = 2.
//       - Sum total = 4 + 2 + 2 = 8 modulo 10^9 + 7 and return 8.
//
//   Example 2:
//     Input:  n = 4
//     Output: 43
//     Explanation:
//       - Start with dp[0] = 1, dp[1] = 2, dp[2] = 4 and compute Tribonacci entries
//         dp[3] = 4 + 2 + 1 = 7 and dp[4] = 7 + 4 + 2 = 13.
//       - Initialize total = dp[4] = 13 for valid records containing zero 'A's.
//       - Add single-'A' splits across pos = 0..3: dp[0]*dp[3] = 7, dp[1]*dp[2] =
//         8, dp[2]*dp[1] = 8, and dp[3]*dp[0] = 7.
//       - Accumulate total = 13 + 7 + 8 + 8 + 7 = 43 and return 43.
//
//   Example 3:
//     Input:  n = 1
//     Output: 3
//     Explanation:
//       - Pass the `n <= 0` guard and hit the early base case `if (n == 1)`.
//       - Return 3 immediately, accounting for the three valid length-1 records
//         "P", "L", and "A".
//
// Algorithm:
//   Split rewardable records into two disjoint families by the count of 'A':
//   1) Records with zero 'A's: let dp[i] be the number of valid strings of
//      length i over {'P', 'L'} containing no "LLL". Every valid string of
//      length i >= 3 ends with one of three mutually exclusive suffixes:
//        - "P"   (preceded by any valid length-(i-1) string): dp[i - 1]
//        - "PL"  (preceded by any valid length-(i-2) string): dp[i - 2]
//        - "PLL" (preceded by any valid length-(i-3) string): dp[i - 3]
//      This yields the Tribonacci recurrence:
//        dp[i] = (dp[i - 1] + dp[i - 2] + dp[i - 3]) mod (10^9 + 7),
//      with base cases dp[0] = 1, dp[1] = 2, dp[2] = 4.
//   2) Records with exactly one 'A': placing the single 'A' at 0-based index i
//      splits the record into an independent 'A'-free prefix of length i and
//      an independent 'A'-free suffix of length (n - 1 - i), contributing
//      (dp[i] * dp[n - 1 - i]) mod (10^9 + 7).
//   Summing dp[n] and all single-'A' splits modulo 10^9 + 7 gives the exact
//   answer.
//   Time Complexity: O(n).
//   Space Complexity: O(n) for the dp table of 'A'-free prefix/suffix counts.
//
// Data Structures:
//   - 1D DP array (`std::vector<std::int64_t>`): stores the number of valid
//     'A'-free records of every length 0..n so each single-'A' split can be
//     evaluated in O(1) time.
//
// Real-World Applications:
//   - Validating and enumerating constrained finite-state sequences in
//     protocol framing, run-length-limited (RLL) channel coding for magnetic
//     and optical storage, and SLA / compliance rule engines.
//   - Counting valid work-shift schedules under consecutive-shift and
//     absence-budget constraints.
//
// Related LeetCode Problems:
//   - #551 Student Attendance Record I [new] (decision version checking a single string)
//   - #70 Climbing Stairs [done] (simpler Fibonacci linear recurrence)
//   - #91 Decode Ways [done] (1D sequence-counting DP with local suffix rules)
//   - #1137 N-th Tribonacci Number [new] (same three-term linear recurrence)
//   - #1220 Count Vowels Permutation [new] (modular finite-automaton string counting)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::int64_t kMod = 1'000'000'007;

class Solution {
 public:
  // Computes the number of rewardable attendance records of length `n` modulo
  // 10^9 + 7 using the 'A'-free Tribonacci DP decomposition.
  // Time: O(n), Space: O(n).
  int checkRecord(int n) {
    // Guard non-positive lengths and handle n == 1 directly ("P", "L", "A").
    if (n <= 0) {
      return 0;
    }
    if (n == 1) {
      return 3;
    }

    const auto length = static_cast<std::size_t>(n);

    // dp[i] = number of valid 'A'-free strings of length i (only 'P' and 'L',
    // with no substring "LLL").
    //   dp[0] = 1 ("")
    //   dp[1] = 2 ("P", "L")
    //   dp[2] = 4 ("PP", "PL", "LP", "LL")
    std::vector<std::int64_t> dp(length + 1uz, 0);
    dp[0] = 1;
    dp[1] = 2;
    dp[2] = 4;

    // Fill 'A'-free counts via the three disjoint legal suffixes:
    // "...P", "...PL", and "...PLL".
    for (std::size_t i = 3uz; i <= length; ++i) {
      dp[i] = (dp[i - 1uz] + dp[i - 2uz] + dp[i - 3uz]) % kMod;
    }

    // Start with all valid records containing zero 'A's.
    std::int64_t total = dp[length];

    // Add records containing exactly one 'A' placed at index `pos`.
    // The prefix [0, pos) and suffix (pos, length) are independent 'A'-free
    // valid records, and the 'A' breaks any consecutive 'L' run across `pos`.
    for (std::size_t pos = 0uz; pos < length; ++pos) {
      const std::int64_t ways_with_a =
          (dp[pos] * dp[(length - 1uz) - pos]) % kMod;
      total = (total + ways_with_a) % kMod;
    }

    return static_cast<int>(total);
  }
};

// Independent 6-state DFA reference implementation (state = absences * 3 +
// trailing_lates) used in unit tests to verify large-n modular correctness.
// Time: O(n), Space: O(1).
int checkRecordAutomaton(int n) {
  if (n <= 0) {
    return 0;
  }
  // state[a][l]: number of records with `a` absences (0..1) and `l` trailing
  // lates (0..2).
  std::array<std::array<std::int64_t, 3>, 2> cur{};
  cur[0][0] = 1;

  for (int step = 0; step < n; ++step) {
    std::array<std::array<std::int64_t, 3>, 2> next{};
    for (std::size_t a = 0uz; a < 2uz; ++a) {
      for (std::size_t l = 0uz; l < 3uz; ++l) {
        const std::int64_t val = cur[a][l];
        if (val == 0) {
          continue;
        }
        // Append 'P': resets trailing 'L' run to 0.
        next[a][0] = (next[a][0] + val) % kMod;
        // Append 'A': allowed only if no prior 'A'; resets trailing 'L' to 0.
        if (a == 0uz) {
          next[1][0] = (next[1][0] + val) % kMod;
        }
        // Append 'L': allowed only if trailing 'L' count is < 2.
        if (l < 2uz) {
          next[a][l + 1uz] = (next[a][l + 1uz] + val) % kMod;
        }
      }
    }
    cur = next;
  }

  std::int64_t sum = 0;
  for (const auto& row : cur) {
    for (const std::int64_t val : row) {
      sum = (sum + val) % kMod;
    }
  }
  return static_cast<int>(sum);
}

LC_PROBLEM(552, "Student Attendance Record II", "dp", ctx) {
  ctx.test("example_n2", [&] {
    Solution sol;
    ctx.expect_eq(sol.checkRecord(2), 8);
  });

  ctx.test("small_values", [&] {
    Solution sol;
    ctx.expect_eq(sol.checkRecord(0), 0);
    ctx.expect_eq(sol.checkRecord(1), 3);
    ctx.expect_eq(sol.checkRecord(3), 19);
    ctx.expect_eq(sol.checkRecord(4), 43);
    ctx.expect_eq(sol.checkRecord(5), 94);
  });

  ctx.test("matches_automaton_medium_and_large", [&] {
    Solution sol;
    for (const int n : {10, 50, 500, 10101}) {
      ctx.expect_eq(sol.checkRecord(n), checkRecordAutomaton(n));
    }
  });

  ctx.test("max_constraint_in_mod_range", [&] {
    Solution sol;
    const int res = sol.checkRecord(100000);
    ctx.expect_true(res >= 0 && res < static_cast<int>(kMod));
    ctx.expect_eq(res, checkRecordAutomaton(100000));
  });
}

}  // namespace
