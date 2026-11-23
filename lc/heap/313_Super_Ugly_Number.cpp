// LeetCode 313: Super Ugly Number
// https://leetcode.com/problems/super-ugly-number/
//
// Problem:
//   A super ugly number is a positive integer whose prime factors are all
//   contained in a given sorted list `primes` of `k` distinct primes. By
//   convention, `1` is the first super ugly number. Given an integer `n` and
//   `primes`, return the n-th super ugly number (guaranteed to fit in a 32-bit
//   signed integer).
//
// Examples:
//   Example 1:
//     Input:  n = 12, primes = [2, 7, 13, 19]
//     Output: 32
//     Explanation:
//       - Initialize uglies = [1] and seed min_heap with one cursor per prime: (2,
//         2, 0), (7, 7, 0), (13, 13, 0), and (19, 19, 0).
//       - Pop minimum 2 to append 2 and push (uglies[1] * 2 = 4, 2, 1), then pop 4
//         to append 4 and push (8, 2, 2), and continue popping the global minimum
//         in ascending order.
//       - When next_ugly = 14, the inner while loop pops both tied entries (14, 2,
//         3) and (14, 7, 1) and advances each to uglies[4] * 2 = 16 and uglies[2] *
//         7 = 28 so 14 is never emitted twice.
//       - After 11 iterations, uglies reaches size 12 with [1, 2, 4, 7, 8, 13, 14,
//         16, 19, 26, 28, 32] and returns uglies.back() = 32.
//
//   Example 2:
//     Input:  n = 10, primes = [2, 3, 5]
//     Output: 12
//     Explanation:
//       - Initialize uglies = [1] and seed min_heap with (2, 2, 0), (3, 3, 0), and
//         (5, 5, 0).
//       - The first four iterations append 2, 3, 4, and 5 to uglies and advance
//         cursors so both (6, 2, 2) and (6, 3, 1) sit at the top of min_heap.
//       - At next_ugly = 6, the inner loop pops both (6, 2, 2) and (6, 3, 1),
//         pushing (uglies[3] * 2 = 8, 2, 3) and (uglies[2] * 3 = 9, 3, 2) while
//         recording 6 once.
//       - Continuing through 8, 9, 10, and 12 produces uglies = [1, 2, 3, 4, 5, 6,
//         8, 9, 10, 12], returning 12.
//
//   Example 3:
//     Input:  n = 5, primes = [3]
//     Output: 81
//     Explanation:
//       - Initialize uglies = [1] and seed min_heap with the single stream cursor
//         (3, 3, 0).
//       - Iteration 1 pops (3, 3, 0), appends 3 to uglies, and pushes (uglies[1] *
//         3 = 9, 3, 1).
//       - Iterations 2 and 3 pop (9, 3, 1) and (27, 3, 2) in sequence, extending
//         uglies to [1, 3, 9, 27] and pushing (81, 3, 3).
//       - Iteration 4 pops (81, 3, 3), appends 81 so uglies.size() reaches 5, and
//         returns 81.
//
// Algorithm:
//   K-way merge of self-referential sorted candidate streams using a min-heap
//   (direct C++23 realization of the Python `heapq.merge` generator solution):
//   Every super ugly number > 1 equals `uglies[idx] * p` for some earlier
//   super ugly number `uglies[idx]` and some prime `p` in `primes`. Thus each
//   prime `p` defines a monotonically increasing lazy stream
//   `uglies[0] * p, uglies[1] * p, ...`.
//   1. Start with `uglies = [1]` and seed a min-heap with one cursor per prime:
//      `(value = p, prime = p, ugly_idx = 0)`.
//   2. Repeatedly take the minimum `next_ugly = min_heap.top().value` and
//      append it to `uglies`.
//   3. Advance EVERY heap cursor whose current `value == next_ugly` to
//      `ugly_idx + 1` and push `(uglies[ugly_idx + 1] * prime, prime, ugly_idx + 1)`.
//      Advancing all tied cursors simultaneously deduplicates identical
//      products (e.g., `2 * 7` and `7 * 2`) in one pass.
//   Products are computed in `std::int64_t` because unused lookahead candidates
//   can exceed `INT_MAX` even when the n-th super ugly number fits in 32 bits.
//   Time Complexity: O(n log k) — each generated ugly number advances at least
//   one cursor (at most `k` total per duplicate value, amortized O(log k) per
//   heap operation).
//   Space Complexity: O(n + k) — `uglies` stores `n` numbers and the heap
//   always contains exactly `k` entries.
//
// Data Structures:
//   - std::vector<int> (`uglies`): stores the generated super ugly sequence in
//     ascending order so every prime stream can index into past outputs in O(1).
//   - std::priority_queue<StreamEntry, ..., std::greater<>> (min-heap of size
//     `k`): performs k-way merge across the `k` lazy prime-multiple streams.
//
// Real-World Applications:
//   - Fast Fourier Transform (FFT) buffer-size selection (smooth / Hamming
//     numbers factored over small radices such as 2, 3, 5, 7 for optimal
//     mixed-radix Cooley-Tukey performance).
//   - Streaming k-way merge of sorted iterators in external merge sort and
//     LSM-tree compaction engines.
//   - Lazy recurrence / Hamming-sequence generation in functional runtimes.
//
// Related LeetCode Problems:
//   - #264 Ugly Number II [new] (special case with fixed primes = [2, 3, 5])
//   - #263 Ugly Number [new] (checks whether a single integer has only prime factors 2, 3, 5)
//   - #23 Merge k Sorted Lists [done] (canonical k-way sorted merge using a min-heap)
//   - #373 Find K Pairs With Smallest Sums [done] (lazy heap-driven k-way enumeration)
//   - #1201 Ugly Number III [new] (binary search + inclusion-exclusion for multiples of a, b, c)

#include "framework/lc_framework.h"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the n-th super ugly number for the given prime factors using a
  // size-k min-heap k-way merge.
  // Time: O(n log k), Space: O(n + k).
  int nthSuperUglyNumber(int n, const std::vector<int>& primes) {
    if (n <= 1 || primes.empty()) {
      return 1;
    }

    // Each entry tracks the head of one lazy stream: `uglies[ugly_idx] * prime`.
    struct StreamEntry {
      std::int64_t value = 0;
      int prime = 0;
      std::size_t ugly_idx = 0uz;

      auto operator<=>(const StreamEntry&) const = default;
    };

    const auto target = static_cast<std::size_t>(n);
    std::vector<int> uglies;
    uglies.reserve(target);
    uglies.push_back(1);

    std::priority_queue<StreamEntry, std::vector<StreamEntry>, std::greater<>>
        min_heap;
    for (const int prime : primes) {
      min_heap.push(StreamEntry{
          .value = static_cast<std::int64_t>(prime),
          .prime = prime,
          .ugly_idx = 0uz,
      });
    }

    while (uglies.size() < target) {
      const std::int64_t next_ugly = min_heap.top().value;
      uglies.push_back(static_cast<int>(next_ugly));

      // Advance every prime stream currently yielding `next_ugly` so duplicate
      // factorizations (e.g., 2*7 and 7*2) are never emitted twice.
      while (!min_heap.empty() && min_heap.top().value == next_ugly) {
        const StreamEntry top = min_heap.top();
        min_heap.pop();

        // Since every prime >= 2, `uglies[top.ugly_idx] < next_ugly`, so
        // `next_idx` is guaranteed to be within `uglies`.
        const std::size_t next_idx = top.ugly_idx + 1uz;
        const std::int64_t next_val =
            static_cast<std::int64_t>(uglies[next_idx]) *
            static_cast<std::int64_t>(top.prime);
        min_heap.push(StreamEntry{
            .value = next_val,
            .prime = top.prime,
            .ugly_idx = next_idx,
        });
      }
    }

    return uglies.back();
  }
};

// Independent O(n * k) pointer-array reference implementation used to cross-check
// larger test cases.
int reference_dp_super_ugly(int n, const std::vector<int>& primes) {
  const auto target = static_cast<std::size_t>(n);
  const std::size_t k = primes.size();
  std::vector<int> uglies(target, 1);
  std::vector<std::size_t> ptrs(k, 0uz);

  for (std::size_t i = 1uz; i < target; ++i) {
    std::int64_t best = std::numeric_limits<std::int64_t>::max();
    for (std::size_t j = 0uz; j < k; ++j) {
      const std::int64_t cand = static_cast<std::int64_t>(uglies[ptrs[j]]) *
                                static_cast<std::int64_t>(primes[j]);
      best = std::min(cand, best);
    }
    uglies[i] = static_cast<int>(best);
    for (std::size_t j = 0uz; j < k; ++j) {
      const std::int64_t cand = static_cast<std::int64_t>(uglies[ptrs[j]]) *
                                static_cast<std::int64_t>(primes[j]);
      if (cand == best) {
        ++ptrs[j];
      }
    }
  }
  return uglies.back();
}

LC_PROBLEM(313, "Super Ugly Number", "heap", ctx) {
  ctx.test("example_1_full_prefix", [&] {
    Solution sol;
    const std::vector<int> primes = {2, 7, 13, 19};
    const std::vector<int> expected = {
        1, 2, 4, 7, 8, 13, 14, 16, 19, 26, 28, 32,
    };
    for (std::size_t i = 0uz; i < expected.size(); ++i) {
      ctx.expect_eq(sol.nthSuperUglyNumber(static_cast<int>(i + 1uz), primes),
                    expected[i]);
    }
  });

  ctx.test("example_2_n_equals_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.nthSuperUglyNumber(1, {2, 3, 5}), 1);
  });

  ctx.test("classic_ugly_numbers", [&] {
    Solution sol;
    ctx.expect_eq(sol.nthSuperUglyNumber(10, {2, 3, 5}), 12);
    ctx.expect_eq(sol.nthSuperUglyNumber(15, {2, 3, 5}), 24);
  });

  ctx.test("single_prime_powers", [&] {
    Solution sol;
    ctx.expect_eq(sol.nthSuperUglyNumber(5, {3}), 81);
  });

  ctx.test("large_n_matches_reference_dp_without_overflow", [&] {
    Solution sol;
    const std::vector<int> primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    const int expected = reference_dp_super_ugly(500, primes);
    ctx.expect_eq(sol.nthSuperUglyNumber(500, primes), expected);
  });
}

}  // namespace
