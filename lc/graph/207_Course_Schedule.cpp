// https://leetcode.com/problems/course-schedule/description/
//
// There are `numCourses` courses labeled from 0 to `numCourses - 1`. Each entry
// `prerequisites[i] = [a, b]` means you must take course `b` before course `a`.
// Return true if it is possible to finish all courses (i.e. the dependency
// graph is a DAG with no directed cycles).

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Detects whether the course prerequisite graph is acyclic using 3-state DFS.
  // Time: O(V + E), Space: O(V + E).
  bool canFinish(int numCourses,
                 const std::vector<std::vector<int>>& prerequisites) {
    if (numCourses <= 0) {
      return true;
    }

    const auto n = static_cast<std::size_t>(numCourses);
    // Edge `course -> prereq`: taking `course` depends on finishing `prereq`.
    std::vector<std::vector<int>> adj(n);
    for (const auto& edge : prerequisites) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
    }

    enum class State : std::uint8_t { kUnvisited, kVisiting, kVisited };
    std::vector<State> state(n, State::kUnvisited);

    // Returns false if a cycle is reachable from `course`.
    auto dfs = [&](this auto&& self, int course) -> bool {
      const auto idx = static_cast<std::size_t>(course);
      if (state[idx] == State::kVisiting) {
        return false;  // Back-edge to an active ancestor indicates a cycle.
      }
      if (state[idx] == State::kVisited) {
        return true;  // Already verified acyclic.
      }

      state[idx] = State::kVisiting;
      for (const int prereq : adj[idx]) {
        if (!self(prereq)) {
          return false;
        }
      }
      state[idx] = State::kVisited;
      return true;
    };

    for (int c = 0; c < numCourses; ++c) {
      if (state[static_cast<std::size_t>(c)] == State::kUnvisited && !dfs(c)) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(207, "Course Schedule", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: simple linear prerequisite", [&] {
    ctx.expect_true(sol.canFinish(2, {{1, 0}}));
    ctx.expect_true(sol.canFinish(2, {{0, 1}}));
  });

  ctx.test("example 2: direct 2-cycle", [&] {
    ctx.expect_false(sol.canFinish(2, {{1, 0}, {0, 1}}));
  });

  ctx.test("diamond DAG with no cycle", [&] {
    ctx.expect_true(sol.canFinish(4, {{1, 0}, {2, 0}, {3, 1}, {3, 2}}));
  });

  ctx.test("disconnected graph containing a cycle", [&] {
    ctx.expect_false(sol.canFinish(5, {{1, 0}, {2, 3}, {3, 4}, {4, 2}}));
  });

  ctx.test("self-loop and empty prerequisites", [&] {
    ctx.expect_false(sol.canFinish(1, {{0, 0}}));
    ctx.expect_true(sol.canFinish(3, {}));
  });
}

}  // namespace
