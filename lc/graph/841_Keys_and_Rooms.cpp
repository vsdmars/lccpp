// LeetCode 841: Keys and Rooms
// https://leetcode.com/problems/keys-and-rooms/
//
// Problem:
//   There are `n` rooms numbered `0..n-1`. All rooms are initially locked
//   except room `0`. Entering room `i` reveals a list of keys `rooms[i]`,
//   where each key unlocks the corresponding room number and can be used to
//   enter that room. Return `true` if and only if every room `0..n-1` is
//   reachable starting from room `0`.
//
// Algorithm:
//   Directed graph reachability via Depth-First Search (DFS). Model rooms as
//   vertices and keys inside `rooms[i]` as directed edges `i -> key`. Start a
//   recursive DFS at room `0`, marking each newly entered room as visited,
//   incrementing `visited_count`, and traversing all keys leading to unvisited
//   rooms. After DFS finishes, all rooms are reachable if and only if
//   `visited_count == rooms.size()`.
//   Time: O(V + E), where `V` is the number of rooms and `E` is the total
//   number of keys. Space: O(V) for the visited array and DFS recursion stack.
//
// Data Structures:
//   - `const std::vector<std::vector<int>>& rooms`: Input directed adjacency
//     list mapping each room to the keys it contains.
//   - `std::vector<std::uint8_t> visited`: Per-room visitation table preventing
//     redundant exploration and infinite loops on key cycles.
//   - `std::size_t visited_count`: Running count of distinct rooms reached,
//     avoiding a second pass over `visited`.
//
// Real-World Applications:
//   - Garbage collection (mark-and-sweep): traversing object references from
//     root pointers to determine which heap objects are reachable.
//   - Web crawling and link auditing: checking whether all pages in a site are
//     discoverable by following hyperlinks from the entry URL.
//   - Security privilege-escalation auditing: testing whether a starting role
//     can transitively acquire credentials/keys to access every protected
//     resource.
//
// Related LeetCode Problems:
//   - #133 Clone Graph (directed/undirected graph traversal with visited set)
//   - #547 Number of Provinces (component reachability via DFS)
//   - #797 All Paths From Source to Target (directed reachability and path
//     enumeration with DFS)
//   - #1298 Maximum Candies You Can Get from Boxes (generalized key-and-box
//     unlock traversal)
//   - #1557 Minimum Number of Vertices to Reach All Nodes (reachability
//     analysis in directed graphs)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Checks full reachability from room 0 via DFS.
  // Time: O(V + E), Space: O(V).
  bool canVisitAllRooms(const std::vector<std::vector<int>>& rooms) {
    if (rooms.empty()) {
      return true;
    }

    std::vector<std::uint8_t> visited(rooms.size(), 0);
    std::size_t visited_count = 0uz;

    auto dfs = [&](this auto&& self, std::size_t room) -> void {
      visited[room] = 1;
      ++visited_count;
      for (const int key : rooms[room]) {
        const auto next = static_cast<std::size_t>(key);
        if (visited[next] == 0) {
          self(next);
        }
      }
    };

    dfs(0uz);
    return visited_count == rooms.size();
  }
};

LC_PROBLEM(841, "Keys and Rooms", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: chain unlocks all rooms", [&] {
    ctx.expect_true(sol.canVisitAllRooms({{1}, {2}, {3}, {}}));
  });

  ctx.test("example 2: room 2 is unreachable", [&] {
    ctx.expect_false(sol.canVisitAllRooms({{1, 3}, {3, 0, 1}, {2}, {0}}));
  });

  ctx.test("single room", [&] { ctx.expect_true(sol.canVisitAllRooms({{}})); });
}

}  // namespace
