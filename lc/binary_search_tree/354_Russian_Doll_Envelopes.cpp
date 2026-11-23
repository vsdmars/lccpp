// LeetCode 354: Russian Doll Envelopes
// https://leetcode.com/problems/russian-doll-envelopes/
//
// Problem:
//   Given a 2D array `envelopes` where `envelopes[i] = [w_i, h_i]` represents
//   the width and height of the i-th envelope, one envelope can fit inside
//   another if and only if both its width and its height are strictly smaller
//   than the outer envelope's width and height (rotation is not allowed).
//   Return the maximum number of envelopes that can be nested (Russian-dolled)
//   one inside another.
//
// Examples:
//   Example 1:
//     Input:  envelopes = [[5, 4], [6, 4], [6, 7], [2, 3]]
//     Output: 3
//     Explanation:
//       - Sort envelopes by width ascending and height descending on ties to get
//         [[2, 3], [5, 4], [6, 7], [6, 4]].
//       - Process heights [3, 4, 7] via patience sorting: each exceeds the current
//         tails maximum, expanding tails to [3], [3, 4], and [3, 4, 7].
//       - For the last height 4, lower_bound(tails, 4) points to index 1 (value 4),
//         leaving tails = [3, 4, 7].
//       - Return tails.size() = 3 as the maximum nesting depth.
//
//   Example 2:
//     Input:  envelopes = [[4, 5], [4, 6], [6, 7], [2, 3], [1, 1]]
//     Output: 4
//     Explanation:
//       - Sort by (width asc, height desc) into [[1, 1], [2, 3], [4, 6], [4, 5],
//         [6, 7]] so [4, 5] cannot extend [4, 6].
//       - Heights 1, 3, and 6 append to tails, giving [1, 3, 6].
//       - For height 5, lower_bound(tails, 5) locates index 2 (value 6) and
//         replaces it, updating tails to [1, 3, 5].
//       - Height 7 is greater than 5 and appends to tails, producing [1, 3, 5, 7]
//         and returning 4.
//
//   Example 3:
//     Input:  envelopes = [[1, 5], [2, 4], [3, 3], [4, 2], [5, 1]]
//     Output: 1
//     Explanation:
//       - Widths are already strictly increasing, so sorting preserves [[1, 5], [2,
//         4], [3, 3], [4, 2], [5, 1]].
//       - Height 5 appends to empty tails, yielding [5].
//       - Each subsequent height 4, 3, 2, and 1 is smaller than tails[0], so
//         lower_bound repeatedly overwrites index 0 down to [1].
//       - Return tails.size() = 1 since no two envelopes can nest.
//
// Algorithm:
//   Reduce the 2D partial-order nesting problem to 1D Longest Strictly
//   Increasing Subsequence (LIS) via a careful tie-breaking sort:
//   1. Sort `envelopes` by width ascending, and by height DESCENDING when
//      widths are equal. Sorting equal-width envelopes in descending height
//      order guarantees that at most one envelope of any given width can be
//      selected in a strictly increasing subsequence of heights (e.g., [6, 7]
//      appears before [6, 4], so [6, 4] cannot extend [6, 7]).
//   2. Run patience sorting on the resulting height sequence: maintain a
//      strictly increasing array `tails`, where `tails[len]` holds the minimum
//      possible ending height of an increasing subsequence of length `len + 1`.
//      For each height `h`, find the leftmost position in `tails` with value
//      >= `h` via binary search (`std::ranges::lower_bound`). Using leftmost
//      insertion (`lower_bound`, not `upper_bound`) enforces strict inequality
//      so equal heights replace an existing tail instead of extending it.
//   3. The final length of `tails` is the maximum number of nested envelopes.
//   Time Complexity: O(n log n) for sorting plus n binary searches over `tails`.
//   Space Complexity: O(n) for the `tails` array.
//
// Data Structures:
//   - `std::vector<std::vector<int>>` sorted in place by `(width asc, height desc)`.
//   - Monotonically increasing `std::vector<int> tails` queried via binary
//     search (`std::ranges::lower_bound`).
//
// Real-World Applications:
//   - 2D box/packaging nesting and logistics bin-packing optimization.
//   - Dominance chain and Pareto-frontier layering in multi-objective
//     optimization and database skyline queries.
//   - Task/job scheduling with two simultaneous resource constraints.
//
// Related LeetCode Problems:
//   - #300 Longest Increasing Subsequence [done] (1D patience-sorting LIS core)
//   - #646 Maximum Length of Pair Chain [new] (interval chaining by sorting)
//   - #1691 Maximum Height by Stacking Cuboids [new] (3D generalization of box nesting)
//   - #1996 The Number of Weak Characters in the Game [done] (same asc/desc 2D sort trick)

#include "framework/lc_framework.h"

#include <algorithm>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum length of a strictly nested chain of envelopes.
  // Time: O(n log n), Space: O(n).
  int maxEnvelopes(std::vector<std::vector<int>> envelopes) {
    if (envelopes.empty()) {
      return 0;
    }

    // Sort by width ascending; break width ties by height descending so two
    // envelopes with the same width can never both enter a strictly increasing
    // height subsequence.
    std::ranges::sort(envelopes, [](const std::vector<int>& lhs,
                                    const std::vector<int>& rhs) {
      if (lhs[0] != rhs[0]) {
        return lhs[0] < rhs[0];
      }
      return lhs[1] > rhs[1];
    });

    // Patience-sorting LIS over heights: tails[k] stores the smallest possible
    // ending height among all strictly increasing subsequences of length k + 1.
    std::vector<int> tails;
    tails.reserve(envelopes.size());

    for (const auto& env : envelopes) {
      const int height = env[1];
      // Leftmost position >= height ensures strict increase (duplicates
      // overwrite rather than extend).
      auto it = std::ranges::lower_bound(tails, height);
      if (it == tails.end()) {
        tails.push_back(height);
      } else {
        // Replacing with a smaller height keeps the subsequence length while
        // making it easier for future wider envelopes to extend it.
        *it = height;
      }
    }

    return static_cast<int>(tails.size());
  }
};

LC_PROBLEM(354, "Russian Doll Envelopes", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    // [2,3] -> [5,4] -> [6,7]
    ctx.expect_eq(sol.maxEnvelopes({{5, 4}, {6, 4}, {6, 7}, {2, 3}}), 3);
  });

  ctx.test("python_build_input", [&] {
    // [2,3] -> [5,5] -> [6,7]
    ctx.expect_eq(sol.maxEnvelopes({{5, 5}, {6, 4}, {6, 7}, {2, 3}}), 3);
  });

  ctx.test("all_identical_envelopes", [&] {
    // Strict inequality forbids nesting equal-sized envelopes.
    ctx.expect_eq(sol.maxEnvelopes({{1, 1}, {1, 1}, {1, 1}}), 1);
  });

  ctx.test("empty_and_single", [&] {
    ctx.expect_eq(sol.maxEnvelopes({}), 0);
    ctx.expect_eq(sol.maxEnvelopes({{10, 20}}), 1);
  });

  ctx.test("same_width_tie_break_trap", [&] {
    // [1,1] -> [2,3] -> [4,5] -> [6,7] has length 4.
    // Must not chain [4,5] -> [4,6].
    ctx.expect_eq(sol.maxEnvelopes({{4, 5}, {4, 6}, {6, 7}, {2, 3}, {1, 1}}),
                  4);
  });

  ctx.test("strictly_antichain_no_nesting", [&] {
    // Width increases while height decreases: no two can nest.
    ctx.expect_eq(sol.maxEnvelopes({{1, 5}, {2, 4}, {3, 3}, {4, 2}, {5, 1}}),
                  1);
  });
}

}  // namespace
