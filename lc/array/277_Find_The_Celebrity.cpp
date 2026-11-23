// LeetCode 277: Find The Celebrity
// https://leetcode.com/problems/find-the-celebrity/
//
// Problem:
//   At a party of `n` people labeled `0` through `n - 1`, there may be at most
//   one celebrity. A celebrity is defined as a person whom all other `n - 1`
//   people know, while the celebrity knows none of the other `n - 1` people.
//   Given only an oracle query `knows(a, b)` returning whether person `a` knows
//   person `b`, identify the celebrity's label or return `-1` if none exists,
//   minimizing the number of `knows` calls.
//
// Algorithm:
//   Two-phase linear elimination and verification (Universal Sink in a Directed
//   Graph):
//   1. Candidate Elimination Pass: start with `candidate = 0` and iterate `i`
//      from `1` to `n - 1`. Ask `knows(candidate, i)`:
//      - If true, `candidate` knows someone and therefore CANNOT be a celebrity;
//        replace `candidate = i`.
//      - If false, `i` is not known by `candidate` and therefore `i` CANNOT be
//        a celebrity; keep `candidate`.
//      Every query eliminates exactly one person, leaving at most one viable
//      candidate after `n - 1` queries.
//   2. Verification Pass: confirm that for every `i != candidate`,
//      `knows(i, candidate)` is true and `knows(candidate, i)` is false. If any
//      check fails, no celebrity exists and we return `-1`.
//   Time Complexity:  O(n) — at most `3 * (n - 1)` calls to `knows`.
//   Space Complexity: O(1) extra space.
//
// Data Structures:
//   - Scalar integer `candidate`: tracks the sole surviving universal-sink
//     candidate during elimination.
//   - `Party` mock API (test harness): wraps an adjacency matrix representing
//     the directed "knows" graph and counts oracle invocations.
//
// Real-World Applications:
//   - Finding a universal sink (e.g., a root collector, master coordinator, or
//     terminal convergence node) in an implicit directed graph without
//     materializing all O(n^2) edges.
//   - Social-graph authority / hub detection under rate-limited API queries.
//   - Distributed leader verification where pairwise trust/reachability probes
//     are expensive network RPCs.
//
// Related LeetCode Problems:
//   - #997 Find the Town Judge [new] (explicit edge-list version of the universal sink problem)
//   - #169 Majority Element [done] (Boyer-Moore linear candidate elimination + verification)
//   - #278 First Bad Version [done] (interactive oracle query minimization)
//   - #1557 Minimum Number of Vertices to Reach All Nodes [new] (directed graph in-degree analysis)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Mock judge API for the interactive `knows(a, b)` oracle, tracking call count
// so tests can verify O(n) query complexity in addition to correctness.
class Party {
 public:
  explicit Party(std::vector<std::vector<int>> adjacency)
      : adjacency_(std::move(adjacency)) {}

  // Returns true if person `a` knows person `b`.
  [[nodiscard]] bool knows(int a, int b) {
    ++call_count_;
    return adjacency_[static_cast<std::size_t>(a)]
                     [static_cast<std::size_t>(b)] != 0;
  }

  [[nodiscard]] int call_count() const { return call_count_; }

 private:
  std::vector<std::vector<int>> adjacency_;
  int call_count_ = 0;
};

class Solution {
 public:
  explicit Solution(Party& party) : party_(party) {}

  // Identifies the unique celebrity among `n` people labeled `[0, n - 1]`, or
  // returns `-1` if no celebrity exists.
  //
  // Invariant (Pass 1):
  //   After inspecting person `i`, no person in `[0, i] \ {candidate}` can be
  //   a celebrity because each eliminated person either knew someone or was
  //   unknown to someone.
  //
  // Time Complexity:  O(n) (<= 3 * (n - 1) oracle queries)
  // Space Complexity: O(1)
  [[nodiscard]] int findCelebrity(int n) const {
    if (n <= 0) {
      return -1;
    }

    // Pass 1: Eliminate n - 1 impossible candidates in n - 1 queries.
    int candidate = 0;
    for (int i = 1; i < n; ++i) {
      if (party_.knows(candidate, i)) {
        // `candidate` knows `i`, so `candidate` cannot be a celebrity.
        candidate = i;
      }
      // Otherwise `candidate` does not know `i`, so `i` cannot be a celebrity.
    }

    // Pass 2: Verify the lone surviving candidate actually satisfies both
    // celebrity properties against every other person.
    for (int i = 0; i < n; ++i) {
      if (i == candidate) {
        continue;
      }
      if (!party_.knows(i, candidate) || party_.knows(candidate, i)) {
        return -1;
      }
    }

    return candidate;
  }

 private:
  Party& party_;
};

LC_PROBLEM(277, "Find The Celebrity", "array", ctx) {
  ctx.test("python_driver_celebrity_is_3", [&] {
    // 5 people where 0, 1, 2, 4 all know 3, and 3 knows nobody else.
    Party party({
        {1, 0, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 1},
    });
    const Solution sol(party);
    ctx.expect_eq(sol.findCelebrity(5), 3);
    ctx.expect_true(party.call_count() <= 3 * (5 - 1));
  });

  ctx.test("leetcode_example_1_celebrity_is_1", [&] {
    Party party({
        {1, 1, 0},
        {0, 1, 0},
        {1, 1, 1},
    });
    const Solution sol(party);
    ctx.expect_eq(sol.findCelebrity(3), 1);
    ctx.expect_true(party.call_count() <= 3 * (3 - 1));
  });

  ctx.test("leetcode_example_2_no_celebrity_cycle", [&] {
    Party party({
        {1, 0, 1},
        {1, 1, 0},
        {0, 1, 1},
    });
    const Solution sol(party);
    ctx.expect_eq(sol.findCelebrity(3), -1);
  });

  ctx.test("no_celebrity_candidate_knows_earlier_person", [&] {
    // Person 2 is known by 0 and 1, but 2 also knows 0 -> not a celebrity.
    Party party({
        {1, 0, 1},
        {0, 1, 1},
        {1, 0, 1},
    });
    const Solution sol(party);
    ctx.expect_eq(sol.findCelebrity(3), -1);
  });

  ctx.test("celebrity_at_index_0", [&] {
    Party party({
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 1, 1},
        {1, 1, 0, 1},
    });
    const Solution sol(party);
    ctx.expect_eq(sol.findCelebrity(4), 0);
  });

  ctx.test("single_person_party", [&] {
    Party party({{1}});
    const Solution sol(party);
    ctx.expect_eq(sol.findCelebrity(1), 0);
  });
}

}  // namespace
