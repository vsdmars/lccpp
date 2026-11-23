// LeetCode 839: Similar String Groups
// https://leetcode.com/problems/similar-string-groups/
//
// Problem:
//   Two strings X and Y are similar if they are identical or if swapping two
//   characters at different positions in X produces Y. Given an array `strs`
//   where every string is an anagram of every other string (so all strings
//   share the same length L), partition `strs` into equivalence groups under
//   the transitive closure of similarity and return the number of groups.
//
// Algorithm:
//   1. Initialize an index-based Disjoint Set Union (`DisjointSet`) with `n`
//      singleton groups (`n = strs.size()`). Indexing by position handles
//      duplicate strings in `strs` naturally without extra deduplication.
//   2. Iterate over every unordered pair `(i, j)` with `0 <= i < j < n`.
//      Skip the O(L) character comparison whenever `i` and `j` already share a
//      root in the DSU.
//   3. Otherwise, compare `strs[i]` and `strs[j]` via `are_similar`: because
//      all inputs are guaranteed anagrams, two strings are equal or differ by a
//      single swap if and only if they disagree at exactly 0 or 2 indices
//      (returning `false` early once more than 2 mismatches are seen).
//   4. Unite similar pairs (decrementing the group count on each merge) and
//      return the remaining number of groups.
//   Time Complexity:
//     O(N^2 * L) worst case, where N = `strs.size()` and L = `strs[0].size()`,
//     with practical speedup from skipping already-connected pairs and early
//     exit on > 2 mismatches.
//   Space Complexity:
//     O(N) for the DSU parent and rank arrays.
//
// Data Structures:
//   - `DisjointSet` (`parent_`: `std::vector<std::size_t>`, `rank_`:
//     `std::vector<int>`, `groups_`: `std::size_t`): Index-based Union-Find
//     with iterative path halving and union by rank that tracks the live count
//     of connected components.
//   - `std::string_view`: Non-owning string slices passed to `are_similar` to
//     avoid copying during pairwise scans.
//
// Real-World Applications:
//   - Typo-tolerant clustering and near-duplicate text grouping (e.g.,
//     Damerau-Levenshtein single-transposition equivalence classes).
//   - Bioinformatics sequence clustering (grouping DNA/protein reads separated
//     by single point mutations or inversions).
//   - Data cleaning and fuzzy record linkage in ETL pipelines.
//
// Related LeetCode Problems:
//   - #854 K-Similar Strings [new] (minimum swaps to transform one anagram into
//     another)
//   - #547 Number of Provinces [done] (counting connected components with Union-Find)
//   - #1202 Smallest String With Swaps [done] (grouping swappable string indices via
//     Union-Find)
//   - #721 Accounts Merge [done] (clustering string records into connected components)
//   - #2157 Groups of Strings [new] (grouping strings connected by single-character
//     edits via Union-Find)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class DisjointSet {
 public:
  explicit DisjointSet(std::size_t n) : parent_(n), rank_(n, 0), groups_(n) {
    for (std::size_t i = 0uz; i < n; ++i) {
      parent_[i] = i;
    }
  }

  std::size_t find(std::size_t x) {
    while (parent_[x] != x) {
      parent_[x] = parent_[parent_[x]];
      x = parent_[x];
    }
    return x;
  }

  void unite(std::size_t a, std::size_t b) {
    std::size_t root_a = find(a);
    std::size_t root_b = find(b);
    if (root_a == root_b) {
      return;
    }
    if (rank_[root_a] < rank_[root_b]) {
      std::swap(root_a, root_b);
    }
    parent_[root_b] = root_a;
    if (rank_[root_a] == rank_[root_b]) {
      ++rank_[root_a];
    }
    --groups_;
  }

  [[nodiscard]] std::size_t groups() const { return groups_; }

 private:
  std::vector<std::size_t> parent_;
  std::vector<int> rank_;
  std::size_t groups_;
};

class Solution {
 public:
  // Index-based Union-Find (using indices rather than strings handles duplicate
  // strings cleanly). Skip string comparison whenever two indices already share
  // a component.
  // Time: O(N^2 * L) worst case, where N = strs.size() and L = strs[0].size().
  // Space: O(N)
  int numSimilarGroups(const std::vector<std::string>& strs) {
    const std::size_t n = strs.size();
    DisjointSet dsu(n);

    for (std::size_t i = 0uz; i < n; ++i) {
      for (std::size_t j = i + 1uz; j < n; ++j) {
        if (dsu.find(i) != dsu.find(j) && are_similar(strs[i], strs[j])) {
          dsu.unite(i, j);
        }
      }
    }
    return static_cast<int>(dsu.groups());
  }

 private:
  // Since all inputs are anagrams, differing at 0 or 2 positions guarantees
  // they are equal or differ by a single swap.
  static bool are_similar(std::string_view lhs, std::string_view rhs) {
    int diff = 0;
    for (std::size_t i = 0uz; i < lhs.size(); ++i) {
      if (lhs[i] != rhs[i]) {
        ++diff;
        if (diff > 2) {
          return false;
        }
      }
    }
    return diff == 0 || diff == 2;
  }
};

LC_PROBLEM(839, "Similar String Groups", "union_find", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"tars", "rats", "arts", "star"};
    ctx.expect_eq(sol.numSimilarGroups(strs), 2);
  });

  ctx.test("with_duplicate_strings", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"tars", "rats", "arts", "star",
                                           "star"};
    ctx.expect_eq(sol.numSimilarGroups(strs), 2);
  });

  ctx.test("example_2_single_group", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"omv", "ovm"};
    ctx.expect_eq(sol.numSimilarGroups(strs), 1);
  });

  ctx.test("single_word", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"abc"};
    ctx.expect_eq(sol.numSimilarGroups(strs), 1);
  });

  ctx.test("cyclic_rotations_all_disjoint", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"abc", "bca", "cab"};
    ctx.expect_eq(sol.numSimilarGroups(strs), 3);
  });
}

}  // namespace
