// LeetCode 907: Sum Of Subarray Minimums
// https://leetcode.com/problems/sum-of-subarray-minimums/
//
// Problem:
//   Given an integer array `arr`, find the sum of `min(b)` over every
//   contiguous subarray `b` of `arr`. Return the result modulo 10^9 + 7.
//
// Examples:
//   Example 1:
//     Input:  arr = [3, 1, 2, 4]
//     Output: 17
//     Explanation:
//       - Push index 0 (3); at i = 1 (1), arr[0] > arr[1] pops mid = 0 with
//         left_span = 1 and right_span = 1, adding 3 * 1 * 1 = 3 to total before
//         pushing index 1.
//       - Indices 2 (2) and 3 (4) preserve non-decreasing order, leaving stack =
//         [1, 2, 3].
//       - At sentinel i = 4, popping mid = 3 (4) gives left_span = 3 - 2 = 1 and
//         right_span = 4 - 3 = 1 (+4), and popping mid = 2 (2) gives left_span = 2
//         - 1 = 1 and right_span = 4 - 2 = 2 (+4), bringing total to 11.
//       - Popping mid = 1 (1) empties stack, giving left_span = 1 + 1 = 2 and
//         right_span = 4 - 1 = 3 (+6), so sumSubarrayMins returns 17.
//
//   Example 2:
//     Input:  arr = [11, 81, 94, 43, 3]
//     Output: 444
//     Explanation:
//       - Indices 0, 1, and 2 (11, 81, 94) are increasing and push onto stack = [0,
//         1, 2].
//       - At i = 3 (43), pop mid = 2 (94) with spans 1 and 1 (+94) and mid = 1 (81)
//         with spans 1 and 2 (+162), pushing 3 so stack = [0, 3] and total = 256.
//       - At i = 4 (3), pop mid = 3 (43) with left_span = 3 - 0 = 3 and right_span
//         = 1 (+129) and mid = 0 (11) with left_span = 1 and right_span = 4 (+44),
//         pushing 4 so stack = [4] and total = 429.
//       - At sentinel i = 5, pop mid = 4 (3) with left_span = 5 and right_span = 1
//         (+15), returning total = 444.
//
//   Example 3:
//     Input:  arr = [5, 5, 5]
//     Output: 30
//     Explanation:
//       - Because the pop condition requires strict arr[stack.back()] > arr[i],
//         equal values at i = 0, 1, 2 all push onto stack = [0, 1, 2].
//       - At sentinel i = 3, popping mid = 2 gives left_span = 1 and right_span = 1
//         (+5), and popping mid = 1 gives left_span = 1 and right_span = 2 (+10).
//       - Popping mid = 0 leaves stack empty with left_span = 1 and right_span = 3
//         (+15), totaling 5 + 10 + 15 = 30 without double-counting duplicate
//         subarrays.
//
// Algorithm:
//   Contribution technique powered by a monotonic non-decreasing stack.
//   Instead of enumerating all O(N^2) subarrays, compute for each index `mid`
//   how many subarrays have `arr[mid]` as their unique chosen minimum:
//   - Let `left` be the nearest index strictly before `mid` with
//     `arr[left] <= arr[mid]` (or `-1` if none exists).
//   - Let `right` be the nearest index strictly after `mid` with
//     `arr[right] < arr[mid]` (or `N` if none exists).
//   Asymmetric tie-breaking (`<=` on the left, `<` on the right) guarantees
//   every subarray with duplicate minimum values is attributed to exactly one
//   index (the leftmost minimum in that subarray), preventing double counting.
//   The number of subarrays whose chosen minimum is `arr[mid]` is
//   `(mid - left) * (right - mid)`, contributing
//   `arr[mid] * (mid - left) * (right - mid)` to the total.
//   Maintain a stack of indices with monotonically non-decreasing values.
//   Scanning `i` from `0` to `N` (where `i == N` acts as a virtual `-infinity`
//   sentinel), whenever `i == N || arr[stack.back()] > arr[i]`, pop `mid`:
//   `right = i`, and `left` is the new `stack.back()` (or `-1` if empty).
//   Time Complexity: O(N), since every index is pushed and popped once.
//   Space Complexity: O(N) for the index stack.
//
// Data Structures:
//   - `std::vector<std::size_t> stack`: monotonic non-decreasing stack storing
//     array indices whose right strictly-smaller boundary has not yet been
//     encountered.
//
// Real-World Applications:
//   - Financial risk analytics computing aggregated worst-case drawdown /
//     minimum price metrics across all sliding time windows.
//   - Database range-aggregate query optimization and time-series window
//     statistics over streaming telemetry.
//   - Computational geometry and histogram area / bottleneck-span analysis
//     (closely related to Largest Rectangle in Histogram).
//
// Related LeetCode Problems:
//   - #84 Largest Rectangle in Histogram [done] (left/right smaller boundary stack)
//   - #2104 Sum of Subarray Ranges [new] (subarray max sum minus min sum)
//   - #1856 Maximum Subarray Min-Product [new] (same monotonic stack + prefix sums)
//   - #739 Daily Temperatures [done] (next greater element monotonic stack)
//   - #496 Next Greater Element I [done] (introductory monotonic stack)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the sum of minimums of all contiguous subarrays of `arr` modulo
  // 10^9 + 7 using a single-pass monotonic stack.
  // Invariant: for every consecutive pair `(a, b)` in `stack`,
  // `arr[a] <= arr[b]` and every element strictly between `a` and `b` is
  // strictly greater than `arr[b]`.
  // Time: O(N), Space: O(N).
  int sumSubarrayMins(const std::vector<int>& arr) {
    static constexpr std::int64_t kMod = 1'000'000'007LL;

    const std::size_t n = arr.size();
    std::vector<std::size_t> stack;
    stack.reserve(n);

    std::int64_t total = 0;

    // Run to `i == n` so a virtual -infinity sentinel flushes all remaining
    // indices from the stack without copying or mutating `arr`.
    for (std::size_t i = 0uz; i <= n; ++i) {
      while (!stack.empty() && (i == n || arr[stack.back()] > arr[i])) {
        const std::size_t mid = stack.back();
        stack.pop_back();

        // Left span extends back to (but excludes) the previous smaller-or-equal
        // element on the stack; right span extends forward to (but excludes) `i`,
        // the first strictly smaller element.
        const std::int64_t left_span =
            stack.empty() ? (static_cast<std::int64_t>(mid) + 1)
                          : static_cast<std::int64_t>(mid - stack.back());
        const auto right_span = static_cast<std::int64_t>(i - mid);

        const std::int64_t subarrays = (left_span * right_span) % kMod;
        const std::int64_t contrib =
            (static_cast<std::int64_t>(arr[mid]) * subarrays) % kMod;
        total = (total + contrib) % kMod;
      }

      if (i < n) {
        stack.push_back(i);
      }
    }

    return static_cast<int>(total);
  }
};

LC_PROBLEM(907, "Sum Of Subarray Minimums", "stack", ctx) {
  Solution sol;

  ctx.test("example 1",
           [&] { ctx.expect_eq(sol.sumSubarrayMins({3, 1, 2, 4}), 17); });

  ctx.test("example 2 with duplicates",
           [&] { ctx.expect_eq(sol.sumSubarrayMins({11, 81, 94, 43, 3}), 444); });

  ctx.test("duplicates do not double count", [&] {
    ctx.expect_eq(sol.sumSubarrayMins({2, 2}), 6);
    ctx.expect_eq(sol.sumSubarrayMins({71, 55, 82, 55}), 593);
    ctx.expect_eq(sol.sumSubarrayMins({5, 5, 5}), 30);
  });

  ctx.test("mixed rise and fall from python build",
           [&] { ctx.expect_eq(sol.sumSubarrayMins({1, 3, 5, 7, 6, 4}), 71); });

  ctx.test("single element and monotonic arrays", [&] {
    ctx.expect_eq(sol.sumSubarrayMins({42}), 42);
    ctx.expect_eq(sol.sumSubarrayMins({1, 2, 3, 4}), 20);
    ctx.expect_eq(sol.sumSubarrayMins({4, 3, 2, 1}), 20);
  });

  ctx.test("large values modulo 10^9 + 7", [&] {
    // 1000 identical elements of value 30000:
    // total subarrays = 1000 * 1001 / 2 = 500,500
    // sum = 500,500 * 30,000 = 15,015,000,000
    // 15,015,000,000 mod (10^9 + 7) = 14,999,895.
    const std::vector<int> large(1000uz, 30000);
    ctx.expect_eq(sol.sumSubarrayMins(large), 14'999'895);
  });
}

}  // namespace
