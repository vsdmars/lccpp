// LeetCode 846: Hand Of Straights
// https://leetcode.com/problems/hand-of-straights/
//
// Problem:
//   Alice has a hand of cards represented by an integer array `hand`, where
//   `hand[i]` is the value written on the i-th card, and an integer
//   `group_size`. Determine whether she can rearrange all cards into groups of
//   exact size `group_size` such that every group consists of `group_size`
//   consecutive integers. Return `true` if such a partition exists, and `false`
//   otherwise.
//   Constraints: 1 <= hand.length <= 10^4, 0 <= hand[i] <= 10^9,
//   1 <= group_size <= hand.length.
//
// Examples:
//   Example 1:
//     Input:  hand = [1, 2, 3, 6, 2, 3, 4, 7, 8], group_size = 3
//     Output: true
//     Explanation:
//       - Check hand.size() % 3 == 0 (9 % 3 == 0) and build ordered frequency map
//         counts = {1: 1, 2: 2, 3: 2, 4: 1, 6: 1, 7: 1, 8: 1}.
//       - Start at smallest key 1 (needed = 1): decrement counts[2] and counts[3]
//         by 1, leaving {2: 1, 3: 1, 4: 1, 6: 1, 7: 1, 8: 1}.
//       - Next active key 2 (needed = 1) consumes 3 and 4; then key 6 (needed = 1)
//         consumes 7 and 8, exhausting counts and returning true.
//
//   Example 2:
//     Input:  hand = [5, 5, 6, 7, 12, 13, 7, 8, 9], group_size = 3
//     Output: false
//     Explanation:
//       - Pass the divisibility check (9 % 3 == 0) and build counts = {5: 2, 6: 1,
//         7: 2, 8: 1, 9: 1, 12: 1, 13: 1}.
//       - At smallest key 5 (needed = 2), consume_straights checks 5 + 1 = 6 and
//         sees counts[6] = 1 < 2, returning false immediately.
//
//   Example 3:
//     Input:  hand = [1, 2, 3, 4, 5], group_size = 4
//     Output: false
//     Explanation:
//       - Evaluate the divisibility guard hand.size() % width = 5 % 4 = 1 != 0.
//       - Return false immediately without building the frequency map because 5
//         cards cannot form equal groups of 4.
//
// Algorithm:
//   Greedy frequency-map partitioning in ascending key order:
//   1. Quick divisibility guard: if `hand.size()` is not divisible by
//      `group_size`, no partition is possible.
//   2. Count occurrences of every card value in a sorted map (`std::map`).
//   3. Iterate through keys in ascending order. Whenever the smallest remaining
//      card `start` has count `needed > 0`, every single one of those `needed`
//      copies MUST serve as the minimum card of a consecutive straight
//      `[start, start + 1, ..., start + group_size - 1]`.
//   4. Decrement the count of each card `start + offset` (`1 <= offset <
//      group_size`) by `needed`. If any required card is missing or has count
//      `< needed`, return `false`.
//   5. If all keys are exhausted without failure, return `true`.
//   Time Complexity: O(n log u), where n = hand.size() and u <= n is the number
//   of distinct card values. Each card is consumed in exactly one group, so at
//   most n / group_size groups perform group_size map lookups each.
//   Space Complexity: O(u) for the ordered frequency map.
//
// Data Structures:
//   - Ordered hash/tree map (`std::map<int, int>`): stores `{card_value ->
//     remaining_count}` in sorted key order so the globally smallest active
//     card is always visited first without a separate sort pass.
//
// Real-World Applications:
//   - Contiguous resource/slot allocation: grouping fragmented memory pages,
//     contiguous IP addresses, or sequential time slots into fixed-width
//     blocks.
//   - Batch job and packet reassembly: verifying whether buffered sequence
//     numbers can be partitioned into complete fixed-size windows.
//   - Inventory kitting and tabletop game engines: checking whether mixed stock
//     or tiles can form valid sequential runs (e.g., Rummikub, Mahjong).
//
// Related LeetCode Problems:
//   - #1296 Divide Array in Sets of K Consecutive Numbers [new] (identical problem statement and greedy map algorithm)
//   - #659 Split Array into Consecutive Subsequences [done] (variable-length consecutive groups of size >= 3 using greedy frequency maps)
//   - #128 Longest Consecutive Sequence [done] (consecutive integer chain detection using hash sets)
//   - #347 Top K Frequent Elements [done] (frequency counting and ordered selection)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `hand` can be partitioned into consecutive groups of
  // `group_size` cards.
  // Time: O(n log u), Space: O(u), where u is the number of distinct values.
  bool isNStraightHand(const std::vector<int>& hand, int group_size) {
    if (group_size <= 0) {
      return false;
    }
    const auto width = static_cast<std::size_t>(group_size);
    // Every group consumes exactly `width` cards, so total count must divide
    // evenly.
    if ((hand.size() % width) != 0uz) {
      return false;
    }
    // Trivial case: any hand can be partitioned into singleton groups.
    if (group_size == 1) {
      return true;
    }

    // Build an ordered frequency table so keys are iterated smallest-first.
    std::map<int, int> counts;
    for (const int card : hand) {
      ++counts[card];
    }

    // Greedily anchor `needed` straights at the smallest remaining card value.
    for (auto& [start, needed] : counts) {
      if (needed == 0) {
        continue;
      }
      if (!consume_straights(counts, start, group_size, needed)) {
        return false;
      }
      needed = 0;
    }

    return true;
  }

 private:
  // Consumes `needed` copies of cards `[start + 1 .. start + group_size - 1]`
  // from `counts`. Returns false if any required value overflows `int` or lacks
  // enough remaining copies.
  static bool consume_straights(std::map<int, int>& counts, int start,
                                int group_size, int needed) {
    for (int offset = 1; offset < group_size; ++offset) {
      // Guard against signed integer overflow when `start` is near INT_MAX.
      if (start > std::numeric_limits<int>::max() - offset) {
        return false;
      }
      const auto it = counts.find(start + offset);
      if (it == counts.end() || it->second < needed) {
        return false;
      }
      it->second -= needed;
    }
    return true;
  }
};

LC_PROBLEM(846, "Hand Of Straights", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_valid_groups_of_3", [&] {
    const std::vector<int> hand = {1, 2, 3, 6, 2, 3, 4, 7, 8};
    ctx.expect_true(sol.isNStraightHand(hand, 3));
  });

  ctx.test("example_2_indivisible_length", [&] {
    const std::vector<int> hand = {1, 2, 3, 4, 5};
    ctx.expect_false(sol.isNStraightHand(hand, 4));
  });

  ctx.test("python_build_duplicate_shortfall", [&] {
    // Two 5s require two 6s, but only one 6 is present.
    const std::vector<int> hand = {5, 5, 6, 7, 12, 13, 7, 8, 9};
    ctx.expect_false(sol.isNStraightHand(hand, 3));
  });

  ctx.test("python_build_pairs", [&] {
    const std::vector<int> hand = {1, 2, 3, 4, 5, 6};
    ctx.expect_true(sol.isNStraightHand(hand, 2));
  });

  ctx.test("overlapping_duplicate_straights", [&] {
    const std::vector<int> hand = {1, 1, 2, 2, 3, 3};
    ctx.expect_true(sol.isNStraightHand(hand, 3));
  });

  ctx.test("shared_middle_card", [&] {
    // Can be partitioned into [1, 2] and [2, 3].
    const std::vector<int> hand = {1, 2, 2, 3};
    ctx.expect_true(sol.isNStraightHand(hand, 2));
  });

  ctx.test("group_size_one_always_valid", [&] {
    const std::vector<int> hand = {9, 1, 100, 9};
    ctx.expect_true(sol.isNStraightHand(hand, 1));
  });

  ctx.test("zero_and_large_values", [&] {
    const std::vector<int> hand = {0, 1, 2, 999999998, 999999999, 1000000000};
    ctx.expect_true(sol.isNStraightHand(hand, 3));
  });
}

}  // namespace
