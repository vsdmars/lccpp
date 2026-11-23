// LeetCode 2405: Optimal Partition Of String
// https://leetcode.com/problems/optimal-partition-of-string/
//
// Problem:
//   Given a string s consisting of lowercase English letters
//   (1 <= s.length <= 10^5), partition s into one or more contiguous
//   substrings such that every character within each substring is unique.
//   Return the minimum number of substrings in such a partition.
//
// Algorithm:
//   Greedy left-to-right scan: extending the current substring as far right as
//   possible is always optimal by a standard greedy exchange argument — cutting
//   earlier can never reduce the number of parts required for the remaining
//   suffix. Track the characters present in the current substring using a
//   26-bit bitmask (`seen_mask`). When the next character's bit is already set,
//   close the current substring, increment the partition count, reset the
//   bitmask, and start a new substring with the current character.
//   Time Complexity: O(n), where n = s.size().
//   Space Complexity: O(1) — a single 32-bit unsigned integer bitmask.
//
// Data Structures:
//   std::uint32_t bitmask: compact O(1)-space set representing the subset of
//   the 26 lowercase English letters already present in the active substring.
//
// Real-World Applications:
//   - Packetizing streaming symbols or key-update batches into the minimum
//     number of conflict-free micro-batches where each key appears at most once
//     per batch.
//   - Register-allocation and instruction-bundling passes that group sequential
//     operations without write-after-write resource collisions.
//
// Related LeetCode Problems:
//   - #763 Partition Labels [done] (greedy string partitioning by last index)
//   - #3 Longest Substring Without Repeating Characters [done] (unique window)
//   - #159 Longest Substring with At Most Two Distinct Characters [done] (window)
//   - #2461 Maximum Sum of Distinct Subarrays With Length K [new] (distinct runs)

#include "framework/lc_framework.h"

#include <cstdint>
#include <string_view>

namespace {

// Returns the 1-bit mask corresponding to a lowercase letter 'a'..'z'.
constexpr std::uint32_t letterBit(char ch) {
  const auto shift = static_cast<unsigned>(static_cast<unsigned char>(ch) -
                                           static_cast<unsigned char>('a'));
  return 1u << shift;
}

class Solution {
 public:
  // Returns the minimum number of contiguous substrings with all unique
  // characters that partition s.
  // Time: O(n), Space: O(1).
  int partitionString(std::string_view s) {
    if (s.empty()) {
      return 0;
    }

    int partitions = 1;
    // Bit i is 1 iff letter ('a' + i) is already in the current substring.
    std::uint32_t seen_mask = 0u;

    for (char ch : s) {
      const std::uint32_t bit = letterBit(ch);
      if ((seen_mask & bit) != 0u) {
        // Duplicate encountered: greedily end the previous substring right
        // before `ch` and begin a new one.
        ++partitions;
        seen_mask = 0u;
      }
      seen_mask |= bit;
    }

    return partitions;
  }
};

LC_PROBLEM(2405, "Optimal Partition Of String", "greedy", ctx) {
  Solution sol;

  ctx.test("example_1_abacaba", [&] {
    ctx.expect_eq(sol.partitionString("abacaba"), 4);
  });

  ctx.test("example_2_all_same", [&] {
    ctx.expect_eq(sol.partitionString("ssssss"), 6);
  });

  ctx.test("all_unique_alphabet", [&] {
    ctx.expect_eq(sol.partitionString("abcdefghijklmnopqrstuvwxyz"), 1);
  });

  ctx.test("single_character_and_empty", [&] {
    ctx.expect_eq(sol.partitionString("a"), 1);
    ctx.expect_eq(sol.partitionString(""), 0);
  });

  ctx.test("repeated_pairs", [&] {
    // "ab" | "ab" | "ab" -> 3 partitions.
    ctx.expect_eq(sol.partitionString("ababab"), 3);
    // "hdklqkcssgxlvehva": "hdklq" | "kcs" | "sgxlveh" | "va" -> 4 partitions.
    ctx.expect_eq(sol.partitionString("hdklqkcssgxlvehva"), 4);
  });
}

}  // namespace
