// https://leetcode.com/problems/range-sum-query-mutable/
//
// Given an integer array `nums`, handle multiple queries of two types:
// 1. `update(index, val)`: update `nums[index]` to `val`.
// 2. `sumRange(left, right)`: return the sum of elements in `nums[left..right]`
//    inclusive.

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
