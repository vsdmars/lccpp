// https://leetcode.com/problems/evaluate-division/description/
//
// Given equations `A / B = k` and floating-point values `k`, evaluate a list of
// division queries `C / D = ?`. Return `-1.0` when an answer cannot be derived.

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
