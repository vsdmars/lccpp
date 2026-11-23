// LeetCode 207: Course Schedule
// https://leetcode.com/problems/course-schedule/
//
// Problem:
//   There are `numCourses` courses labeled from 0 to `numCourses - 1`, and a
//   list of prerequisite pairs `[a, b]` indicating that course `b` must be
//   taken before course `a`. Return true if it is possible to finish all
//   courses, or false if circular prerequisites make completion impossible.
//   Equivalently, determine whether the directed dependency graph is a DAG
//   (contains no directed cycles).
//
// Algorithm:
//   Build a directed adjacency list with edges `course -> prereq` and run a
//   3-state depth-first search (DFS) over every unvisited vertex:
//     - kUnvisited: node has not been explored yet.
//     - kVisiting:  node is currently on the active DFS recursion stack.
//     - kVisited:   node and all reachable dependencies are verified acyclic.
//   Key invariant: encountering a neighbor in state `kVisiting` detects a
//   back-edge to an active ancestor, which exists if and only if the graph has
//   a directed cycle. Nodes marked `kVisited` are memoized and skipped in O(1).
//   Time Complexity:  O(V + E), where V = numCourses and E = prerequisites.size.
//   Space Complexity: O(V + E) for the adjacency list, state array, and DFS
//                     recursion stack (up to O(V) depth).
//
// Data Structures:
//   - `std::vector<std::vector<int>> adj`: directed adjacency list mapping each
//     course to its direct prerequisites.
//   - `std::vector<State> state`: 3-color visitation table (`uint8_t` enum)
//     tracking active recursion ancestors vs. fully verified nodes.
//   - Call stack (recursive lambda `dfs`): tracks the active exploration path.
//
// Real-World Applications:
//   - Build systems (Bazel, Make, CMake) detecting circular target dependencies.
//   - Package managers (npm, Cargo, pip) validating install dependency graphs.
//   - Database deadlock detection via wait-for graph cycle checking.
//   - Spreadsheet formula engines catching circular cell references.
//
// Related LeetCode Problems:
//   - #210 Course Schedule II [done] (returns an explicit valid topological ordering)
//   - #269 Alien Dictionary [done] (infers letter precedence and detects order cycles)
//   - #802 Find Eventual Safe States [done] (finds nodes unable to reach any cycle)
//   - #1462 Course Schedule IV [new] (answers transitive prerequisite reachability)
//   - #2050 Parallel Courses III [new] (longest path / minimum time in a course DAG)

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
