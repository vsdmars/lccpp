// LeetCode 255: Verify Preorder Scequence In Binary Search Tree
// https://leetcode.com/problems/verify-preorder-sequence-in-binary-search-tree/
//
// Problem:
//   Given an array of unique integers `preorder`, return true if it represents
//   a valid preorder traversal (Root -> Left -> Right) sequence of some binary
//   search tree (BST), and false otherwise.
//
// Examples:
//   Example 1:
//     Input:  preorder = [5, 2, 6, 1, 3]
//     Output: false
//     Explanation:
//       - 5 and 2 strictly decrease and are pushed onto stack = [5, 2] with low
//         unset.
//       - Processing 6 > stack.back() pops 2 and 5, setting low = 5 and pushing 6
//         (stack = [6]).
//       - The next value 1 satisfies 1 <= *low (5), violating the right-subtree
//         bound of ancestor 5, so false is returned immediately.
//
//   Example 2:
//     Input:  preorder = [5, 2, 1, 3, 6]
//     Output: true
//     Explanation:
//       - 5, 2, and 1 are pushed in decreasing order (stack = [5, 2, 1], low
//         unset).
//       - Value 3 pops 1 and 2, updating low = 2 and pushing 3 (stack = [5, 3]).
//       - Value 6 exceeds low (2), pops 3 and 5 to set low = 5, pushes 6, and
//         finishes the loop returning true.
//
//   Example 3:
//     Input:  preorder = [10, 5, 2, 7, 15, 12, 9]
//     Output: false
//     Explanation:
//       - 10, 5, and 2 push onto stack = [10, 5, 2], then 7 pops 2 and 5 to set low
//         = 5 and push 7 (stack = [10, 7]).
//       - 15 pops 7 and 10 to set low = 10 and push 15, after which 12 pushes onto
//         stack = [15, 12].
//       - Next element 9 violates 9 <= *low (10) in the right subtree of 10,
//         causing an immediate return of false.
//
// Algorithm:
//   Monotonic decreasing stack simulating a BST preorder walk while tracking
//   the tightest lower bound `low` enforced by completed left subtrees:
//   - While descending into left subtrees, values strictly decrease and are
//     pushed onto the stack (representing open ancestor nodes).
//   - When the current value `val` exceeds `stack.back()`, we have transitioned
//     into the right subtree of one or more ancestors. We pop every ancestor
//     smaller than `val`; the last popped ancestor becomes the new `low` bound,
//     because every subsequent node lies in that ancestor's right subtree and
//     must therefore be strictly greater than `low`.
//   - If at any point `val <= low`, the BST ordering invariant is violated and
//     we immediately return false.
//   Two implementations are provided:
//   1. `verifyPreorder`: Uses an auxiliary stack without mutating the input.
//      - Time: O(n), Space: O(n) worst case (O(h) for tree height h).
//   2. `verifyPreorderConstantSpace`: Follow-up using the input array itself as
//      the monotonic stack in-place.
//      - Time: O(n), Space: O(1) auxiliary space.
//
// Data Structures:
//   - Monotonic decreasing stack (`std::vector<int>` or in-place prefix of
//     `preorder`) holding active ancestors whose right subtrees have not yet
//     been entered.
//   - Optional lower bound (`std::optional<int>`) cleanly supporting the full
//     32-bit signed integer domain including INT_MIN.
//
// Real-World Applications:
//   - Deserialization validators for compact tree wire formats (verifying
//     untrusted preorder-serialized BST payloads before allocating tree nodes).
//   - Stack-based syntactic validation of recursive-descent AST streams.
//   - Index integrity checks when bulk-loading sorted/tree-structured files.
//
// Related LeetCode Problems:
//   - #98 Validate Binary Search Tree [done] (validating an explicit BST structure)
//   - #449 Serialize and Deserialize BST [done] (reconstructing a BST from its preorder sequence)
//   - #1008 Construct Binary Search Tree from Preorder Traversal [done] (building the BST from valid preorder)
//   - #331 Verify Preorder Serialization of a Binary Tree [done] (validating general binary tree preorder with null markers)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <optional>
#include <vector>

namespace {

class Solution {
 public:
  // Verifies whether `preorder` is a valid BST preorder sequence using an
  // auxiliary monotonic decreasing stack.
  // Time: O(n), Space: O(n).
  bool verifyPreorder(const std::vector<int>& preorder) {
    // `stack` stores active ancestors along the current root-to-node path in
    // strictly decreasing order.
    std::vector<int> stack;
    stack.reserve(preorder.size());
    // `low` is the largest ancestor whose right subtree we have already
    // entered; every future element must be strictly greater than `low`.
    std::optional<int> low;

    for (const int val : preorder) {
      // Any value at or below `low` violates the BST invariant for the right
      // subtree of ancestor `low`.
      if (low.has_value() && val <= *low) {
        return false;
      }

      // Pop ancestors smaller than `val`: we have finished their left subtrees
      // and moved into the right subtree of the last popped ancestor.
      while (!stack.empty() && val > stack.back()) {
        low = stack.back();
        stack.pop_back();
      }

      stack.push_back(val);
    }

    return true;
  }

  // Follow-up: O(1) extra space solution reusing `preorder` as the monotonic
  // stack in-place (by value so caller's vector stays untouched if desired).
  // Time: O(n), Space: O(1) auxiliary.
  bool verifyPreorderConstantSpace(std::vector<int> preorder) {
    // `stack_size` marks the logical end of the in-place decreasing stack
    // occupying preorder[0 .. stack_size - 1]. Since we write at most one
    // element per read index, `stack_size` never overtakes the read loop.
    std::size_t stack_size = 0uz;
    std::optional<int> low;

    for (const int val : preorder) {
      if (low.has_value() && val <= *low) {
        return false;
      }

      while (stack_size > 0uz && val > preorder[stack_size - 1uz]) {
        low = preorder[stack_size - 1uz];
        --stack_size;
      }

      preorder[stack_size] = val;
      ++stack_size;
    }

    return true;
  }
};

LC_PROBLEM(255, "Verify Preorder Scequence In Binary Search Tree",
           "binary_search_tree", ctx) {
  Solution sol;

  auto expect_both = [&](const std::vector<int>& seq, bool expected) {
    ctx.expect_eq(sol.verifyPreorder(seq), expected);
    ctx.expect_eq(sol.verifyPreorderConstantSpace(seq), expected);
  };

  ctx.test("example_1_invalid_preorder",
           [&] { expect_both({5, 2, 6, 1, 3}, false); });

  ctx.test("example_2_valid_preorder_python_build",
           [&] { expect_both({5, 2, 1, 3, 6}, true); });

  ctx.test("strictly_monotonic_skewed_trees", [&] {
    // Strictly increasing -> right-skewed BST.
    expect_both({1, 2, 3, 4, 5}, true);
    // Strictly decreasing -> left-skewed BST.
    expect_both({5, 4, 3, 2, 1}, true);
  });

  ctx.test("deep_right_subtree_violation", [&] {
    // 10 -> left(5, 2, 7), right(15, 12, 9): 9 < 10 is invalid.
    expect_both({10, 5, 2, 7, 15, 12, 9}, false);
    // Valid version replacing 9 with 11 (10 < 11 < 12).
    expect_both({10, 5, 2, 7, 15, 12, 11}, true);
  });

  ctx.test("single_empty_and_int32_extremes", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();

    expect_both({}, true);
    expect_both({42}, true);
    expect_both({0, kMin, kMax}, true);
    expect_both({0, kMax, kMin}, false);
  });
}

}  // namespace
