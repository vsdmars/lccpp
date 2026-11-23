// LeetCode 1985: Find The Kth Largest Integer In The Array
// https://leetcode.com/problems/find-the-kth-largest-integer-in-the-array/
//
// Problem:
//   Given an array of numeric strings `nums` (each representing a non-negative
//   integer with 1 to 100 digits and no leading zeros) and an integer `k`
//   (1 <= k <= nums.size() <= 10^4), return the string representing the k-th
//   largest integer in `nums`. Duplicate values are counted distinctly (e.g.,
//   in ["1", "2", "2"], the 1st and 2nd largest are "2", and the 3rd is "1").
//
// Algorithm:
//   Because strings can have up to 100 digits, they overflow 64-bit and even
//   128-bit integer types. Since strings contain only ASCII digits and have no
//   leading zeros, numeric order reduces directly to string-length order first
//   and lexicographical order on equal lengths.
//
//   Approach 1 (Bounded Min-Heap — primary):
//     Maintain a min-heap of at most `k` `std::string_view` entries ordered by
//     numeric value. For each string in `nums`, push it onto the heap; whenever
//     the heap size exceeds `k`, pop the smallest element. After scanning all
//     elements, the heap holds the `k` largest integers and its top is the k-th
//     largest.
//     Time Complexity:  O(N * L * log k), where N = nums.size() and L is the
//                       maximum string length (each comparison takes O(L)).
//     Space Complexity: O(k) for storing `std::string_view` handles in the heap
//                       (O(1) extra string memory beyond the input).
//
//   Approach 2 (Length Bucketing — secondary):
//     Group `std::string_view` entries into buckets indexed by string length.
//     Iterate bucket lengths from largest to smallest, subtracting bucket sizes
//     from `k` until reaching the unique bucket containing the k-th largest
//     value, then sort only that bucket descending (pure lexicographical order
//     since all strings in the bucket share the same length) and return index
//     `k - 1`.
//     Time Complexity:  O(N + M * L * log M), where M is the size of the single
//                       target length bucket (O(N * L * log N) worst case).
//     Space Complexity: O(N + L) for the length buckets of string views.
//
// Data Structures:
//   - Bounded min-heap (`std::priority_queue<std::string_view, ...>`) with a
//     custom length-then-lexicographical comparator to track the top `k`
//     largest numeric strings without copying string data.
//   - Length-indexed bucket table (`std::vector<std::vector<std::string_view>>`)
//     in the secondary bucketing approach to partition inputs by digit count.
//
// Real-World Applications:
//   - Selecting top-k arbitrary-precision financial transaction amounts,
//     cryptographic nonces, or big-integer metrics stored as decimal strings.
//   - Streaming telemetry / log analytics engines computing rolling k-th
//     percentile thresholds with O(k) memory.
//   - Database query engines executing `ORDER BY ... LIMIT k` over variable-
//     length DECIMAL / NUMERIC columns without full table sorts.
//
// Related LeetCode Problems:
//   - #215 Kth Largest Element in an Array [done] (fixed-width integer version using min-heap or quickselect)
//   - #703 Kth Largest Element in a Stream [done] (online streaming top-k maintenance with a size-k min-heap)
//   - #347 Top K Frequent Elements [done] (top-k selection via bounded min-heap or frequency bucketing)
//   - #973 K Closest Points to Origin [done] (bounded heap selection with a custom comparator)
//   - #179 Largest Number [done] (custom comparator over numeric strings without integer conversion)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Strict weak ordering comparing two decimal strings (with no leading zeros)
// by their numeric value in descending sense (`lhs > rhs`).
// Used as the comparator of `std::priority_queue` so that the numerically
// smallest string sits on top (forming a min-heap).
struct NumericGreater {
  [[nodiscard]] bool operator()(std::string_view lhs,
                                std::string_view rhs) const noexcept {
    // Longer decimal representation (without leading zeros) is strictly larger.
    if (lhs.size() != rhs.size()) {
      return lhs.size() > rhs.size();
    }
    // Equal-length ASCII digit strings compare identically to their numeric
    // values in standard lexicographical order.
    return lhs > rhs;
  }
};

class Solution {
 public:
  // Finds the k-th largest integer string using a bounded min-heap of size k.
  //
  // Invariant: After processing prefix `nums[0..i]`, `min_heap` contains the
  // min(i + 1, k) numerically largest strings seen so far, with the smallest
  // among them at `min_heap.top()`.
  //
  // Time Complexity:  O(N * L * log k)
  // Space Complexity: O(k) string views
  std::string kthLargestNumber(const std::vector<std::string>& nums, int k) {
    const auto target_k = static_cast<std::size_t>(k);
    std::priority_queue<std::string_view, std::vector<std::string_view>,
                        NumericGreater>
        min_heap;

    for (const std::string& num : nums) {
      // Store a non-owning view to avoid copying up-to-100-byte strings.
      min_heap.emplace(num);
      // Evict the smallest candidate whenever more than `k` elements are held.
      if (min_heap.size() > target_k) {
        min_heap.pop();
      }
    }

    // The root of the size-k min-heap is the k-th largest integer overall.
    return std::string(min_heap.top());
  }

  // Finds the k-th largest integer string by bucketing strings by digit count
  // and sorting only the single length bucket that contains rank `k`.
  //
  // Time Complexity:  O(N + M * L * log M), where M is the target bucket size
  // Space Complexity: O(N + L)
  std::string kthLargestNumberBucket(const std::vector<std::string>& nums,
                                     int k) {
    // Determine the maximum digit length dynamically so the method handles any
    // non-empty decimal string length, not only <= 100.
    std::size_t max_len = 0uz;
    for (const std::string& num : nums) {
      max_len = std::max(max_len, num.size());
    }

    // Partition string views by length; strings in a higher bucket are always
    // strictly larger than all strings in any lower bucket.
    std::vector<std::vector<std::string_view>> buckets(max_len + 1uz);
    for (const std::string& num : nums) {
      buckets[num.size()].emplace_back(num);
    }

    // Scan from the largest digit length downward until `remaining` falls
    // inside the current bucket.
    auto remaining = static_cast<std::size_t>(k);
    for (std::size_t len = max_len; len > 0uz; --len) {
      auto& bucket = buckets[len];
      if (remaining <= bucket.size()) {
        // Within a fixed length, numeric descending order equals reverse
        // lexicographical order.
        std::ranges::sort(bucket, std::ranges::greater{});
        return std::string(bucket[remaining - 1uz]);
      }
      remaining -= bucket.size();
    }

    return {};
  }
};

LC_PROBLEM(1985, "Find The Kth Largest Integer In The Array", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::string> nums = {"3", "6", "7", "10"};
    ctx.expect_eq(sol.kthLargestNumber(nums, 4), std::string("3"));
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 4), std::string("3"));
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<std::string> nums = {"2", "21", "12", "1"};
    ctx.expect_eq(sol.kthLargestNumber(nums, 3), std::string("2"));
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 3), std::string("2"));
  });

  ctx.test("example_3_duplicates_zero", [&] {
    Solution sol;
    const std::vector<std::string> nums = {"0", "0"};
    ctx.expect_eq(sol.kthLargestNumber(nums, 2), std::string("0"));
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 2), std::string("0"));
  });

  ctx.test("duplicates_counted_distinctly", [&] {
    Solution sol;
    const std::vector<std::string> nums = {"1", "2", "2"};
    ctx.expect_eq(sol.kthLargestNumber(nums, 1), std::string("2"));
    ctx.expect_eq(sol.kthLargestNumber(nums, 2), std::string("2"));
    ctx.expect_eq(sol.kthLargestNumber(nums, 3), std::string("1"));
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 1), std::string("2"));
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 2), std::string("2"));
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 3), std::string("1"));
  });

  ctx.test("beyond_128bit_integers", [&] {
    Solution sol;
    const std::string largest = "100000000000000000000000000000000000000000";
    const std::string second = "99999999999999999999999999999999999999999";
    const std::string third = "99999999999999999999999999999999999999998";
    const std::vector<std::string> nums = {second, largest, third};

    ctx.expect_eq(sol.kthLargestNumber(nums, 1), largest);
    ctx.expect_eq(sol.kthLargestNumber(nums, 2), second);
    ctx.expect_eq(sol.kthLargestNumber(nums, 3), third);
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 1), largest);
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 2), second);
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 3), third);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    const std::vector<std::string> nums = {"42"};
    ctx.expect_eq(sol.kthLargestNumber(nums, 1), std::string("42"));
    ctx.expect_eq(sol.kthLargestNumberBucket(nums, 1), std::string("42"));
  });
}

}  // namespace
