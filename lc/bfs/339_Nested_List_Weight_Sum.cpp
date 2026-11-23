// LeetCode 339: Nested List Weight Sum
// https://leetcode.com/problems/nested-list-weight-sum/
//
// Problem:
//   Given a nested list of integers `nestedList`, where each element is either
//   a single integer or a list whose elements may themselves be integers or
//   nested lists, return the sum of every integer multiplied by its 1-based
//   nesting depth.
//
// Examples:
//   Example 1:
//     Input:  nestedList = [[1, 1], 2, [1, 1]]
//     Output: 10
//     Explanation:
//       - Initialize frontier at depth = 1 with pointers to the three top-level
//         elements: [1, 1], 2, and [1, 1].
//       - At depth = 1, integer 2 adds 1 * 2 = 2 to total, while both [1, 1] lists
//         expand their four integer children into next_frontier.
//       - Advance to depth = 2 with frontier = [1, 1, 1, 1]; summing depth * value
//         adds 4 * (2 * 1) = 8 to reach total = 10 before next_frontier becomes
//         empty.
//
//   Example 2:
//     Input:  nestedList = [1, [4, [6]]]
//     Output: 27
//     Explanation:
//       - At depth = 1, integer 1 adds 1 * 1 = 1 to total and list [4, [6]] pushes
//         4 and [6] into next_frontier.
//       - At depth = 2, integer 4 adds 2 * 4 = 8 (total = 9) and list [6] pushes 6
//         into next_frontier.
//       - At depth = 3, integer 6 adds 3 * 6 = 18 (total = 27), next_frontier is
//         empty, and depthSum returns 27.
//
//   Example 3:
//     Input:  nestedList = [[], [[[]]]]
//     Output: 0
//     Explanation:
//       - At depth = 1, frontier contains [] and [[[]]]; neither is an integer, so
//         total remains 0 and [[[]]] pushes [[]] into next_frontier.
//       - At depth = 2, [[]] expands its child [] into next_frontier without
//         changing total.
//       - At depth = 3, the innermost [] has no children, so next_frontier is empty
//         and depthSum returns 0.
//
// Algorithm:
//   We provide both BFS and DFS traversals matching the Python source:
//   1. Level-order BFS (`depthSum`): Store non-owning pointers to the top-level
//      `NestedInteger` items in `frontier` with `depth = 1`. At each level,
//      add `depth * item->getInteger()` for every integer node and push
//      pointers to every child of every list node into `next_frontier`.
//      Increment `depth` after each level until `frontier` is empty. Using
//      pointers avoids deep-copying nested vectors.
//   2. Recursive DFS (`depthSumDfs`): Walk the nested hierarchy recursively
//      passing `depth + 1` into each sublist and accumulating `value * depth`
//      at integer leaves.
//   Time Complexity:  O(N), where N is the total number of nested elements
//                     (integers plus nested lists).
//   Space Complexity: O(W) <= O(N) for the BFS level frontier; O(D) <= O(N)
//                     for DFS recursion depth, where D is max nesting depth.
//
// Data Structures:
//   - `NestedInteger`: Models LeetCode's recursive tagged union (either a
//     single `int` or a `std::vector<NestedInteger>`).
//   - `std::vector<const NestedInteger*>`: Zero-copy level frontier for BFS.
//
// Real-World Applications:
//   - Depth-weighted scoring of hierarchical JSON/XML/YAML documents and DOM
//     trees (e.g., search ranking or complexity linting).
//   - Bill-of-materials (BOM) and nested organizational cost rollups where
//     nesting level affects overhead or discount tiers.
//   - Static analysis of nested lexical scopes and loop-nesting cost models in
//     compilers.
//
// Related LeetCode Problems:
//   - #364 Nested List Weight Sum II [done] (inverse depth weighting)
//   - #341 Flatten Nested List Iterator [done] (lazy iteration over NestedInteger)
//   - #385 Mini Parser [new] (parses string notation into NestedInteger)
//   - #565 Array Nesting [done] (depth exploration on nested structures)

#include "framework/lc_framework.h"

#include <utility>
#include <vector>

namespace {

// Self-contained model of LeetCode's NestedInteger interface.
class NestedInteger {
 public:
  // Initializes an empty nested list.
  NestedInteger() = default;

  // Initializes a single integer equal to `value`.
  explicit NestedInteger(int value) : is_integer_(true), value_(value) {}

  // Initializes a nested list from an existing vector of elements.
  explicit NestedInteger(std::vector<NestedInteger> list)
      : list_(std::move(list)) {}

  // Returns true iff this NestedInteger holds a single integer.
  [[nodiscard]] bool isInteger() const noexcept { return is_integer_; }

  // Returns the stored integer (valid when `isInteger()` is true).
  [[nodiscard]] int getInteger() const noexcept { return value_; }

  // Switches this instance to hold the single integer `value`.
  void setInteger(int value) {
    is_integer_ = true;
    value_ = value;
    list_.clear();
  }

  // Switches this instance to list mode (if needed) and appends `elem`.
  void add(NestedInteger elem) {
    if (is_integer_) {
      is_integer_ = false;
      value_ = 0;
    }
    list_.push_back(std::move(elem));
  }

  // Returns the stored nested list (valid when `isInteger()` is false).
  [[nodiscard]] const std::vector<NestedInteger>& getList() const noexcept {
    return list_;
  }

 private:
  bool is_integer_ = false;
  int value_ = 0;
  std::vector<NestedInteger> list_;
};

// Concise builder helpers for constructing NestedInteger test cases.
NestedInteger make_int(int value) { return NestedInteger(value); }

NestedInteger make_list(std::vector<NestedInteger> elems) {
  return NestedInteger(std::move(elems));
}

class Solution {
 public:
  // Computes the depth-weighted sum using level-order BFS.
  // Invariant: Every element pointer in `frontier` sits at exact nesting
  // `depth` (1-indexed).
  // Time: O(N), Space: O(N).
  int depthSum(const std::vector<NestedInteger>& nested_list) {
    std::vector<const NestedInteger*> frontier;
    frontier.reserve(nested_list.size());
    for (const NestedInteger& elem : nested_list) {
      frontier.push_back(&elem);
    }

    int depth = 1;
    int total = 0;

    while (!frontier.empty()) {
      std::vector<const NestedInteger*> next_frontier;
      for (const NestedInteger* item : frontier) {
        if (item->isInteger()) {
          total += depth * item->getInteger();
          continue;
        }
        for (const NestedInteger& child : item->getList()) {
          next_frontier.push_back(&child);
        }
      }
      frontier = std::move(next_frontier);
      ++depth;
    }

    return total;
  }

  // Recursive DFS alternative matching the Python `review()` implementation.
  // Time: O(N), Space: O(D) where D is the maximum nesting depth.
  int depthSumDfs(const std::vector<NestedInteger>& nested_list) {
    auto dfs = [](this auto&& self, const std::vector<NestedInteger>& list,
                  int depth) -> int {
      int sum = 0;
      for (const NestedInteger& elem : list) {
        if (elem.isInteger()) {
          sum += depth * elem.getInteger();
        } else {
          sum += self(elem.getList(), depth + 1);
        }
      }
      return sum;
    };
    return dfs(nested_list, 1);
  }
};

LC_PROBLEM(339, "Nested List Weight Sum", "bfs", ctx) {
  Solution sol;

  ctx.test("example_1_balanced_nested", [&] {
    // [[1, 1], 2, [1, 1]] -> 4 * (1 * 2) + 2 * 1 = 10.
    const std::vector<NestedInteger> input = {
        make_list({make_int(1), make_int(1)}),
        make_int(2),
        make_list({make_int(1), make_int(1)}),
    };
    ctx.expect_eq(sol.depthSum(input), 10);
    ctx.expect_eq(sol.depthSumDfs(input), 10);
  });

  ctx.test("example_2_deeply_nested", [&] {
    // [1, [4, [6]]] -> 1*1 + 4*2 + 6*3 = 27.
    const std::vector<NestedInteger> input = {
        make_int(1),
        make_list({make_int(4), make_list({make_int(6)})}),
    };
    ctx.expect_eq(sol.depthSum(input), 27);
    ctx.expect_eq(sol.depthSumDfs(input), 27);
  });

  ctx.test("empty_and_nested_empty_lists", [&] {
    const std::vector<NestedInteger> empty;
    ctx.expect_eq(sol.depthSum(empty), 0);
    ctx.expect_eq(sol.depthSumDfs(empty), 0);

    const std::vector<NestedInteger> nested_empty = {
        make_list({}),
        make_list({make_list({make_list({})})}),
    };
    ctx.expect_eq(sol.depthSum(nested_empty), 0);
    ctx.expect_eq(sol.depthSumDfs(nested_empty), 0);
  });

  ctx.test("negative_values_and_single_integer", [&] {
    // [-1, [2, [-3]]] -> -1*1 + 2*2 + (-3)*3 = -6.
    const std::vector<NestedInteger> input = {
        make_int(-1),
        make_list({make_int(2), make_list({make_int(-3)})}),
    };
    ctx.expect_eq(sol.depthSum(input), -6);
    ctx.expect_eq(sol.depthSumDfs(input), -6);

    const std::vector<NestedInteger> single = {make_int(0)};
    ctx.expect_eq(sol.depthSum(single), 0);
    ctx.expect_eq(sol.depthSumDfs(single), 0);
  });

  ctx.test("mutator_api_add_and_set_integer", [&] {
    NestedInteger root_list;
    NestedInteger leaf;
    leaf.setInteger(5);
    root_list.add(std::move(leaf));
    const std::vector<NestedInteger> input = {std::move(root_list)};
    // [[5]] -> 5 * 2 = 10.
    ctx.expect_eq(sol.depthSum(input), 10);
    ctx.expect_eq(sol.depthSumDfs(input), 10);
  });
}

}  // namespace
