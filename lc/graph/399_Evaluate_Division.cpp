// LeetCode 399: Evaluate Division
// https://leetcode.com/problems/evaluate-division/
//
// Problem:
//   Given a list of variable pairs `equations[i] = [A_i, B_i]` with matching
//   positive real values `values[i]` representing `A_i / B_i = values[i]`,
//   evaluate a list of division queries `queries[j] = [C_j, D_j]`. Return the
//   numeric value of `C_j / D_j` for each query, or `-1.0` if either variable
//   never appeared in `equations` or no ratio path links them. Input equations
//   are guaranteed to be consistent and free of division by zero.
//
// Algorithm:
//   Model the system of ratio equations as a weighted directed graph: each
//   equation `A / B = k` adds edge `A -> B` with weight `k` and reciprocal edge
//   `B -> A` with weight `1.0 / k`. Because ratios telescope along a path
//   (`(A/B) * (B/C) = A/C`), answering `src / dst` reduces to multiplying edge
//   weights along any simple path from `src` to `dst`.
//   For each query:
//     - If `src` or `dst` is absent from `graph`, return `-1.0`.
//     - If `src == dst`, return `1.0`.
//     - Otherwise run DFS from `src` with a per-query `visited` set; when `dst`
//       is reached, unwind the recursion multiplying each edge weight along the
//       path, or return `-1.0` if `dst` is unreachable.
//   Time Complexity:  O(E + Q * (V + E)), where E = equations.size(), V is the
//                     number of distinct variables, and Q = queries.size().
//   Space Complexity: O(V + E) for the weighted graph, `visited` set, and DFS
//                     recursion stack.
//
// Data Structures:
//   - `std::unordered_map<std::string, std::vector<std::pair<std::string,
//     double>>> graph`: weighted adjacency list mapping each variable to its
//     `(neighbor, ratio)` edges.
//   - `std::unordered_set<std::string> visited`: per-query set preventing
//     cycles during DFS path search.
//   - `std::vector<double> results`: pre-reserved output vector storing answers.
//
// Real-World Applications:
//   - Currency exchange and foreign-exchange (FX) cross-rate conversion across
//     quoted currency pairs.
//   - Unit conversion engines (length, mass, energy, custom domain units)
//     deriving indirect conversion factors from base ratios.
//   - Pricing, recipe scaling, and bill-of-materials ratio propagation across
//     linked commodities.
//
// Related LeetCode Problems:
//   - #2307 Check for Contradictions in Equations [new] (validates consistency of
//     ratio equations on a weighted graph)
//   - #990 Satisfiability of Equality Equations [new] (checks transitive equality
//     and inequality constraints across variables)
//   - #721 Accounts Merge [done] (groups transitively linked string-keyed entities)
//   - #547 Number of Provinces [done] (reachability and connected components)
//   - #1631 Path With Minimum Effort [new] (weighted graph path queries)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Models equations as a weighted directed graph: `u -> v` with weight `k` and
  // `v -> u` with weight `1 / k`. Each query is answered by multiplying edge
  // weights along a simple DFS path from numerator to denominator.
  // Time: O(Q * (V + E)), Space: O(V + E).
  std::vector<double> calcEquation(
      const std::vector<std::vector<std::string>>& equations,
      const std::vector<double>& values,
      const std::vector<std::vector<std::string>>& queries) {
    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>>
        graph;

    for (std::size_t i = 0uz; i < equations.size(); ++i) {
      const std::string& num = equations[i][0];
      const std::string& den = equations[i][1];
      const double val = values[i];
      graph[num].emplace_back(den, val);
      graph[den].emplace_back(num, 1.0 / val);
    }

    std::vector<double> results;
    results.reserve(queries.size());

    for (const auto& query : queries) {
      const std::string& src = query[0];
      const std::string& dst = query[1];

      if (!graph.contains(src) || !graph.contains(dst)) {
        results.push_back(-1.0);
        continue;
      }
      if (src == dst) {
        results.push_back(1.0);
        continue;
      }

      std::unordered_set<std::string> visited;
      visited.insert(src);

      auto dfs = [&](this auto&& self, const std::string& cur) -> double {
        for (const auto& [next, weight] : graph[cur]) {
          if (!visited.insert(next).second) {
            continue;
          }
          if (next == dst) {
            return weight;
          }
          const double sub = self(next);
          if (sub != -1.0) {
            return weight * sub;
          }
        }
        return -1.0;
      };

      results.push_back(dfs(src));
    }

    return results;
  }
};

bool vectors_near(const std::vector<double>& a, const std::vector<double>& b,
                  double eps = 1e-6) {
  if (a.size() != b.size()) {
    return false;
  }
  for (std::size_t i = 0uz; i < a.size(); ++i) {
    if (std::fabs(a[i] - b[i]) > eps) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(399, "Evaluate Division", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: chain equations with unknown variables", [&] {
    const std::vector<std::vector<std::string>> equations = {{"a", "b"},
                                                             {"b", "c"}};
    const std::vector<double> values = {2.0, 3.0};
    const std::vector<std::vector<std::string>> queries = {
        {"a", "c"}, {"b", "a"}, {"a", "e"}, {"a", "a"}, {"x", "x"}};
    const std::vector<double> expected = {6.0, 0.5, -1.0, 1.0, -1.0};
    ctx.expect_true(
        vectors_near(sol.calcEquation(equations, values, queries), expected));
  });

  ctx.test("disconnected components return -1.0 across components", [&] {
    const std::vector<std::vector<std::string>> equations = {{"a", "b"},
                                                             {"c", "d"}};
    const std::vector<double> values = {2.0, 4.0};
    const std::vector<std::vector<std::string>> queries = {
        {"a", "d"}, {"b", "a"}, {"d", "c"}};
    const std::vector<double> expected = {-1.0, 0.5, 0.25};
    ctx.expect_true(
        vectors_near(sol.calcEquation(equations, values, queries), expected));
  });
}

}  // namespace
