// https://leetcode.com/problems/minimum-increment-to-make-array-unique/
//
// Given an integer array `nums`, in one move you can pick any index `i` and
// increment `nums[i]` by 1. Return the minimum number of moves required to make
// every value in `nums` unique.

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Approach 1: Sort + greedy tracking of the next strictly larger target.
  // Time: O(N * log(N))
  // Space: O(1) auxiliary beyond sorting the input copy.
  int minIncrementForUnique(std::vector<int> nums) {
    if (nums.empty()) {
      return 0;
    }
    std::ranges::sort(nums);

    int moves = 0;
    int level = nums[0];
    for (std::size_t i = 1uz; i < nums.size(); ++i) {
      if (nums[i] > level) {
        level = nums[i];
      } else {
        ++level;
        moves += level - nums[i];
      }
    }
    return moves;
  }

  // Approach 2: Path-compressing Union-Find over occupied integer slots.
  // `root[x]` stores the end of the contiguous occupied block containing `x`,
  // so `find(x)` directly jumps to the smallest free slot >= x.
  // Time: O(N * alpha(N)) average
  // Space: O(N)
  int minIncrementForUniqueUnionFind(const std::vector<int>& nums) {
    std::unordered_map<int, int> root;
    root.reserve(nums.size());

    auto find_free_slot = [&](int x) -> int {
      int cur = x;
      std::vector<int> visited;
      for (auto it = root.find(cur); it != root.end(); it = root.find(cur)) {
        visited.push_back(cur);
        cur = it->second + 1;
      }
      root[cur] = cur;
      for (int node : visited) {
        root[node] = cur;
      }
      return cur;
    };

    int moves = 0;
    for (int num : nums) {
      moves += find_free_slot(num) - num;
    }
    return moves;
  }
};

LC_PROBLEM(945, "Minimum Increment To Make Array Unique", "union_find", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<int> nums = {1, 2, 2};
    ctx.expect_eq(sol.minIncrementForUnique(nums), 1);
    ctx.expect_eq(sol.minIncrementForUniqueUnionFind(nums), 1);
  });

  ctx.test("example_2_build_case", [&] {
    Solution sol;
    const std::vector<int> nums = {3, 2, 1, 2, 1, 7};
    ctx.expect_eq(sol.minIncrementForUnique(nums), 6);
    ctx.expect_eq(sol.minIncrementForUniqueUnionFind(nums), 6);
  });

  ctx.test("already_unique", [&] {
    Solution sol;
    const std::vector<int> nums = {1, 4, 2, 7};
    ctx.expect_eq(sol.minIncrementForUnique(nums), 0);
    ctx.expect_eq(sol.minIncrementForUniqueUnionFind(nums), 0);
  });

  ctx.test("all_equal_zeros", [&] {
    Solution sol;
    const std::vector<int> nums = {0, 0, 0, 0};
    ctx.expect_eq(sol.minIncrementForUnique(nums), 6);
    ctx.expect_eq(sol.minIncrementForUniqueUnionFind(nums), 6);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    const std::vector<int> nums = {5};
    ctx.expect_eq(sol.minIncrementForUnique(nums), 0);
    ctx.expect_eq(sol.minIncrementForUniqueUnionFind(nums), 0);
  });
}

}  // namespace
