// LeetCode 945: Minimum Increment to Make Array Unique
// https://leetcode.com/problems/minimum-increment-to-make-array-unique/
//
// Problem:
//   Given an integer array `nums`, in one move you may choose any index `i`
//   and increment `nums[i]` by 1. Return the minimum total number of moves
//   needed so that every element in `nums` is strictly unique.
//
// Algorithm:
//   This file implements two approaches:
//   1. Sorting + Greedy (`minIncrementForUnique`):
//      Sort a copy of `nums` in non-decreasing order and maintain `level`, the
//      largest assigned unique value so far. For each subsequent `nums[i]`:
//      - If `nums[i] > level`, keep it as-is and advance `level = nums[i]`.
//      - Otherwise, bump it to `++level` and add `level - nums[i]` to `moves`.
//      Time: O(N log N). Space: O(1) auxiliary beyond sorting the value copy.
//   2. Path-Compressing Union-Find (`minIncrementForUniqueUnionFind`):
//      Process `nums` online in any order using a sparse hash map `root` over
//      occupied integer slots. Invariant: `root[x]` stores the rightmost end
//      of the contiguous occupied block containing `x`. For each `num`,
//      `find_free_slot(num)` jumps via `cur = root[cur] + 1` until reaching an
//      unoccupied integer `cur`, marks `root[cur] = cur`, and path-compresses
//      every visited key to `cur`. The moves needed for `num` is `cur - num`.
//      Time: O(N * alpha(N)) average, where alpha is the inverse Ackermann
//      function. Space: O(N).
//
// Data Structures:
//   - `std::vector<int>` (`nums` in Approach 1): Sorted in-place to enable a
//     single linear greedy pass.
//   - `std::unordered_map<int, int>` (`root` in Approach 2): Sparse Union-Find
//     mapping each occupied integer slot to the end of its contiguous occupied
//     run, pre-reserved to `nums.size()`.
//   - `std::vector<int>` (`visited` in Approach 2): Records traversed keys
//     during a probe so all of them can be compressed to the newly claimed slot.
//
// Real-World Applications:
//   - Linear-probing hash table slot allocation and open-addressing collision
//     resolution with jump-pointer acceleration.
//   - Unique ID, port, or time-slot scheduling when requests collide on the
//     same preferred slot and must be shifted forward minimally.
//   - Monotonic timestamp deduplication in event logs and distributed tracing.
//
// Related LeetCode Problems:
//   - #1647 Minimum Deletions to Make Character Frequencies Unique [new] (making
//     counts unique via greedy decrements)
//   - #1846 Maximum Element After Decreasing and Rearranging [new] (sorting with
//     greedy level tracking)
//   - #41 First Missing Positive [done] (locating the smallest unused integer slot)
//   - #128 Longest Consecutive Sequence [done] (tracking contiguous integer blocks
//     with a hash map or Union-Find)
//   - #2195 Append K Integers With Minimal Sum [new] (greedily assigning smallest
//     available integers)

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
