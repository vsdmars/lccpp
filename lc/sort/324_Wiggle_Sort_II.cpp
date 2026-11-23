// LeetCode 324: Wiggle Sort II
// https://leetcode.com/problems/wiggle-sort-ii/
//
// Problem:
//   Given an unsorted integer array nums, reorder it in-place so that it
//   satisfies the strict alternating inequality:
//     nums[0] < nums[1] > nums[2] < nums[3] > ...
//   Every input is guaranteed to have at least one valid wiggle ordering.
//
// Algorithm:
//   1. Sort-and-reversed-half interleave (primary, mirrors Python solution):
//      Sort a copy of nums and split it at half = (n + 1) / 2 into a smaller
//      half sorted[0 .. half - 1] and a larger half sorted[half .. n - 1].
//      Fill even indices (0, 2, 4, ...) from the smaller half in REVERSE order
//      and odd indices (1, 3, 5, ...) from the larger half in REVERSE order.
//      Reversing both halves is crucial when duplicate medians exist (e.g.,
//      [4, 5, 5, 6] -> [5, 6, 4, 5]): it places copies of the median at the
//      far left (index 0) and far right (last odd index) so identical medians
//      are never adjacent.
//      Time Complexity: O(n log n). Space Complexity: O(n).
//   2. Follow-up O(n) average time / O(1) extra space (secondary):
//      Find the median with std::nth_element, then run a 3-way Dutch National
//      Flag partition (> median, == median, < median) over the virtual index
//      mapping vi(i) = (2 * i + 1) % (n | 1), which visits odd slots first
//      (1, 3, 5, ...) and even slots next (0, 2, 4, ...).
//      Time Complexity: O(n) average. Space Complexity: O(1).
//
// Data Structures:
//   - Primary: sorted copy std::vector<int> partitioned into lower/upper
//     halves and interleaved back into nums.
//   - Secondary: in-place 3-pointer Dutch National Flag partition with
//     arithmetic virtual-index remapping.
//
// Real-World Applications:
//   - Load-balancing / interleaving heavy and light jobs across alternating
//     slots to avoid thermal or bandwidth bursts in hardware pipelines.
//   - Signal dithering and alternating-polarity pulse-width modulation
//     (PWM) layout in digital-to-analog converters.
//   - Virtual-index permutation and in-place 3-way partitioning in memory-
//     constrained embedded sorting libraries.
//
// Related LeetCode Problems:
//   - #280 Wiggle Sort [done] (non-strict <= / >= version solvable in one pass)
//   - #75 Sort Colors [done] (3-way Dutch National Flag partitioning)
//   - #215 Kth Largest Element in an Array [done] (linear-time nth_element selection)
//   - #376 Wiggle Subsequence [new] (longest alternating subsequence via greedy/DP)
//   - #905 Sort Array By Parity [done] (even/odd index partitioning patterns)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Reorders nums in-place so nums[0] < nums[1] > nums[2] < nums[3] ...
  // using sorted reversed-half interleaving.
  // Time: O(n log n), Space: O(n).
  void wiggleSort(std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 1uz) {
      return;
    }

    std::vector<int> sorted = nums;
    std::ranges::sort(sorted);

    // Even indices count is ceil(n / 2) = (n + 1) / 2.
    // Traverse both halves backward so duplicate medians are maximally apart.
    const std::size_t half = (n + 1uz) / 2uz;
    std::size_t small_idx = half;
    std::size_t large_idx = n;

    for (std::size_t i = 0uz; i < n; ++i) {
      if ((i % 2uz) == 0uz) {
        --small_idx;
        nums[i] = sorted[small_idx];
      } else {
        --large_idx;
        nums[i] = sorted[large_idx];
      }
    }
  }

  // Follow-up in-place O(n)-average-time, O(1)-space solution using
  // std::nth_element and 3-way Dutch National Flag partitioning over virtual
  // indices.
  // Time: O(n) average, Space: O(1).
  void wiggleSortLinear(std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 1uz) {
      return;
    }

    const std::size_t mid = n / 2uz;
    std::ranges::nth_element(
        nums, nums.begin() + static_cast<std::ptrdiff_t>(mid));
    const int median = nums[mid];

    // Virtual index maps 0, 1, 2, ... -> 1, 3, 5, ..., 0, 2, 4, ...
    // (n | 1) is the smallest odd integer >= n.
    const std::size_t modulus = n | 1uz;
    auto at = [&](std::size_t i) -> int& {
      return nums[((2uz * i) + 1uz) % modulus];
    };

    // 3-way partition in descending order under virtual indices:
    //   [0, left)        -> values > median  (placed into first odd slots)
    //   [left, cur)      -> values == median (middle odd & early even slots)
    //   (right, n - 1]   -> values < median  (placed into last even slots)
    std::size_t left = 0uz;
    std::size_t cur = 0uz;
    std::size_t right = n - 1uz;

    while (cur <= right) {
      if (at(cur) > median) {
        std::swap(at(left), at(cur));
        ++left;
        ++cur;
      } else if (at(cur) < median) {
        std::swap(at(cur), at(right));
        if (right == 0uz) {
          break;
        }
        --right;
      } else {
        ++cur;
      }
    }
  }
};

// Verifies that `actual` is a permutation of `original` and satisfies the
// strict wiggle inequality nums[0] < nums[1] > nums[2] < nums[3] ...
bool isValidWiggleOrder(const std::vector<int>& original,
                        const std::vector<int>& actual) {
  if (original.size() != actual.size()) {
    return false;
  }

  std::vector<int> expected_sorted = original;
  std::vector<int> actual_sorted = actual;
  std::ranges::sort(expected_sorted);
  std::ranges::sort(actual_sorted);
  if (expected_sorted != actual_sorted) {
    return false;
  }

  for (std::size_t i = 0uz; (i + 1uz) < actual.size(); ++i) {
    if ((i % 2uz) == 0uz) {
      if (actual[i] >= actual[i + 1uz]) {
        return false;
      }
    } else {
      if (actual[i] <= actual[i + 1uz]) {
        return false;
      }
    }
  }
  return true;
}

LC_PROBLEM(324, "Wiggle Sort II", "sort", ctx) {
  Solution sol;

  auto check_both = [&](const std::vector<int>& input) {
    std::vector<int> a = input;
    sol.wiggleSort(a);
    ctx.expect_true(isValidWiggleOrder(input, a));

    std::vector<int> b = input;
    sol.wiggleSortLinear(b);
    ctx.expect_true(isValidWiggleOrder(input, b));
  };

  ctx.test("example_1_repeated_small_values", [&] {
    check_both({1, 5, 1, 1, 6, 4});
  });

  ctx.test("example_2_duplicate_medians", [&] {
    check_both({1, 3, 2, 2, 3, 1});
  });

  ctx.test("critical_duplicate_median_separation", [&] {
    // Naive forward interleave fails on [4, 5, 5, 6] by placing 5 next to 5.
    check_both({4, 5, 5, 6});
  });

  ctx.test("python_build_cases", [&] {
    check_both({1, 3, 4});
    check_both({0, 1, 1, 4, 4, 5, 6});
    check_both({11, 15});
    check_both({100});
  });

  ctx.test("edge_cases_empty_and_negative_values", [&] {
    check_both({});
    check_both({-5, -1, -5, 0, 2, -1});
  });
}

}  // namespace
