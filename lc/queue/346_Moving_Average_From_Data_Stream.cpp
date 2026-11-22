// https://leetcode.com/problems/moving-average-from-data-stream/
//
// Given a stream of integers and a window size, calculate the moving average of
// all integers in the sliding window.

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
