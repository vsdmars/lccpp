// LeetCode 2246: Longest Path With Different Adjacent Characters
// https://leetcode.com/problems/longest-path-with-different-adjacent-characters/
//
// Problem:
//   You are given a rooted tree of n nodes labeled 0..n-1 represented by an
//   array `parent` (where `parent[0] == -1` and `parent[i]` is the parent of
//   node `i`), together with a string `s` of length n assigning character
//   `s[i]` to node `i`. Return the length (number of nodes) of the longest
//   simple path in the tree such that no two adjacent nodes along the path
//   share the same character.
//
// Algorithm:
//   Build a children adjacency list from `parent` and run a single post-order
//   DFS (tree DP / diameter-style recursion) starting from root `0`.
//   For each node `u`:
//     - Recursively compute `child_len = dfs(v)` for EVERY child `v` of `u`
//       (even when `s[u] == s[v]`, because the globally longest valid path may
//       lie strictly inside `v`'s subtree).
//     - Among children with `s[v] != s[u]`, track the two largest downward
//       branch lengths `longest` and `second_longest`.
//     - The longest valid path whose highest node (turning point) is `u` has
//       length `longest + second_longest + 1`; update the global answer.
//     - Return `longest + 1` to `u`'s parent (only one downward branch can be
//       extended upward through `u`).
//
//   Time Complexity:  O(n), visiting each node and tree edge once.
//   Space Complexity: O(n) for the children adjacency list and DFS stack.
//
// Data Structures:
//   - `std::vector<std::vector<int>>` (`children`) adjacency list of the
//     rooted tree.
//   - Two scalar top-2 accumulators (`longest`, `second_longest`) per DFS
//     frame, avoiding per-node heap allocations.
//
// Real-World Applications:
//   - Network routing with policy/color-alternation constraints (e.g., finding
//     the longest path across alternating administrative domains or frequency
//     channels without adjacent interference).
//   - Bioinformatics / phylogenetics: identifying the longest alternating-trait
//     evolutionary path across a rooted lineage tree.
//   - Organizational / dependency hierarchy analysis: measuring the longest
//     chain of cross-team handoffs where adjacent nodes belong to distinct
//     teams.
//
// Related LeetCode Problems:
//   - #687 Longest Univalue Path [done] (complement constraint: adjacent nodes must share the same value)
//   - #543 Diameter of Binary Tree [done] (top-2 downward arm combination per node)
//   - #124 Binary Tree Maximum Path Sum [done] (tree DP combining best two branches at a turning point)
//   - #1245 Tree Diameter [done] (unconstrained diameter of an n-ary tree)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the number of nodes in the longest path in the rooted tree where
  // every pair of adjacent nodes has different assigned characters in `s`.
  //
  // Complexity: O(n) time, O(n) space.
  int longestPath(const std::vector<int>& parent, std::string_view s) {
    const std::size_t n = parent.size();
    if (n == 0uz) {
      return 0;
    }

    // Build rooted tree adjacency list: `children[p]` lists all direct
    // children of node `p`.
    std::vector<std::vector<int>> children(n);
    for (std::size_t i = 1uz; i < n; ++i) {
      children[static_cast<std::size_t>(parent[i])].push_back(
          static_cast<int>(i));
    }

    int global_max = 1;

    // Post-order DFS returning the longest valid downward path (in nodes)
    // starting at `node`.
    auto dfs = [&](this auto&& self, int node) -> int {
      const auto u = static_cast<std::size_t>(node);
      int longest = 0;
      int second_longest = 0;

      for (const int child : children[u]) {
        // Always recurse so valid paths wholly inside `child`'s subtree still
        // update `global_max`, even if the edge (u, child) itself is blocked.
        const int child_len = self(child);
        if (s[u] == s[static_cast<std::size_t>(child)]) {
          continue;
        }

        // Maintain the top two valid downward arm lengths in O(1) time.
        if (child_len > longest) {
          second_longest = longest;
          longest = child_len;
        } else if (child_len > second_longest) {
          second_longest = child_len;
        }
      }

      // Combine the two best valid arms through `node` as the path apex.
      global_max = std::max(global_max, longest + second_longest + 1);
      return longest + 1;
    };

    dfs(0);
    return global_max;
  }
};

LC_PROBLEM(2246, "Longest Path With Different Adjacent Characters", "dfs",
           ctx) {
  Solution sol;

  ctx.test("example_1_branching_tree", [&] {
    // Longest valid path: 0 ('a') -> 1 ('b') -> 3 ('c') or 2 -> 0 -> 1 -> ...
    // Node 0 ('a') and 2 ('a') share 'a', so edge 0-2 is blocked; best is 3.
    ctx.expect_eq(sol.longestPath({-1, 0, 0, 1, 1, 2}, "abacbe"), 3);
  });

  ctx.test("example_2_star_tree_two_arms", [&] {
    // Path 2 ('b') -> 0 ('a') -> 3 ('c') has length 3.
    ctx.expect_eq(sol.longestPath({-1, 0, 0, 0}, "aabc"), 3);
  });

  ctx.test("single_node", [&] {
    ctx.expect_eq(sol.longestPath({-1}, "z"), 1);
  });

  ctx.test("all_identical_characters", [&] {
    ctx.expect_eq(sol.longestPath({-1, 0, 1, 2}, "aaaa"), 1);
  });

  ctx.test("best_path_inside_disconnected_subtree", [&] {
    // Edge 0-1 is blocked ('a' == 'a'), so DFS must still explore subtree 1 to
    // discover path 2 ('b') -> 1 ('a') -> 3 ('c') of length 3.
    ctx.expect_eq(sol.longestPath({-1, 0, 1, 1}, "aabc"), 3);
  });

  ctx.test("alternating_linear_chain", [&] {
    ctx.expect_eq(sol.longestPath({-1, 0, 1, 2, 3}, "ababa"), 5);
  });
}

}  // namespace
