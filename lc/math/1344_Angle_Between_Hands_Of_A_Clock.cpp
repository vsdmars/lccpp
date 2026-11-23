// LeetCode 1344: Angle Between Hands Of A Clock
// https://leetcode.com/problems/angle-between-hands-of-a-clock/
//
// Problem:
//   Given two integers `hour` (1 <= hour <= 12) and `minutes`
//   (0 <= minutes <= 59), return the smaller angle in degrees (within [0, 180])
//   formed between the hour hand and the minute hand on a standard 12-hour
//   analog clock. Answers within 10^-5 of the exact value are accepted.
//
// Examples:
//   Example 1:
//     Input:  hour = 12, minutes = 30
//     Output: 165.0
//     Explanation:
//       - Normalize hour % 12 = 12 % 12 = 0, giving hour_angle = 0 * 30.0 + 30 *
//         0.5 = 15.0 degrees.
//       - Compute minute_angle = 30 * 6.0 = 180.0 degrees.
//       - Compute diff = |15.0 - 180.0| = 165.0 and return min(165.0, 360.0 -
//         165.0) = 165.0.
//
//   Example 2:
//     Input:  hour = 3, minutes = 15
//     Output: 7.5
//     Explanation:
//       - Compute hour_angle = (3 % 12) * 30.0 + 15 * 0.5 = 90.0 + 7.5 = 97.5
//         degrees.
//       - Compute minute_angle = 15 * 6.0 = 90.0 degrees.
//       - Compute diff = |97.5 - 90.0| = 7.5 and return min(7.5, 360.0 - 7.5) =
//         7.5.
//
//   Example 3:
//     Input:  hour = 1, minutes = 57
//     Output: 76.5
//     Explanation:
//       - Compute hour_angle = (1 % 12) * 30.0 + 57 * 0.5 = 30.0 + 28.5 = 58.5
//         degrees.
//       - Compute minute_angle = 57 * 6.0 = 342.0 degrees.
//       - Compute diff = |58.5 - 342.0| = 283.5 and take the shorter arc min(283.5,
//         360.0 - 283.5) = 76.5.
//
// Algorithm:
//   Measure both hands' clockwise angular positions relative to 12 o'clock:
//     - Minute hand speed: 360 / 60 = 6.0 degrees per minute, so
//       `minute_angle = minutes * 6.0`.
//     - Hour hand speed: 360 / 12 = 30.0 degrees per hour, plus continuous
//       drift of 30 / 60 = 0.5 degrees per minute, so
//       `hour_angle = (hour % 12) * 30.0 + minutes * 0.5`.
//       Reducing `hour % 12` places 12 o'clock at 0 degrees cleanly.
//   The direct angular separation is `diff = |hour_angle - minute_angle|`, and
//   the smaller interior angle between the two hands is `min(diff, 360 - diff)`.
//   Time Complexity:  O(1).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Scalar floating-point variables only; no containers needed.
//
// Real-World Applications:
//   - Robotics and servo/stepper motor control: computing the shortest angular
//     displacement between two rotary joint angles on a circular [0, 360) ring.
//   - Navigation, radar, and game physics: finding the minimal heading error
//     between a current bearing and a target azimuth across the 0/360 wrap
//     boundary.
//   - Analog gauge and UI rendering: computing exact hand angles for instrument
//     clusters and SVG/Canvas clock widgets.
//
// Related LeetCode Problems:
//   - #539 Minimum Time Difference [new] (shortest circular distance on a 24-hour clock)
//   - #2437 Number of Valid Clock Times [new] (combinatorial clock hour/minute arithmetic)
//   - #1904 The Number of Full Rounds You Have Played [new] (converting HH:MM clock times into linear minutes)
//   - #1232 Check If It Is a Straight Line [new] (elementary 2D coordinate/angle geometry)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr double kDegreesPerHour = 30.0;
constexpr double kHourHandDegreesPerMinute = 0.5;
constexpr double kMinuteHandDegreesPerMinute = 6.0;
constexpr double kFullCircleDegrees = 360.0;
constexpr double kEps = 1e-5;

bool approxEqual(double actual, double expected) {
  return std::fabs(actual - expected) <= kEps;
}

class Solution {
 public:
  // Returns the smaller angle in degrees between the hour and minute hands.
  // Time: O(1), Space: O(1).
  double angleClock(int hour, int minutes) {
    // Normalize 12 -> 0 so both hands share 12 o'clock as the 0-degree origin.
    const double hour_angle =
        (static_cast<double>(hour % 12) * kDegreesPerHour) +
        (static_cast<double>(minutes) * kHourHandDegreesPerMinute);

    const double minute_angle =
        static_cast<double>(minutes) * kMinuteHandDegreesPerMinute;

    // Take the shorter of the clockwise and counter-clockwise arcs.
    const double diff = std::fabs(hour_angle - minute_angle);
    return std::min(diff, kFullCircleDegrees - diff);
  }
};

LC_PROBLEM(1344, "Angle Between Hands Of A Clock", "math", ctx) {
  Solution sol;

  ctx.test("example_1_12_30", [&] {
    ctx.expect_true(approxEqual(sol.angleClock(12, 30), 165.0));
  });

  ctx.test("example_2_3_30", [&] {
    ctx.expect_true(approxEqual(sol.angleClock(3, 30), 75.0));
  });

  ctx.test("example_3_3_15", [&] {
    ctx.expect_true(approxEqual(sol.angleClock(3, 15), 7.5));
  });

  ctx.test("example_4_4_50", [&] {
    ctx.expect_true(approxEqual(sol.angleClock(4, 50), 155.0));
  });

  ctx.test("example_5_12_00_zero_angle", [&] {
    ctx.expect_true(approxEqual(sol.angleClock(12, 0), 0.0));
  });

  ctx.test("wrap_around_and_straight_angle", [&] {
    // 6:00 forms a straight 180-degree angle.
    ctx.expect_true(approxEqual(sol.angleClock(6, 0), 180.0));
    // 1:57 -> hour=58.5, min=342.0, diff=283.5 -> 360 - 283.5 = 76.5.
    ctx.expect_true(approxEqual(sol.angleClock(1, 57), 76.5));
    // 11:59 -> hour=359.5, min=354.0 -> 5.5.
    ctx.expect_true(approxEqual(sol.angleClock(11, 59), 5.5));
  });
}

}  // namespace
