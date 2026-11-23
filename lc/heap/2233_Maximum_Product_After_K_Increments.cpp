// LeetCode 2233: Maximum Product After K Increments
// https://leetcode.com/problems/maximum-product-after-k-increments/
//
// Problem:
//   Given an array of non-negative integers `nums` and an integer `k`
//   (1 <= nums.size(), k <= 10^5; 0 <= nums[i] <= 10^6), you may perform at
//   most `k` operations where each operation increments any single element of
//   `nums` by 1. Return the maximum possible product of all elements after
//   these operations, modulo 10^9 + 7. Maximize the true product first before
//   applying the modulus.
//
// Algorithm:
//   Greedy Min-Heap Increment:
//     Consider any two elements x > y >= 0 with remaining product P >= 0:
//       - Incrementing x changes the product by +P * y.
//       - Incrementing y changes the product by +P * x.
//     Because x > y and P >= 0, incrementing the smaller element always yields
//     a greater or equal product (and if any element is 0, incrementing a zero
//     is required to lift the product above 0). Moreover, since all elements
//     are non-negative, using all `k` increments never decreases the product.
//
//     Therefore, build a min-heap from `nums` in O(N) time, repeat `k` times
//     popping the current minimum element and pushing `smallest + 1`, and
//     finally multiply all heap elements together modulo 10^9 + 7 using 64-bit
//     arithmetic to avoid intermediate overflow.
//
//     Time Complexity:  O((N + k) * log N), where N = nums.size() (O(N)
//                       heapify, O(k log N) for k pop/push steps, and
//                       O(N log N) to drain the priority queue).
//     Space Complexity: O(N) for the min-heap.
//
// Data Structures:
//   - Min-heap (`std::priority_queue<std::int64_t, std::vector<std::int64_t>,
//     std::greater<>>`) constructed in linear time via range construction to
//     fetch and update the global minimum in O(log N) per operation.
//
// Real-World Applications:
//   - Water-filling power/bandwidth allocation in wireless communication
//     channels (maximizing total capacity / log-product by boosting the weakest
//     sub-channels first).
//   - Resource provisioning across parallel pipeline stages where overall
//     throughput or reliability scales multiplicatively across components.
//   - Portfolio or inventory balancing under discrete unit-addition budgets to
//     maximize geometric-mean growth.
//
// Related LeetCode Problems:
//   - #1005 Maximize Sum Of Array After K Negations [new] (greedy min-heap repeatedly modifying the smallest element k times)
//   - #1962 Remove Stones to Minimize the Total [new] (greedy max-heap performing k operations on the extremal element)
//   - #2530 Maximal Score After Applying K Operations [new] (greedy heap simulation updating the top element for k rounds)
//   - #628 Maximum Product of Three Numbers [new] (maximizing array products under element selection constraints)
//   - #215 Kth Largest Element in an Array [done] (fundamental heap-based order-statistic management)

#include "framework/lc_framework.h"

#include <cstdint>
#include <functional>
#include <queue>
#include <vector>

namespace {

constexpr std::int64_t kMod = 1'000'000'007;

class Solution {
 public:
  // Returns the maximum possible product of `nums` after at most `k`
  // single-element increments, computed modulo 10^9 + 7.
  //
  // Invariant: Before each of the `k` steps, `min_heap.top()` is the smallest
  // element currently in the array, which maximizes the marginal product gain
  // of adding 1.
  //
  // Time Complexity:  O(N + (N + k) * log N)
  // Space Complexity: O(N)
  int maximumProduct(const std::vector<int>& nums, int k) {
    // Build the min-heap in O(N) via range construction; store values as
    // 64-bit integers so increments and modular products never overflow.
    std::priority_queue<std::int64_t, std::vector<std::int64_t>, std::greater<>>
        min_heap(nums.begin(), nums.end());

    // Greedily apply all `k` increments to the current minimum element.
    for (int step = 0; step < k; ++step) {
      const std::int64_t smallest = min_heap.top();
      min_heap.pop();
      min_heap.push(smallest + 1);
    }

    // Multiply all final values modulo `kMod`. Because we maximized the true
    // values above, reducing modulo `kMod` at each step preserves the exact
    // result while preventing 64-bit overflow (max product < kMod^2 < 2^63).
    std::int64_t product = 1;
    while (!min_heap.empty()) {
      const std::int64_t val = min_heap.top();
      min_heap.pop();
      if (val == 0) {
        // Any remaining zero collapses the entire product to 0.
        return 0;
      }
      product = (product * (val % kMod)) % kMod;
    }

    return static_cast<int>(product);
  }
};

LC_PROBLEM(2233, "Maximum Product After K Increments", "heap", ctx) {
  ctx.test("example_1_with_zero", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumProduct({0, 4}, 5), 20);
  });

  ctx.test("example_2_balanced_increments", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumProduct({6, 3, 3, 2}, 2), 216);
  });

  ctx.test("zero_increments", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumProduct({2, 3, 5}, 0), 30);
  });

  ctx.test("remaining_zero_collapses_product", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumProduct({0, 0, 5}, 1), 0);
  });

  ctx.test("all_zeros_lifted", [&] {
    Solution sol;
    // [0, 0, 0] after 5 increments -> [2, 2, 1], product = 4.
    ctx.expect_eq(sol.maximumProduct({0, 0, 0}, 5), 4);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumProduct({7}, 10), 17);
  });

  ctx.test("large_product_modulo", [&] {
    Solution sol;
    // [99999, 100000, 100000] + 1 increment -> [100000, 100000, 100000].
    // 10^15 mod (10^9 + 7) = 993000007.
    ctx.expect_eq(sol.maximumProduct({99'999, 100'000, 100'000}, 1),
                  993'000'007);
  });
}

}  // namespace
