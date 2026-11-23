// LeetCode 1387: Sort Integers By The Power Value
// https://leetcode.com/problems/sort-integers-by-the-power-value/
//
// Problem:
//   The power value of a positive integer `x` is the number of Collatz steps
//   required to transform `x` into 1, where each step replaces `x` with
//   `x / 2` if `x` is even, or `3 * x + 1` if `x` is odd. Given integers
//   `lo`, `hi`, and `k` (1 <= lo <= hi <= 1000, 1 <= k <= hi - lo + 1), sort
//   all integers in `[lo, hi]` by ascending power value (breaking ties by
//   ascending integer value) and return the 1-indexed `k`-th integer.
//
// Algorithm:
//   Top-Down Memoized Collatz Computation + Min-Heap Selection:
//   1. Compute `power(x)` recursively with a hash map memo table initialized
//      with `memo[1] = 0`. Use 64-bit intermediate values (`std::int64_t`) so
//      Collatz trajectories never risk 32-bit signed overflow.
//   2. Collect `(power(n), n)` for every `n` in `[lo, hi]` into a vector and
//      build a min-heap in O(M) time (where M = hi - lo + 1).
//   3. Pop `k - 1` smallest elements from the min-heap and return the value of
//      the `k`-th element at the top.
//   Invariant: Because Collatz trajectories share long suffixes (e.g., powers
//   of 2 and small odd cycles leading to 1), memoization ensures every
//   distinct visited intermediate integer is evaluated at most once.
//   Time Complexity:  O(U + M + k log M), where M = hi - lo + 1 and U is the
//                     total number of distinct intermediate Collatz states
//                     visited across `[lo, hi]`.
//   Space Complexity: O(U + M) for the memoization table and min-heap.
//
// Data Structures:
//   - Hash map (`std::unordered_map<std::int64_t, int>`): caches already
//     computed Collatz step counts across overlapping trajectories.
//   - Min-heap (`std::priority_queue<PowerEntry, ..., std::greater<>>`):
//     extracts the `k` smallest `(power, value)` pairs in O(M + k log M) time.
//
// Real-World Applications:
//   - Top-k ranking and query-result pagination when ranking keys are
//     expensive to compute and share overlapping sub-computations.
//   - Memoized cost evaluation in dynamic program / workflow DAG schedulers
//     selecting the k cheapest jobs.
//   - Iterative convergence-rate benchmarking (ranking starting states by
//     iterations needed to reach a fixed point).
//
// Related LeetCode Problems:
//   - #215 Kth Largest Element in an Array [done] (k-th order statistic via heap / quickselect)
//   - #973 K Closest Points to Origin [done] (top-k selection by computed metric with heap)
//   - #347 Top K Frequent Elements [done] (ranking items by derived score using a heap)
//   - #1356 Sort Integers by The Number of 1 Bits [done] (custom primary/tie-break integer sort)
//   - #692 Top K Frequent Words [done] (top-k extraction with deterministic tie-breaking)

#include "framework/lc_framework.h"

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Pairs an integer with its Collatz power value; natural ordering sorts by
// `power` ascending and breaks ties by `value` ascending.
struct PowerEntry {
  int power = 0;
  int value = 0;

  auto operator<=>(const PowerEntry&) const = default;
};

class Solution {
 public:
  // Returns the 1-indexed `k`-th integer in `[lo, hi]` when ordered by
  // ascending Collatz power value (tie-broken by integer value ascending).
  //
  // Time Complexity:  O(U + M + k log M) where M = hi - lo + 1
  // Space Complexity: O(U + M)
  [[nodiscard]] int getKth(int lo, int hi, int k) {
    // Memoization map from integer value -> steps to reach 1.
    std::unordered_map<std::int64_t, int> memo;
    memo.emplace(1, 0);

    // C++23 deducing-this recursive lambda for memoized Collatz step count.
    auto compute_power = [&](this auto&& self, std::int64_t x) -> int {
      if (const auto it = memo.find(x); it != memo.end()) {
        return it->second;
      }

      const std::int64_t next = ((x % 2) != 0) ? ((3 * x) + 1) : (x / 2);
      const int steps = 1 + self(next);
      memo.emplace(x, steps);
      return steps;
    };

    // Evaluate power values for every integer in [lo, hi].
    const std::size_t range_size =
        static_cast<std::size_t>(hi) - static_cast<std::size_t>(lo) + 1uz;
    std::vector<PowerEntry> entries;
    entries.reserve(range_size);
    for (int n = lo; n <= hi; ++n) {
      entries.push_back(PowerEntry{
          .power = compute_power(static_cast<std::int64_t>(n)),
          .value = n,
      });
    }

    // Build min-heap in linear O(M) time, then extract the k-th smallest.
    std::priority_queue<PowerEntry, std::vector<PowerEntry>, std::greater<>>
        min_heap(std::greater<>{}, std::move(entries));

    for (int pop_count = 0; pop_count < k - 1; ++pop_count) {
      min_heap.pop();
    }

    return min_heap.top().value;
  }
};

LC_PROBLEM(1387, "Sort Integers By The Power Value", "heap", ctx) {
  ctx.test("example_1_tie_breaking", [&] {
    Solution sol;
    // Powers: 12->9, 13->9, 14->17, 15->17 -> sorted [12, 13, 14, 15].
    ctx.expect_eq(sol.getKth(12, 15, 1), 12);
    ctx.expect_eq(sol.getKth(12, 15, 2), 13);
    ctx.expect_eq(sol.getKth(12, 15, 3), 14);
    ctx.expect_eq(sol.getKth(12, 15, 4), 15);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    // Powers: 8->3, 10->6, 11->14, 7->16, 9->19 -> 4th is 7.
    ctx.expect_eq(sol.getKth(7, 11, 4), 7);
  });

  ctx.test("single_element_base_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.getKth(1, 1, 1), 1);
  });

  ctx.test("range_1_to_10", [&] {
    Solution sol;
    // Sorted by (power, val): 1(0), 2(1), 4(2), 8(3), 5(5), 10(6), 3(7), 6(8),
    // 7(16), 9(19).
    ctx.expect_eq(sol.getKth(1, 10, 1), 1);
    ctx.expect_eq(sol.getKth(1, 10, 5), 5);
    ctx.expect_eq(sol.getKth(1, 10, 7), 3);
    ctx.expect_eq(sol.getKth(1, 10, 10), 9);
  });
}

}  // namespace
