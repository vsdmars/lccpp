// LeetCode 763: Partition Labels
// https://leetcode.com/problems/partition-labels/
//
// Problem:
//   Given a string s consisting of lowercase English letters
//   (1 <= s.length <= 500), partition s into as many contiguous parts as
//   possible so that each letter appears in at most one part. Return a vector
//   of integers giving the length of each part in order.
//
// Examples:
//   Example 1:
//     Input:  s = "ababcbacadefegdehijhklij"
//     Output: [9, 7, 8]
//     Explanation:
//       - Pass 1 records rightmost indices: 'a': 8, 'b': 5, 'c': 7, 'd': 14, 'e':
//         15, 'f': 11, 'g': 13, 'h': 19, 'i': 22, 'j': 23, 'k': 20, 'l': 21.
//       - Starting at part_start = 0, i = 0 ('a') sets part_end = 8, indices 1..7
//         stay within 8, and at i = 8 == part_end we close a part of length 9 and
//         set part_start = 9.
//       - From part_start = 9, 'd' sets part_end = 14 and 'e' at i = 10 expands
//         part_end to 15; at i = 15 == part_end we close a part of length 7 and set
//         part_start = 16.
//       - From part_start = 16, 'h', 'i', and 'j' push part_end to 19, 22, and 23,
//         closing the final part of length 8 at i = 23 and returning [9, 7, 8].
//
//   Example 2:
//     Input:  s = "eccbbbbdec"
//     Output: [10]
//     Explanation:
//       - Pass 1 records last_pos['e'] = 8, last_pos['c'] = 9, last_pos['b'] = 6,
//         and last_pos['d'] = 7.
//       - At i = 0 ('e'), part_end becomes 8; at i = 1 ('c'), part_end expands to
//         max(8, 9) = 9.
//       - Indices 2..8 never reach part_end = 9, and at i = 9 == part_end a single
//         partition of length (9 - 0) + 1 = 10 is closed, returning [10].
//
//   Example 3:
//     Input:  s = "abcdef"
//     Output: [1, 1, 1, 1, 1, 1]
//     Explanation:
//       - Pass 1 sets last_pos for 'a'..'f' to 0, 1, 2, 3, 4, and 5 respectively.
//       - At each index i from 0 to 5, part_end = max(part_end, i) = i immediately
//         matches i == part_end.
//       - Each step pushes length (i - i) + 1 = 1 and advances part_start to i + 1,
//         returning [1, 1, 1, 1, 1, 1].
//
// Algorithm:
//   First pass: record the last occurrence index of each letter 'a'..'z' in a
//   fixed 26-element table.
//   Second pass: scan s from left to right maintaining the current partition's
//   start index and required end boundary (`partition_end`), defined as the
//   maximum last-occurrence index among all characters seen in the current
//   part. Whenever the scan index i catches up to `partition_end`, no letter
//   seen in [start, i] appears anywhere after i, so we greedily close the
//   partition to maximize the total number of parts.
//   A companion remaining-frequency active-count variant (mirroring the
//   original Python solution) is also included for comparison.
//   Time Complexity: O(n), where n = s.size().
//   Space Complexity: O(1) auxiliary space (26-element fixed array), excluding
//   the output vector.
//
// Data Structures:
//   std::array<std::size_t, 26>: constant-size lookup table mapping each
//   lowercase letter to its rightmost index in s (and frequency/active arrays
//   in the remaining-count variant).
//
// Real-World Applications:
//   - Splitting event logs, transaction streams, or compiler basic-block live
//     ranges into independent, non-overlapping shards so entities never cross
//     partition boundaries.
//   - Interval-merging and independent batch scheduling in distributed
//     data-processing pipelines.
//
// Related LeetCode Problems:
//   - #56 Merge Intervals [done] (equivalent view of merging each letter's span)
//   - #55 Jump Game [done] (greedy farthest-reachable boundary sweep)
//   - #45 Jump Game II [done] (greedy level/boundary partitioning)
//   - #2405 Optimal Partition Of String [done] (greedy string partitioning)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26;

// Maps a lowercase ASCII letter 'a'..'z' to its 0-based table index.
constexpr std::size_t letterIndex(char ch) {
  return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                  static_cast<unsigned char>('a'));
}

class Solution {
 public:
  // Greedy last-occurrence sweep: closes a partition at the earliest index
  // where every character seen so far has reached its final occurrence.
  // Time: O(n), Space: O(1) auxiliary.
  std::vector<int> partitionLabels(std::string_view s) {
    if (s.empty()) {
      return {};
    }

    // Step 1: Record the rightmost index of every letter in s.
    std::array<std::size_t, kAlphabetSize> last_pos{};
    for (std::size_t i = 0uz; i < s.size(); ++i) {
      last_pos[letterIndex(s[i])] = i;
    }

    // Step 2: Sweep left-to-right, expanding the current partition's right
    // boundary to cover the last occurrence of every character included so far.
    std::vector<int> result;
    std::size_t part_start = 0uz;
    std::size_t part_end = 0uz;

    for (std::size_t i = 0uz; i < s.size(); ++i) {
      part_end = std::max(part_end, last_pos[letterIndex(s[i])]);

      // Invariant: no character in [part_start, i] appears after part_end.
      // When i reaches part_end, cutting immediately maximizes partition count.
      if (i == part_end) {
        result.push_back(static_cast<int>((i - part_start) + 1uz));
        part_start = i + 1uz;
      }
    }

    return result;
  }

  // Remaining-count active-set variant (mirrors the Python Counter + set
  // solution using O(1) fixed arrays): closes a partition as soon as the count
  // of currently open letters with remaining future occurrences drops to zero.
  // Time: O(n), Space: O(1) auxiliary.
  std::vector<int> partitionLabelsActiveCount(std::string_view s) {
    if (s.empty()) {
      return {};
    }

    std::array<int, kAlphabetSize> remaining{};
    for (char ch : s) {
      ++remaining[letterIndex(ch)];
    }

    std::array<bool, kAlphabetSize> in_part{};
    std::vector<int> result;
    int active_letters = 0;
    std::size_t part_start = 0uz;

    for (std::size_t i = 0uz; i < s.size(); ++i) {
      const std::size_t idx = letterIndex(s[i]);
      if (!in_part[idx]) {
        in_part[idx] = true;
        ++active_letters;
      }
      --remaining[idx];

      // Once the last copy of this letter is consumed, it no longer blocks a
      // cut at index i.
      if (remaining[idx] == 0) {
        in_part[idx] = false;
        --active_letters;
        if (active_letters == 0) {
          result.push_back(static_cast<int>((i - part_start) + 1uz));
          part_start = i + 1uz;
        }
      }
    }

    return result;
  }
};

LC_PROBLEM(763, "Partition Labels", "greedy", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    const std::string_view input = "ababcbacadefegdehijhklij";
    const std::vector<int> expected = {9, 7, 8};
    ctx.expect_eq(sol.partitionLabels(input), expected);
    ctx.expect_eq(sol.partitionLabelsActiveCount(input), expected);
  });

  ctx.test("example_2_merged_single_part", [&] {
    const std::string_view input = "eccbbbbdec";
    const std::vector<int> expected = {10};
    ctx.expect_eq(sol.partitionLabels(input), expected);
    ctx.expect_eq(sol.partitionLabelsActiveCount(input), expected);
  });

  ctx.test("all_unique_characters", [&] {
    const std::string_view input = "abcdef";
    const std::vector<int> expected = {1, 1, 1, 1, 1, 1};
    ctx.expect_eq(sol.partitionLabels(input), expected);
    ctx.expect_eq(sol.partitionLabelsActiveCount(input), expected);
  });

  ctx.test("all_identical_and_single_char", [&] {
    ctx.expect_eq(sol.partitionLabels("zzzzz"), std::vector<int>{5});
    ctx.expect_eq(sol.partitionLabelsActiveCount("zzzzz"), std::vector<int>{5});
    ctx.expect_eq(sol.partitionLabels("a"), std::vector<int>{1});
    ctx.expect_eq(sol.partitionLabelsActiveCount("a"), std::vector<int>{1});
  });

  ctx.test("empty_string", [&] {
    ctx.expect_eq(sol.partitionLabels(""), std::vector<int>{});
    ctx.expect_eq(sol.partitionLabelsActiveCount(""), std::vector<int>{});
  });
}

}  // namespace
