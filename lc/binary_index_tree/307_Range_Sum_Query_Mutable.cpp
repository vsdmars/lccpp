// LeetCode 307: Range Sum Query - Mutable
// https://leetcode.com/problems/range-sum-query-mutable/
//
// Problem:
//   Design a data structure initialized with an integer array `nums` that
//   efficiently supports two interleaved operations:
//   1. `update(index, val)`: set `nums[index]` to `val`.
//   2. `sumRange(left, right)`: return the sum of `nums[left..right]`
//      inclusive (with `0 <= left <= right < nums.size()`).
//
// Examples:
//   Example 1:
//     Input:  NumArray([1, 3, 5]), sumRange(0, 2), update(1, 2), sumRange(0, 2)
//     Output: sumRange(0, 2) = 9, sumRange(0, 2) = 8
//     Explanation:
//       - Linear construction builds the 1-indexed Fenwick tree tree_ = [0, 1, 4,
//         5], where tree_[2] holds 1 + 3 = 4 and tree_[3] holds 5.
//       - sumRange(0, 2) computes prefix_sum(3) - prefix_sum(0) = (tree_[3] +
//         tree_[2]) - 0 = (5 + 4) - 0 = 9.
//       - update(1, 2) computes delta = 2 - nums_[1] = -1, sets nums_[1] = 2, and
//         adds -1 at 1-based index i = 2 (tree_[2] becomes 3; next i = 4 exceeds
//         size 3).
//       - The second sumRange(0, 2) evaluates (tree_[3] + tree_[2]) - 0 = (5 + 3) -
//         0 = 8.
//
//   Example 2:
//     Input:  NumArray([1, 2, 1, 3, 2, 5]), sumRange(1, 2), sumRange(0, 5),
//             update(3, 5), sumRange(0, 5), update(3, 3), sumRange(0, 5)
//     Output: sumRange(1, 2) = 3, sumRange(0, 5) = 14, sumRange(0, 5) = 16,
//             sumRange(0, 5) = 14
//     Explanation:
//       - Construction populates tree_ = [0, 1, 3, 1, 7, 2, 7] via single-parent
//         propagation i + lowbit(i).
//       - sumRange(1, 2) returns prefix_sum(3) - prefix_sum(1) = (tree_[3] +
//         tree_[2]) - tree_[1] = (1 + 3) - 1 = 3, and sumRange(0, 5) returns
//         tree_[6] + tree_[4] = 7 + 7 = 14.
//       - update(3, 5) computes delta = 5 - 3 = 2 and adds 2 to tree_[4] (becoming
//         9), so sumRange(0, 5) increases to 7 + 9 = 16.
//       - update(3, 3) applies delta = 3 - 5 = -2 to tree_[4] (restoring 7),
//         bringing sumRange(0, 5) back to 14.
//
//   Example 3:
//     Input:  NumArray([-10]), sumRange(0, 0), update(0, 7), sumRange(0, 0),
//             update(0, -3), sumRange(0, 0)
//     Output: sumRange(0, 0) = -10, sumRange(0, 0) = 7, sumRange(0, 0) = -3
//     Explanation:
//       - Construction initializes nums_ = [-10] and tree_ = [0, -10], so
//         sumRange(0, 0) returns prefix_sum(1) - prefix_sum(0) = -10.
//       - update(0, 7) applies delta = 7 - (-10) = 17 to tree_[1] (-10 + 17 = 7),
//         returning 7 on sumRange(0, 0).
//       - update(0, -3) applies delta = -3 - 7 = -10 to tree_[1] (7 - 10 = -3),
//         returning -3 on sumRange(0, 0).
//
// Algorithm:
//   1-indexed Binary Indexed Tree (Fenwick Tree). Each slot `tree_[i]` stores
//   the sum of the half-open block of length `lowbit(i) = i & -i` ending at
//   index `i`. Construction runs in linear time by adding `nums_[i - 1]` into
//   `tree_[i]` and propagating `tree_[i]` once to its immediate parent
//   `i + lowbit(i)`. Point updates compute `delta = val - nums_[idx]`, update
//   `nums_[idx]`, and add `delta` along the ancestor chain `i += lowbit(i)`.
//   Prefix sums accumulate `tree_[i]` along `i -= lowbit(i)`, and any range
//   sum is answered as `prefix_sum(right + 1) - prefix_sum(left)`.
//   Time: O(N) build, O(log N) per `update` and `sumRange`. Space: O(N).
//
// Data Structures:
//   - `std::vector<int> nums_`: mirrors the current array values so each
//     `update` can compute the incremental `delta` in O(1) time.
//   - `std::vector<int> tree_`: 1-indexed Fenwick tree of size `N + 1` storing
//     hierarchical partial sums indexed by lowest set bit.
//
// Real-World Applications:
//   - Real-time financial order books and telemetry dashboards tracking dynamic
//     cumulative volumes or sliding-window metrics under high-frequency updates.
//   - Adaptive arithmetic coding and online frequency tables requiring fast
//     cumulative probability queries and symbol-count updates.
//   - Leaderboard and ranking services maintaining dynamic prefix counts.
//
// Related LeetCode Problems:
//   - #303 Range Sum Query [done] - Immutable (static prefix-sum baseline without
//     point updates)
//   - #308 Range Sum Query 2D [done] - Mutable (2D Fenwick tree extension for mutable
//     submatrix sum queries)
//   - #315 Count of Smaller Numbers After Self [done] (uses a Fenwick tree over
//     coordinate-compressed values to count inversions)
//   - #327 Count of Range Sum [new] (counts valid prefix-sum ranges via Fenwick tree
//     or divide and conquer)
//   - #493 Reverse Pairs [new] (tracks dynamic value counts with a Fenwick tree)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// 1-indexed Binary Indexed Tree (Fenwick Tree) supporting point updates and
// prefix-sum queries.
// Time: O(N) construction, O(log N) per update and sumRange.
// Space: O(N).
class NumArray {
 public:
  explicit NumArray(std::vector<int> nums)
      : nums_(std::move(nums)), tree_(nums_.size() + 1uz, 0) {
    const std::size_t n = nums_.size();
    for (std::size_t i = 1uz; i <= n; ++i) {
      tree_[i] += nums_[i - 1uz];
      const std::size_t parent = i + lowbit(i);
      if (parent <= n) {
        tree_[parent] += tree_[i];
      }
    }
  }

  void update(int index, int val) {
    const auto idx = static_cast<std::size_t>(index);
    const int delta = val - nums_[idx];
    nums_[idx] = val;

    for (std::size_t i = idx + 1uz; i < tree_.size(); i += lowbit(i)) {
      tree_[i] += delta;
    }
  }

  [[nodiscard]] int sumRange(int left, int right) const {
    return prefix_sum(static_cast<std::size_t>(right) + 1uz) -
           prefix_sum(static_cast<std::size_t>(left));
  }

 private:
  static constexpr std::size_t lowbit(std::size_t x) noexcept {
    return x & ((~x) + 1uz);
  }

  [[nodiscard]] int prefix_sum(std::size_t idx) const {
    int sum = 0;
    for (std::size_t i = idx; i > 0uz; i -= lowbit(i)) {
      sum += tree_[i];
    }
    return sum;
  }

  std::vector<int> nums_;
  std::vector<int> tree_;
};

LC_PROBLEM(307, "Range Sum Query Mutable", "binary_index_tree", ctx) {
  ctx.test("leetcode_example", [&] {
    NumArray arr({1, 3, 5});
    ctx.expect_eq(arr.sumRange(0, 2), 9);
    arr.update(1, 2);
    ctx.expect_eq(arr.sumRange(0, 2), 8);
  });

  ctx.test("python_build_sequence", [&] {
    NumArray arr({1, 2, 1, 3, 2, 5});
    ctx.expect_eq(arr.sumRange(1, 2), 3);
    ctx.expect_eq(arr.sumRange(0, 5), 14);
    arr.update(3, 5);
    ctx.expect_eq(arr.sumRange(0, 5), 16);
    arr.update(3, 3);
    ctx.expect_eq(arr.sumRange(0, 5), 14);
  });

  ctx.test("single_element_and_negatives", [&] {
    NumArray arr({-10});
    ctx.expect_eq(arr.sumRange(0, 0), -10);
    arr.update(0, 7);
    ctx.expect_eq(arr.sumRange(0, 0), 7);
    arr.update(0, -3);
    ctx.expect_eq(arr.sumRange(0, 0), -3);
  });

  ctx.test("multiple_updates_and_subranges", [&] {
    NumArray arr({0, 9, 5, 7, 3});
    ctx.expect_eq(arr.sumRange(4, 4), 3);
    ctx.expect_eq(arr.sumRange(2, 4), 15);
    ctx.expect_eq(arr.sumRange(3, 3), 7);
    arr.update(4, 5);
    arr.update(1, 7);
    arr.update(0, 8);
    ctx.expect_eq(arr.sumRange(1, 2), 12);
    arr.update(1, 9);
    ctx.expect_eq(arr.sumRange(4, 4), 5);
    arr.update(3, 4);
    ctx.expect_eq(arr.sumRange(0, 4), 31);
  });
}

}  // namespace
