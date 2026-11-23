// LeetCode 346: Moving Average from Data Stream
// https://leetcode.com/problems/moving-average-from-data-stream/
//
// Problem:
//   Design a `MovingAverage` class initialized with a positive integer window
//   size `size`. Each call to `next(int val)` appends `val` to the stream and
//   returns the arithmetic mean of the most recent `min(count, size)` values
//   as a `double` (answers within 10^-5 are accepted).
//   Constraints: 1 <= size <= 1000, -10^5 <= val <= 10^5, up to 10^4 calls to
//   `next` (implementation also guards against `size <= 0` and uses 64-bit
//   accumulation to prevent integer overflow on large values).
//
// Algorithm:
//   Sliding-window FIFO queue with incremental sum maintenance. On each
//   `next(val)` call, push `val` to the back of `window_` and add it to
//   `running_sum_`. If `window_.size()` exceeds `max_size_`, subtract
//   `window_.front()` from `running_sum_` and pop the front element. Return
//   `static_cast<double>(running_sum_) / static_cast<double>(window_.size())`.
//   Key invariant: `running_sum_` always equals the exact sum of elements
//   currently inside `window_`, avoiding O(size) rescanning per query and
//   preventing unbounded prefix-sum growth.
//   Time: O(1) per `next()` call, Space: O(size) for the bounded window.
//
// Data Structures:
//   - `std::deque<int> window_`: double-ended queue storing at most `max_size_`
//     most recent stream values with O(1) back insertion and front eviction.
//   - `std::int64_t running_sum_`: 64-bit running sum of active window values
//     to avoid 32-bit signed integer overflow.
//
// Real-World Applications:
//   - Smoothing noisy sensor readings (temperature, accelerometer, GPS) in
//     embedded and IoT telemetry pipelines.
//   - Computing real-time rolling latency, throughput, and error-rate metrics
//     in observability dashboards and autoscalers.
//   - Calculating Simple Moving Averages (SMA) on live financial market ticks
//     and TCP round-trip time estimators.
//
// Related LeetCode Problems:
//   - #239 Sliding Window Maximum [done] (sliding-window aggregate with a deque)
//   - #933 Number of Recent Calls [new] (bounded FIFO queue over a trailing window)
//   - #295 Find Median from Data Stream [done] (online statistic on an unbounded stream)
//   - #622 Design Circular Queue [done] (ring-buffer alternative for bounded windows)
//   - #2090 K Radius Subarray Averages [new] (fixed-window average on a static array)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>

namespace {

bool approxEqual(double a, double b) {
  return std::fabs(a - b) <= 1e-5;
}

// Maintains the active sliding window in a bounded deque alongside a 64-bit
// running sum (avoiding unbounded cumulative prefix-sum growth).
//
// Time:  O(1) per next() call
// Space: O(size)
class MovingAverage {
 public:
  explicit MovingAverage(int size)
      : max_size_(size > 0 ? static_cast<std::size_t>(size) : 0uz) {}

  double next(int val) {
    if (max_size_ == 0uz) {
      return 0.0;
    }

    window_.push_back(val);
    running_sum_ += val;

    if (window_.size() > max_size_) {
      running_sum_ -= window_.front();
      window_.pop_front();
    }

    return static_cast<double>(running_sum_) /
           static_cast<double>(window_.size());
  }

 private:
  std::size_t max_size_ = 0uz;
  std::deque<int> window_;
  std::int64_t running_sum_ = 0;
};

LC_PROBLEM(346, "Moving Average From Data Stream", "queue", ctx) {
  ctx.test("example_window_3", [&] {
    MovingAverage avg(3);
    ctx.expect_true(approxEqual(avg.next(1), 1.0));
    ctx.expect_true(approxEqual(avg.next(10), 5.5));
    ctx.expect_true(approxEqual(avg.next(3), 14.0 / 3.0));
    ctx.expect_true(approxEqual(avg.next(5), 6.0));
    ctx.expect_true(approxEqual(avg.next(2), 10.0 / 3.0));
  });

  ctx.test("window_size_1", [&] {
    MovingAverage avg(1);
    ctx.expect_true(approxEqual(avg.next(4), 4.0));
    ctx.expect_true(approxEqual(avg.next(-7), -7.0));
    ctx.expect_true(approxEqual(avg.next(0), 0.0));
  });

  ctx.test("negative_and_large_values", [&] {
    MovingAverage avg(2);
    ctx.expect_true(approxEqual(avg.next(-5), -5.0));
    ctx.expect_true(approxEqual(avg.next(5), 0.0));
    ctx.expect_true(approxEqual(avg.next(-3), 1.0));

    MovingAverage large_avg(2);
    ctx.expect_true(approxEqual(large_avg.next(2000000000), 2000000000.0));
    ctx.expect_true(approxEqual(large_avg.next(2000000000), 2000000000.0));
  });
}

}  // namespace
