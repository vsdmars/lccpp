// LeetCode 528: Random Pick With Weight
// https://leetcode.com/problems/random-pick-with-weight/
//
// Problem:
//   Given an array `w` of positive integers where `w[i]` represents the weight
//   of index `i` (0-indexed), implement a class that supports `pickIndex()`,
//   which randomly selects and returns an index `i` with probability
//   `w[i] / sum(w)`.
//   Constraints: 1 <= w.length <= 10^4, 1 <= w[i] <= 10^5, up to 10^4 calls to
//   `pickIndex()`.
//
// Algorithm:
//   Compute the cumulative prefix-sum array `prefix_sums_` in the constructor,
//   so index `i` owns the half-open interval `(prefix_sums_[i-1], prefix_sums_[i]]`
//   of integer length `w[i]` (with `prefix_sums_[-1] = 0`).
//   On each `pickIndex()` call, draw a uniform integer `target` in
//   `[1, total_weight]` and run a lower-bound binary search for the smallest
//   index `i` such that `target <= prefix_sums_[i]`. Because each index covers
//   exactly `w[i]` distinct integers out of `total_weight`, the selection
//   probability is exact. Zero-weight entries are naturally skipped because
//   `target >= 1`.
//   Time Complexity: O(n) constructor, O(log n) per `pickIndex()`.
//   Space Complexity: O(n) to store the prefix sums.
//
// Data Structures:
//   - `std::vector<std::int64_t>` prefix-sum array mapping contiguous integer
//     intervals to indices in O(log n) via binary search.
//   - `std::mt19937` PRNG and `std::uniform_int_distribution<std::int64_t>`
//     for unbiased discrete uniform sampling over `[1, total_weight]`.
//
// Real-World Applications:
//   - Weighted load balancing across backend servers with heterogeneous CPU
//     or memory capacities.
//   - A/B testing traffic allocation and canary rollout routing by percentage
//     weights.
//   - Ad-serving auction sampling and Monte Carlo / importance sampling in
//     simulations and reinforcement learning.
//
// Related LeetCode Problems:
//   - #382 Linked List Random Node [done] (uniform reservoir sampling over a stream)
//   - #398 Random Pick Index [done] (uniform random selection among matching indices)
//   - #497 Random Point in Non-overlapping Rectangles [new] (area-weighted prefix-sum binary search)
//   - #704 Binary Search [done] (fundamental lower-bound binary search template)
//   - #710 Random Pick with Blacklist [new] (remapping uniform random draws over valid indices)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace {

class Solution {
 public:
  static constexpr std::uint32_t kDefaultSeed = 42U;

  // Builds the prefix-sum distribution from `weights` in O(n) time and O(n)
  // space. An optional deterministic seed allows reproducible testing while
  // preserving the standard single-argument LeetCode constructor signature.
  explicit Solution(const std::vector<int>& weights,
                    std::uint32_t seed = kDefaultSeed)
      : rng_(seed) {
    prefix_sums_.reserve(weights.size());
    std::int64_t running = 0;
    for (const int weight : weights) {
      running += static_cast<std::int64_t>(weight);
      prefix_sums_.push_back(running);
    }
    if (!prefix_sums_.empty() && prefix_sums_.back() > 0) {
      dist_ = std::uniform_int_distribution<std::int64_t>(1,
                                                          prefix_sums_.back());
    }
  }

  // Randomly picks an index proportional to its weight in O(log n) time and
  // O(1) extra space.
  int pickIndex() {
    if (prefix_sums_.empty() || prefix_sums_.back() <= 0) {
      return -1;
    }
    const std::int64_t target = dist_(rng_);
    return findIndex(target);
  }

 private:
  // Finds the first index `i` such that `prefix_sums_[i] >= target` using a
  // half-open `[lhs, rhs)` binary search.
  // Invariant: for all `j < lhs`, `prefix_sums_[j] < target`; for all
  // `j >= rhs`, `prefix_sums_[j] >= target`.
  [[nodiscard]] int findIndex(std::int64_t target) const {
    std::size_t lhs = 0uz;
    std::size_t rhs = prefix_sums_.size();
    while (lhs < rhs) {
      const std::size_t mid = lhs + ((rhs - lhs) / 2uz);
      if (target <= prefix_sums_[mid]) {
        rhs = mid;
      } else {
        lhs = mid + 1uz;
      }
    }
    return static_cast<int>(lhs);
  }

  std::vector<std::int64_t> prefix_sums_;
  std::mt19937 rng_;
  std::uniform_int_distribution<std::int64_t> dist_{1, 1};
};

LC_PROBLEM(528, "Random Pick With Weight", "binary_search", ctx) {
  ctx.test("example_1_single_element", [&] {
    Solution sol({1});
    for (int i = 0; i < 20; ++i) {
      ctx.expect_eq(sol.pickIndex(), 0);
    }
  });

  ctx.test("example_2_ratio_1_to_3", [&] {
    Solution sol({1, 3}, 12345U);
    constexpr int kTrials = 20000;
    std::vector<int> counts(2, 0);
    for (int i = 0; i < kTrials; ++i) {
      const int idx = sol.pickIndex();
      ctx.expect_true(idx == 0 || idx == 1);
      ++counts[static_cast<std::size_t>(idx)];
    }
    const double p0 =
        static_cast<double>(counts[0]) / static_cast<double>(kTrials);
    const double p1 =
        static_cast<double>(counts[1]) / static_cast<double>(kTrials);
    ctx.expect_true(std::fabs(p0 - 0.25) < 0.02);
    ctx.expect_true(std::fabs(p1 - 0.75) < 0.02);
  });

  ctx.test("python_build_with_leading_zero_weight", [&] {
    // Matches build() = [0, 1, 3, 7]: index 0 has weight 0 and must never be
    // selected; indices 1, 2, 3 must be selected in 1:3:7 proportion.
    Solution sol({0, 1, 3, 7}, 2025U);
    constexpr int kTrials = 22000;
    std::vector<int> counts(4, 0);
    for (int i = 0; i < kTrials; ++i) {
      const int idx = sol.pickIndex();
      ctx.expect_true(idx >= 0 && idx < 4);
      ++counts[static_cast<std::size_t>(idx)];
    }
    ctx.expect_eq(counts[0], 0);
    const double p1 =
        static_cast<double>(counts[1]) / static_cast<double>(kTrials);
    const double p2 =
        static_cast<double>(counts[2]) / static_cast<double>(kTrials);
    const double p3 =
        static_cast<double>(counts[3]) / static_cast<double>(kTrials);
    ctx.expect_true(std::fabs(p1 - (1.0 / 11.0)) < 0.02);
    ctx.expect_true(std::fabs(p2 - (3.0 / 11.0)) < 0.02);
    ctx.expect_true(std::fabs(p3 - (7.0 / 11.0)) < 0.02);
  });

  ctx.test("skewed_1_to_99", [&] {
    Solution sol({1, 99}, 999U);
    constexpr int kTrials = 20000;
    int zero_count = 0;
    for (int i = 0; i < kTrials; ++i) {
      const int idx = sol.pickIndex();
      ctx.expect_true(idx == 0 || idx == 1);
      if (idx == 0) {
        ++zero_count;
      }
    }
    const double p0 =
        static_cast<double>(zero_count) / static_cast<double>(kTrials);
    ctx.expect_true(std::fabs(p0 - 0.01) < 0.005);
  });

  ctx.test("equal_weights_uniform", [&] {
    Solution sol({5, 5, 5, 5}, 777U);
    constexpr int kTrials = 20000;
    std::vector<int> counts(4, 0);
    for (int i = 0; i < kTrials; ++i) {
      const int idx = sol.pickIndex();
      ctx.expect_true(idx >= 0 && idx < 4);
      ++counts[static_cast<std::size_t>(idx)];
    }
    for (const int count : counts) {
      const double prob =
          static_cast<double>(count) / static_cast<double>(kTrials);
      ctx.expect_true(std::fabs(prob - 0.25) < 0.02);
    }
  });
}

}  // namespace
