// https://leetcode.com/problems/keys-and-rooms/description/
//
// There are N rooms numbered 0..N-1. Room 0 is initially unlocked and every
// other room is locked. Entering room `i` gives you the keys listed in
// `rooms[i]`. Return true if and only if you can visit every room.

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
