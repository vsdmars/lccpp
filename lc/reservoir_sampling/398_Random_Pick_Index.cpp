// LeetCode 398: Random Pick Index
// https://leetcode.com/problems/random-pick-index/
//
// Problem:
//   Given an integer array `nums` that may contain duplicates
//   (1 <= nums.length <= 2 * 10^4, -2^31 <= nums[i] <= 2^31 - 1, up to 10^4
//   calls to `pick`), implement `pick(target)` to return a random index `i`
//   such that `nums[i] == target`. It is guaranteed that `target` exists in
//   `nums`, and every valid index must have equal probability of being chosen.
//
// Algorithm:
//   Implements two complementary approaches demonstrating the space-time
//   trade-off:
//     1. `Solution` (Filtered Reservoir Sampling, k = 1): Stores only `nums_`
//        with no auxiliary index map. On each `pick(target)`, scans `nums_`;
//        when the k-th occurrence of `target` is seen, replaces `chosen` with
//        the current index with probability 1 / k, yielding an exact uniform
//        distribution over all matching indices.
//        - Constructor: Time O(1) (moves `nums`), Space O(1) auxiliary.
//        - `pick`:      Time O(N), Space O(1) auxiliary.
//     2. `HashMapSolution` (Precomputed Index Map): Constructor groups indices
//        by value into a hash map so each `pick(target)` selects directly from
//        the matching index vector in O(1) time.
//        - Constructor: Time O(N), Space O(N) auxiliary.
//        - `pick`:      Time O(1), Space O(1) per call.
//
// Data Structures:
//   - `std::vector<int>`: Holds the input array in `Solution` and per-value
//     index lists in `HashMapSolution`.
//   - `std::unordered_map<int, std::vector<int>>`: Maps each distinct value to
//     all of its array indices in `HashMapSolution`.
//   - `std::mt19937` and `std::uniform_int_distribution`: Unbiased PRNG and
//     uniform index selector used by both implementations.
//
// Real-World Applications:
//   - Load balancers selecting a random healthy backend instance matching a
//     requested service tag or shard group.
//   - Ad-serving and recommendation engines uniformly picking a candidate item
//     matching a category or audience filter.
//   - Stratified and predicate-filtered sampling in streaming analytics and
//     SQL query engines.
//
// Related LeetCode Problems:
//   - #382 Linked List Random Node [done] (unfiltered k = 1 reservoir sampling over a
//     singly linked list)
//   - #528 Random Pick with Weight [done] (non-uniform discrete sampling with prefix
//     sums and binary search)
//   - #710 Random Pick with Blacklist [new] (uniform random selection over a range
//     excluding forbidden values)
//   - #380 Insert Delete GetRandom O [done] (1) (dynamic set with O(1) uniform random
//     sampling)
//   - #381 Insert Delete GetRandom O [done] (1) - Duplicates allowed (dynamic multiset
//     supporting O(1) uniform sampling)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Approach 1: Reservoir Sampling.
// Preserves O(1) auxiliary space (only stores the input array) while choosing
// each matching index uniformly at random in O(N) time per `pick` call.
// When the k-th occurrence of `target` is seen, replace the chosen index with
// probability 1 / k.
class Solution {
 public:
  explicit Solution(std::vector<int> nums,
                    std::uint32_t seed = std::random_device{}())
      : nums_(std::move(nums)), rng_(seed) {}

  // Time: O(N), Space: O(1) extra.
  int pick(int target) {
    int chosen = -1;
    int seen = 0;
    for (std::size_t i = 0uz; i < nums_.size(); ++i) {
      if (nums_[i] == target) {
        ++seen;
        std::uniform_int_distribution<int> dist(0, seen - 1);
        if (dist(rng_) == 0) {
          chosen = static_cast<int>(i);
        }
      }
    }
    return chosen;
  }

 private:
  std::vector<int> nums_;
  std::mt19937 rng_;
};

// Approach 2: Hash Map Index Lookup (matching the Python dictionary solution).
// Precomputes value -> list of indices for O(1) `pick` at the cost of O(N)
// extra space.
class HashMapSolution {
 public:
  explicit HashMapSolution(const std::vector<int>& nums,
                           std::uint32_t seed = std::random_device{}())
      : rng_(seed) {
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      index_map_[nums[i]].push_back(static_cast<int>(i));
    }
  }

  // Time: O(1), Space: O(N).
  int pick(int target) {
    const auto& indices = index_map_.at(target);
    std::uniform_int_distribution<std::size_t> dist(0uz, indices.size() - 1uz);
    return indices[dist(rng_)];
  }

 private:
  std::unordered_map<int, std::vector<int>> index_map_;
  std::mt19937 rng_;
};

LC_PROBLEM(398, "Random Pick Index", "reservoir_sampling", ctx) {
  ctx.test("example_unique_and_duplicates", [&] {
    const std::vector<int> nums{1, 2, 3, 3, 3};
    Solution sol(nums, 42u);
    HashMapSolution map_sol(nums, 42u);

    for (int trial = 0; trial < 20; ++trial) {
      ctx.expect_eq(sol.pick(1), 0);
      ctx.expect_eq(sol.pick(2), 1);
      ctx.expect_eq(map_sol.pick(1), 0);
      ctx.expect_eq(map_sol.pick(2), 1);
    }

    std::unordered_map<int, int> counts;
    constexpr int kTrials = 3000;
    for (int trial = 0; trial < kTrials; ++trial) {
      const int idx = sol.pick(3);
      ctx.expect_true(idx == 2 || idx == 3 || idx == 4);
      ++counts[idx];
    }
    // Each of the 3 indices has expected count 1000; verify rough uniformity.
    for (const int idx : {2, 3, 4}) {
      ctx.expect_true(counts[idx] > 750 && counts[idx] < 1250);
    }
  });

  ctx.test("python_build_array", [&] {
    const std::vector<int> nums{1, 2, 3, 3, 3, 3, 4, 5, 5, 5, 7, 7, 7};
    Solution sol(nums, 123u);
    HashMapSolution map_sol(nums, 123u);

    ctx.expect_eq(sol.pick(4), 6);
    ctx.expect_eq(map_sol.pick(4), 6);

    for (int trial = 0; trial < 100; ++trial) {
      const int idx3 = sol.pick(3);
      ctx.expect_true(idx3 >= 2 && idx3 <= 5);
      const int idx5 = map_sol.pick(5);
      ctx.expect_true(idx5 >= 7 && idx5 <= 9);
      const int idx7 = sol.pick(7);
      ctx.expect_true(idx7 >= 10 && idx7 <= 12);
    }
  });

  ctx.test("single_and_all_identical", [&] {
    Solution single({42}, 7u);
    ctx.expect_eq(single.pick(42), 0);

    Solution identical({9, 9, 9, 9}, 99u);
    std::unordered_map<int, int> seen;
    for (int trial = 0; trial < 200; ++trial) {
      const int idx = identical.pick(9);
      ctx.expect_true(idx >= 0 && idx <= 3);
      ++seen[idx];
    }
    ctx.expect_eq(seen.size(), 4uz);
  });
}

}  // namespace
