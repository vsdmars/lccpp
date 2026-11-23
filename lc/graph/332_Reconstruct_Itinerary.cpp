// LeetCode 332: Reconstruct Itinerary
// https://leetcode.com/problems/reconstruct-itinerary/
//
// Problem:
//   Given a list of airline tickets `tickets[i] = [from, to]` representing
//   directed flights between 3-letter airport codes, reconstruct the itinerary
//   starting at `"JFK"` that uses every ticket exactly once (a directed
//   Eulerian path). At least one valid itinerary is guaranteed to exist; if
//   multiple valid itineraries exist, return the lexicographically smallest one
//   when read as a sequence of airport codes.
//
// Examples:
//   Example 1:
//     Input:  tickets = [["MUC", "LHR"], ["JFK", "MUC"], ["SFO", "SJC"], ["LHR",
//             "SFO"]]
//     Output: ["JFK", "MUC", "LHR", "SFO", "SJC"]
//     Explanation:
//       - Builds adj with descending-sorted destinations: JFK:["MUC"], MUC:["LHR"],
//         LHR:["SFO"], SFO:["SJC"].
//       - dfs("JFK") follows the unique outgoing edges through "MUC", "LHR", and
//         "SFO" down to "SJC", which has no outgoing edges.
//       - Post-order unwinding pushes "SJC", "SFO", "LHR", "MUC", and "JFK" onto
//         route, and reversing route yields ["JFK", "MUC", "LHR", "SFO", "SJC"].
//
//   Example 2:
//     Input:  tickets = [["JFK", "SFO"], ["JFK", "ATL"], ["SFO", "ATL"], ["ATL",
//             "JFK"], ["ATL", "SFO"]]
//     Output: ["JFK", "ATL", "JFK", "SFO", "ATL", "SFO"]
//     Explanation:
//       - Builds adj sorted descending: JFK:["SFO", "ATL"], ATL:["SFO", "JFK"],
//         SFO:["ATL"].
//       - Popping from the back of each vector visits the lexically smallest
//         remaining destination at each step: "JFK" -> "ATL" -> "JFK" -> "SFO" ->
//         "ATL" -> "SFO".
//       - At the second "SFO" no edges remain, so post-order unwinding pushes
//         ["SFO", "ATL", "SFO", "JFK", "ATL", "JFK"].
//       - Reversing route returns ["JFK", "ATL", "JFK", "SFO", "ATL", "SFO"].
//
//   Example 3:
//     Input:  tickets = [["JFK", "KUL"], ["JFK", "NRT"], ["NRT", "JFK"]]
//     Output: ["JFK", "NRT", "JFK", "KUL"]
//     Explanation:
//       - Builds adj sorted descending: JFK:["NRT", "KUL"], NRT:["JFK"].
//       - dfs("JFK") pops "KUL" first from the back of adj["JFK"]; because "KUL"
//         has no outgoing flights, post-order immediately pushes "KUL" onto route.
//       - Resuming at "JFK", the while loop pops "NRT", which traverses "NRT" ->
//         "JFK", and post-order pushes "JFK", "NRT", and "JFK" to form route =
//         ["KUL", "JFK", "NRT", "JFK"].
//       - Reversing route places the dead-end branch "KUL" at the end: ["JFK",
//         "NRT", "JFK", "KUL"].
//
// Algorithm:
//   Implements Hierholzer's algorithm for finding a directed Eulerian path in
//   both recursive (`findItinerary`) and iterative (`review`) forms.
//   Build an adjacency list from `tickets` and sort each airport's destination
//   list in descending lexicographical order so the smallest remaining
//   destination can be removed in O(1) from the back of the vector. Starting at
//   `"JFK"`, greedily consume outgoing edges until stuck at a node with no
//   unused outgoing flights, then append that airport to `route` in post-order.
//   Key invariant: at most one vertex in an Eulerian trail is a dead-end sink
//   (out-degree < in-degree); post-order recording guarantees that dead-end
//   subpaths are appended first (ending up last after reversing `route`), while
//   all cycle detours are spliced cleanly before it in lexicographical order.
//   Time Complexity:  O(E log E), dominated by sorting adjacency lists, where
//                     E = tickets.size().
//   Space Complexity: O(E) for the adjacency map, recursion/explicit stack, and
//                     output route of length E + 1.
//
// Data Structures:
//   - `std::unordered_map<std::string, std::vector<std::string>> adj`: maps
//     each departure airport to a descending-sorted vector of destinations
//     acting as a min-pop stack (`back()` / `pop_back()`).
//   - `std::vector<std::string> route`: collects airports in reverse Eulerian
//     order before a final `std::ranges::reverse`.
//   - `std::vector<std::string> stack` (in `review`): explicit traversal stack
//     eliminating recursion depth limits.
//
// Real-World Applications:
//   - Genome / DNA fragment assembly reconstructing a sequence from k-mer
//     overlap graphs (De Bruijn graph Eulerian paths).
//   - Postal, snowplow, street-sweeping, and garbage-collection route planning
//     (Chinese Postman / edge-covering traversal).
//   - PCB routing, laser cutting, and CNC toolpath planning minimizing pen-up
//     moves across required segments.
//
// Related LeetCode Problems:
//   - #753 Cracking the Safe [done] (Eulerian circuit over a De Bruijn graph)
//   - #2097 Valid Arrangement of Pairs [new] (directed Eulerian path over integer
//     pairs using Hierholzer's algorithm)
//   - #743 Network Delay Time [new] (weighted shortest paths in a directed network)
//   - #787 Cheapest Flights Within K Stops [done] (flight-graph routing with hop cap)

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
