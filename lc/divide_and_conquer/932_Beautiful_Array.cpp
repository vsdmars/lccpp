// https://leetcode.com/problems/beautiful-array/
//
// An array `nums` of length `n` is beautiful if:
// - `nums` is a permutation of the integers in `[1, n]`.
// - For every `0 <= i < k < j < n`, `2 * nums[k] != nums[i] + nums[j]`.
//
// Given `n` (1 <= n <= 1000), return any beautiful array of length `n`.

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Bottom-up divide and conquer using affine invariance of beautiful arrays:
  // If A is beautiful, both (2 * A - 1) [all odd] and (2 * A) [all even] are
  // beautiful, and concatenating them cannot create an arithmetic triple
  // across the boundary because odd + even is odd while 2 * nums[k] is even.
  // Time: O(N), Space: O(N).
  std::vector<int> beautifulArray(int n) {
    if (n <= 0) {
      return {};
    }

    const auto target = static_cast<std::size_t>(n);
    std::vector<int> res = {1};

    while (res.size() < target) {
      std::vector<int> next;
      next.reserve(res.size() * 2uz);
      for (const int val : res) {
        next.push_back((2 * val) - 1);
      }
      for (const int val : res) {
        next.push_back(2 * val);
      }
      res = std::move(next);
    }

    std::vector<int> filtered;
    filtered.reserve(target);
    for (const int val : res) {
      if (val <= n) {
        filtered.push_back(val);
      }
    }
    return filtered;
  }
};

bool is_beautiful_array(const std::vector<int>& arr, int n) {
  if (n < 0 || arr.size() != static_cast<std::size_t>(n)) {
    return false;
  }

  std::vector<bool> seen(static_cast<std::size_t>(n) + 1uz, false);
  for (const int val : arr) {
    if (val < 1 || val > n) {
      return false;
    }
    const auto idx = static_cast<std::size_t>(val);
    if (seen[idx]) {
      return false;
    }
    seen[idx] = true;
  }

  for (std::size_t i = 0uz; i < arr.size(); ++i) {
    for (std::size_t k = i + 1uz; k < arr.size(); ++k) {
      for (std::size_t j = k + 1uz; j < arr.size(); ++j) {
        if (2 * arr[k] == arr[i] + arr[j]) {
          return false;
        }
      }
    }
  }
  return true;
}

LC_PROBLEM(932, "Beautiful Array", "divide_and_conquer", ctx) {
  ctx.test("small_values", [&] {
    Solution sol;
    for (int n = 1; n <= 10; ++n) {
      const auto res = sol.beautifulArray(n);
      ctx.expect_true(is_beautiful_array(res, n));
    }
  });

  ctx.test("n_equals_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.beautifulArray(1), std::vector<int>{1});
  });

  ctx.test("n_equals_4_and_5", [&] {
    Solution sol;
    ctx.expect_true(is_beautiful_array(sol.beautifulArray(4), 4));
    ctx.expect_true(is_beautiful_array(sol.beautifulArray(5), 5));
  });

  ctx.test("larger_values", [&] {
    Solution sol;
    for (const int n : {16, 31, 50, 100}) {
      ctx.expect_true(is_beautiful_array(sol.beautifulArray(n), n));
    }
  });
}

}  // namespace
