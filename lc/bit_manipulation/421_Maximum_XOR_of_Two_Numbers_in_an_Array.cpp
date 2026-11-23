// LeetCode 421: Maximum XOR of Two Numbers in an Array
// https://leetcode.com/problems/maximum-xor-of-two-numbers-in-an-array/
//
// Problem:
//   Given an integer array `nums` of non-negative 32-bit integers
//   (0 <= nums[i] <= 2^31 - 1), return the maximum value of `nums[i] ^ nums[j]`
//   over all pairs `0 <= i <= j < nums.length`.
//   Constraints: 1 <= nums.length <= 2 * 10^5.
//
// Algorithm:
//   Greedy MSB-to-LSB bit-prefix construction with a hash set:
//   Because setting bit `k` contributes 2^k — strictly greater than the sum of
//   all lower bits (2^k - 1) — we determine each bit of the optimal XOR answer
//   from bit 30 down to bit 0:
//     1. Shift the currently known prefix `answer` left by 1 and optimistically
//        propose setting the new lowest bit: `candidate = answer | 1U`.
//     2. Collect the shifted `(31 - bit)`-bit prefixes `num >> bit` of every
//        element in `nums` into an `std::unordered_set<std::uint32_t>`.
//     3. By the self-inverse property of XOR, there exist two prefixes `p` and
//        `q` with `p ^ q == candidate` if and only if `(candidate ^ p)` is also
//        present in the prefix set. If found, commit `answer = candidate`;
//        otherwise that bit must remain `0`.
//   Time Complexity:  O(31 * n) = O(n) average time (31 passes of O(n) hash
//                     insertions and lookups).
//   Space Complexity: O(n) for the prefix hash set (reused across iterations).
//
// Data Structures:
//   `std::unordered_set<std::uint32_t>` storing unique high-bit prefixes at the
//   current bit depth for O(1) average-time complement lookup.
//
// Real-World Applications:
//   Finding maximally dissimilar binary feature vectors (SimHash / LSH) for
//   diversity sampling, error-correcting code distance analysis, and network
//   routing / prefix-trie packet matching.
//
// Related LeetCode Problems:
//   - #1707 Maximum XOR With an Element From Array [new] (offline queries + bitwise trie)
//   - #208 Implement Trie [done] (Prefix Tree) (trie alternative to prefix hash set)
//   - #1803 Count Pairs With XOR in a Range [new] (counting pairs using bitwise prefix trie)
//   - #2935 Maximum Strong Pair XOR II [new] (sliding window + bitwise XOR maximization)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the maximum pairwise XOR in `nums` by greedily resolving bits from
  // MSB (bit 30) to LSB (bit 0).
  // Invariant: after processing bit `b`, `answer` equals the exact maximum
  // achievable prefix `(nums[i] >> b) ^ (nums[j] >> b)` across all pairs.
  // Time: O(n) average (31 passes), Space: O(n).
  int findMaximumXOR(const std::vector<int>& nums) {
    if (nums.size() <= 1uz) {
      return 0;
    }

    static constexpr int kHighestBit = 30;  // 0 <= nums[i] <= 2^31 - 1

    std::uint32_t answer = 0U;
    std::unordered_set<std::uint32_t> prefixes;
    prefixes.reserve(nums.size());

    for (int bit = kHighestBit; bit >= 0; --bit) {
      // Shift already-decided high bits left and test whether the current bit
      // can be set to 1.
      answer <<= 1U;
      const std::uint32_t candidate = answer | 1U;

      // Project every number onto its top (31 - bit) bits.
      prefixes.clear();
      const auto shift = static_cast<unsigned int>(bit);
      for (const int num : nums) {
        prefixes.insert(static_cast<std::uint32_t>(num) >> shift);
      }

      // p ^ q == candidate  <=>  q == candidate ^ p.
      for (const std::uint32_t prefix : prefixes) {
        if (prefixes.contains(candidate ^ prefix)) {
          answer = candidate;
          break;
        }
      }
    }

    return static_cast<int>(answer);
  }
};

LC_PROBLEM(421, "Maximum XOR of Two Numbers in an Array", "bit_manipulation",
           ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    // 5 (00101) ^ 25 (11001) = 28 (11100)
    ctx.expect_eq(sol.findMaximumXOR({3, 10, 5, 25, 2, 8}), 28);
  });

  ctx.test("example_2_larger_set", [&] {
    // 91 ^ 36 = 127
    ctx.expect_eq(
        sol.findMaximumXOR({14, 70, 53, 83, 49, 91, 36, 80, 92, 51, 66, 70}),
        127);
  });

  ctx.test("single_and_duplicate_elements", [&] {
    ctx.expect_eq(sol.findMaximumXOR({0}), 0);
    ctx.expect_eq(sol.findMaximumXOR({42}), 0);
    ctx.expect_eq(sol.findMaximumXOR({7, 7, 7, 7}), 0);
  });

  ctx.test("two_elements_and_max_31bit_bound", [&] {
    ctx.expect_eq(sol.findMaximumXOR({2, 4}), 6);
    ctx.expect_eq(sol.findMaximumXOR({8, 10, 2}), 10);
    ctx.expect_eq(sol.findMaximumXOR({0, std::numeric_limits<int>::max()}),
                  std::numeric_limits<int>::max());
  });
}

}  // namespace
