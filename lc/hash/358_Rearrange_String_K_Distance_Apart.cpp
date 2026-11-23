// LeetCode 358: Rearrange String K Distance Apart
// https://leetcode.com/problems/rearrange-string-k-distance-apart/
//
// Problem:
//   Given a string s of lowercase English letters and an integer k, rearrange
//   the characters of s so that any two occurrences of the same character are
//   separated by at least distance k (i.e., |idx1 - idx2| >= k). Return any
//   valid rearrangement, or an empty string "" if no valid arrangement exists.
//
// Algorithm:
//   Greedy scheduling with a max-heap and a cooldown queue:
//   1. If k <= 1, no separation constraint applies, so return s immediately.
//   2. Count the frequency of all 26 lowercase letters and push every present
//      (count, char) pair into a max-heap so the most frequent available letter
//      is always selected first (minimizing the risk of getting trapped with
//      excess copies of one letter at the end).
//   3. Repeatedly pop the highest-frequency available character from the heap,
//      append it to the output, decrement its remaining count, and enqueue it
//      into a FIFO cooldown queue.
//   4. Once the cooldown queue holds k entries, its front character was placed
//      exactly k steps ago and is now eligible again: pop it from the queue
//      and push it back onto the max-heap if its remaining count is positive.
//   5. When the max-heap becomes empty, every character was placed iff
//      result.size() == s.size(); otherwise no valid rearrangement exists.
//   Time Complexity: O(n log 26) = O(n), where n = s.size().
//   Space Complexity: O(26) = O(1) auxiliary (excluding the O(n) result).
//
// Data Structures:
//   - std::array<int, 26>: exact character frequency histogram.
//   - std::priority_queue<std::pair<int, char>>: max-heap selecting the
//     highest-frequency currently available character in O(log 26) time.
//   - std::queue<std::pair<int, char>>: FIFO cooldown window of length k that
//     releases each used character exactly k positions after its placement.
//
// Real-World Applications:
//   - CPU / GPU task scheduling with cooldown intervals between identical
//     resource-intensive jobs or thermal throttling zones.
//   - Music and media playlist shuffling ensuring the same artist or track
//     does not replay within k songs.
//   - Packet pacing and network traffic shaping to avoid back-to-back bursts
//     from the same tenant or flow.
//
// Related LeetCode Problems:
//   - #767 Reorganize String [done] (special case of #358 with k = 2)
//   - #621 Task Scheduler [done] (cooldown scheduling with idle slots allowed)
//   - #1054 Distant Barcodes [new] (rearranging integers with k = 2 separation)
//   - #1405 Longest Happy String [done] (greedy max-heap character placement with adjacency limits)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <utility>

namespace {

constexpr std::size_t kAlphabetSize = 26;

class Solution {
 public:
  // Rearranges s so that identical characters are at least distance k apart,
  // or returns "" if impossible.
  //
  // Invariant: Every character sitting in `cooldown` was emitted within the
  // last (k - 1) positions and therefore cannot be emitted again until it
  // reaches the front of the queue after k placements.
  //
  // Time Complexity: O(n log 26) = O(n).
  // Space Complexity: O(1) auxiliary space (plus O(n) output string).
  std::string rearrangeString(std::string_view s, int k) {
    // Distance <= 1 imposes no restriction on adjacent characters.
    if (k <= 1 || s.size() <= 1) {
      return std::string(s);
    }

    // Phase 1: Count occurrences of each lowercase English letter.
    std::array<int, kAlphabetSize> freq{};
    for (const char ch : s) {
      ++freq[static_cast<std::size_t>(ch - 'a')];
    }

    // Phase 2: Populate a max-heap of (remaining_count, char).
    std::priority_queue<std::pair<int, char>> available;
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      if (freq[i] > 0) {
        available.emplace(freq[i], static_cast<char>('a' + static_cast<int>(i)));
      }
    }

    // Phase 3: Greedily emit the most frequent available character and place it
    // into a k-step cooldown queue before re-enabling it.
    std::queue<std::pair<int, char>> cooldown;
    std::string result;
    result.reserve(s.size());
    const auto min_gap = static_cast<std::size_t>(k);

    while (!available.empty()) {
      auto [remaining, ch] = available.top();
      available.pop();

      result.push_back(ch);
      cooldown.emplace(remaining - 1, ch);

      // Once k characters have been placed since the front entry was emitted,
      // that character is again at least distance k away from the next slot.
      if (cooldown.size() == min_gap) {
        auto [ready_count, ready_ch] = cooldown.front();
        cooldown.pop();
        if (ready_count > 0) {
          available.emplace(ready_count, ready_ch);
        }
      }
    }

    // If not all characters were placed, no valid k-distance arrangement exists.
    return result.size() == s.size() ? result : std::string{};
  }
};

// Verifies that `arranged` is a permutation of `original` where every repeated
// character is separated by at least `k` positions.
bool isValidArrangement(std::string_view original, std::string_view arranged,
                        int k) {
  if (original.size() != arranged.size()) {
    return false;
  }

  std::array<int, kAlphabetSize> orig_freq{};
  std::array<int, kAlphabetSize> arr_freq{};
  for (const char ch : original) {
    ++orig_freq[static_cast<std::size_t>(ch - 'a')];
  }
  for (const char ch : arranged) {
    ++arr_freq[static_cast<std::size_t>(ch - 'a')];
  }
  if (orig_freq != arr_freq) {
    return false;
  }

  if (k <= 1) {
    return true;
  }

  // Check distance between consecutive occurrences of each letter.
  std::array<int, kAlphabetSize> last_pos{};
  last_pos.fill(-k - 1);
  for (std::size_t i = 0uz; i < arranged.size(); ++i) {
    const auto idx = static_cast<std::size_t>(arranged[i] - 'a');
    const int pos = static_cast<int>(i);
    if ((pos - last_pos[idx]) < k) {
      return false;
    }
    last_pos[idx] = pos;
  }
  return true;
}

LC_PROBLEM(358, "Rearrange String K Distance Apart", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_aabbcc_k3", [&] {
    const std::string res = sol.rearrangeString("aabbcc", 3);
    ctx.expect_true(isValidArrangement("aabbcc", res, 3));
  });

  ctx.test("example_2_impossible_aaabc_k3", [&] {
    ctx.expect_eq(sol.rearrangeString("aaabc", 3), "");
  });

  ctx.test("example_3_aaadbbcc_k2", [&] {
    const std::string res = sol.rearrangeString("aaadbbcc", 2);
    ctx.expect_true(isValidArrangement("aaadbbcc", res, 2));
  });

  ctx.test("build_cases_and_uneven_residue_trap", [&] {
    // "aabbc", k = 3 exposes a classic bug in naive stride-k placement;
    // greedy heap + cooldown reliably produces a valid rearrangement ("abcab").
    const std::string res_trap = sol.rearrangeString("aabbc", 3);
    ctx.expect_true(isValidArrangement("aabbc", res_trap, 3));

    const std::string res_aaabc = sol.rearrangeString("aaabc", 2);
    ctx.expect_true(isValidArrangement("aaabc", res_aaabc, 2));

    const std::string res_k1 = sol.rearrangeString("aabbcc", 1);
    ctx.expect_true(isValidArrangement("aabbcc", res_k1, 1));
  });

  ctx.test("edge_cases_k0_single_and_empty", [&] {
    ctx.expect_eq(sol.rearrangeString("aa", 0), "aa");
    ctx.expect_eq(sol.rearrangeString("a", 5), "a");
    ctx.expect_eq(sol.rearrangeString("", 3), "");
    ctx.expect_eq(sol.rearrangeString("aa", 2), "");
  });
}

}  // namespace
