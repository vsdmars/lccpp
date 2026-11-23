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
// Examples:
//   Example 1:
//     Input:  rooms = [[1], [2], [3], []]
//     Output: true
//     Explanation:
//       - Initialize visited = [0, 0, 0, 0] and visited_count = 0, then enter
//         dfs(0) which sets visited[0] = 1 and visited_count = 1.
//       - Follow key 1 to dfs(1) (visited_count = 2), key 2 to dfs(2)
//         (visited_count = 3), and key 3 to dfs(3) (visited_count = 4).
//       - Room 3 has no keys so DFS unwinds, and visited_count == 4 == rooms.size()
//         returns true.
//
//   Example 2:
//     Input:  rooms = [[1, 3], [3, 0, 1], [2], [0]]
//     Output: false
//     Explanation:
//       - Enter dfs(0) (visited_count = 1), follow key 1 to dfs(1) (visited_count =
//         2), and follow key 3 to dfs(3) (visited_count = 3).
//       - From room 3, key 0 is skipped because visited[0] == 1; backtracking to
//         rooms 1 and 0 also finds only already-visited keys 0, 1, and 3.
//       - DFS finishes without ever reaching room 2, so visited_count = 3 != 4
//         returns false.
//
//   Example 3:
//     Input:  rooms = [[]]
//     Output: true
//     Explanation:
//       - Call dfs(0), which sets visited[0] = 1 and increments visited_count to 1.
//       - Room 0 contains no keys, so DFS terminates and visited_count == 1 ==
//         rooms.size() returns true.
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
//   - #133 Clone Graph [done] (directed/undirected graph traversal with visited set)
//   - #547 Number of Provinces [done] (component reachability via DFS)
//   - #797 All Paths From Source to Target [done] (directed reachability and path
//     enumeration with DFS)
//   - #1298 Maximum Candies You Can Get from Boxes [new] (generalized key-and-box
//     unlock traversal)
//   - #1557 Minimum Number of Vertices to Reach All Nodes [new] (reachability
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
