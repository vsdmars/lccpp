// LeetCode 1136: Parallel Courses
// https://leetcode.com/problems/parallel-courses/
//
// Problem:
//   There are `n` courses labeled `1..n` and a list of directed prerequisite
//   pairs `[prevCourse, nextCourse]`, meaning `prevCourse` must be completed
//   in an earlier semester than `nextCourse`. In a single semester you may
//   take any number of courses whose prerequisites have all been finished.
//   Return the minimum number of semesters required to complete all `n`
//   courses, or `-1` if a prerequisite cycle makes completion impossible.
//
// Algorithm:
//   Level-order Kahn's BFS topological sort. Build the directed adjacency list
//   and indegree count for each course, then initialize `frontier` with all
//   zero-indegree courses (those eligible in semester 1). Each BFS wave
//   processes the entire current `frontier` in parallel, increments
//   `semesters`, tallies `completed`, and decrements the indegree of each
//   dependent course—pushing newly unlocked zero-indegree courses into
//   `next_frontier` for the following semester. The wave count equals the
//   longest directed path (critical path) in a DAG; if `completed < n`, a
//   cycle prevented remaining nodes from reaching indegree zero, so return -1.
//   Time: O(V + E), Space: O(V + E), where V = n and E = relations.size().
//
// Data Structures:
//   - `std::vector<std::vector<int>> adj`: 1-indexed adjacency list mapping
//     each prerequisite course to its dependent courses.
//   - `std::vector<int> indegree`: 1-indexed count of remaining unsatisfied
//     prerequisites for each course.
//   - `std::vector<int> frontier`, `next_frontier`: double-buffered BFS level
//     queues separating the current semester from the next.
//
// Real-World Applications:
//   - Build systems (Bazel, Ninja, Make) computing the minimum parallel build
//     depth and scheduling independent compilation targets concurrently.
//   - CI/CD pipeline and workflow DAG schedulers (Airflow, Dagster) executing
//     all ready tasks per stage while detecting circular dependencies.
//   - Critical-path analysis in project management and hardware instruction
//     scheduling on parallel execution units.
//
// Related LeetCode Problems:
//   - #207 Course Schedule [done] (detects whether a prerequisite graph is acyclic)
//   - #210 Course Schedule II [done] (returns a valid topological ordering of courses)
//   - #1494 Parallel Courses II [new] (adds a per-semester limit `k`, turning greedy
//     BFS into bitmask dynamic programming)
//   - #2050 Parallel Courses III [new] (extends unit semesters to weighted course
//     durations via DAG longest-path DP)
//   - #329 Longest Increasing Path in a Matrix [done] (computes the longest chain
//     depth in an implicit DAG via topological layering)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Level-order Kahn's BFS topological sort: each BFS frontier represents one
  // semester of courses whose prerequisites have all been satisfied.
  // Time: O(V + E), Space: O(V + E).
  int minimumSemesters(int n, const std::vector<std::vector<int>>& relations) {
    const auto size = static_cast<std::size_t>(n) + 1uz;
    std::vector<std::vector<int>> adj(size);
    std::vector<int> indegree(size, 0);

    for (const auto& rel : relations) {
      adj[static_cast<std::size_t>(rel[0])].push_back(rel[1]);
      ++indegree[static_cast<std::size_t>(rel[1])];
    }

    // Seed the first semester with all courses that have zero prerequisites.
    std::vector<int> frontier;
    for (int course = 1; course <= n; ++course) {
      if (indegree[static_cast<std::size_t>(course)] == 0) {
        frontier.push_back(course);
      }
    }

    int semesters = 0;
    int completed = 0;

    while (!frontier.empty()) {
      ++semesters;
      std::vector<int> next_frontier;
      for (const int course : frontier) {
        ++completed;
        for (const int next : adj[static_cast<std::size_t>(course)]) {
          if (--indegree[static_cast<std::size_t>(next)] == 0) {
            next_frontier.push_back(next);
          }
        }
      }
      frontier = std::move(next_frontier);
    }

    return completed == n ? semesters : -1;
  }
};

LC_PROBLEM(1136, "Parallel Courses", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: two prerequisites in parallel then final course", [&] {
    ctx.expect_eq(sol.minimumSemesters(3, {{1, 3}, {2, 3}}), 2);
  });

  ctx.test("example 2: cycle returns -1", [&] {
    ctx.expect_eq(sol.minimumSemesters(3, {{1, 2}, {2, 3}, {3, 1}}), -1);
  });

  ctx.test("longest chain determines semester count", [&] {
    ctx.expect_eq(sol.minimumSemesters(4, {{1, 2}, {2, 3}, {3, 4}}), 4);
  });
}

}  // namespace
