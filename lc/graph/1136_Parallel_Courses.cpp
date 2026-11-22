// https://leetcode.com/problems/parallel-courses/
//
// There are `n` courses labeled 1..n and directed prerequisite pairs
// `[prevCourse, nextCourse]`. Each semester you may take any number of courses
// whose prerequisites were all completed in earlier semesters. Return the
// minimum number of semesters needed to finish all courses, or -1 if a cycle
// makes it impossible.

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
