// https://leetcode.com/problems/reconstruct-itinerary/description/
//
// Given airline tickets `[from, to]`, reconstruct the itinerary starting at
// `"JFK"` that uses every ticket exactly once (a directed Eulerian path). If
// multiple valid itineraries exist, return the lexicographically smallest one.

#include "framework/lc_framework.h"

#include <algorithm>
#include <functional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Recursive Hierholzer's algorithm for finding the lexicographically smallest
  // Eulerian path starting at "JFK".
  // Sorting each adjacency list in descending order lets us pop the smallest
  // destination in O(1) from the back of the vector.
  // Time: O(E log E), Space: O(E).
  std::vector<std::string> findItinerary(
      const std::vector<std::vector<std::string>>& tickets) {
    std::unordered_map<std::string, std::vector<std::string>> adj;
    for (const auto& ticket : tickets) {
      adj[ticket[0]].push_back(ticket[1]);
    }
    for (auto& [_, dests] : adj) {
      std::ranges::sort(dests, std::greater<>{});
    }

    std::vector<std::string> route;
    route.reserve(tickets.size() + 1uz);

    auto dfs = [&](this auto&& self, const std::string& airport) -> void {
      auto& dests = adj[airport];
      while (!dests.empty()) {
        std::string next = std::move(dests.back());
        dests.pop_back();
        self(next);
      }
      // Post-order insertion places dead-end subpaths at the end of the route.
      route.push_back(airport);
    };

    dfs("JFK");
    std::ranges::reverse(route);
    return route;
  }

  // Iterative Hierholzer's algorithm using an explicit stack (mirrors review()
  // in 332_Reconstruct_Itinerary.py).
  std::vector<std::string> review(
      const std::vector<std::vector<std::string>>& tickets) {
    std::unordered_map<std::string, std::vector<std::string>> adj;
    for (const auto& ticket : tickets) {
      adj[ticket[0]].push_back(ticket[1]);
    }
    for (auto& [_, dests] : adj) {
      std::ranges::sort(dests, std::greater<>{});
    }

    std::vector<std::string> stack = {"JFK"};
    std::vector<std::string> route;
    route.reserve(tickets.size() + 1uz);

    while (!stack.empty()) {
      auto& dests = adj[stack.back()];
      if (!dests.empty()) {
        stack.push_back(std::move(dests.back()));
        dests.pop_back();
      } else {
        route.push_back(std::move(stack.back()));
        stack.pop_back();
      }
    }

    std::ranges::reverse(route);
    return route;
  }
};

LC_PROBLEM(332, "Reconstruct Itinerary", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: simple linear itinerary", [&] {
    const std::vector<std::vector<std::string>> tickets = {
        {"MUC", "LHR"}, {"JFK", "MUC"}, {"SFO", "SJC"}, {"LHR", "SFO"}};
    const std::vector<std::string> expected = {
        "JFK", "MUC", "LHR", "SFO", "SJC"};
    ctx.expect_eq(sol.findItinerary(tickets), expected);
    ctx.expect_eq(sol.review(tickets), expected);
  });

  ctx.test("example 2: lexical tie-breaking across cycles", [&] {
    const std::vector<std::vector<std::string>> tickets = {
        {"JFK", "SFO"}, {"JFK", "ATL"}, {"SFO", "ATL"},
        {"ATL", "JFK"}, {"ATL", "SFO"}};
    const std::vector<std::string> expected = {
        "JFK", "ATL", "JFK", "SFO", "ATL", "SFO"};
    ctx.expect_eq(sol.findItinerary(tickets), expected);
    ctx.expect_eq(sol.review(tickets), expected);
  });

  ctx.test("dead-end branch smaller lexically must be visited last", [&] {
    const std::vector<std::vector<std::string>> tickets = {
        {"JFK", "KUL"}, {"JFK", "NRT"}, {"NRT", "JFK"}};
    const std::vector<std::string> expected = {"JFK", "NRT", "JFK", "KUL"};
    ctx.expect_eq(sol.findItinerary(tickets), expected);
    ctx.expect_eq(sol.review(tickets), expected);
  });
}

}  // namespace
