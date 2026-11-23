// LeetCode 667: Beautiful Arrangement II
// https://leetcode.com/problems/beautiful-arrangement-ii/
//
// Problem:
//   Given two integers `n` and `k` (`1 <= k < n`), construct any permutation
//   `ans` of the integers `1, 2, ..., n` such that the list of adjacent
//   absolute differences `[|ans[0] - ans[1]|, |ans[1] - ans[2]|, ...,
//   |ans[n - 2] - ans[n - 1]|]` contains *exactly* `k` distinct integers.
//
// Algorithm:
//   Constructive Monotonic Prefix + Alternating Zig-Zag Suffix (O(n) time,
//   O(1) auxiliary space beyond the output array):
//     1. Emit the ascending prefix `1, 2, ..., n - k - 1`. Every adjacent pair
//        inside this prefix has difference `1`, and the boundary transition
//        from `n - k - 1` into `n - k` also has difference `1`.
//     2. Emit the remaining `k + 1` numbers `[n - k, n]` in alternating low /
//        high order: `lo, hi, lo + 1, hi - 1, ...` where initially `lo = n - k`
//        and `hi = n`.
//        Because the two pointers start `k` apart and converge by 1 on each
//        step, this suffix produces the exact sequence of adjacent differences
//        `k, k - 1, k - 2, ..., 1` — which is precisely the set `{1, 2, ..., k}`
//        of size `k`. Since `1` is already included in that set, the prefix
//        adds zero extra distinct differences.
//
//   Time Complexity: O(n) — each integer from `1` to `n` is appended once.
//   Space Complexity: O(1) auxiliary space (O(n) for the returned vector).
//
// Data Structures:
//   - Pre-reserved `std::vector<int>` storing the constructed permutation.
//   - Converging two-pointer bounds (`lo`, `hi`) generating the zig-zag tail.
//
// Real-World Applications:
//   - Synthetic benchmark and stress-test generation (producing test sequences
//     with exact stride/delta cardinality for cache, branch-predictor, and
//     delta-encoding compression evaluators).
//   - Combinatorial experimental design and frequency-hopping schedules that
//     require a prescribed number of distinct step sizes without repeats.
//   - Cryptographic permutation and S-box differential-profile prototyping.
//
// Related LeetCode Problems:
//   - #526 Beautiful Arrangement [done] (backtracking count of divisibility permutations)
//   - #280 Wiggle Sort [done] (alternating low/high ordering constraint)
//   - #324 Wiggle Sort II [done] (strict alternating ordering via interleaved halves)
//   - #1313 Decompress Run-Length Encoded List [new] (direct constructive array generation)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdlib>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Constructs a permutation of `[1..n]` whose adjacent absolute differences
  // contain exactly `k` distinct values.
  //
  // Invariant:
  //   - Prefix `[1 .. n - k - 1]` contributes only difference `1`.
  //   - Alternating suffix from `[n - k .. n]` contributes every difference
  //     from `k` down to `1`, yielding exactly `k` distinct differences overall.
  //
  // Time Complexity: O(n).
  // Space Complexity: O(1) auxiliary (O(n) for the output vector).
  std::vector<int> constructArray(int n, int k) {
    if (n <= 0 || k <= 0 || k >= n) {
      return {};
    }

    std::vector<int> result;
    result.reserve(static_cast<std::size_t>(n));

    // Phase 1: Ascending prefix `1, 2, ..., n - k - 1` produces only step 1.
    const int prefix_end = n - k - 1;
    for (int val = 1; val <= prefix_end; ++val) {
      result.push_back(val);
    }

    // Phase 2: Interleave the remaining range `[n - k, n]` from both ends to
    // realize distinct steps `k, k - 1, ..., 1` without introducing duplicates.
    int lo = n - k;
    int hi = n;
    bool pick_low = true;
    while (lo <= hi) {
      if (pick_low) {
        result.push_back(lo);
        ++lo;
      } else {
        result.push_back(hi);
        --hi;
      }
      pick_low = !pick_low;
    }

    return result;
  }
};

// Verifies that `arr` is a valid permutation of `[1..n]` whose adjacent
// absolute differences contain exactly `k` distinct integers.
bool is_valid_arrangement(const std::vector<int>& arr, int n, int k) {
  if (arr.size() != static_cast<std::size_t>(n)) {
    return false;
  }

  std::vector<bool> seen(static_cast<std::size_t>(n) + 1, false);
  for (int val : arr) {
    if (val < 1 || val > n) {
      return false;
    }
    const auto idx = static_cast<std::size_t>(val);
    if (seen[idx]) {
      return false;
    }
    seen[idx] = true;
  }

  std::unordered_set<int> diffs;
  for (std::size_t i = 1; i < arr.size(); ++i) {
    diffs.insert(std::abs(arr[i] - arr[i - 1]));
  }
  return diffs.size() == static_cast<std::size_t>(k);
}

LC_PROBLEM(667, "Beautiful Arrangement II", "array", ctx) {
  Solution sol;

  ctx.test("example_1_n3_k1", [&] {
    const auto ans = sol.constructArray(3, 1);
    ctx.expect_eq(ans, std::vector<int>{1, 2, 3});
    ctx.expect_true(is_valid_arrangement(ans, 3, 1));
  });

  ctx.test("example_2_n3_k2", [&] {
    const auto ans = sol.constructArray(3, 2);
    ctx.expect_eq(ans, std::vector<int>{1, 3, 2});
    ctx.expect_true(is_valid_arrangement(ans, 3, 2));
  });

  ctx.test("all_valid_k_for_n6", [&] {
    for (int k = 1; k < 6; ++k) {
      const auto ans = sol.constructArray(6, k);
      ctx.expect_true(is_valid_arrangement(ans, 6, k));
    }
  });

  ctx.test("minimal_n2_k1", [&] {
    const auto ans = sol.constructArray(2, 1);
    ctx.expect_eq(ans, std::vector<int>{1, 2});
    ctx.expect_true(is_valid_arrangement(ans, 2, 1));
  });

  ctx.test("larger_n_max_and_mid_k", [&] {
    ctx.expect_true(is_valid_arrangement(sol.constructArray(10, 9), 10, 9));
    ctx.expect_true(is_valid_arrangement(sol.constructArray(10, 5), 10, 5));
    ctx.expect_true(is_valid_arrangement(sol.constructArray(10, 1), 10, 1));
  });
}

}  // namespace
