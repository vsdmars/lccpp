// https://leetcode.com/problems/two-sum/description/
//
// Given an array of integers `nums` and an integer `target`, return indices of
// the two numbers such that they add up to `target`.

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  std::vector<int> twoSum(const std::vector<int>& nums, int target) {
    std::unordered_map<int, int> seen;
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      const int need = target - nums[i];
      if (auto it = seen.find(need); it != seen.end()) {
        return {it->second, static_cast<int>(i)};
      }
      seen.emplace(nums[i], static_cast<int>(i));
    }
    return {};
  }
};

LC_PROBLEM(1, "Two Sum", "array", ctx) {
  Solution sol;

  ctx.test("example 1: [2,7,11,15], target=9", [&] {
    ctx.expect_eq(sol.twoSum({2, 7, 11, 15}, 9), std::vector<int>{0, 1});
  });

  ctx.test("example 2: [3,2,4], target=6", [&] {
    ctx.expect_eq(sol.twoSum({3, 2, 4}, 6), std::vector<int>{1, 2});
  });

  ctx.test("example 3: duplicate values [3,3], target=6", [&] {
    ctx.expect_eq(sol.twoSum({3, 3}, 6), std::vector<int>{0, 1});
  });
}

}  // namespace
