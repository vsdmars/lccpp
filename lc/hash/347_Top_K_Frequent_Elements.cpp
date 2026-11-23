// LeetCode 347: Top K Frequent Elements
// https://leetcode.com/problems/top-k-frequent-elements/
//
// Problem:
//   Given a non-empty integer array `nums` and an integer `k`
//   (`1 <= k <= number of unique elements`), return the `k` most frequent
//   elements in any order. The algorithm must run in better than O(N log N)
//   time complexity, where N = nums.size().
//
// Algorithm:
//   Use frequency counting followed by bucket sort:
//     1. Count each element's frequency in a hash map (`val -> count`) in O(N)
//        average time.
//     2. Allocate `N + 1` frequency buckets (`buckets[count]` stores all
//        distinct values appearing exactly `count` times). Since no element
//        can appear more than `N` times, every frequency fits directly as an
//        array index without sorting.
//     3. Sweep `buckets` from `N` down to `1`, collecting elements until `k`
//        values have been gathered.
//   Time Complexity: O(N) average time (strictly better than O(N log N)).
//   Space Complexity: O(N) for the frequency map and bucket array.
//
// Data Structures:
//   - `std::unordered_map<int, std::size_t>`: tracks the occurrence count of
//     each distinct integer.
//   - `std::vector<std::vector<int>>`: frequency-indexed buckets enabling
//     linear-time selection of the top `k` elements.
//
// Real-World Applications:
//   - Real-time analytics and trending-topic / hot-key detection in caches,
//     CDNs, and telemetry streams.
//   - Database query-planner histogram construction (identifying Most Common
//     Values for cardinality estimation).
//   - Network intrusion detection and DDoS mitigation tracking top talker IPs.
//
// Related LeetCode Problems:
//   - #215 Kth Largest Element in an Array [done] (linear-time Quickselect / heap selection)
//   - #451 Sort Characters By Frequency [done] (bucket sort by character frequency)
//   - #692 Top K Frequent Words [done] (top-k frequencies with lexicographical tie-breaking)
//   - #973 K Closest Points to Origin [done] (top-k selection by geometric distance)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the `k` most frequent elements in `nums` using O(N) bucket sort.
  // Time: O(N) average, Space: O(N).
  std::vector<int> topKFrequent(const std::vector<int>& nums, int k) {
    if (k <= 0 || nums.empty()) {
      return {};
    }

    // Phase 1: Count occurrences of every distinct integer.
    std::unordered_map<int, std::size_t> freq;
    freq.reserve(nums.size());
    for (const int val : nums) {
      ++freq[val];
    }

    // Phase 2: Group values into buckets indexed directly by their frequency
    // (bounded above by nums.size()).
    std::vector<std::vector<int>> buckets(nums.size() + 1uz);
    for (const auto& [val, count] : freq) {
      buckets[count].push_back(val);
    }

    // Phase 3: Scan from highest frequency downwards and harvest `k` items.
    const auto target_k = static_cast<std::size_t>(k);
    std::vector<int> result;
    result.reserve(target_k);

    for (std::size_t count = nums.size();
         count > 0uz && result.size() < target_k; --count) {
      for (const int val : buckets[count]) {
        result.push_back(val);
        if (result.size() == target_k) {
          break;
        }
      }
    }

    return result;
  }
};

LC_PROBLEM(347, "Top K Frequent Elements", "hash", ctx) {
  auto sorted_vec = [](std::vector<int> v) {
    std::ranges::sort(v);
    return v;
  };

  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sorted_vec(sol.topKFrequent({1, 1, 1, 2, 2, 3}, 2)),
                  std::vector<int>{1, 2});
  });

  ctx.test("example_2_single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.topKFrequent({1}, 1), std::vector<int>{1});
  });

  ctx.test("unsorted_with_distinct_frequencies", [&] {
    Solution sol;
    ctx.expect_eq(sorted_vec(sol.topKFrequent({5, 3, 1, 1, 1, 3, 73, 1}, 2)),
                  std::vector<int>{1, 3});
  });

  ctx.test("negative_values", [&] {
    Solution sol;
    ctx.expect_eq(sorted_vec(sol.topKFrequent({-1, -1, 2, 2, 2, 3}, 2)),
                  std::vector<int>{-1, 2});
  });

  ctx.test("k_equals_unique_count_and_all_identical", [&] {
    Solution sol;
    ctx.expect_eq(sorted_vec(sol.topKFrequent({4, 1, 9, 2}, 4)),
                  std::vector<int>{1, 2, 4, 9});
    ctx.expect_eq(sol.topKFrequent({7, 7, 7, 7}, 1), std::vector<int>{7});
  });
}

}  // namespace
