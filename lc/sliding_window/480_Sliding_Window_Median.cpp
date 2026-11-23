// LeetCode 480: Sliding Window Median
// https://leetcode.com/problems/sliding-window-median/
//
// Problem:
//   Given an integer array nums (1 <= k <= nums.length <= 10^5, elements in
//   the full 32-bit signed range [-2^31, 2^31 - 1]) and a sliding window size
//   k, compute the median of each contiguous window of size k as it slides one
//   element at a time from left to right. When k is even, the median is the
//   arithmetic mean of the two middle elements. Answers within 10^-5 of the
//   exact value are accepted.
//
// Algorithm:
//   Maintain the active window partitioned into two balanced std::multiset<int>
//   containers:
//   - `low_` holds the smaller ceil(k / 2) elements; its maximum (*low_.rbegin())
//     is the lower median.
//   - `high_` holds the larger floor(k / 2) elements; its minimum (*high_.begin())
//     is the upper median.
//   Invariant: every element in `low_` <= every element in `high_`, and
//   `low_.size() == high_.size()` (even k) or `low_.size() == high_.size() + 1`
//   (odd k).
//   For each incoming element, insert into `low_` or `high_` and rebalance by
//   moving a node handle (`extract` + `insert`, zero heap reallocation). Once
//   the window exceeds k elements, erase exactly one copy of the outgoing
//   element via iterator and rebalance. Compute the median in `double` to
//   prevent 32-bit integer overflow on extreme values like INT_MAX + INT_MAX.
//   Time Complexity: O(n log k) — each insertion, erasure, and rebalance is
//   O(log k).
//   Space Complexity: O(k) auxiliary space for the two multisets.
//
// Data Structures:
//   Two balanced red-black trees (`std::multiset<int>` `low_` and `high_`):
//   support O(log k) insertion, O(log k) arbitrary single-element deletion
//   with duplicates, O(log k) node-handle rebalancing, and O(1) median lookup.
//
// Real-World Applications:
//   - Robust signal denoising (1D rolling median filter) that removes impulse
//     spikes while preserving sharp edges in sensor/audio/image streams.
//   - Streaming latency monitoring (rolling p50 SLA tracking) and anomaly
//     detection where mean is overly sensitive to outliers.
//
// Related LeetCode Problems:
//   - #295 Find Median from Data Stream [done] (two-heap streaming median)
//   - #239 Sliding Window Maximum [done] (monotonic deque sliding window)
//   - #1825 Finding MK Average [new] (multi-multiset sliding window quantiles)
//   - #4 Median of Two Sorted Arrays [done] (order-statistic median finding)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <set>
#include <vector>

namespace {

constexpr double kEps = 1e-5;

// Balanced two-multiset structure maintaining the sorted lower and upper halves
// of a sliding window so insertion, single-copy deletion, and rebalancing all
// run in O(log k) time and median queries run in O(1) time.
class DualMultisetWindow {
 public:
  // Inserts `val` into the appropriate half and restores size balance.
  void insert(int val) {
    if (low_.empty() || val <= *std::prev(low_.end())) {
      low_.insert(val);
    } else {
      high_.insert(val);
    }
    rebalance();
  }

  // Removes a single occurrence of `val` from whichever half contains it and
  // restores size balance.
  void erase(int val) {
    if (auto it = low_.find(val); it != low_.end()) {
      low_.erase(it);
    } else if (auto hi_it = high_.find(val); hi_it != high_.end()) {
      high_.erase(hi_it);
    }
    rebalance();
  }

  // Returns the current window median as a double, avoiding signed 32-bit
  // integer overflow when averaging two middle elements.
  [[nodiscard]] double median() const {
    const double low_max = static_cast<double>(*std::prev(low_.end()));
    if (low_.size() > high_.size()) {
      return low_max;
    }
    const double high_min = static_cast<double>(*high_.begin());
    return (low_max + high_min) / 2.0;
  }

 private:
  // Enforces the invariant: high_.size() <= low_.size() <= high_.size() + 1.
  // Uses C++17 node extraction to transfer nodes without heap reallocation.
  void rebalance() {
    while (low_.size() > high_.size() + 1uz) {
      high_.insert(low_.extract(std::prev(low_.end())));
    }
    while (low_.size() < high_.size()) {
      low_.insert(high_.extract(high_.begin()));
    }
  }

  std::multiset<int> low_;
  std::multiset<int> high_;
};

class Solution {
 public:
  // Computes the median for every contiguous window of size k in nums.
  // Time: O(n log k), Space: O(k) auxiliary.
  std::vector<double> medianSlidingWindow(const std::vector<int>& nums, int k) {
    if (nums.empty() || k <= 0) {
      return {};
    }

    const auto win = static_cast<std::size_t>(k);
    if (win > nums.size()) {
      return {};
    }

    DualMultisetWindow window;
    std::vector<double> medians;
    medians.reserve((nums.size() - win) + 1uz);

    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      window.insert(nums[i]);

      // Once the window holds more than k elements, evict the element that
      // just slid past the left boundary.
      if (i >= win) {
        window.erase(nums[i - win]);
      }

      // Record the median as soon as the first full window of size k is ready.
      if (i + 1uz >= win) {
        medians.push_back(window.median());
      }
    }

    return medians;
  }
};

// Compares two vectors of doubles element-wise within tolerance kEps.
void expectMediansNear(lc::TestContext& ctx, const std::vector<double>& actual,
                       const std::vector<double>& expected) {
  ctx.expect_eq(actual.size(), expected.size());
  if (actual.size() != expected.size()) {
    return;
  }
  for (std::size_t i = 0uz; i < actual.size(); ++i) {
    ctx.expect_true(std::fabs(actual[i] - expected[i]) <= kEps);
  }
}

LC_PROBLEM(480, "Sliding Window Median", "sliding_window", ctx) {
  Solution sol;

  ctx.test("example_1_odd_window", [&] {
    const std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    const std::vector<double> expected = {1.0, -1.0, -1.0, 3.0, 5.0, 6.0};
    expectMediansNear(ctx, sol.medianSlidingWindow(nums, 3), expected);
  });

  ctx.test("python_build_even_full_window", [&] {
    const std::vector<int> nums = {1, 4, 2, 3};
    const std::vector<double> expected = {2.5};
    expectMediansNear(ctx, sol.medianSlidingWindow(nums, 4), expected);
  });

  ctx.test("even_sliding_window_with_duplicates", [&] {
    const std::vector<int> nums = {1, 2, 3, 4, 2, 3, 1, 4, 2};
    const std::vector<double> expected = {2.0, 3.0, 3.0, 3.0, 2.0, 3.0, 2.0};
    expectMediansNear(ctx, sol.medianSlidingWindow(nums, 3), expected);

    const std::vector<double> even_expected = {2.5, 2.5, 3.0, 2.5, 2.5, 2.5};
    expectMediansNear(ctx, sol.medianSlidingWindow(nums, 4), even_expected);
  });

  ctx.test("window_size_one", [&] {
    const std::vector<int> nums = {5, -2, 8};
    const std::vector<double> expected = {5.0, -2.0, 8.0};
    expectMediansNear(ctx, sol.medianSlidingWindow(nums, 1), expected);
  });

  ctx.test("int32_overflow_boundaries", [&] {
    constexpr int kMax = std::numeric_limits<int>::max();
    constexpr int kMin = std::numeric_limits<int>::min();
    const std::vector<int> nums = {kMax, kMax, kMin, kMin};
    const std::vector<double> expected = {static_cast<double>(kMax), -0.5,
                                          static_cast<double>(kMin)};
    expectMediansNear(ctx, sol.medianSlidingWindow(nums, 2), expected);
  });
}

}  // namespace
