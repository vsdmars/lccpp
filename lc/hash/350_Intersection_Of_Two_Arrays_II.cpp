// LeetCode 350: Intersection Of Two Arrays II
// https://leetcode.com/problems/intersection-of-two-arrays-ii/
//
// Problem:
//   Given two integer arrays `nums1` and `nums2`, return an array of their
//   multiset intersection. Each element in the result must appear as many
//   times as it shows in both arrays (i.e., `min(count1[x], count2[x])`), and
//   the result may be returned in any order.
//
// Examples:
//   Example 1:
//     Input:  nums1 = [1, 2, 2, 1], nums2 = [2, 2]
//     Output: [2, 2]
//     Explanation:
//       - Select nums2 (size 2 <= 4) as smaller and nums1 as larger to minimize
//         hash map space.
//       - Build counts = {2: 2} from smaller = [2, 2].
//       - Stream through nums1: 1 is absent, the two 2s each find counts[2] > 0 and
//         decrement it (2 -> 1 -> 0) while appending 2 twice, and the final 1 is
//         absent, returning [2, 2].
//
//   Example 2:
//     Input:  nums1 = [4, 9, 5], nums2 = [9, 4, 9, 8, 4]
//     Output: [9, 4]
//     Explanation:
//       - Select nums1 (size 3 <= 5) as smaller and build counts = {4: 1, 9: 1, 5:
//         1}.
//       - Probe larger = [9, 4, 9, 8, 4]: the first 9 and 4 match with count 1,
//         appending 9 and 4 and decrementing counts[9] and counts[4] to 0.
//       - The remaining 9, 8, and 4 either have remaining count 0 or are absent
//         from counts, returning [9, 4] (same multiset as [4, 9]).
//
//   Example 3:
//     Input:  nums1 = [-1, -1, 0, 3], nums2 = [-1, -1, -1, 5]
//     Output: [-1, -1]
//     Explanation:
//       - Since both arrays have size 4, choose nums1 as smaller and build counts =
//         {-1: 2, 0: 1, 3: 1}.
//       - Scan nums2: the first two -1 entries decrement counts[-1] from 2 to 0 and
//         append -1 twice.
//       - The third -1 sees counts[-1] == 0 and 5 is absent from counts, capping
//         the output at [-1, -1].
//
// Algorithm:
//   Select the smaller input array to build a frequency hash map, keeping
//   auxiliary memory bounded by O(min(M, N)). Then stream through the larger
//   array: whenever an element has a positive remaining count in the map,
//   append it to the result and decrement its count.
//   Follow-up considerations:
//     - If both arrays are already sorted, a two-pointer scan computes the
//       intersection in O(M + N) time and O(1) extra space.
//     - If `nums2` is stored on disk and cannot fit in RAM, building the hash
//       map on the smaller in-memory `nums1` and streaming `nums2` in chunks
//       solves the problem directly with this exact algorithm.
//   Time Complexity: O(M + N) average time.
//   Space Complexity: O(min(M, N)) for the frequency hash map.
//
// Data Structures:
//   - `std::unordered_map<int, int>`: tracks remaining matchable multiplicity
//     for each integer in the smaller input array.
//
// Real-World Applications:
//   - Database hash-join execution (building the hash table on the smaller
//     build relation and probing with the larger streaming probe relation).
//   - Inventory reconciliation and bag-of-words overlap computation where
//     duplicate multiplicities must be preserved.
//   - Distributed log deduplication / set-multiset intersection across sharded
//     disk-backed datasets.
//
// Related LeetCode Problems:
//   - #349 Intersection of Two Arrays [done] (unique set intersection without multiplicities)
//   - #299 Bulls And Cows [done] (multiset overlap count over a fixed digit alphabet)
//   - #1002 Find Common Characters [new] (multiset intersection across multiple strings)
//   - #1213 Intersection of Three Sorted Arrays [done] (multi-pointer sorted intersection)

#include "framework/lc_framework.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the multiset intersection of `nums1` and `nums2`.
  // Time: O(|nums1| + |nums2|) average, Space: O(min(|nums1|, |nums2|)).
  std::vector<int> intersect(const std::vector<int>& nums1,
                             const std::vector<int>& nums2) {
    // Build the hash table on the smaller array so memory stays O(min(M, N))
    // and the larger array can be streamed in a single pass.
    const bool first_is_smaller = nums1.size() <= nums2.size();
    const std::vector<int>& smaller = first_is_smaller ? nums1 : nums2;
    const std::vector<int>& larger = first_is_smaller ? nums2 : nums1;

    std::unordered_map<int, int> counts;
    counts.reserve(smaller.size());
    for (const int val : smaller) {
      ++counts[val];
    }

    // Probe each element of the larger array, decrementing its remaining
    // quota whenever a match is consumed.
    std::vector<int> result;
    for (const int val : larger) {
      if (const auto it = counts.find(val);
          it != counts.end() && it->second > 0) {
        result.push_back(val);
        --it->second;
      }
    }
    return result;
  }
};

LC_PROBLEM(350, "Intersection Of Two Arrays II", "hash", ctx) {
  auto sorted_vec = [](std::vector<int> v) {
    std::ranges::sort(v);
    return v;
  };

  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sorted_vec(sol.intersect({1, 2, 2, 1}, {2, 2})),
                  std::vector<int>{2, 2});
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sorted_vec(sol.intersect({4, 9, 5}, {9, 4, 9, 8, 4})),
                  std::vector<int>{4, 9});
  });

  ctx.test("disjoint_and_empty_inputs", [&] {
    Solution sol;
    ctx.expect_eq(sol.intersect({1, 2, 3}, {4, 5, 6}), std::vector<int>{});
    ctx.expect_eq(sol.intersect({}, {1, 2}), std::vector<int>{});
    ctx.expect_eq(sol.intersect({1, 2}, {}), std::vector<int>{});
  });

  ctx.test("multiplicity_capped_by_smaller_count", [&] {
    Solution sol;
    ctx.expect_eq(sorted_vec(sol.intersect({2, 2, 2}, {2, 2})),
                  std::vector<int>{2, 2});
    ctx.expect_eq(sorted_vec(sol.intersect({-1, -1, 0, 3}, {-1, -1, -1, 5})),
                  std::vector<int>{-1, -1});
  });
}

}  // namespace
