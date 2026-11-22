// https://leetcode.com/problems/course-schedule-ii/description/
//
// Given `numCourses` courses labeled 0..numCourses-1 and prerequisite pairs
// `[a, b]` meaning course `b` must be taken before course `a`, return any valid
// ordering to finish all courses, or an empty vector if impossible.

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Topological sort via post-order 3-state DFS.
  // Because edges point `course -> prereq`, appending a node after visiting all
  // of its prerequisites naturally yields a valid course order without reversal.
  // Time: O(V + E), Space: O(V + E).
  std::vector<int> findOrder(
      int numCourses, const std::vector<std::vector<int>>& prerequisites) {
    if (numCourses <= 0) {
      return {};
    }

    const auto n = static_cast<std::size_t>(numCourses);
    std::vector<std::vector<int>> adj(n);
    for (const auto& edge : prerequisites) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
    }

    enum class State : std::uint8_t { kUnvisited, kVisiting, kVisited };
    std::vector<State> state(n, State::kUnvisited);
    std::vector<int> order;
    order.reserve(n);

    auto dfs = [&](this auto&& self, int course) -> bool {
      const auto idx = static_cast<std::size_t>(course);
      if (state[idx] == State::kVisiting) {
        return false;  // Cycle detected.
      }
      if (state[idx] == State::kVisited) {
        return true;
      }

      state[idx] = State::kVisiting;
      for (const int prereq : adj[idx]) {
        if (!self(prereq)) {
          return false;
        }
      }
      state[idx] = State::kVisited;
      order.push_back(course);
      return true;
    };

    for (int c = 0; c < numCourses; ++c) {
      if (state[static_cast<std::size_t>(c)] == State::kUnvisited && !dfs(c)) {
        return {};
      }
    }
    return order;
  }
};

// Verifies that `order` contains each course 0..n-1 once and respects all
// prerequisite pairs `[course, prereq]`.
bool is_valid_order(int num_courses,
                    const std::vector<std::vector<int>>& prerequisites,
                    const std::vector<int>& order) {
  const auto n = static_cast<std::size_t>(num_courses);
  if (order.size() != n) {
    return false;
  }
  std::vector<int> pos(n, -1);
  for (std::size_t i = 0uz; i < n; ++i) {
    if (order[i] < 0 || order[i] >= num_courses) {
      return false;
    }
    const auto course = static_cast<std::size_t>(order[i]);
    if (pos[course] != -1) {
      return false;
    }
    pos[course] = static_cast<int>(i);
  }
  for (const auto& edge : prerequisites) {
    const auto course = static_cast<std::size_t>(edge[0]);
    const auto prereq = static_cast<std::size_t>(edge[1]);
    if (pos[prereq] >= pos[course]) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(210, "Course Schedule II", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: two courses", [&] {
    const std::vector<std::vector<int>> prereqs = {{1, 0}};
    const auto order = sol.findOrder(2, prereqs);
    ctx.expect_eq(order, std::vector<int>{0, 1});
  });

  ctx.test("example 2: diamond DAG", [&] {
    const std::vector<std::vector<int>> prereqs = {
        {1, 0}, {2, 0}, {3, 1}, {3, 2}};
    const auto order = sol.findOrder(4, prereqs);
    ctx.expect_true(is_valid_order(4, prereqs, order),
                    "returned ordering must satisfy all prerequisites");
  });

  ctx.test("cycle returns empty order", [&] {
    ctx.expect_eq(sol.findOrder(2, {{1, 0}, {0, 1}}), std::vector<int>{});
  });

  ctx.test("single course without prerequisites", [&] {
    ctx.expect_eq(sol.findOrder(1, {}), std::vector<int>{0});
  });
}

}  // namespace
