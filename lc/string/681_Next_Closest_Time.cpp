// LeetCode 681: Next Closest Time
// https://leetcode.com/problems/next-closest-time/
//
// Problem:
//   Given a valid 24-hour clock time formatted as "HH:MM", find the next
//   closest valid time that can be formed by reusing only the digits already
//   present in the input string. Each available digit may be reused any number
//   of times. If no strictly later valid time exists on the same day, wrap
//   around to the earliest valid time on the next day (which may be the input
//   time itself when only one distinct digit appears, e.g., "00:00").
//
// Algorithm:
//   Parse the input time into total minutes elapsed since 00:00 (in [0, 1439])
//   and record the set of allowed decimal digits in a 10-element boolean array.
//   Simulate advancing the clock one minute at a time from delta = 1 through
//   1440 (wrapping modulo 1440). For each candidate minute, decompose it into
//   its four HH:MM digits and check whether all four belong to the allowed set.
//   Because minutes are visited in strictly increasing chronological distance
//   from the starting time, the very first valid candidate is guaranteed to be
//   the next closest time (with delta = 1440 naturally handling single-digit
//   inputs such as "00:00" or "22:22").
//   Time Complexity:  O(1) — at most 1,440 iterations (24 * 60 minutes).
//   Space Complexity: O(1) — fixed 10-element boolean lookup table.
//
// Data Structures:
//   - std::array<bool, 10>: O(1) direct-indexed membership table storing which
//     decimal digits ('0'-'9') appear in the input time.
//
// Real-World Applications:
//   - Cron and recurring job schedulers computing the next fire timestamp
//     under restricted time-field masks (allowed hours/minutes).
//   - Embedded digital-clock / seven-segment display diagnostics and
//     fault-tolerant clock fallback when specific digit segments are broken.
//   - Circular time-of-day wrap-around arithmetic in calendar and SLA engines.
//
// Related LeetCode Problems:
//   - #539 Minimum Time Difference [new] (circular 1440-minute clock distance)
//   - #949 Largest Time for Given Digits [new] (forming valid HH:MM from digits)
//   - #1736 Latest Time by Replacing Hidden Digits [new] (constrained HH:MM selection)
//   - #2437 Number of Valid Clock Times [new] (counting valid HH:MM combinations)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the chronologically next valid "HH:MM" time composed solely of
  // digits present in `time`.
  // Time: O(1) (<= 1440 steps), Space: O(1).
  std::string nextClosestTime(std::string_view time) {
    // Record which decimal digits are available for reuse.
    std::array<bool, kDecimalBase> allowed{};
    for (const char ch : time) {
      if (ch != ':') {
        allowed[static_cast<std::size_t>(ch - '0')] = true;
      }
    }

    // Convert "HH:MM" into minutes since midnight in [0, 1439].
    const int start_hour = ((time[0] - '0') * 10) + (time[1] - '0');
    const int start_minute = ((time[3] - '0') * 10) + (time[4] - '0');
    const int start_total = (start_hour * kMinutesPerHour) + start_minute;

    // Step forward minute-by-minute (wrapping at 24:00). Stopping at delta ==
    // 1440 guarantees termination and returns `time` itself when all 4 digits
    // are identical (e.g., "00:00").
    for (int delta = 1; delta <= kMinutesPerDay; ++delta) {
      const int candidate = (start_total + delta) % kMinutesPerDay;
      const int hour = candidate / kMinutesPerHour;
      const int minute = candidate % kMinutesPerHour;

      if (all_digits_allowed(hour, minute, allowed)) {
        return format_time(hour, minute);
      }
    }

    // Unreachable for valid "HH:MM" inputs because delta == kMinutesPerDay
    // always reproduces the input time.
    return std::string(time);
  }

 private:
  static constexpr std::size_t kDecimalBase = 10;
  static constexpr int kMinutesPerHour = 60;
  static constexpr int kMinutesPerDay = 24 * kMinutesPerHour;

  // Checks whether all four decimal digits of (hour, minute) are allowed.
  static bool all_digits_allowed(int hour, int minute,
                                 const std::array<bool, kDecimalBase>& allowed) {
    return allowed[static_cast<std::size_t>(hour / 10)] &&
           allowed[static_cast<std::size_t>(hour % 10)] &&
           allowed[static_cast<std::size_t>(minute / 10)] &&
           allowed[static_cast<std::size_t>(minute % 10)];
  }

  // Formats (hour, minute) into a 5-character zero-padded "HH:MM" string.
  static std::string format_time(int hour, int minute) {
    return {
        static_cast<char>('0' + (hour / 10)),
        static_cast<char>('0' + (hour % 10)),
        ':',
        static_cast<char>('0' + (minute / 10)),
        static_cast<char>('0' + (minute % 10)),
    };
  }
};

LC_PROBLEM(681, "Next Closest Time", "string", ctx) {
  Solution sol;

  ctx.test("example_1_same_day", [&] {
    ctx.expect_eq(sol.nextClosestTime("19:34"), "19:39");
  });

  ctx.test("example_2_next_day_wrap", [&] {
    ctx.expect_eq(sol.nextClosestTime("23:59"), "22:22");
  });

  ctx.test("all_same_digits_midnight", [&] {
    ctx.expect_eq(sol.nextClosestTime("00:00"), "00:00");
  });

  ctx.test("all_same_digits_nonzero", [&] {
    ctx.expect_eq(sol.nextClosestTime("22:22"), "22:22");
  });

  ctx.test("hour_rollover", [&] {
    ctx.expect_eq(sol.nextClosestTime("13:55"), "15:11");
  });

  ctx.test("next_minute_available", [&] {
    ctx.expect_eq(sol.nextClosestTime("01:32"), "01:33");
  });

  ctx.test("minute_tens_rollover_same_hour", [&] {
    ctx.expect_eq(sol.nextClosestTime("23:45"), "23:52");
  });

  ctx.test("wrap_to_earliest_next_day_hours", [&] {
    ctx.expect_eq(sol.nextClosestTime("19:59"), "11:11");
  });
}

}  // namespace
