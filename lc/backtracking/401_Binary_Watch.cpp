// LeetCode 401: Binary Watch
// https://leetcode.com/problems/binary-watch/
//
// Problem:
//   A binary watch has 4 top LEDs representing hours (0-11, weights 8, 4, 2, 1)
//   and 6 bottom LEDs representing minutes (0-59, weights 32, 16, 8, 4, 2, 1).
//   Given an integer `turned_on` representing the number of LEDs currently lit,
//   return all valid times the watch could represent in any order.
//   Formatting rules:
//     - Hours must NOT have a leading zero (e.g., "1:00", "0:05").
//     - Minutes MUST always be two digits with a leading zero if < 10
//       (e.g., "10:02").
//   Constraints: 0 <= turned_on <= 10.
//
// Algorithm:
//   1. Backtracking Over LED Combinations (`readBinaryWatch`):
//      Treat all 10 LEDs as an ordered array (indices 0..3 for hours, 4..9 for
//      minutes). Recursively choose `remaining` LEDs from `[idx, 10)`:
//        - Prune immediately if `hour >= 12` or `minute >= 60` (invalid clock
//          state) or if fewer than `remaining` LEDs remain.
//        - When `remaining == 0`, format `"hour:mm"` and record the result.
//   2. Exhaustive Bit-Count Enumeration (`readBinaryWatchPopcount`):
//      Iterate over all `12 * 60 = 720` valid `(hour, minute)` pairs and emit
//      every pair where `std::popcount(hour) + std::popcount(minute) == turned_on`.
//   Time Complexity:  O(C(10, turned_on)) <= O(252) = O(1) for backtracking;
//                     O(720) = O(1) for popcount enumeration.
//   Space Complexity: O(1) auxiliary space (recursion depth <= 10, fixed-size
//                     LED weight table).
//
// Data Structures:
//   - Fixed-size compile-time array (`std::array<int, 10>`) holding the binary
//     weights of the 4 hour LEDs followed by the 6 minute LEDs.
//   - Output vector (`std::vector<std::string>`) of formatted timestamps.
//
// Real-World Applications:
//   - Hardware diagnostic tools and embedded LED/GPIO test suites enumerating
//     all valid register states with a fixed Hamming weight (power budget).
//   - Error-correcting code and constant-weight code generation for low-power
//     optical and RF signaling.
//   - Constrained bit-vector enumeration in formal verification of digital
//     circuits.
//
// Related LeetCode Problems:
//   - #77 Combinations [done] (choosing k items out of n without repetition)
//   - #78 Subsets [done] (enumerating subsets over all bitmasks)
//   - #191 Number of 1 Bits [done] (Hamming weight / popcount primitive)
//   - #357 Count Numbers With Unique Digits [done] (small-space backtracking vs math)
//   - #17 Letter Combinations of a Phone Number [done] (backtracking string formatting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <string>
#include <vector>

namespace {

// Binary weights for the 10 LEDs: indices [0..3] are hours, [4..9] are minutes.
constexpr std::array<int, 10> kLedWeights = {8, 4, 2, 1, 32, 16, 8, 4, 2, 1};
constexpr std::size_t kHourLedCount = 4;
constexpr std::size_t kTotalLedCount = kLedWeights.size();
constexpr int kMaxValidHour = 11;
constexpr int kMaxValidMinute = 59;

// Formats a valid `(hour, minute)` pair as `"H:MM"` without leading zero on
// the hour and with a zero-padded two-digit minute.
std::string format_time(int hour, int minute) {
  std::string out = std::to_string(hour);
  out.push_back(':');
  if (minute < 10) {
    out.push_back('0');
  }
  out += std::to_string(minute);
  return out;
}

class Solution {
 public:
  // Backtracks over all combinations of `turned_on` lit LEDs out of 10,
  // pruning invalid hour/minute sums as soon as they exceed clock bounds.
  // Time: O(C(10, turned_on)) = O(1), Space: O(1) auxiliary.
  std::vector<std::string> readBinaryWatch(int turned_on) {
    // At most 3 hour LEDs (max 11 = 1011b) and 5 minute LEDs (max 59 = 111011b)
    // can be simultaneously lit for a valid watch reading.
    if (turned_on < 0 || turned_on > 8) {
      return {};
    }

    std::vector<std::string> results;
    backtrack(0uz, turned_on, 0, 0, results);
    return results;
  }

  // Alternative O(1) (720-iteration) enumeration using C++20/23 `std::popcount`
  // over all valid `(hour, minute)` clock states.
  // Time: O(720) = O(1), Space: O(1) auxiliary.
  std::vector<std::string> readBinaryWatchPopcount(int turned_on) {
    if (turned_on < 0 || turned_on > 8) {
      return {};
    }

    std::vector<std::string> results;
    for (unsigned hour = 0; hour <= static_cast<unsigned>(kMaxValidHour);
         ++hour) {
      const int hour_bits = std::popcount(hour);
      for (unsigned minute = 0;
           minute <= static_cast<unsigned>(kMaxValidMinute); ++minute) {
        if (hour_bits + std::popcount(minute) == turned_on) {
          results.push_back(
              format_time(static_cast<int>(hour), static_cast<int>(minute)));
        }
      }
    }
    return results;
  }

 private:
  // Recursively picks `remaining` LEDs from indices `[idx, kTotalLedCount)`
  // while accumulating `(hour, minute)` and pruning out-of-range clock states.
  static void backtrack(std::size_t idx, int remaining, int hour, int minute,
                        std::vector<std::string>& results) {
    if (hour > kMaxValidHour || minute > kMaxValidMinute) {
      return;
    }
    if (remaining == 0) {
      results.push_back(format_time(hour, minute));
      return;
    }
    // Prune if the remaining LEDs cannot satisfy the required count.
    if (kTotalLedCount - idx < static_cast<std::size_t>(remaining)) {
      return;
    }

    for (std::size_t i = idx; i < kTotalLedCount; ++i) {
      const int next_hour =
          (i < kHourLedCount) ? (hour + kLedWeights[i]) : hour;
      const int next_minute =
          (i >= kHourLedCount) ? (minute + kLedWeights[i]) : minute;
      backtrack(i + 1, remaining - 1, next_hour, next_minute, results);
    }
  }
};

LC_PROBLEM(401, "Binary Watch", "backtracking", ctx) {
  Solution sol;

  ctx.test("example_one_led", [&] {
    auto actual = sol.readBinaryWatch(1);
    std::ranges::sort(actual);
    std::vector<std::string> expected = {
        "0:01", "0:02", "0:04", "0:08", "0:16",
        "0:32", "1:00", "2:00", "4:00", "8:00",
    };
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("zero_and_impossible_led_counts", [&] {
    const std::vector<std::string> expected_zero = {"0:00"};
    ctx.expect_eq(sol.readBinaryWatch(0), expected_zero);
    ctx.expect_true(sol.readBinaryWatch(9).empty());
    ctx.expect_true(sol.readBinaryWatch(10).empty());
  });

  ctx.test("backtracking_matches_popcount_for_all_counts", [&] {
    for (int k = 0; k <= 10; ++k) {
      auto backtrack = sol.readBinaryWatch(k);
      auto popcount = sol.readBinaryWatchPopcount(k);
      std::ranges::sort(backtrack);
      std::ranges::sort(popcount);
      ctx.expect_eq(backtrack, popcount);
    }
  });
}

}  // namespace
