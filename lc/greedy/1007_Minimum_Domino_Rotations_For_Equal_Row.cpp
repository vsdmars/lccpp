// LeetCode 1007: Minimum Domino Rotations For Equal Row
// https://leetcode.com/problems/minimum-domino-rotations-for-equal-row/
//
// Problem:
//   In a row of n dominoes (2 <= n <= 20000), tops[i] and bottoms[i] represent
//   the top and bottom values (integers from 1 to 6) of the i-th domino. You
//   may rotate any domino i to swap tops[i] and bottoms[i]. Return the minimum
//   number of rotations required so that either every value in tops is equal
//   or every value in bottoms is equal, or return -1 if it is impossible.
//
// Examples:
//   Example 1:
//     Input:  tops = [2, 1, 2, 4, 2, 2], bottoms = [5, 2, 6, 2, 3, 2]
//     Output: 2
//     Explanation:
//       - Test candidate target = tops[0] = 2 in rotationsForTarget across i =
//         0..5.
//       - Every domino contains 2; tops[i] != 2 at indices 1 and 3 gives
//         flip_to_top = 2, while bottoms[i] != 2 at indices 0, 2, and 4 gives
//         flip_to_bottom = 3.
//       - Return std::min(2, 3) = 2 immediately without needing to check bottoms[0]
//         = 5.
//
//   Example 2:
//     Input:  tops = [3, 5, 1, 2, 3], bottoms = [3, 6, 3, 3, 4]
//     Output: -1
//     Explanation:
//       - Test candidate target = tops[0] = 3; at i = 1, tops[1] = 5 != 3 and
//         bottoms[1] = 6 != 3, so rotationsForTarget returns -1.
//       - Since bottoms[0] == tops[0] == 3, skip the duplicate second candidate and
//         return -1.
//
//   Example 3:
//     Input:  tops = [5, 2, 2, 2], bottoms = [2, 6, 4, 1]
//     Output: 1
//     Explanation:
//       - Test target = tops[0] = 5 first; at i = 1, neither tops[1] = 2 nor
//         bottoms[1] = 6 equals 5, returning from_top = -1.
//       - Because bottoms[0] = 2 != tops[0] = 5, evaluate rotationsForTarget(2,
//         tops, bottoms).
//       - All dominoes contain 2, with flip_to_top = 1 (from i = 0) and
//         flip_to_bottom = 3 (from i = 1, 2, 3), returning std::min(1, 3) = 1.
//
// Algorithm:
//   If a valid uniform row exists, its common value `target` MUST appear on
//   the very first domino, leaving at most two candidates: tops[0] and
//   bottoms[0]. For a candidate `target`, scan all n dominoes once:
//   - If neither tops[i] nor bottoms[i] equals `target`, `target` cannot form
//     a uniform row.
//   - Otherwise, count how many dominoes need a flip to put `target` on top
//     (`tops[i] != target`) and how many need a flip to put `target` on bottom
//     (`bottoms[i] != target`).
//   If tops[0] succeeds, we can return immediately without checking bottoms[0]:
//   if both tops[0] and bottoms[0] (when distinct) appear on every domino,
//   every domino is the pair {tops[0], bottoms[0]} in some orientation, so the
//   minimum rotation count is identical for both candidates.
//   Time Complexity: O(n) — at most two linear passes over the dominoes.
//   Space Complexity: O(1) — only scalar counters are maintained.
//
// Data Structures:
//   Const references to the input std::vector<int> arrays plus constant-space
//   integer counters for top and bottom flip counts.
//
// Real-World Applications:
//   - Dual-rail / primary-backup failover alignment: choosing the minimum
//     number of channel swaps needed to standardize all nodes onto a single
//     protocol version or active circuit path.
//   - Hardware pin-polarity and configuration-lane harmonization in PCB/FPGA
//     routing where each lane supports two interchangeable orientations.
//
// Related LeetCode Problems:
//   - #134 Gas Station [done] (greedy candidate elimination in O(n))
//   - #169 Majority Element [done] (O(1)-space candidate verification)
//   - #871 Minimum Number of Refueling Stops [new] (greedy minimum operations)
//   - #1221 Split a String in Balanced Strings [new] (single-pass greedy count)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum rotations to make all values in `tops` or all values
  // in `bottoms` identical, or -1 if impossible.
  // Time: O(n), Space: O(1).
  int minDominoRotations(const std::vector<int>& tops,
                         const std::vector<int>& bottoms) {
    if (tops.empty() || tops.size() != bottoms.size()) {
      return -1;
    }

    // Any valid uniform row must match either tops[0] or bottoms[0].
    const int from_top = rotationsForTarget(tops[0], tops, bottoms);
    if (from_top != -1) {
      // If both tops[0] and bottoms[0] were feasible, every domino would
      // contain both values, giving the exact same minimum rotation count.
      return from_top;
    }

    if (bottoms[0] == tops[0]) {
      return -1;
    }
    return rotationsForTarget(bottoms[0], tops, bottoms);
  }

 private:
  // Computes the minimum rotations needed to make either row all equal to
  // `target`, or returns -1 if some domino lacks `target` on both halves.
  static int rotationsForTarget(int target, const std::vector<int>& tops,
                                const std::vector<int>& bottoms) {
    int flip_to_top = 0;
    int flip_to_bottom = 0;

    for (std::size_t i = 0uz; i < tops.size(); ++i) {
      if (tops[i] != target && bottoms[i] != target) {
        return -1;
      }
      // If a domino has `target` on both halves, neither counter increments.
      if (tops[i] != target) {
        ++flip_to_top;
      } else if (bottoms[i] != target) {
        ++flip_to_bottom;
      }
    }

    return std::min(flip_to_top, flip_to_bottom);
  }
};

LC_PROBLEM(1007, "Minimum Domino Rotations For Equal Row", "greedy", ctx) {
  Solution sol;

  ctx.test("example_1_two_rotations", [&] {
    const std::vector<int> tops = {2, 1, 2, 4, 2, 2};
    const std::vector<int> bottoms = {5, 2, 6, 2, 3, 2};
    ctx.expect_eq(sol.minDominoRotations(tops, bottoms), 2);
  });

  ctx.test("example_2_impossible", [&] {
    const std::vector<int> tops = {3, 5, 1, 2, 3};
    const std::vector<int> bottoms = {3, 6, 3, 3, 4};
    ctx.expect_eq(sol.minDominoRotations(tops, bottoms), -1);
  });

  ctx.test("python_build_cases", [&] {
    ctx.expect_eq(
        sol.minDominoRotations({2, 1, 1, 3, 2, 1, 2, 2, 1},
                               {3, 2, 3, 1, 3, 2, 3, 3, 2}),
        -1);
    ctx.expect_eq(sol.minDominoRotations({1, 1, 1}, {1, 1, 1}), 0);
    ctx.expect_eq(sol.minDominoRotations({2, 1, 1, 3}, {3, 2, 3, 1}), -1);
  });

  ctx.test("bottom_candidate_only", [&] {
    // tops[0] = 5 fails at index 1, while bottoms[0] = 2 succeeds with 1 flip.
    const std::vector<int> tops = {5, 2, 2, 2};
    const std::vector<int> bottoms = {2, 6, 4, 1};
    ctx.expect_eq(sol.minDominoRotations(tops, bottoms), 1);
  });

  ctx.test("both_candidates_feasible", [&] {
    const std::vector<int> tops = {1, 2, 1, 2, 1};
    const std::vector<int> bottoms = {2, 1, 2, 1, 2};
    ctx.expect_eq(sol.minDominoRotations(tops, bottoms), 2);
  });
}

}  // namespace
