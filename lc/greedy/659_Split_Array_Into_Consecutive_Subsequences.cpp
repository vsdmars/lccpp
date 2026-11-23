// LeetCode 659: Split Array Into Consecutive Subsequences
// https://leetcode.com/problems/split-array-into-consecutive-subsequences/
//
// Problem:
//   Given an integer array `nums` sorted in non-decreasing order, return true
//   if and only if `nums` can be partitioned into one or more subsequences such
//   that every subsequence consists of strictly consecutive integers
//   (x, x + 1, x + 2, ...) and has length at least 3.
//
// Algorithm:
//   Greedy frequency-and-tail tracking in a single sorted pass:
//     1. Count all unused occurrences of every value in `remaining`.
//     2. Maintain `tails[v]`, the number of already-valid (length >= 3)
//        consecutive subsequences currently ending at value `v`.
//     3. Iterate `num` through `nums` in ascending order:
//        - If `remaining[num] == 0`, this copy was already consumed when
//          starting an earlier length-3 sequence; skip it.
//        - Prefer extending an existing sequence ending at `num - 1`
//          (`tails[num - 1] > 0`): move one tail from `num - 1` to `num` and
//          decrement `remaining[num]`.
//          (Crucial greedy invariant: extending an existing valid sequence is
//          always at least as good as opening a fresh triple [num, num+1,
//          num+2], because opening a fresh triple consumes future values
//          num+1 and num+2 that could otherwise strand smaller sequences — e.g.,
//          in [1, 2, 3, 4, 5, 5, 6, 7], extending yields [1..5] + [5..7],
//          whereas starting [4, 5, 6] eagerly strands 5 and 7.)
//        - Otherwise, if `remaining[num + 1] > 0` and `remaining[num + 2] > 0`,
//          open a new length-3 sequence [num, num + 1, num + 2] by decrementing
//          all three counts and incrementing `tails[num + 2]`.
//        - Otherwise, `num` cannot be placed legally; return false.
//
//   Time Complexity:  O(n) average time (one pass to count, one pass to place).
//   Space Complexity: O(n) for the two frequency hash maps.
//
// Data Structures:
//   - std::unordered_map<int, int> `remaining` tracking unused element counts.
//   - std::unordered_map<int, int> `tails` tracking active valid subsequences
//     keyed by their current last value.
//
// Real-World Applications:
//   - Card-game / tile-game hand validation (Rummy, Mahjong runs of length >= 3
//     with duplicate tiles).
//   - Packet sequence-number stream reconstruction where interleaved bursts
//     must each meet a minimum contiguous length.
//   - Contiguous reservation/slot block allocation in calendar or storage
//     defragmentation systems.
//
// Related LeetCode Problems:
//   - #846 Hand of Straights [done] (partitioning into fixed-size consecutive groups of length W)
//   - #1296 Divide Array in Sets of K Consecutive Numbers [new] (same as #846 with group size k)
//   - #128 Longest Consecutive Sequence [done] (finding the single longest consecutive run via hash set)
//   - #300 Longest Increasing Subsequence [done] (classic subsequence partitioning / patience-sorting family)

#include "framework/lc_framework.h"

#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff non-decreasing `nums` can be split into consecutive
  // integer subsequences each of length at least 3.
  // Time: O(n) average, Space: O(n).
  bool isPossible(const std::vector<int>& nums) {
    std::unordered_map<int, int> remaining;
    remaining.reserve(nums.size());
    for (const int num : nums) {
      ++remaining[num];
    }

    // tails[v] = count of valid subsequences (length >= 3) currently ending at
    // value `v`.
    std::unordered_map<int, int> tails;
    tails.reserve(nums.size());

    for (const int num : nums) {
      if (remaining[num] == 0) {
        // Already consumed as the middle or end element of an earlier triple.
        continue;
      }

      // First choice (greedy): extend an existing valid subsequence ending at
      // `num - 1` so we preserve `num + 1` and `num + 2` for future needs.
      if (auto tail_it = tails.find(num - 1);
          tail_it != tails.end() && tail_it->second > 0) {
        --(tail_it->second);
        ++tails[num];
        --remaining[num];
        continue;
      }

      // Second choice: start a brand-new minimal valid subsequence
      // [num, num + 1, num + 2].
      auto next1 = remaining.find(num + 1);
      auto next2 = remaining.find(num + 2);
      if (next1 != remaining.end() && next1->second > 0 &&
          next2 != remaining.end() && next2->second > 0) {
        --remaining[num];
        --(next1->second);
        --(next2->second);
        ++tails[num + 2];
        continue;
      }

      // Cannot extend any subsequence nor start a valid triple.
      return false;
    }

    return true;
  }
};

LC_PROBLEM(659, "Split Array Into Consecutive Subsequences", "greedy", ctx) {
  ctx.test("example_1_true", [&] {
    Solution sol;
    ctx.expect_true(sol.isPossible({1, 2, 3, 3, 4, 5}));
  });

  ctx.test("example_2_true", [&] {
    Solution sol;
    ctx.expect_true(sol.isPossible({1, 2, 3, 3, 4, 4, 5, 5}));
  });

  ctx.test("example_3_false", [&] {
    Solution sol;
    ctx.expect_false(sol.isPossible({1, 2, 3, 4, 4, 5}));
  });

  ctx.test("python_build_extend_before_start", [&] {
    Solution sol;
    // Extending [1,2,3] through 4,5 allows [5,6,7]; starting [4,5,6] would
    // fail.
    ctx.expect_true(sol.isPossible({1, 2, 3, 4, 5, 5, 6, 7}));
    ctx.expect_true(sol.isPossible({1, 1, 2, 2, 3, 3, 4, 5, 6, 7}));
  });

  ctx.test("negative_values_and_disjoint_runs", [&] {
    Solution sol;
    ctx.expect_true(sol.isPossible({-3, -2, -1, 0, 1}));
    ctx.expect_true(sol.isPossible({1, 2, 3, 10, 11, 12}));
  });

  ctx.test("too_short", [&] {
    Solution sol;
    ctx.expect_false(sol.isPossible({1, 2}));
    ctx.expect_false(sol.isPossible({5}));
  });
}

}  // namespace
