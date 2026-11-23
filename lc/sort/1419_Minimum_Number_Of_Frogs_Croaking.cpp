// LeetCode 1419: Minimum Number Of Frogs Croaking
// https://leetcode.com/problems/minimum-number-of-frogs-croaking/
//
// Problem:
//   Given a string `croakOfFrogs` representing an interleaved combination of
//   the string `"croak"` spoken by one or more frogs, return the minimum
//   number of distinct frogs required to produce all croaks in the string.
//   Each frog must emit `'c'`, `'r'`, `'o'`, `'a'`, `'k'` sequentially to
//   complete a croak, after which it may immediately start another croak.
//   If `croakOfFrogs` cannot be decomposed into valid sequential `"croak"`
//   strings (e.g., out-of-order letters, invalid letters, or incomplete
//   croaks), return -1.
//   Constraints: 1 <= croakOfFrogs.length <= 10^5.
//
// Algorithm:
//   Model each in-progress frog as a state machine advancing through the five
//   stages 0:'c' -> 1:'r' -> 2:'o' -> 3:'a' -> 4:'k'. Maintain `stage_counts`,
//   the number of frogs whose most recently emitted letter is each stage, along
//   with `active_frogs` (frogs currently mid-croak) and `max_frogs` (peak
//   concurrent frogs):
//     - On `'c'` (stage 0): a new or idle frog begins croaking. Increment
//       `stage_counts[0]` and `active_frogs`, and update `max_frogs`.
//     - On `'r'`, `'o'`, `'a'`, or `'k'` (stage `s > 0`): some frog must be
//       waiting at stage `s - 1`. If `stage_counts[s - 1] == 0`, the sequence
//       is invalid -> return -1. Otherwise transfer one frog from stage `s - 1`
//       to stage `s`. If `s == 4` (`'k'`), that frog finishes and becomes idle,
//       so decrement `active_frogs`.
//     - Any character outside `"croak"` immediately returns -1.
//   After scanning the entire string, every started croak must have finished
//   (`active_frogs == 0`); otherwise return -1.
//   Time Complexity:  O(n), single pass over `croakOfFrogs`.
//   Space Complexity: O(1), fixed 5-element state counter array.
//
// Data Structures:
//   - `std::array<int, 5>`: counts frogs currently positioned at each stage of
//     the `"croak"` finite-state pipeline.
//
// Real-World Applications:
//   - Sizing worker pools or connection pools from interleaved event logs
//     (determining peak concurrency from start/progress/finish trace events).
//   - Validating multi-step protocol state machines (e.g., TCP handshake or
//     distributed transaction commit phases) in multiplexed packet captures.
//   - Counting minimum concurrent pipelines / hardware execution lanes required
//     to satisfy an observed trace of staged instructions.
//
// Related LeetCode Problems:
//   - #253 Meeting Rooms II [done] (computes peak concurrency of overlapping intervals)
//   - #20 Valid Parentheses [done] (validates nested/sequential state transitions)
//   - #1094 Car Pooling [done] (tracks peak concurrent occupancy along a timeline)
//   - #392 Is Subsequence [done] (matches ordered character sequences in a stream)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the minimum number of frogs needed to produce `croak_of_frogs`,
  // or -1 if the string is not a valid interleaving of complete `"croak"`s.
  // Time: O(n), Space: O(1).
  int minNumberOfFrogs(std::string_view croak_of_frogs) {
    // stage_counts[s] holds the number of frogs currently at stage s
    // (0='c', 1='r', 2='o', 3='a', 4='k').
    std::array<int, kStages> stage_counts{};
    int active_frogs = 0;
    int max_frogs = 0;

    for (const char ch : croak_of_frogs) {
      const int stage = charToStage(ch);
      if (stage < 0) {
        return -1;
      }

      const auto idx = static_cast<std::size_t>(stage);
      if (idx == 0uz) {
        // A frog starts a fresh "croak"; reuse an idle frog if available,
        // otherwise peak concurrency increases.
        ++stage_counts[0];
        ++active_frogs;
        max_frogs = std::max(max_frogs, active_frogs);
      } else {
        // Every non-'c' letter must advance a frog waiting at the previous
        // stage; if none is waiting, the ordering is invalid.
        if (stage_counts[idx - 1uz] == 0) {
          return -1;
        }
        --stage_counts[idx - 1uz];
        ++stage_counts[idx];

        if (idx == kStages - 1uz) {
          // Reaching 'k' completes one croak and frees that frog for reuse.
          --active_frogs;
        }
      }
    }

    // All frogs must have completed their final 'k' by the end of the string.
    return active_frogs == 0 ? max_frogs : -1;
  }

 private:
  static constexpr std::size_t kStages = 5uz;

  // Maps a letter of "croak" to its 0-based stage index, or -1 if invalid.
  static constexpr int charToStage(char ch) {
    switch (ch) {
      case 'c':
        return 0;
      case 'r':
        return 1;
      case 'o':
        return 2;
      case 'a':
        return 3;
      case 'k':
        return 4;
      default:
        return -1;
    }
  }
};

LC_PROBLEM(1419, "Minimum Number Of Frogs Croaking", "sort", ctx) {
  Solution sol;

  ctx.test("example1_sequential_single_frog", [&] {
    ctx.expect_eq(sol.minNumberOfFrogs("croakcroak"), 1);
  });

  ctx.test("example2_two_overlapping_frogs", [&] {
    ctx.expect_eq(sol.minNumberOfFrogs("crcoakroak"), 2);
  });

  ctx.test("invalid_misspelled_croak", [&] {
    ctx.expect_eq(sol.minNumberOfFrogs("croakcrook"), -1);
  });

  ctx.test("invalid_unfinished_croak", [&] {
    ctx.expect_eq(sol.minNumberOfFrogs("croakcroa"), -1);
  });

  ctx.test("invalid_out_of_order_and_bad_char", [&] {
    ctx.expect_eq(sol.minNumberOfFrogs("roakc"), -1);
    ctx.expect_eq(sol.minNumberOfFrogs("croax"), -1);
  });

  ctx.test("three_concurrent_frogs_and_reuse", [&] {
    ctx.expect_eq(sol.minNumberOfFrogs("cccrrroooaaakkk"), 3);
    ctx.expect_eq(sol.minNumberOfFrogs("crcoakroakcroak"), 2);
  });
}

}  // namespace
