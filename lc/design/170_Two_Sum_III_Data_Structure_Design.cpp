// LeetCode 170: Two Sum III Data Structure Design
// https://leetcode.com/problems/two-sum-iii-data-structure-design/
//
// Problem:
//   Design a data structure `TwoSum` that accepts a stream of integers and
//   checks whether any pair of previously added numbers sums to a target value:
//     - `add(number)`: Inserts `number` into the data structure.
//     - `find(value)`: Returns true if there exists a pair of added numbers
//       whose sum equals `value`, otherwise returns false.
//
// Examples:
//   Example 1:
//     Input:  ["TwoSum", "add", "add", "add", "find", "find"], [[], [1], [3], [5],
//             [4], [7]]
//     Output: [null, null, null, null, true, false]
//     Explanation:
//       - Three `add` calls populate `counts_` with `{1: 1, 3: 1, 5: 1}`.
//       - `find(4)` visits `num = 1`, computes `complement = 4 - 1 = 3 != 1`, finds
//         `3` in `counts_`, caches `4` in `found_cache_`, and returns `true`.
//       - `find(7)` misses `found_cache_` and checks complements `6`, `4`, and `2`
//         for keys `1`, `3`, and `5`; none exist in `counts_`, so it returns
//         `false`.
//
//   Example 2:
//     Input:  ["TwoSum", "add", "add", "add", "add", "add", "add", "add", "find",
//             "find", "find", "find", "find", "find"], [[], [3], [4], [1], [-7],
//             [9], [1], [1], [10], [10], [-3], [5], [15], [2]]
//     Output: [null, null, null, null, null, null, null, null, true, true, true,
//             true, false, true]
//     Explanation:
//       - The seven `add` calls build `counts_ = {3: 1, 4: 1, 1: 3, -7: 1, 9: 1}`.
//       - The first `find(10)` pairs `9` with `1` and inserts `10` into
//         `found_cache_`, so the second `find(10)` returns `true` immediately from
//         `found_cache_`.
//       - `find(-3)` matches `4` with `-7`, `find(5)` matches `4` with `1`, and
//         `find(15)` finds no valid complement in `counts_` and returns `false`.
//       - `find(2)` checks `num = 1` where `complement = 2 - 1 = 1 == num`; since
//         `freq = 3 > 1`, it returns `true`.
//
//   Example 3:
//     Input:  ["TwoSum", "add", "find", "add", "find"], [[], [0], [0], [0], [0]]
//     Output: [null, null, false, null, true]
//     Explanation:
//       - The first `add(0)` sets `counts_ = {0: 1}`.
//       - The first `find(0)` computes `complement = 0 - 0 = 0 == num`, checks
//         `freq > 1` (`1 > 1` is false), and returns `false` without adding `0` to
//         `found_cache_`.
//       - The second `add(0)` increments `counts_[0]` to `2`, so the second
//         `find(0)` satisfies `freq > 1`, caches `0` in `found_cache_`, and returns
//         `true`.
//
// Algorithm:
//   Frequency Hash Map with Positive-Hit Memoization:
//   - Store added numbers in a frequency map `counts_` (`number -> count`).
//     Using `std::int64_t` for keys and complement arithmetic prevents signed
//     32-bit overflow when computing `value - num` near `INT_MIN`/`INT_MAX`.
//   - On `add(number)`: increment `counts_[number]` in O(1) average time.
//   - On `find(value)`:
//     * Fast path: because numbers are never removed, any target previously
//       confirmed as achievable stays achievable forever; check `found_cache_`
//       in O(1) time.
//     * Otherwise iterate over each `(num, freq)` entry in `counts_` and compute
//       `complement = value - num`. A valid pair exists if either:
//       (a) `complement == num` and `freq > 1`, or
//       (b) `complement != num` and `counts_.contains(complement)`.
//       On a match, cache `value` in `found_cache_` and return true.
//   - Time Complexity: O(1) average for `add`; O(u) worst case (O(1) on cached
//     positive queries) for `find`, where u is the number of unique values.
//   - Space Complexity: O(u + c), where u is the number of unique values and c
//     is the number of distinct positive targets cached.
//
// Data Structures:
//   - Frequency hash map (`std::unordered_map<std::int64_t, int>`) enabling
//     O(1) insertions and O(1) complement lookups while distinguishing single
//     vs. duplicate occurrences.
//   - Positive-hit hash set (`std::unordered_set<std::int64_t>`) memoizing
//     confirmed reachable targets across repeated `find` queries.
//
// Real-World Applications:
//   - Financial order-book / payment-matching engines checking whether two
//     pending transactions reconcile to a target settlement amount.
//   - Streaming telemetry/anomaly systems answering pair-sum threshold queries
//     over write-heavy ingestion streams.
//   - Inventory bundling systems matching pairs of item prices to a gift-card
//     or discount tier total.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (static single-query two-sum via hash map)
//   - #167 Two Sum II [done] - Input Array Is Sorted (two-pointer pair search on sorted input)
//   - #653 Two Sum IV [done] - Input is a BST (pair-sum lookup over a binary search tree)
//   - #1865 Finding Pairs With a Certain Sum [new] (dynamic frequency-map pair counting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <unordered_set>

namespace {

// Write-optimized TwoSum data structure supporting O(1) add and O(u) find
// (with O(1) memoization for repeated positive targets).
class TwoSum {
 public:
  // Records one occurrence of `number`.
  // Time: O(1) average.
  void add(int number) { ++counts_[static_cast<std::int64_t>(number)]; }

  // Returns true if any two added numbers sum to `value`.
  // Time: O(u) worst case, O(1) on cached positive hits.
  bool find(int value) {
    if (counts_.empty()) {
      return false;
    }

    const auto target = static_cast<std::int64_t>(value);
    if (found_cache_.contains(target)) {
      return true;
    }

    const bool found = std::ranges::any_of(counts_, [&](const auto& entry) {
      const auto& [num, freq] = entry;
      const std::int64_t complement = target - num;
      // Same-value pair requires at least two occurrences of `num`.
      if (complement == num) {
        return freq > 1;
      }
      return counts_.contains(complement);
    });

    if (found) {
      found_cache_.insert(target);
    }
    return found;
  }

 private:
  std::unordered_map<std::int64_t, int> counts_;
  // Safe monotonic cache: elements are only ever added, never removed.
  std::unordered_set<std::int64_t> found_cache_;
};

LC_PROBLEM(170, "Two Sum III Data Structure Design", "design", ctx) {
  ctx.test("example_1_basic_operations", [&] {
    TwoSum ts;
    ts.add(1);
    ts.add(3);
    ts.add(5);
    ctx.expect_true(ts.find(4));
    ctx.expect_false(ts.find(7));
  });

  ctx.test("python_main_sequence_with_negatives_and_duplicates", [&] {
    TwoSum ts;
    for (const int num : {3, 4, 1, -7, 9, 1, 1}) {
      ts.add(num);
    }
    ctx.expect_true(ts.find(10));   // 9 + 1
    ctx.expect_true(ts.find(10));   // hits positive cache
    ctx.expect_true(ts.find(-3));   // 4 + (-7)
    ctx.expect_true(ts.find(5));    // 4 + 1
    ctx.expect_false(ts.find(15));  // no pair sums to 15
    ctx.expect_true(ts.find(2));    // duplicate 1 + 1
  });

  ctx.test("duplicate_requirement_for_half_target", [&] {
    TwoSum ts;
    ts.add(0);
    ctx.expect_false(ts.find(0));
    ts.add(0);
    ctx.expect_true(ts.find(0));
  });

  ctx.test("empty_and_single_element", [&] {
    TwoSum ts;
    ctx.expect_false(ts.find(0));
    ts.add(5);
    ctx.expect_false(ts.find(5));
    ctx.expect_false(ts.find(10));
  });

  ctx.test("int32_boundary_overflow_safety", [&] {
    TwoSum ts;
    ts.add(std::numeric_limits<int>::max());
    ts.add(std::numeric_limits<int>::min());
    ctx.expect_true(ts.find(-1));
    ctx.expect_false(ts.find(std::numeric_limits<int>::min()));
  });
}

}  // namespace
