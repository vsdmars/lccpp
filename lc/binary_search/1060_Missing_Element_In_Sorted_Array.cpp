// LeetCode 1060: Missing Element In Sorted Array
// https://leetcode.com/problems/missing-element-in-sorted-array/
//
// Problem:
//   Given a strictly increasing integer array `nums` of unique values and a
//   positive integer `k`, return the k-th missing integer starting from the
//   leftmost element `nums[0]`.
//   Constraints: 1 <= nums.length <= 50,000, 1 <= nums[i] <= 10^7,
//   1 <= k <= 10^8.
//
// Algorithm:
//   Define `missingBefore(i) = (nums[i] - nums[0]) - i`, which counts how many
//   integers are missing strictly inside `[nums[0], nums[i]]`. Because `nums`
//   is strictly increasing, `missingBefore(i)` is monotonically non-decreasing
//   with `i` and evaluates in O(1) time.
//   1. Fast boundary check: if `k > missingBefore(n - 1)`, the k-th missing
//      number lies past the end of `nums`, so the answer is directly
//      `nums[n - 1] + (k - missingBefore(n - 1))`.
//   2. Binary search (`missingElement`, optimal): find the smallest index
//      `left` in `[0, n - 1]` with `missingBefore(left) >= k`. Because
//      `missingBefore(0) == 0 < k`, we are guaranteed `left >= 1` and
//      `missingBefore(left - 1) < k <= missingBefore(left)`. Thus the k-th
//      missing number falls in the gap `(nums[left - 1], nums[left])` and
//      equals `nums[left - 1] + (k - missingBefore(left - 1))`.
//      Time Complexity:  O(log n).
//      Space Complexity: O(1).
//   3. Linear gap scan (`missingElementLinear`, secondary): walk adjacent pairs
//      accumulating gap sizes `(nums[i] - nums[i - 1] - 1)` until the gap
//      containing the k-th missing value is reached.
//      Time Complexity:  O(n).
//      Space Complexity: O(1).
//
// Data Structures:
//   - `std::vector<int>` (sorted input array queried by random-access index).
//
// Real-World Applications:
//   - Detecting missing sequence numbers in TCP streams, Raft/Kafka write-ahead
//     logs, or time-series IDs without materializing gaps.
//   - Allocating the k-th free ID/port/inode from a sorted list of allocated
//     identifiers in O(log n) time.
//   - Sparse index lookups in columnar databases and inverted lists.
//
// Related LeetCode Problems:
//   - #1539 Kth Missing Positive Number [done] (same concept starting from 1 instead of nums[0])
//   - #268 Missing Number [done] (single missing element in [0, n])
//   - #41 First Missing Positive [done] (unsorted variant using cyclic placement)
//   - #1055 Shortest Way To Form String [done] (binary search on sorted index lists)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the k-th missing number starting from `nums[0]` using binary search
  // on the monotonic prefix-missing count function.
  // Time: O(log n), Space: O(1).
  int missingElement(const std::vector<int>& nums, int k) {
    if (nums.empty() || k <= 0) {
      return 0;
    }

    const std::size_t n = nums.size();
    const std::int64_t total_missing = missingBefore(nums, n - 1uz);
    if (k > total_missing) {
      // The k-th missing integer lies beyond the largest element in `nums`.
      return static_cast<int>(static_cast<std::int64_t>(nums.back()) +
                              (k - total_missing));
    }

    // Binary search for the smallest index `left` where missingBefore(left) >= k.
    // Invariant: missingBefore(left - 1) < k <= missingBefore(right).
    std::size_t left = 0uz;
    std::size_t right = n - 1uz;
    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2uz);
      if (missingBefore(nums, mid) < k) {
        left = mid + 1uz;
      } else {
        right = mid;
      }
    }

    // `left >= 1` because missingBefore(0) == 0 < k. Advance past `nums[left - 1]`
    // by the remaining missing count not yet accounted for before `left - 1`.
    const std::int64_t missing_up_to_prev = missingBefore(nums, left - 1uz);
    return static_cast<int>(static_cast<std::int64_t>(nums[left - 1uz]) +
                            (k - missing_up_to_prev));
  }

  // Linear-time gap scan: inspects each consecutive interval in O(n) without
  // iterating one integer at a time, remaining fast even for large `k`.
  // Time: O(n), Space: O(1).
  int missingElementLinear(const std::vector<int>& nums, int k) {
    if (nums.empty() || k <= 0) {
      return 0;
    }

    std::int64_t remaining = k;
    for (std::size_t i = 1uz; i < nums.size(); ++i) {
      const std::int64_t gap =
          static_cast<std::int64_t>(nums[i]) - nums[i - 1uz] - 1;
      if (remaining <= gap) {
        return static_cast<int>(static_cast<std::int64_t>(nums[i - 1uz]) +
                                remaining);
      }
      remaining -= gap;
    }

    return static_cast<int>(static_cast<std::int64_t>(nums.back()) + remaining);
  }

 private:
  // Returns the number of integers missing in the closed range [nums[0], nums[idx]].
  // Time: O(1), Space: O(1).
  static std::int64_t missingBefore(const std::vector<int>& nums,
                                    std::size_t idx) {
    return (static_cast<std::int64_t>(nums[idx]) - nums[0]) -
           static_cast<std::int64_t>(idx);
  }
};

LC_PROBLEM(1060, "Missing Element In Sorted Array", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.missingElement({4, 7, 9, 10}, 1), 5);
    ctx.expect_eq(sol.missingElementLinear({4, 7, 9, 10}, 1), 5);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.missingElement({4, 7, 9, 10}, 3), 8);
    ctx.expect_eq(sol.missingElementLinear({4, 7, 9, 10}, 3), 8);
  });

  ctx.test("example_3_past_end", [&] {
    ctx.expect_eq(sol.missingElement({1, 2, 4}, 3), 6);
    ctx.expect_eq(sol.missingElementLinear({1, 2, 4}, 3), 6);
  });

  ctx.test("build_case_far_past_end", [&] {
    ctx.expect_eq(sol.missingElement({4, 7, 9, 10}, 10), 17);
    ctx.expect_eq(sol.missingElementLinear({4, 7, 9, 10}, 10), 17);
  });

  ctx.test("single_element_and_contiguous_array", [&] {
    ctx.expect_eq(sol.missingElement({5}, 3), 8);
    ctx.expect_eq(sol.missingElementLinear({5}, 3), 8);
    ctx.expect_eq(sol.missingElement({10, 11, 12, 13}, 5), 18);
    ctx.expect_eq(sol.missingElementLinear({10, 11, 12, 13}, 5), 18);
  });

  ctx.test("large_k_no_overflow", [&] {
    ctx.expect_eq(sol.missingElement({1, 10'000'000}, 100'000'000),
                  100'000'002);
    ctx.expect_eq(sol.missingElementLinear({1, 10'000'000}, 100'000'000),
                  100'000'002);
  });
}

}  // namespace
