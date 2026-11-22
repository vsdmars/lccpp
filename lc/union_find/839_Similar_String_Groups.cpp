// https://leetcode.com/problems/similar-string-groups/
//
// Two strings X and Y are similar if they are equal or if swapping two letters
// at different positions in X yields Y. Given a list of strings `strs` where
// every string is an anagram of every other string, return the number of
// connected groups formed by this similarity relation.

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
