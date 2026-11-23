// LeetCode 303: Range Sum Query Immutable
// https://leetcode.com/problems/range-sum-query-immutable/
//
// Problem:
//   Design a data structure `NumArray` initialized with an immutable integer
//   array `nums` that supports frequent range-sum queries `sumRange(left, right)`
//   returning the sum of elements `nums[left] + nums[left + 1] + ... + nums[right]`
//   for 0 <= left <= right < nums.size().
//
// Algorithm:
//   Prefix-sum (cumulative sum) precomputation.
//   In the constructor, build a 1-indexed prefix-sum array `prefix_` of size
//   `nums.size() + 1` where `prefix_[0] = 0` and `prefix_[i + 1] = prefix_[i] + nums[i]`
//   equals the sum of the first i + 1 elements `nums[0..i]`.
//   Any inclusive range query `[left, right]` then resolves in O(1) via a single
//   subtraction with no special-case branch for `left == 0`:
//     sumRange(left, right) = prefix_[right + 1] - prefix_[left].
//   Time Complexity:  O(n) preprocessing in constructor; O(1) per `sumRange`.
//   Space Complexity: O(n) for the prefix-sum array.
//
// Data Structures:
//   1-indexed `std::vector<int>` prefix-sum array (`prefix_`) with a leading 0
//   sentinel to handle left-boundary queries uniformly.
//
// Real-World Applications:
//   - Time-series analytics and financial charting (instant window totals over
//     static historical volume, revenue, or telemetry logs).
//   - Database OLAP range aggregations and histogram cumulative distribution
//     queries (CDFs).
//   - 1D/2D integral images in computer vision for constant-time box filtering.
//
// Related LeetCode Problems:
//   - #304 Range Sum Query 2D [done] - Immutable (2D prefix-sum table with inclusion-exclusion)
//   - #307 Range Sum Query [done] - Mutable (adds point updates via Fenwick tree / segment tree)
//   - #560 Subarray Sum Equals K [done] (combines prefix sums with a hash map)
//   - #325 Maximum Size Subarray Sum Equals k [done] (tracks earliest prefix-sum indices)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

// Immutable array wrapper supporting O(1) inclusive range-sum queries via a
// precomputed 1-indexed prefix-sum table.
class NumArray {
 public:
  // Precomputes prefix sums in O(n) time and O(n) space.
  //
  // Invariant: For all k in [0, nums.size()], `prefix_[k]` equals the sum of
  // the first k elements `nums[0 .. k-1]` (with `prefix_[0] == 0`).
  explicit NumArray(const std::vector<int>& nums)
      : prefix_(nums.size() + 1uz, 0) {
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      prefix_[i + 1uz] = prefix_[i] + nums[i];
    }
  }

  // Returns the inclusive sum of `nums[left .. right]` in O(1) time.
  // Because `prefix_` is 1-indexed with `prefix_[0] == 0`, `left == 0` requires
  // no special-case conditional.
  [[nodiscard]] int sumRange(int left, int right) const {
    const auto l_idx = static_cast<std::size_t>(left);
    const auto r_idx = static_cast<std::size_t>(right) + 1uz;
    return prefix_[r_idx] - prefix_[l_idx];
  }

 private:
  std::vector<int> prefix_;
};

LC_PROBLEM(303, "Range Sum Query Immutable", "dp", ctx) {
  ctx.test("leetcode_example_and_build", [&] {
    const NumArray arr({-2, 0, 3, -5, 2, -1});
    ctx.expect_eq(arr.sumRange(0, 2), 1);
    ctx.expect_eq(arr.sumRange(2, 5), -1);
    ctx.expect_eq(arr.sumRange(0, 5), -3);
  });

  ctx.test("single_element_and_point_queries", [&] {
    const NumArray single({42});
    ctx.expect_eq(single.sumRange(0, 0), 42);

    const NumArray multi({-2, 0, 3, -5, 2, -1});
    ctx.expect_eq(multi.sumRange(0, 0), -2);
    ctx.expect_eq(multi.sumRange(3, 3), -5);
    ctx.expect_eq(multi.sumRange(5, 5), -1);
  });

  ctx.test("all_negative_and_all_positive", [&] {
    const NumArray neg({-1, -2, -3, -4});
    ctx.expect_eq(neg.sumRange(0, 3), -10);
    ctx.expect_eq(neg.sumRange(1, 2), -5);

    const NumArray pos({1, 2, 3, 4, 5});
    ctx.expect_eq(pos.sumRange(0, 4), 15);
    ctx.expect_eq(pos.sumRange(2, 4), 12);
  });
}

}  // namespace
