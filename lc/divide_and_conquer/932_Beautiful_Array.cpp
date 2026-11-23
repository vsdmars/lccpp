// LeetCode 932: Beautiful Array
// https://leetcode.com/problems/beautiful-array/
//
// Problem:
//   An array `nums` of length `n` (`1 <= n <= 1000`) is called beautiful if:
//   1. It is a permutation of the integers `1..n`.
//   2. For every triple of indices `0 <= i < k < j < n`,
//      `2 * nums[k] != nums[i] + nums[j]` (no element is the exact arithmetic
//      mean of two elements on opposite sides of it).
//   Given `n`, construct and return any valid beautiful array of length `n`.
//
// Examples:
//   Example 1:
//     Input:  n = 1
//     Output: [1]
//     Explanation:
//       - beautifulArray initializes res = [1] and checks the loop guard res.size()
//         < 1, which is false.
//       - Filtering elements <= 1 into filtered keeps [1] and returns [1].
//
//   Example 2:
//     Input:  n = 4
//     Output: [1, 3, 2, 4]
//     Explanation:
//       - Starting from res = [1], the first while iteration maps res to odds (2 *
//         val - 1 = [1]) followed by evens (2 * val = [2]), giving res = [1, 2].
//       - Because res.size() = 2 < 4, the second iteration maps [1, 2] to odds [1,
//         3] followed by evens [2, 4], producing res = [1, 3, 2, 4].
//       - The loop terminates with res.size() = 4, and filtering values <= 4
//         returns [1, 3, 2, 4], which has no 3-term arithmetic progression.
//
//   Example 3:
//     Input:  n = 5
//     Output: [1, 5, 3, 2, 4]
//     Explanation:
//       - After two doubling steps reach res = [1, 3, 2, 4] (size 4 < 5), a third
//         iteration appends odds [1, 5, 3, 7] and evens [2, 6, 4, 8] to form res =
//         [1, 5, 3, 7, 2, 6, 4, 8].
//       - Now res.size() = 8 >= 5 so the loop ends, and filtering elements <= 5
//         drops 7, 6, and 8.
//       - The returned array [1, 5, 3, 2, 4] preserves the beautiful property
//         across all triples because deletion cannot introduce arithmetic triples.
//
// Algorithm:
//   Bottom-up divide and conquer exploiting three closure properties of
//   beautiful arrays:
//   1. Affine invariance: if `A` is beautiful, then `2 * A - 1` (all odd) and
//      `2 * A` (all even) are also beautiful.
//   2. Parity separation: concatenating `[odd elements] + [even elements]`
//      cannot form an invalid triple across the split because `odd + even` is
//      odd whereas `2 * nums[k]` is always even.
//   3. Deletion invariance: removing any elements preserves the condition.
//   Starting from `res = {1}`, we repeatedly double the array by mapping `res`
//   to `(2 * val - 1)` followed by `(2 * val)` until `res.size() >= n`, then
//   filter out all values exceeding `n`. Because the sizes grow geometrically
//   (`1 + 2 + 4 + ... < 2N`), total work is linear.
//   Time: O(N), Space: O(N).
//
// Data Structures:
//   - `std::vector<int> res`, `next`, `filtered`: contiguous arrays holding
//     the current level's beautiful permutation, the doubled odd-then-even
//     permutation, and the final filtered permutation of `1..n`.
//
// Real-World Applications:
//   - Combinatorial design and coding theory (constructing Salem-Spencer /
//     3-term arithmetic-progression-free sequences for interference avoidance).
//   - Signal processing and radar/communication frequency hopping patterns
//     that avoid intermodulation harmonics (`2*f_k = f_i + f_j`).
//   - Bit-reversal permutation orderings closely related to Cooley-Tukey FFT
//     butterfly stages and low-discrepancy (van der Corput) sampling.
//
// Related LeetCode Problems:
//   - #1982 Find Array Given Subset Sums [new] (divide-and-conquer reconstruction
//     by splitting elements with an invariant offset)
//   - #1027 Longest Arithmetic Subsequence [done] (detects arithmetic progressions
//     rather than avoiding them)
//   - #413 Arithmetic Slices [new] (identifies contiguous 3-term arithmetic triples)
//   - #324 Wiggle Sort II [done] (constructive permutation satisfying global index
//     and ordering invariants)

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
