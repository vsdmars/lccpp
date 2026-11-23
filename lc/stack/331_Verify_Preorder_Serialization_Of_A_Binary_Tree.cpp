// LeetCode 331: Verify Preorder Serialization Of A Binary Tree
// https://leetcode.com/problems/verify-preorder-serialization-of-a-binary-tree/
//
// Problem:
//   One way to serialize a binary tree is preorder traversal, writing each
//   non-null node's integer value and `#` for each null child, separated by
//   commas (for example, `"9,3,4,#,#,1,#,#,2,#,6,#,#"`). Given a comma-
//   separated string `preorder`, verify whether it represents a valid preorder
//   serialization of a binary tree without reconstructing the tree.
//
// Examples:
//   Example 1:
//     Input:  preorder = "9,3,4,#,#,1,#,#,2,#,6,#,#"
//     Output: true
//     Explanation:
//       - Initialize slots = 1 for the root; token "9" consumes 1 slot and opens 2
//         child slots, updating slots from 1 -> 0 -> 2.
//       - Tokens "3" and "4" each consume 1 slot and add 2, advancing slots from 2
//         -> 3 -> 4, then two "#" tokens consume 1 slot each to leave slots = 2.
//       - Token "1" changes slots 2 -> 1 -> 3, followed by two "#" tokens reducing
//         slots to 1.
//       - Tokens "2", "#", "6", "#", "#" update slots as 1 -> 2 -> 1 -> 2 -> 1 ->
//         0, returning true because slots == 0 at the end.
//
//   Example 2:
//     Input:  preorder = "9,#,#,1"
//     Output: false
//     Explanation:
//       - Initialize slots = 1; token "9" decrements slots to 0 and adds 2 child
//         slots so slots = 2.
//       - The next two "#" tokens each decrement slots by 1, bringing slots to 0
//         and closing the tree.
//       - Token "1" decrements slots to -1 (< 0), immediately returning false
//         because no open slot remains.
//
//   Example 3:
//     Input:  preorder = "1,#"
//     Output: false
//     Explanation:
//       - Initialize slots = 1; token "1" decrements slots to 0 and adds 2 child
//         slots so slots = 2.
//       - Token "#" decrements slots by 1 to leave slots = 1 without adding any new
//         slots.
//       - All tokens are exhausted while slots == 1 != 0 (one right-child slot
//         remains unclosed), so isValidSerialization returns false.
//
// Algorithm:
//   Two complementary linear-time verifiers are provided:
//   1. Slot-Counting / Degree Balance (`isValidSerialization`):
//      Treat available child slots as a balance counter initialized to `1`
//      (for the root). Scanning comma-delimited tokens left to right:
//      - Every token (null or non-null) consumes 1 slot (`--slots`).
//      - If `slots < 0` at any point, extra tokens exist after the tree was
//        already closed, so return `false`.
//      - Every non-null node creates 2 child slots (`slots += 2`).
//      The serialization is valid iff at least one token was processed and
//      `slots == 0` at the end.
//      Time: O(N), Space: O(1).
//   2. Explicit Stack Leaf-Collapsing (`isValidSerializationStack`):
//      Directly captures the bottom-up reduction from the Python recursive
//      approach without risking call-stack overflow: push each token's nullity
//      onto a stack, and whenever the top three entries match `(node, #, #)`,
//      collapse them into a single `#`. Valid iff the final stack contains
//      exactly one `#`.
//      Time: O(N), Space: O(N).
//
// Data Structures:
//   - Slot counter (`int`) and zero-copy `std::string_view` token scanner for
//     the O(1)-space primary solution.
//   - `std::vector<bool>` (or `std::vector<char>`) stack for the explicit
//     leaf-collapsing verifier.
//
// Real-World Applications:
//   - Streaming validation of serialized ASTs, expression trees, or wire-format
//     tree messages before allocating heap objects (preventing DoS on malformed
//     payloads).
//   - Prefix-notation (Polish notation) arity verification in compilers and
//     query planners.
//   - Integrity checks for tree-structured snapshots in storage engines.
//
// Related LeetCode Problems:
//   - #297 Serialize and Deserialize Binary Tree [done] (building and parsing binary tree serializations)
//   - #449 Serialize and Deserialize BST [done] (compact preorder serialization leveraging BST bounds)
//   - #652 Find Duplicate Subtrees [done] (canonical subtree serialization for structural deduplication)
//   - #1008 Construct Binary Search Tree from Preorder Traversal [done] (reconstructing a tree directly from preorder values)
//   - #1028 Recover a Tree From Preorder Traversal [new] (stack-based parsing of depth-annotated preorder strings)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Verifies a comma-separated preorder binary tree serialization using O(1)
  // slot-balance tracking (out-degree minus in-degree).
  // Time: O(N), Space: O(1).
  bool isValidSerialization(std::string_view preorder) {
    if (preorder.empty()) {
      return false;
    }

    // Initially, exactly one slot is open for the root node (or `#`).
    int slots = 1;
    std::size_t pos = 0uz;

    while (pos <= preorder.size()) {
      const std::string_view token = next_token(preorder, pos);

      // Every token fills one previously opened slot.
      --slots;
      if (slots < 0) {
        return false;
      }

      // A non-null node opens two new slots for its left and right children.
      if (token != "#") {
        slots += 2;
      }
    }

    // Valid iff every opened slot was filled with no leftover tokens.
    return slots == 0;
  }

  // Alternative stack-based verifier that repeatedly collapses every completed
  // subtree `(non-null, #, #)` into a single `#` sentinel.
  // Time: O(N), Space: O(N).
  bool isValidSerializationStack(std::string_view preorder) {
    if (preorder.empty()) {
      return false;
    }

    // Store '#' for null / collapsed subtrees and 'n' for non-null nodes.
    std::vector<char> stack;
    std::size_t pos = 0uz;

    while (pos <= preorder.size()) {
      const std::string_view token = next_token(preorder, pos);
      stack.push_back(token == "#" ? '#' : 'n');

      // Collapse any `(node, #, #)` suffix into `#` until no more reductions
      // apply.
      while (stack.size() >= 3uz && stack.back() == '#' &&
             stack[stack.size() - 2uz] == '#' &&
             stack[stack.size() - 3uz] == 'n') {
        stack.pop_back();
        stack.pop_back();
        stack.back() = '#';
      }
    }

    return stack.size() == 1uz && stack.back() == '#';
  }

 private:
  // Extracts the next comma-separated token starting at `pos` and advances
  // `pos` past the comma (or to `preorder.size() + 1` after the final token).
  static std::string_view next_token(std::string_view preorder,
                                     std::size_t& pos) {
    const std::size_t comma = preorder.find(',', pos);
    if (comma == std::string_view::npos) {
      const std::string_view token = preorder.substr(pos);
      pos = preorder.size() + 1uz;
      return token;
    }
    const std::string_view token = preorder.substr(pos, comma - pos);
    pos = comma + 1uz;
    return token;
  }
};

LC_PROBLEM(331, "Verify Preorder Serialization Of A Binary Tree", "stack",
           ctx) {
  Solution sol;

  auto expect_both = [&](std::string_view preorder, bool expected) {
    ctx.expect_eq(sol.isValidSerialization(preorder), expected);
    ctx.expect_eq(sol.isValidSerializationStack(preorder), expected);
  };

  ctx.test("leetcode_examples", [&] {
    expect_both("9,3,4,#,#,1,#,#,2,#,6,#,#", true);
    expect_both("1,#", false);
    expect_both("9,#,#,1", false);
  });

  ctx.test("source_build_cases", [&] {
    expect_both("#,1,2", false);
    expect_both("91,13,14,#,#,10", false);
    expect_both("19,3,24,#,#,1,#,#,2,#,6,#,#", true);
    expect_both("223,#,#", true);
  });

  ctx.test("edge_cases", [&] {
    // Empty tree serialized as a single null sentinel is valid.
    expect_both("#", true);
    expect_both("", false);
    // Extra trailing nulls after a closed tree must fail.
    expect_both("#,#", false);
    expect_both("1,#,#,#,#", false);
    // Left-skewed and right-skewed multi-digit trees.
    expect_both("10,20,30,#,#,#,#", true);
    expect_both("10,#,20,#,30,#,#", true);
  });
}

}  // namespace
