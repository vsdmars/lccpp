// LeetCode 1512: Number Of Good Pairs
// https://leetcode.com/problems/number-of-good-pairs/
//
// Problem:
//   Given an integer array `nums`, count the number of "good pairs" (i, j)
//   such that `nums[i] == nums[j]` and `i < j`.
//   Constraints: 1 <= nums.length <= 100, 1 <= nums[i] <= 100.
//
// Algorithm:
//   Single-pass frequency counting using a hash map. As we scan `nums` from
//   left to right, each occurrence of a value `v` can pair with every earlier
//   occurrence of `v` already seen. If `v` has appeared `c` times prior to the
//   current index, this element contributes `c` new good pairs before we
//   increment `counts[v]`. Summing 0 + 1 + ... + (k - 1) across a group of `k`
//   identical elements yields the exact binomial count k * (k - 1) / 2 without
//   needing a second pass or storing index lists.
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(u), where u <= n is the number of distinct values.
//
// Data Structures:
//   - `std::unordered_map<int, int>`: maps each distinct integer value to the
//     number of times it has been observed so far during the left-to-right
//     scan.
//
// Real-World Applications:
//   - Duplicate detection and collision counting in hash tables and storage
//     deduplication pipelines.
//   - Pairwise co-occurrence counting in analytics, telemetry aggregation, and
//     entity-resolution blocking keys.
//   - Computing match counts in combinatorial sampling and tournament pairing.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (single-pass hash map counting/lookup of prior elements)
//   - #217 Contains Duplicate [done] (detecting whether at least one good pair exists)
//   - #1010 Pairs of Songs With Total Durations Divisible by 60 [done] (same incremental pairing pattern modulo 60)
//   - #2001 Number of Pairs of Interchangeable Rectangles [new] (combinatorial pair counting over equivalence classes)
//   - #2364 Count Number of Bad Pairs [new] (complementary pair counting after index transformation)

#include "framework/lc_framework.h"

#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Counts index pairs (i, j) with i < j and nums[i] == nums[j].
  // Maintains a running frequency table so each new element immediately adds
  // the number of matching predecessors.
  // Time: O(n), Space: O(u) for u distinct values.
  int numIdenticalPairs(const std::vector<int>& nums) {
    // Maps each seen value to how many times it has appeared at earlier indices.
    std::unordered_map<int, int> seen_counts;
    seen_counts.reserve(nums.size());

    int good_pairs = 0;
    for (const int num : nums) {
      // Every prior occurrence of `num` at index i < current index forms a
      // distinct good pair with the current element.
      int& count = seen_counts[num];
      good_pairs += count;
      ++count;
    }

    return good_pairs;
  }
};

LC_PROBLEM(1512, "Number Of Good Pairs", "math", ctx) {
  ctx.test("example_1_build", [&] {
    Solution sol;
    ctx.expect_eq(sol.numIdenticalPairs({1, 2, 3, 1, 1, 3}), 4);
  });

  ctx.test("example_2_all_equal", [&] {
    Solution sol;
    // Four identical elements produce C(4, 2) = 6 pairs.
    ctx.expect_eq(sol.numIdenticalPairs({1, 1, 1, 1}), 6);
  });

  ctx.test("example_3_all_distinct", [&] {
    Solution sol;
    ctx.expect_eq(sol.numIdenticalPairs({1, 2, 3}), 0);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.numIdenticalPairs({42}), 0);
  });

  ctx.test("negative_and_repeated_values", [&] {
    Solution sol;
    // Three copies of -5 -> 3 pairs; two copies of 1000 -> 1 pair.
    ctx.expect_eq(sol.numIdenticalPairs({-5, -5, 1000, -5, 1000}), 4);
  });
}

}  // namespace
