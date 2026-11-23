// LeetCode 215: Kth Largest Element In An Array
// https://leetcode.com/problems/kth-largest-element-in-an-array/
//
// Problem:
//   Given an unsorted integer array `nums` and an integer `k` (1 <= k <=
//   nums.size()), return the k-th largest element in sorted descending order.
//   Note that duplicates count individually (it is the k-th largest element in
//   sorted order, not the k-th distinct value).
//
// Algorithm:
//   Maintain a min-heap containing the `k` largest elements seen so far:
//   1. Iterate through each `num` in `nums` and push it into the min-heap.
//   2. Whenever the heap size exceeds `k`, pop the minimum element — it is
//      strictly smaller than (or equal to) at least `k` other elements already
//      in the heap, so it can never be in the top `k`.
//   3. After scanning all elements, the heap holds the `k` largest values in
//      the array, and its minimum (`min_heap.top()`) is the exact k-th largest
//      element.
//   Time Complexity: O(N log k) — each of the N elements performs at most one
//   push and one pop on a heap of size at most `k + 1`.
//   Space Complexity: O(k) — stores at most `k + 1` elements in the heap.
//
// Data Structures:
//   - std::priority_queue<int, std::vector<int>, std::greater<>> (binary
//     min-heap): tracks the running top-k window and evicts the smallest
//     candidate in O(log k) time.
//
// Real-World Applications:
//   - Top-K leaderboard / trending items / heavy-hitter queries in streaming
//     analytics where memory is bounded to O(k) rather than full dataset size.
//   - Database `ORDER BY score DESC LIMIT k` execution plans using a bounded
//     priority queue (top-N heap sort) instead of a full O(N log N) sort.
//   - Information retrieval and vector search reranking that keeps only the
//     top-K highest-scoring candidate documents.
//
// Related LeetCode Problems:
//   - #347 Top K Frequent Elements [done] (bounded min-heap over frequency counts)
//   - #973 K Closest Points to Origin [done] (bounded max-heap for k smallest distances)
//   - #703 Kth Largest Element in a Stream [done] (online streaming variant maintaining a size-k min-heap)
//   - #373 Find K Pairs With Smallest Sums [done] (heap-based k-th order statistic over pair sums)
//   - #295 Find Median From Data Stream [done] (two-heap order-statistic tracking)

#include "framework/lc_framework.h"

#include <cstddef>
#include <functional>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the k-th largest element in `nums` using a bounded min-heap of
  // size `k`.
  // Time: O(N log k), Space: O(k).
  int findKthLargest(const std::vector<int>& nums, int k) {
    const auto target_size = static_cast<std::size_t>(k);

    // Min-heap invariant: after processing each element, `min_heap` holds the
    // `min(processed, k)` largest values seen so far, with the smallest of
    // those top-k values at the root.
    std::priority_queue<int, std::vector<int>, std::greater<>> min_heap;

    for (const int num : nums) {
      min_heap.push(num);
      if (min_heap.size() > target_size) {
        // Evict the smallest candidate since at least `k` larger-or-equal
        // elements remain in the heap.
        min_heap.pop();
      }
    }

    return min_heap.top();
  }
};

LC_PROBLEM(215, "Kth Largest Element In An Array", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.findKthLargest({3, 2, 1, 5, 6, 4}, 2), 5);
  });

  ctx.test("example_2_with_duplicates", [&] {
    Solution sol;
    ctx.expect_eq(sol.findKthLargest({3, 2, 3, 1, 2, 4, 5, 5, 6}, 4), 4);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.findKthLargest({42}, 1), 42);
  });

  ctx.test("k_equals_1_and_k_equals_n", [&] {
    Solution sol;
    const std::vector<int> nums = {-10, 5, 0, -3, 12, 8};
    ctx.expect_eq(sol.findKthLargest(nums, 1), 12);
    ctx.expect_eq(sol.findKthLargest(nums, 6), -10);
  });

  ctx.test("all_equal_elements", [&] {
    Solution sol;
    ctx.expect_eq(sol.findKthLargest({7, 7, 7, 7}, 3), 7);
  });
}

}  // namespace
