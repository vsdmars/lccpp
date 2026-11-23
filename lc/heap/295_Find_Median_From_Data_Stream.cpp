// LeetCode 295: Find Median From Data Stream
// https://leetcode.com/problems/find-median-from-data-stream/
//
// Problem:
//   Design a data structure `MedianFinder` that supports two operations over
//   an online stream of integers:
//   - `addNum(int num)`: inserts an integer from the stream.
//   - `findMedian()`: returns the median of all integers inserted so far
//     (the middle value for an odd count, or the average of the two middle
//     values for an even count).
//
// Examples:
//   Example 1:
//     Input:  operations = ["MedianFinder", "addNum", "findMedian", "addNum",
//             "findMedian", "addNum", "findMedian"], args = [[], [1], [], [2], [],
//             [3], []]
//     Output: [null, null, 1.0, null, 1.5, null, 2.0]
//     Explanation:
//       - addNum(1) sees lower_.size() == upper_.size() (0 == 0), pushes 1 through
//         upper_ into lower_ so lower_ = [1] and upper_ = [], and findMedian()
//         returns lower_.top() = 1.0.
//       - addNum(2) sees lower_.size() > upper_.size(), pushes 2 to lower_ and
//         moves lower_.top() = 2 to upper_ so lower_ = [1] and upper_ = [2], and
//         findMedian() returns (1.0 + 2.0) / 2.0 = 1.5.
//       - addNum(3) sees equal heap sizes, pushes 3 to upper_ and moves
//         upper_.top() = 2 to lower_ so lower_ = [2, 1] and upper_ = [3], and
//         findMedian() returns lower_.top() = 2.0.
//
//   Example 2:
//     Input:  operations = ["MedianFinder", "addNum", "findMedian", "addNum",
//             "findMedian", "addNum", "findMedian", "addNum", "findMedian",
//             "addNum", "findMedian"], args = [[], [-1], [], [-2], [], [-3], [],
//             [-4], [], [-5], []]
//     Output: [null, null, -1.0, null, -1.5, null, -2.0, null, -2.5, null, -3.0]
//     Explanation:
//       - addNum(-1) places -1 into lower_ via upper_ so lower_ = [-1], upper_ =
//         [], and findMedian() returns -1.0.
//       - addNum(-2) pushes -2 into lower_ and moves max top -1 into upper_,
//         leaving lower_ = [-2] and upper_ = [-1] for median (-2.0 + -1.0) / 2.0 =
//         -1.5.
//       - addNum(-3) pushes -3 into upper_ and moves min top -3 into lower_, giving
//         lower_ = [-2, -3] and upper_ = [-1] for median -2.0.
//       - addNum(-4) transfers -2 from lower_ to upper_ (median -2.5), and
//         addNum(-5) transfers -5 from upper_ to lower_ so lower_ = [-3, -4, -5]
//         and upper_ = [-2, -1], returning -3.0.
//
//   Example 3:
//     Input:  operations = ["MedianFinder", "addNum", "addNum", "findMedian"], args
//             = [[], [-2147483648], [2147483647], []]
//     Output: [null, null, null, -0.5]
//     Explanation:
//       - addNum(-2147483648) balances equal-sized heaps by moving -2147483648
//         through upper_ into lower_.
//       - addNum(2147483647) pushes 2147483647 into lower_, where it becomes the
//         max top and transfers to upper_, leaving lower_ = [-2147483648] and
//         upper_ = [2147483647].
//       - findMedian() casts both heap tops to double before adding to avoid 32-bit
//         signed overflow, returning (-2147483648.0 + 2147483647.0) / 2.0 = -0.5.
//
// Algorithm:
//   Maintain two balanced heaps partitioning all seen numbers into a smaller
//   half (`lower_`, max-heap) and a larger half (`upper_`, min-heap):
//   Invariants maintained after every `addNum`:
//     1. Ordering: every element in `lower_` <= every element in `upper_`.
//     2. Size balance: `lower_.size() == upper_.size()` (even total count) or
//        `lower_.size() == upper_.size() + 1` (odd total count).
//   On `addNum(num)`:
//     - If `lower_.size() > upper_.size()`, push `num` into `lower_` and move
//       the largest element of `lower_` over to `upper_` so sizes become equal.
//     - Otherwise (`lower_.size() == upper_.size()`), push `num` into `upper_`
//       and move the smallest element of `upper_` over to `lower_` so `lower_`
//       gains the extra middle element.
//   Because the new value always passes through the opposite half before
//   rebalancing, the ordering invariant holds unconditionally without extra
//   comparisons.
//   On `findMedian()`:
//     - If `lower_.size() > upper_.size()`, return `lower_.top()`.
//     - Otherwise, return `(lower_.top() + upper_.top()) / 2.0`.
//   Time Complexity: `addNum` is O(log N); `findMedian` is O(1).
//   Space Complexity: O(N) to store all inserted elements across the two heaps.
//
// Data Structures:
//   - `lower_` (std::priority_queue<int>, max-heap): holds the smaller half of
//     the stream so the largest element of the lower half is at the top in O(1).
//   - `upper_` (std::priority_queue<int, std::vector<int>, std::greater<>>,
//     min-heap): holds the larger half so the smallest element of the upper
//     half is at the top in O(1).
//
// Real-World Applications:
//   - Real-time latency monitoring (p50 dashboards) over live request streams.
//   - Financial tick analytics and streaming anomaly detection computing exact
//     running medians without re-sorting historical data.
//   - Adaptive signal processing / online median filtering in sensor pipelines.
//
// Related LeetCode Problems:
//   - #4 Median Of Two Sorted Arrays [done] (offline median of two sorted arrays in O(log(min(m, n))))
//   - #480 Sliding Window Median [done] (extends the two-heap design with lazy deletion for sliding windows)
//   - #215 Kth Largest Element In An Array [done] (heap-based order statistic)
//   - #502 IPO [new] (dual-heap / greedy selection over streaming candidates)
//   - #1825 Finding MK Average [new] (multi-partition order-statistic maintenance over streams)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <vector>

namespace {

// Online median tracker backed by a max-heap for the lower half and a min-heap
// for the upper half.
class MedianFinder {
 public:
  // Inserts `num` into the stream while preserving ordering and size balance:
  // `lower_.size() == upper_.size()` or `lower_.size() == upper_.size() + 1`.
  // Time: O(log N).
  void addNum(int num) {
    if (lower_.size() > upper_.size()) {
      // Push through `lower_` first so any value larger than `lower_.top()`
      // rises to the top and gets transferred into `upper_`.
      lower_.push(num);
      upper_.push(lower_.top());
      lower_.pop();
    } else {
      // Push through `upper_` first so the smallest value of the upper half
      // (which may be `num` itself) moves into `lower_`.
      upper_.push(num);
      lower_.push(upper_.top());
      upper_.pop();
    }
  }

  // Returns the median of all numbers inserted so far in O(1) time.
  [[nodiscard]] double findMedian() const {
    if (lower_.empty()) {
      return 0.0;
    }
    if (lower_.size() > upper_.size()) {
      return static_cast<double>(lower_.top());
    }
    // Convert to double before adding to avoid 32-bit signed overflow.
    return (static_cast<double>(lower_.top()) +
            static_cast<double>(upper_.top())) /
           2.0;
  }

 private:
  // Smaller half of the stream (max-heap): top is the lower median candidate.
  std::priority_queue<int> lower_;
  // Larger half of the stream (min-heap): top is the upper median candidate.
  std::priority_queue<int, std::vector<int>, std::greater<>> upper_;
};

constexpr double kEps = 1e-5;

bool approx_eq(double actual, double expected) {
  return std::fabs(actual - expected) <= kEps;
}

LC_PROBLEM(295, "Find Median From Data Stream", "heap", ctx) {
  ctx.test("example_1", [&] {
    MedianFinder mf;
    mf.addNum(1);
    ctx.expect_true(approx_eq(mf.findMedian(), 1.0));
    mf.addNum(2);
    ctx.expect_true(approx_eq(mf.findMedian(), 1.5));
    mf.addNum(3);
    ctx.expect_true(approx_eq(mf.findMedian(), 2.0));
  });

  ctx.test("descending_negative_stream", [&] {
    MedianFinder mf;
    mf.addNum(-1);
    ctx.expect_true(approx_eq(mf.findMedian(), -1.0));
    mf.addNum(-2);
    ctx.expect_true(approx_eq(mf.findMedian(), -1.5));
    mf.addNum(-3);
    ctx.expect_true(approx_eq(mf.findMedian(), -2.0));
    mf.addNum(-4);
    ctx.expect_true(approx_eq(mf.findMedian(), -2.5));
    mf.addNum(-5);
    ctx.expect_true(approx_eq(mf.findMedian(), -3.0));
  });

  ctx.test("unsorted_stream_with_duplicates", [&] {
    MedianFinder mf;
    const std::vector<int> stream = {5, 15, 1, 3, 2, 8, 7, 9, 10, 6, 11, 4};
    const std::vector<double> expected = {
        5.0, 10.0, 5.0, 4.0, 3.0, 4.0, 5.0, 6.0, 7.0, 6.5, 7.0, 6.5,
    };
    for (std::size_t i = 0uz; i < stream.size(); ++i) {
      mf.addNum(stream[i]);
      ctx.expect_true(approx_eq(mf.findMedian(), expected[i]));
    }
  });

  ctx.test("int_extremes_no_overflow", [&] {
    MedianFinder mf;
    const int max_v = std::numeric_limits<int>::max();
    const int min_v = std::numeric_limits<int>::min();

    mf.addNum(max_v);
    mf.addNum(max_v);
    ctx.expect_true(approx_eq(mf.findMedian(), static_cast<double>(max_v)));

    MedianFinder mf_mix;
    mf_mix.addNum(min_v);
    mf_mix.addNum(max_v);
    ctx.expect_true(approx_eq(mf_mix.findMedian(), -0.5));
  });
}

}  // namespace
