// LeetCode 444: Sequence Reconstruction
// https://leetcode.com/problems/sequence-reconstruction/
//
// Problem:
//   Given an integer array `org` representing a permutation of `1..n` and a
//   list of subsequences `seqs`, determine whether `org` is the unique
//   shortest common supersequence that can be reconstructed from `seqs`.
//   Equivalently, every sequence in `seqs` must be a valid subsequence of
//   `org`, and no other permutation of `1..n` may also contain all sequences
//   in `seqs` as subsequences.
//
// Algorithm:
//   Direct position-and-edge verification without constructing an explicit
//   graph. A DAG over `1..n` has `org` as its unique topological order if and
//   only if:
//     1. Every element in `seqs` lies in `1..n`, and every consecutive pair
//        `(a, b)` inside any sequence strictly advances along `org`
//        (`pos[a] < pos[b]`), ruling out inversions and self-loops.
//     2. Every element of `org` appears in at least one sequence.
//     3. Every adjacent pair `(org[i], org[i + 1])` appears directly as a
//        consecutive pair in at least one sequence, guaranteeing a unique
//        Hamiltonian path through the DAG.
//   The implementation first builds an inverse index `pos[val] = index`, then
//   streams through `seqs` once to verify all three conditions.
//   Time: O(n + sum(|seq|)), Space: O(n).
//
// Data Structures:
//   - `std::vector<int> pos`: Maps each value in `1..n` to its 0-based index
//     in `org` for O(1) order lookups.
//   - `Tracker` (`seen` and `matched_edge` `std::vector<std::uint8_t>` arrays
//     plus `seen_count` and `matched_count`): Tracks node coverage and which
//     consecutive edges `(org[i], org[i + 1])` have been witnessed.
//
// Real-World Applications:
//   - Genome and read assembly: verifying whether overlapping DNA/RNA fragments
//     uniquely determine a reference sequence without ambiguous orderings.
//   - Distributed event reconstruction: checking if partial causal logs from
//     independent services define a single unambiguous global timeline.
//   - Build and workflow verification: confirming that partial dependency
//     chains enforce a single deterministic execution order.
//
// Related LeetCode Problems:
//   - #207 Course Schedule [done] (directed cycle detection / topological feasibility)
//   - #210 Course Schedule II [done] (computing a valid topological ordering of a DAG)
//   - #269 Alien Dictionary [done] (inferring a character ordering from precedence
//     constraints)
//   - #1136 Parallel Courses [done] (level-by-level topological sort on a DAG)
//   - #2392 Build a Matrix With Conditions [new] (constructing total orderings from
//     relative precedence pairs)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Validates ordering consistency and Hamiltonian path coverage directly
  // against `org` without building an explicit adjacency list.
  // Time: O(n + sum(|seq|)), Space: O(n).
  bool sequenceReconstruction(const std::vector<int>& org,
                              const std::vector<std::vector<int>>& seqs) {
    const std::size_t n = org.size();
    if (n == 0uz) {
      return false;
    }

    // Map each value in 1..n to its 0-based position in `org`.
    std::vector<int> pos(n + 1uz, -1);
    for (std::size_t i = 0uz; i < n; ++i) {
      if (org[i] < 1 || std::cmp_greater(org[i], n)) {
        return false;
      }
      pos[static_cast<std::size_t>(org[i])] = static_cast<int>(i);
    }

    Tracker tracker(n);
    for (const auto& seq : seqs) {
      if (!process_sequence(seq, pos, tracker)) {
        return false;
      }
    }
    return tracker.seen_count == n && tracker.matched_count == n - 1uz;
  }

 private:
  struct Tracker {
    std::size_t n = 0uz;
    std::vector<std::uint8_t> seen;
    std::vector<std::uint8_t> matched_edge;
    std::size_t seen_count = 0uz;
    std::size_t matched_count = 0uz;

    explicit Tracker(std::size_t size)
        : n(size), seen(size + 1uz, 0), matched_edge(size, 0) {}
  };

  // Verifies one subsequence against `pos` and records seen nodes and matched
  // consecutive edges `(org[k], org[k + 1])`.
  static bool process_sequence(const std::vector<int>& seq,
                               const std::vector<int>& pos, Tracker& tracker) {
    int prev_pos = -1;
    for (const int cur : seq) {
      if (cur < 1 || std::cmp_greater(cur, tracker.n)) {
        return false;
      }
      const auto ucur = static_cast<std::size_t>(cur);
      if (tracker.seen[ucur] == 0) {
        tracker.seen[ucur] = 1;
        ++tracker.seen_count;
      }

      const int cur_pos = pos[ucur];
      if (prev_pos != -1) {
        // Must strictly advance along `org` (rejects inversions & `[1, 1]`).
        if (prev_pos >= cur_pos) {
          return false;
        }
        const auto edge_idx = static_cast<std::size_t>(prev_pos);
        if (prev_pos + 1 == cur_pos && tracker.matched_edge[edge_idx] == 0) {
          tracker.matched_edge[edge_idx] = 1;
          ++tracker.matched_count;
        }
      }
      prev_pos = cur_pos;
    }
    return true;
  }
};

LC_PROBLEM(444, "Sequence Reconstruction", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: ambiguous order [1,2,3] vs [1,3,2]", [&] {
    ctx.expect_false(sol.sequenceReconstruction({1, 2, 3}, {{1, 2}, {1, 3}}));
  });

  ctx.test("example 2: missing element 3", [&] {
    ctx.expect_false(sol.sequenceReconstruction({1, 2, 3}, {{1, 2}}));
  });

  ctx.test("example 3: uniquely reconstructed", [&] {
    ctx.expect_true(
        sol.sequenceReconstruction({1, 2, 3}, {{1, 2}, {1, 3}, {2, 3}}));
  });

  ctx.test("example 4: longer permutation", [&] {
    ctx.expect_true(sol.sequenceReconstruction(
        {4, 1, 5, 2, 6, 3}, {{5, 2, 6, 3}, {4, 1, 5, 2}}));
  });

  ctx.test("edge cases: duplicate self-edge and single element", [&] {
    ctx.expect_false(sol.sequenceReconstruction({1}, {{1, 1}}));
    ctx.expect_false(sol.sequenceReconstruction({1}, {{}}));
    ctx.expect_true(sol.sequenceReconstruction({1}, {{1}}));
  });
}

}  // namespace
