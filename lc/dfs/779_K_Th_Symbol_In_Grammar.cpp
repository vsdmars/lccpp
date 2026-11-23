// LeetCode 779: K Th Symbol In Grammar
// https://leetcode.com/problems/k-th-symbol-in-grammar/
//
// Problem:
//   We build a table of `n` rows (1-indexed). Row 1 contains `0`. Each
//   subsequent row is generated from the previous row by replacing every `0`
//   with `01` and every `1` with `10`. Given 1-indexed integers `n` and `k`,
//   return the `k`-th symbol (0 or 1) in row `n`.
//   Constraints: 1 <= n <= 30, 1 <= k <= 2^(n - 1).
//
// Algorithm:
//   The table forms an implicit full binary tree of height `n` rooted at
//   `(row 1, col 1) = 0`:
//     - Every node `v` produces left child `v` (at odd position `2*p - 1`) and
//       right child `1 - v` (at even position `2*p`).
//   Therefore, the parent of `(n, k)` sits at row `n - 1` and position
//   `parent_k = (k + 1) / 2`. We recursively compute the parent's symbol down
//   to the base case `n == 1` (which returns `0`):
//     - If `k` is odd (left child), return `parent`.
//     - If `k` is even (right child), return `1 - parent`.
//   (Equivalently, every right-child step flips the bit, so the answer equals
//   `std::popcount(k - 1) & 1`, the parity of the Thue-Morse sequence.)
//   Time Complexity: O(n), since each recursive step moves up one row.
//   Space Complexity: O(n) for the recursion stack (at most 30 frames).
//
// Data Structures:
//   - Implicit binary tree represented purely by integer coordinates `(n, k)`
//     on the call stack; no heap allocation is required.
//
// Real-World Applications:
//   - Computing arbitrary indices in fractal / L-system / Thue-Morse
//     substitution sequences without materializing exponentially long strings.
//   - Fair turn-ordering schedules (Thue-Morse sequence minimizes first-mover
//     advantage in alternating drafts and sports tiebreakers).
//   - Implicit binary heap / segment-tree ancestor navigation via index math.
//
// Related LeetCode Problems:
//   - #1545 Find Kth Bit in Nth Binary String [new] (similar recursive divide-and-conquer index mapping)
//   - #3304 Find the K-th Character in String Game I [new] (exponential string doubling via bit parity)
//   - #89 Gray Code [new] (recursive bit-reflection sequence generation)

#include "framework/lc_framework.h"

#include <array>
#include <bit>
#include <cstddef>

namespace {

class Solution {
 public:
  // Returns the 1-indexed `k`-th symbol in row `n` via recursive parent lookup.
  // Time: O(n); Space: O(n) recursion depth.
  int kthGrammar(int n, int k) {
    // Base case: row 1 consists solely of symbol 0.
    if (n == 1) {
      return 0;
    }

    // In the implicit binary tree, positions (2p - 1) and (2p) both descend
    // from parent position `p = (k + 1) / 2` in row `n - 1`.
    const int parent = kthGrammar(n - 1, (k + 1) / 2);

    // Odd positions are left children (same bit as parent); even positions are
    // right children (inverted bit).
    if ((k % 2) != 0) {
      return parent;
    }
    return 1 - parent;
  }
};

LC_PROBLEM(779, "K Th Symbol In Grammar", "dfs", ctx) {
  ctx.test("examples_1_2_3", [&] {
    Solution sol;
    ctx.expect_eq(sol.kthGrammar(1, 1), 0);
    ctx.expect_eq(sol.kthGrammar(2, 1), 0);
    ctx.expect_eq(sol.kthGrammar(2, 2), 1);
  });

  ctx.test("row_4_full_sequence", [&] {
    Solution sol;
    // Row 4 is "01101001".
    static constexpr std::array<int, 8> kExpected = {0, 1, 1, 0, 1, 0, 0, 1};
    for (std::size_t i = 0uz; i < kExpected.size(); ++i) {
      ctx.expect_eq(sol.kthGrammar(4, static_cast<int>(i) + 1), kExpected[i]);
    }
  });

  ctx.test("python_build_case_n10_k12", [&] {
    Solution sol;
    ctx.expect_eq(sol.kthGrammar(10, 12), 1);
  });

  ctx.test("max_constraint_matches_thue_morse_parity", [&] {
    Solution sol;
    // For n = 30, k = 2^29 = 536870912, (k - 1) has 29 set bits => parity 1.
    static constexpr int kMaxN = 30;
    static constexpr int kMaxK = static_cast<int>(1u << 29u);
    const int expected =
        std::popcount(static_cast<unsigned int>(kMaxK - 1)) % 2;
    ctx.expect_eq(sol.kthGrammar(kMaxN, kMaxK), expected);
    ctx.expect_eq(sol.kthGrammar(kMaxN, 1), 0);
  });
}

}  // namespace
