// LeetCode 278: First Bad Version
// https://leetcode.com/problems/first-bad-version/
//
// Problem:
//   You are managing n sequential product versions [1, 2, ..., n]. Because
//   each version is built on the previous one, once a version fails quality
//   checks, every subsequent version is also bad. Given an oracle API
//   `isBadVersion(version)`, find the first bad version while minimizing the
//   number of API calls.
//
// Examples:
//   Example 1:
//     Input:  n = 5, firstBadVersion = 4
//     Output: 4
//     Explanation:
//       - Initialize closed search bounds lhs = 1 and rhs = 5.
//       - Iteration 1 checks mid = 1 + (5 - 1) / 2 = 3; isBadVersion(3) is false,
//         so narrow to [4, 5] by setting lhs = 4.
//       - Iteration 2 checks mid = 4 + (5 - 4) / 2 = 4; isBadVersion(4) is true, so
//         narrow to [4, 4] by setting rhs = 4.
//       - Since lhs == rhs == 4, the loop ends and returns 4 after 2 oracle calls.
//
//   Example 2:
//     Input:  n = 7, firstBadVersion = 5
//     Output: 5
//     Explanation:
//       - Initialize search bounds lhs = 1 and rhs = 7.
//       - Iteration 1 tests mid = 4 and gets false, updating lhs = 5; iteration 2
//         tests mid = 6 and gets true, updating rhs = 6.
//       - Iteration 3 tests mid = 5 + (6 - 5) / 2 = 5; isBadVersion(5) is true,
//         updating rhs = 5.
//       - The interval collapses to lhs == rhs == 5, returning 5 after 3 oracle
//         calls.
//
//   Example 3:
//     Input:  n = 1, firstBadVersion = 1
//     Output: 1
//     Explanation:
//       - Initialize search bounds lhs = 1 and rhs = 1.
//       - The loop condition while (lhs < rhs) is immediately false because 1 < 1
//         does not hold.
//       - Return lhs = 1 directly with 0 calls to isBadVersion.
//
// Algorithm:
//   Lower-bound binary search over the monotonic boolean predicate
//   `isBadVersion(v)` on the closed interval [lhs, rhs] = [1, n]:
//     - Maintain the invariant that the first bad version lies in [lhs, rhs].
//     - Compute mid = lhs + ((rhs - lhs) / 2) to prevent signed 32-bit
//       overflow when n approaches INT_MAX.
//     - If isBadVersion(mid) is true, mid itself could be the first bad
//       version, so narrow the search to [lhs, mid] (rhs = mid).
//     - Otherwise, mid and all earlier versions are good, so narrow the search
//       to [mid + 1, rhs] (lhs = mid + 1).
//     - When lhs == rhs, the interval has collapsed to the first bad version.
//   Time: O(log n) API calls, Space: O(1).
//
// Data Structures:
//   Two integer search bounds (lhs, rhs) plus a lightweight mock oracle inside
//   Solution that records the number of API invocations for verification.
//
// Real-World Applications:
//   - `git bisect` locating the exact commit that introduced a regression in
//     a linear history of builds.
//   - Automated canary rollback / binary-searching release artifacts or
//     compiler flags to isolate the first failing version.
//   - Hardware and firmware burn-in bisection across monotonic voltage or
//     frequency steps.
//
// Related LeetCode Problems:
//   - #35 Search Insert Position [done] (classic lower-bound binary search on a sorted array)
//   - #34 Find First and Last Position of Element in Sorted Array [done] (leftmost/rightmost boundary binary search)
//   - #374 Guess Number Higher Or Lower [done] (interactive oracle binary search with three-way feedback)
//   - #162 Find Peak Element [done] (binary search on a local monotonicity invariant)
//   - #704 Binary Search [done] (foundational interval halving)

#include "framework/lc_framework.h"

#include <limits>

namespace {

// Models LeetCode's interactive VersionControl judge API together with
// Solution::firstBadVersion and tracks total oracle calls.
class Solution {
 public:
  explicit Solution(int first_bad) : first_bad_(first_bad) {}

  // Returns the smallest version in [1, n] for which isBadVersion(v) is true.
  //
  // Invariant: Every version < lhs is good, and version rhs is bad (assuming
  // at least one bad version exists in [1, n]).
  // Time: O(log n), Space: O(1).
  int firstBadVersion(int n) {
    int lhs = 1;
    int rhs = n;

    while (lhs < rhs) {
      // Overflow-safe midpoint calculation for n up to INT_MAX.
      const int mid = lhs + ((rhs - lhs) / 2);

      if (isBadVersion(mid)) {
        // mid is bad; the first bad version is at mid or to its left.
        rhs = mid;
      } else {
        // mid is good; the first bad version must be strictly to its right.
        lhs = mid + 1;
      }
    }

    return lhs;
  }

  // Returns how many times the mock isBadVersion API was queried.
  [[nodiscard]] int callCount() const noexcept { return call_count_; }

 private:
  // Simulated judge oracle: versions >= first_bad_ are bad.
  bool isBadVersion(int version) {
    ++call_count_;
    return version >= first_bad_;
  }

  int first_bad_ = 1;
  int call_count_ = 0;
};

LC_PROBLEM(278, "First Bad Version", "binary_search", ctx) {
  ctx.test("python_source_example", [&] {
    // Mirrors the Python fixture: n = 7 (and up to 10), first bad = 5.
    Solution sol(5);
    ctx.expect_eq(sol.firstBadVersion(7), 5);
    ctx.expect_true(sol.callCount() <= 4);
  });

  ctx.test("leetcode_example_n5_bad4", [&] {
    Solution sol(4);
    ctx.expect_eq(sol.firstBadVersion(5), 4);
    ctx.expect_true(sol.callCount() <= 3);
  });

  ctx.test("boundary_first_and_last_versions", [&] {
    Solution first_sol(1);
    ctx.expect_eq(first_sol.firstBadVersion(1), 1);
    ctx.expect_eq(first_sol.callCount(), 0);

    Solution first_of_many(1);
    ctx.expect_eq(first_of_many.firstBadVersion(100), 1);
    ctx.expect_true(first_of_many.callCount() <= 7);

    Solution last_of_many(100);
    ctx.expect_eq(last_of_many.firstBadVersion(100), 100);
    ctx.expect_true(last_of_many.callCount() <= 7);
  });

  ctx.test("int_max_no_overflow", [&] {
    constexpr int kMaxInt = std::numeric_limits<int>::max();

    Solution sol_max(kMaxInt);
    ctx.expect_eq(sol_max.firstBadVersion(kMaxInt), kMaxInt);
    ctx.expect_true(sol_max.callCount() <= 32);

    Solution sol_mid(1'702'766'719);
    ctx.expect_eq(sol_mid.firstBadVersion(2'126'753'390), 1'702'766'719);
    ctx.expect_true(sol_mid.callCount() <= 32);
  });
}

}  // namespace
