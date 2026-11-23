// LeetCode 238: Product Of Array Except Self
// https://leetcode.com/problems/product-of-array-except-self/
//
// Problem:
//   Given an integer array `nums` of length n (n >= 2 in standard constraints),
//   return an array `output` such that `output[i]` equals the product of all
//   elements of `nums` except `nums[i]`. The solution must run in O(n) time
//   without using the division operator. Follow-up: use O(1) extra space
//   (excluding the returned output array).
//
// Algorithm:
//   Two-pass prefix and suffix product accumulation directly inside the output
//   array:
//   1. Left-to-right prefix pass: store in `result[i]` the product of all
//      elements strictly to the left of index `i` (`nums[0] * ... * nums[i-1]`),
//      with `result[0] = 1` as the multiplicative identity.
//   2. Right-to-left suffix pass: maintain a single scalar `suffix_product`
//      holding the product of all elements strictly to the right of index `i`
//      (`nums[i+1] * ... * nums[n-1]`), multiplying it into `result[i]` so that
//      `result[i] = prefix[i] * suffix[i]`.
//   Because division is never used, zero elements are handled naturally without
//   special-casing zero counts.
//   Time Complexity:  O(n) — two linear passes over `nums`.
//   Space Complexity: O(1) auxiliary space beyond the O(n) output array.
//
// Data Structures:
//   - `std::vector<int>` (`result`): serves first as the prefix-product table
//     and then as the final output array after folding suffix products in-place.
//
// Real-World Applications:
//   - Leave-one-out statistical and ML feature scoring (computing aggregate
//     likelihoods or scaling factors excluding a single sample without division
//     instability near zero).
//   - Distributed consensus and cryptographic threshold schemes (computing
//     Lagrange basis numerators or multiplicative shares over rings where
//     multiplicative inverses may not exist).
//   - Numerical signal processing and financial portfolio attribution where
//     zero weights make division-based total-over-element formulas invalid.
//
// Related LeetCode Problems:
//   - #42 Trapping Rain Water [done] (prefix/suffix sweeps from both ends of an array)
//   - #152 Maximum Product Subarray [done] (tracking running products across negative/zero values)
//   - #560 Subarray Sum Equals K [done] (prefix accumulation over arrays)
//   - #1352 Product of the Last K Numbers [new] (online prefix products with zero resets)
//   - #2906 Construct Product Matrix [new] (2D modular generalization of product except self)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes an array where each index `i` holds the product of every element
  // in `nums` except `nums[i]`, without using division.
  //
  // Invariants:
  //   - After pass 1: `result[i] == prod_{j < i} nums[j]`.
  //   - During pass 2: `suffix_product == prod_{j > i} nums[j]`, so multiplying
  //     `result[i] *= suffix_product` yields the full product excluding `i`.
  //
  // Time Complexity:  O(n)
  // Space Complexity: O(1) extra space (excluding the returned vector)
  std::vector<int> productExceptSelf(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n == 0) {
      return {};
    }

    // Initialize with 1 (multiplicative identity); result[0] has no left
    // elements, so its prefix product stays 1.
    std::vector<int> result(n, 1);

    // Pass 1 (left -> right): fill each slot with the product of all elements
    // strictly before it.
    for (std::size_t idx = 1uz; idx < n; ++idx) {
      result[idx] = result[idx - 1uz] * nums[idx - 1uz];
    }

    // Pass 2 (right -> left): accumulate the running suffix product and fold it
    // into `result`. Looping `idx` from `n - 1` down to `1` avoids unsigned
    // underflow while updating `result[idx - 1]` with `nums[idx .. n - 1]`.
    int suffix_product = 1;
    for (std::size_t idx = n - 1uz; idx > 0uz; --idx) {
      suffix_product *= nums[idx];
      result[idx - 1uz] *= suffix_product;
    }

    return result;
  }
};

LC_PROBLEM(238, "Product Of Array Except Self", "array", ctx) {
  Solution sol;

  ctx.test("example_basic", [&] {
    ctx.expect_eq(sol.productExceptSelf({1, 2, 3, 4}),
                  std::vector<int>{24, 12, 8, 6});
  });

  ctx.test("single_zero_short", [&] {
    ctx.expect_eq(sol.productExceptSelf({3, 0, 2, 1}),
                  std::vector<int>{0, 6, 0, 0});
  });

  ctx.test("single_zero_long", [&] {
    ctx.expect_eq(sol.productExceptSelf({3, 7, 0, 5, 3, 1, 2, 6}),
                  std::vector<int>{0, 0, 3780, 0, 0, 0, 0, 0});
  });

  ctx.test("multiple_zeros", [&] {
    ctx.expect_eq(sol.productExceptSelf({0, 0}), std::vector<int>{0, 0});
    ctx.expect_eq(sol.productExceptSelf({-1, 1, 0, -3, 3}),
                  std::vector<int>{0, 0, 9, 0, 0});
  });

  ctx.test("negatives_and_two_elements", [&] {
    ctx.expect_eq(sol.productExceptSelf({-2, 3}), std::vector<int>{3, -2});
    ctx.expect_eq(sol.productExceptSelf({-1, -2, -3, -4}),
                  std::vector<int>{-24, -12, -8, -6});
  });

  ctx.test("empty_and_single_element", [&] {
    ctx.expect_eq(sol.productExceptSelf({}), std::vector<int>{});
    ctx.expect_eq(sol.productExceptSelf({42}), std::vector<int>{1});
  });
}

}  // namespace
