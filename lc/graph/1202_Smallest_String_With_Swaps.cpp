// LeetCode 1202: Smallest String With Swaps
// https://leetcode.com/problems/smallest-string-with-swaps/
//
// Problem:
//   Given a lowercase string `s` of length `N` and a list of index pairs
//   `pairs`, where each pair `[a, b]` allows swapping `s[a]` and `s[b]` any
//   number of times, return the lexicographically smallest string that can be
//   obtained after any sequence of allowed swaps.
//
// Examples:
//   Example 1:
//     Input:  s = "dcab", pairs = [[0, 3], [1, 2]]
//     Output: "bacd"
//     Explanation:
//       - Build undirected adjacency lists of size 4 with edges 0 <-> 3 and 1 <->
//         2.
//       - At i = 0, DFS visits component {0, 3}, collecting comp_indices = [0, 3]
//         and comp_chars = "db"; sorting both to [0, 3] and "bd" sets s[0] = 'b'
//         and s[3] = 'd' (s becomes "bcad").
//       - At i = 1, DFS visits component {1, 2}, collecting comp_indices = [1, 2]
//         and comp_chars = "ca"; sorting to [1, 2] and "ac" sets s[1] = 'a' and
//         s[2] = 'c'.
//       - Skip already visited indices 2 and 3 and return "bacd".
//
//   Example 2:
//     Input:  s = "dcab", pairs = [[0, 3], [1, 2], [0, 2]]
//     Output: "abcd"
//     Explanation:
//       - Build undirected adjacency lists with edges 0 <-> 3, 1 <-> 2, and 0 <->
//         2, connecting all four indices into one component.
//       - At i = 0, DFS traverses 0 -> 3 -> 2 -> 1, collecting comp_indices = [0,
//         3, 2, 1] and comp_chars = "dbac".
//       - Sort comp_indices to [0, 1, 2, 3] and comp_chars to "abcd", write s[0..3]
//         = "abcd", skip visited indices 1..3, and return "abcd".
//
//   Example 3:
//     Input:  s = "dcba", pairs = []
//     Output: "dcba"
//     Explanation:
//       - Construct adj of size 4 with no edges so adj[i].empty() is true for all i
//         in 0..3.
//       - Every iteration of the outer loop hits the skip condition adj[i].empty(),
//         leaving s unchanged and returning "dcba".
//
// Algorithm:
//   Connected-components DFS with greedy per-component sorting. Treat string
//   indices `0..N-1` as vertices and each pair `[a, b]` as an undirected edge.
//   Because swaps are transitive and unlimited, any connected component can
//   realize every permutation of its characters across its member indices.
//   Therefore, the globally lexicographically smallest string is achieved by
//   independently sorting each component's indices in ascending order and its
//   characters in ascending order, then placing the smallest characters at the
//   earliest indices. Isolated indices (`adj[i].empty()`) are skipped directly.
//   Time: O(N log N + E), Space: O(N + E), where N = s.size() and
//   E = pairs.size().
//
// Data Structures:
//   - `std::vector<std::vector<std::size_t>> adj`: undirected adjacency list
//     representing allowed index swaps.
//   - `std::vector<std::uint8_t> visited`: byte-packed visited flags tracking
//     already-processed vertices during DFS.
//   - `std::vector<std::size_t> comp_indices` and `std::string comp_chars`:
//     reusable buffers collecting the indices and characters of each connected
//     component before sorting and writing back into `s`.
//
// Real-World Applications:
//   - Equivalence-class canonicalization in record deduplication, alias
//     resolution, and entity-merging pipelines where linked records share a
//     canonical representative.
//   - Memory/storage defragmentation and network routing optimization where
//     items within connected swap domains can be reordered into optimal layout.
//   - Symmetry breaking and canonical form generation in combinatorial solvers.
//
// Related LeetCode Problems:
//   - #1722 Minimize Hamming Distance After Swap Operations [new] (same connected-
//     component swappability graph, greedily matching character multisets)
//   - #1998 GCD Sort of an Array [new] (swappability graph induced by shared prime
//     factors, checking whether sorting each component sorts the whole array)
//   - #547 Number of Provinces [done] (foundational undirected connected-components
//     traversal via DFS or Union-Find)
//   - #721 Accounts Merge [done] (groups transitively connected accounts into
//     components and sorts elements within each component)
//   - #2948 Make Lexicographically Smallest Array by Swapping Elements [new] (groups
//     transitively swappable elements within threshold `limit` and sorts each)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Swappability is transitive, so indices within each connected component can
  // be arbitrarily permuted. Collect each component via DFS, sort its indices
  // and characters in ascending order, and place characters back greedily.
  // Time: O(N log N + E), Space: O(N + E).
  std::string smallestStringWithSwaps(
      std::string s, const std::vector<std::vector<int>>& pairs) {
    const std::size_t n = s.size();
    std::vector<std::vector<std::size_t>> adj(n);
    for (const auto& p : pairs) {
      const auto u = static_cast<std::size_t>(p[0]);
      const auto v = static_cast<std::size_t>(p[1]);
      adj[u].push_back(v);
      adj[v].push_back(u);
    }

    std::vector<std::uint8_t> visited(n, 0);
    std::vector<std::size_t> comp_indices;
    std::string comp_chars;

    auto dfs = [&](this auto&& self, std::size_t u) -> void {
      visited[u] = 1;
      comp_indices.push_back(u);
      comp_chars.push_back(s[u]);
      for (const std::size_t v : adj[u]) {
        if (visited[v] == 0) {
          self(v);
        }
      }
    };

    for (std::size_t i = 0uz; i < n; ++i) {
      if (visited[i] != 0 || adj[i].empty()) {
        continue;
      }
      comp_indices.clear();
      comp_chars.clear();
      dfs(i);

      std::ranges::sort(comp_indices);
      std::ranges::sort(comp_chars);
      for (std::size_t k = 0uz; k < comp_indices.size(); ++k) {
        s[comp_indices[k]] = comp_chars[k];
      }
    }

    return s;
  }
};

LC_PROBLEM(1202, "Smallest String With Swaps", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: two disjoint swap pairs", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("dcab", {{0, 3}, {1, 2}}),
                  "bacd");
  });

  ctx.test("example 2: single connected component sorts entire string", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("dcab", {{0, 3}, {1, 2}, {0, 2}}),
                  "abcd");
  });

  ctx.test("example 3: chain swaps", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("cba", {{0, 1}, {1, 2}}), "abc");
  });

  ctx.test("no swap pairs leaves string unchanged", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("dcba", {}), "dcba");
  });
}

}  // namespace
