// LeetCode 46: Permutations
// https://leetcode.com/problems/permutations/
//
// Problem:
//   Given an array `nums` of distinct integers, return all possible
//   permutations of `nums`. You may return the answer in any order.
//   Constraints: 1 <= nums.length <= 6, -10 <= nums[i] <= 10, all integers
//   of `nums` are unique.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 3]
//     Output: [[1, 2, 3], [1, 3, 2], [2, 1, 3], [2, 3, 1], [3, 1, 2], [3, 2, 1]]
//     Explanation:
//       - Initialize used = [0, 0, 0] and current = [], then at depth 0 pick index
//         0 (1), setting used = [1, 0, 0].
//       - Extend with unused indices 1 then 2 to hit current.size() == 3 and emit
//         [1, 2, 3], then backtrack to [1] and pick 2 then 1 to emit [1, 3, 2].
//       - Backtrack to depth 0, clear used[0] = 0, and repeat with starting choices
//         2 and 3 to append [2, 1, 3], [2, 3, 1], [3, 1, 2], and [3, 2, 1].
//
//   Example 2:
//     Input:  nums = [-1, 0, 1]
//     Output: [[-1, 0, 1], [-1, 1, 0], [0, -1, 1], [0, 1, -1], [1, -1, 0], [1, 0,
//             -1]]
//     Explanation:
//       - DFS tracks occupancy via used = [0, 0, 0] by index, so negative values
//         work identically to positive integers.
//       - Choosing index 0 (-1) first explores the two remaining indices in order
//         to emit [-1, 0, 1] and [-1, 1, 0].
//       - Advancing the root loop to index 1 (0) and index 2 (1) produces [0, -1,
//         1], [0, 1, -1], [1, -1, 0], and [1, 0, -1].
//
//   Example 3:
//     Input:  nums = []
//     Output: [[]]
//     Explanation:
//       - With n = 0, allocate empty current and used vectors and invoke dfs().
//       - At entry, current.size() == n (0 == 0) holds immediately, so
//         results.push_back(current) appends [] and returns [[]].
//
// Algorithm:
//   Backtracking DFS with an occupancy array (`used`):
//     - Maintain `current` (the partially built permutation) and `used[i]`
//       (whether `nums[i]` is already present in `current`).
//     - At each depth, iterate `i` from `0` to `n - 1`. Whenever `used[i] == 0`,
//       mark `used[i] = 1`, push `nums[i]` onto `current`, recurse, and then
//       pop `current` and reset `used[i] = 0` on backtrack.
//     - When `current.size() == nums.size()`, append `current` to `results`.
//     - Compared to slicing sub-arrays on every call, the `used` array avoids
//       O(n) allocations per tree node while preserving input-order
//       lexicographical output.
//   Time Complexity:  O(n * n!) — n! permutations are emitted, each copied in
//                     O(n) time.
//   Space Complexity: O(n) auxiliary space for the recursion stack, `current`
//                     buffer, and `used` flags (excluding the O(n * n!) output).
//
// Data Structures:
//   - `std::vector<std::uint8_t>` (`used`): O(1) membership bit-table tracking
//     which input indices are active on the current DFS path.
//   - Path buffer (`std::vector<int>`): stores the in-progress permutation.
//   - Result list (`std::vector<std::vector<int>>`): holds all n! permutations.
//
// Real-World Applications:
//   - Exhaustive job-order scheduling and small-scale Traveling Salesman /
//     vehicle-routing route enumeration.
//   - Compiler instruction-scheduling search over small basic-block dependency
//     windows.
//   - Combinatorial fuzzing and property-based testing (testing all API call
//     or event arrival orderings to detect race conditions or state bugs).
//   - Cryptographic permutation generation and key-schedule exploration.
//
// Related LeetCode Problems:
//   - #47 Permutations II [done] (handles duplicate elements via sort + pruning)
//   - #31 Next Permutation [done] (generates the next lexicographical order in O(n))
//   - #60 Permutation Sequence [new] (jumps directly to the k-th permutation in O(n^2))
//   - #77 Combinations [done] (order-independent k-subsets via backtracking)
//   - #78 Subsets [done] (power-set enumeration via backtracking)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all permutations of distinct integers in `nums` in input-driven
  // DFS order.
  // Time: O(n * n!), Space: O(n) auxiliary (excluding output).
  std::vector<std::vector<int>> permute(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    std::vector<std::vector<int>> results;
    std::vector<int> current;
    current.reserve(n);
    std::vector<std::uint8_t> used(n, 0);

    // DFS invariant: `current` contains exactly those `nums[i]` with
    // `used[i] == 1`, in their chosen placement order.
    auto dfs = [&](this auto&& self) -> void {
      if (current.size() == n) {
        results.push_back(current);
        return;
      }

      for (std::size_t i = 0uz; i < n; ++i) {
        if (used[i] != 0) {
          continue;
        }

        // Choose nums[i] for the next slot, explore, then undo on backtrack.
        used[i] = 1;
        current.push_back(nums[i]);
        self();
        current.pop_back();
        used[i] = 0;
      }
    };

    dfs();
    return results;
  }
};

LC_PROBLEM(46, "Permutations", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_three_elements", [&] {
    const std::vector<std::vector<int>> expected = {
        {1, 2, 3}, {1, 3, 2}, {2, 1, 3},
        {2, 3, 1}, {3, 1, 2}, {3, 2, 1},
    };
    ctx.expect_eq(sol.permute({1, 2, 3}), expected);
  });

  ctx.test("example_2_two_elements", [&] {
    const std::vector<std::vector<int>> expected = {
        {0, 1},
        {1, 0},
    };
    ctx.expect_eq(sol.permute({0, 1}), expected);
  });

  ctx.test("example_3_single_element", [&] {
    const std::vector<std::vector<int>> expected = {{1}};
    ctx.expect_eq(sol.permute({1}), expected);
  });

  ctx.test("negative_values", [&] {
    const std::vector<std::vector<int>> expected = {
        {-1, 0, 1}, {-1, 1, 0}, {0, -1, 1},
        {0, 1, -1}, {1, -1, 0}, {1, 0, -1},
    };
    ctx.expect_eq(sol.permute({-1, 0, 1}), expected);
  });

  ctx.test("empty_input", [&] {
    const std::vector<std::vector<int>> expected = {{}};
    ctx.expect_eq(sol.permute({}), expected);
  });
}

}  // namespace
