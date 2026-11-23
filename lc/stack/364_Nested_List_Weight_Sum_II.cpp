// LeetCode 364: Nested List Weight Sum II
// https://leetcode.com/problems/nested-list-weight-sum-ii/
//
// Problem:
//   Given a nested list of integers `nestedList`, each element is either a
//   single integer or a list whose elements may themselves be integers or
//   other lists. The depth of an integer is the number of lists that contain
//   it (top-level elements have depth 1). Let `maxDepth` be the maximum depth
//   of any integer in `nestedList`. The weight of an integer is defined
//   bottom-up as `maxDepth - depth + 1`. Return the sum of each integer
//   multiplied by its weight.
//
// Examples:
//   Example 1:
//     Input:  nestedList = [[1, 1], 2, [1, 1]]
//     Output: 8
//     Explanation:
//       - At BFS level 0 (depth 1), integer 2 sets level_sums[0] = 2 and
//         deepest_int_depth = 1, while the two [1, 1] lists enqueue four 1s into
//         next_level.
//       - At BFS level 1 (depth 2), summing the four integers yields level_sums[1]
//         = 4 and updates deepest_int_depth = 2, with next_level empty.
//       - Prefix pass over depth = 0..1 computes depth 0 (running_sum = 2,
//         weighted_sum = 2) and depth 1 (running_sum = 2 + 4 = 6, weighted_sum = 2
//         + 6 = 8).
//
//   Example 2:
//     Input:  nestedList = [1, [4, [6]]]
//     Output: 17
//     Explanation:
//       - Level-by-level BFS records level_sums = [1, 4, 6] across depths 1, 2, and
//         3, setting deepest_int_depth = 3.
//       - Prefix pass adds depth 0 (running_sum = 1, weighted_sum = 1) and depth 1
//         (running_sum = 5, weighted_sum = 1 + 5 = 6).
//       - At depth 2, running_sum becomes 5 + 6 = 11 and weighted_sum becomes 6 +
//         11 = 17, matching 1*3 + 4*2 + 6*1.
//
//   Example 3:
//     Input:  nestedList = [1, [[[]]]]
//     Output: 1
//     Explanation:
//       - At BFS level 0 (depth 1), integer 1 gives level_sums[0] = 1 with
//         saw_integer = true, setting deepest_int_depth = 1, and enqueues [[[]]].
//       - BFS expands levels 1, 2, and 3 through the nested empty lists with
//         level_sum = 0 and saw_integer = false, so deepest_int_depth remains 1.
//       - Prefix pass runs only for depth = 0 < 1, returning weighted_sum = 1 so
//         deeper empty lists do not inflate maxDepth.
//
// Algorithm:
//   Breadth-first level-by-level traversal with running prefix accumulation:
//   1. Traverse the nested structure one depth level at a time using two
//      non-owning pointer vectors (`current_level` and `next_level`).
//   2. For each level `d`, sum all integers found at level `d` into
//      `level_sums[d]` and enqueue child elements of nested lists into
//      `next_level`. Whenever at least one integer is seen at level `d`,
//      update `deepest_int_depth = d + 1`. (Tracking `deepest_int_depth`
//      ensures deeper empty lists like `[1, [[[]]]]` do not inflate
//      `maxDepth`.)
//   3. Accumulate the inverse-weighted sum over levels `0 .. deepest_int_depth
//      - 1` using running prefix sums:
//        `running_sum += level_sums[d]`
//        `weighted_sum += running_sum`
//      An integer at depth `k` is added into `weighted_sum` once for every
//      level from `k` to `maxDepth`, yielding total weight `maxDepth - k + 1`.
//   Time Complexity: O(N + L), visiting every integer (N) and nested list (L)
//   once.
//   Space Complexity: O(N + L) in the worst case for BFS level frontiers.
//
// Data Structures:
//   - `NestedInteger`: recursive value type representing either an `int` or a
//     `std::vector<NestedInteger>`.
//   - Two `std::vector<const NestedInteger*>` buffers for zero-copy BFS level
//     expansion.
//   - `std::vector<int>` storing per-level integer sums.
//
// Real-World Applications:
//   - Hierarchical scoring systems where top-level categories carry greater
//     weight than deeply nested sub-items (e.g., taxonomy or org-chart rollups).
//   - Evaluating nested expression trees or configuration overrides with
//     depth-dependent priority weights.
//   - Computing bottom-up height metrics over ragged JSON/AST documents in a
//     single breadth-first pass.
//
// Related LeetCode Problems:
//   - #339 Nested List Weight Sum [done] (top-down depth weighting over NestedInteger)
//   - #341 Flatten Nested List Iterator [done] (lazy stack-based traversal of NestedInteger)
//   - #385 Mini Parser [new] (parsing serialized text into NestedInteger trees)
//   - #102 Binary Tree Level Order Traversal [done] (BFS level-by-level traversal pattern)
//   - #565 Array Nesting [done] (measuring maximum nesting depth in structured sequences)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Mock of LeetCode's NestedInteger interface: holds either a single integer
// or a nested list of NestedInteger objects.
class NestedInteger {
 public:
  explicit NestedInteger(int value) : is_integer_(true), value_(value) {}

  explicit NestedInteger(std::vector<NestedInteger> list)
      : list_(std::move(list)) {}

  [[nodiscard]] bool isInteger() const noexcept { return is_integer_; }

  [[nodiscard]] int getInteger() const noexcept { return value_; }

  [[nodiscard]] const std::vector<NestedInteger>& getList() const noexcept {
    return list_;
  }

 private:
  bool is_integer_ = false;
  int value_ = 0;
  std::vector<NestedInteger> list_;
};

class Solution {
 public:
  // Computes the inverse-depth-weighted sum of all integers in `nested_list`.
  // Time: O(N + L), Space: O(N + L).
  int depthSumInverse(const std::vector<NestedInteger>& nested_list) {
    // Initialize the BFS frontier with non-owning pointers to top-level items.
    std::vector<const NestedInteger*> current_level;
    current_level.reserve(nested_list.size());
    for (const NestedInteger& item : nested_list) {
      current_level.push_back(&item);
    }

    std::vector<int> level_sums;
    std::size_t deepest_int_depth = 0uz;

    // Expand one depth level at a time, recording the integer sum at each depth
    // and the deepest level that actually contains an integer.
    while (!current_level.empty()) {
      int level_sum = 0;
      bool saw_integer = false;
      std::vector<const NestedInteger*> next_level;

      for (const NestedInteger* item : current_level) {
        if (item->isInteger()) {
          level_sum += item->getInteger();
          saw_integer = true;
        } else {
          for (const NestedInteger& child : item->getList()) {
            next_level.push_back(&child);
          }
        }
      }

      level_sums.push_back(level_sum);
      if (saw_integer) {
        deepest_int_depth = level_sums.size();
      }
      current_level = std::move(next_level);
    }

    // Repeated prefix addition gives depth-k sums weight (max_depth - k + 1):
    //   level 0 is added `max_depth` times,
    //   level 1 is added `max_depth - 1` times, ...,
    //   level `max_depth - 1` is added once.
    int running_sum = 0;
    int weighted_sum = 0;
    for (std::size_t depth = 0uz; depth < deepest_int_depth; ++depth) {
      running_sum += level_sums[depth];
      weighted_sum += running_sum;
    }

    return weighted_sum;
  }
};

NestedInteger make_int(int value) { return NestedInteger(value); }

NestedInteger make_list(std::vector<NestedInteger> items) {
  return NestedInteger(std::move(items));
}

LC_PROBLEM(364, "Nested List Weight Sum II", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    // [[1, 1], 2, [1, 1]] -> 2*2 + (1+1+1+1)*1 = 8
    std::vector<NestedInteger> input;
    input.push_back(make_list({make_int(1), make_int(1)}));
    input.push_back(make_int(2));
    input.push_back(make_list({make_int(1), make_int(1)}));
    ctx.expect_eq(sol.depthSumInverse(input), 8);
  });

  ctx.test("leetcode_example_2", [&] {
    // [1, [4, [6]]] -> 1*3 + 4*2 + 6*1 = 17
    std::vector<NestedInteger> level3;
    level3.push_back(make_int(6));
    std::vector<NestedInteger> level2;
    level2.push_back(make_int(4));
    level2.push_back(make_list(std::move(level3)));
    std::vector<NestedInteger> input;
    input.push_back(make_int(1));
    input.push_back(make_list(std::move(level2)));
    ctx.expect_eq(sol.depthSumInverse(input), 17);
  });

  ctx.test("empty_lists_and_intermediate_empty_depth", [&] {
    // Completely empty input and nested empty lists evaluate to 0.
    ctx.expect_eq(sol.depthSumInverse({}), 0);

    std::vector<NestedInteger> only_empty;
    only_empty.push_back(make_list({}));
    std::vector<NestedInteger> nested_empty;
    nested_empty.push_back(make_list({}));
    only_empty.push_back(make_list(std::move(nested_empty)));
    ctx.expect_eq(sol.depthSumInverse(only_empty), 0);

    // Deeper empty lists must not increase maxDepth: [1, [[[]]]] -> 1*1 = 1.
    std::vector<NestedInteger> empty_d3;
    empty_d3.push_back(make_list({}));
    std::vector<NestedInteger> empty_d2;
    empty_d2.push_back(make_list(std::move(empty_d3)));
    std::vector<NestedInteger> with_deep_empty;
    with_deep_empty.push_back(make_int(1));
    with_deep_empty.push_back(make_list(std::move(empty_d2)));
    ctx.expect_eq(sol.depthSumInverse(with_deep_empty), 1);

    // Intermediate depth with no integers still counts when a deeper integer
    // exists: [1, [[2]]] -> 1*3 + 0*2 + 2*1 = 5.
    std::vector<NestedInteger> deep_two;
    deep_two.push_back(make_int(2));
    std::vector<NestedInteger> mid;
    mid.push_back(make_list(std::move(deep_two)));
    std::vector<NestedInteger> gap_case;
    gap_case.push_back(make_int(1));
    gap_case.push_back(make_list(std::move(mid)));
    ctx.expect_eq(sol.depthSumInverse(gap_case), 5);
  });

  ctx.test("negative_values", [&] {
    // [-1, [-2, [3]]] -> (-1)*3 + (-2)*2 + 3*1 = -4
    std::vector<NestedInteger> level3;
    level3.push_back(make_int(3));
    std::vector<NestedInteger> level2;
    level2.push_back(make_int(-2));
    level2.push_back(make_list(std::move(level3)));
    std::vector<NestedInteger> input;
    input.push_back(make_int(-1));
    input.push_back(make_list(std::move(level2)));
    ctx.expect_eq(sol.depthSumInverse(input), -4);
  });
}

}  // namespace
