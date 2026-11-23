// LeetCode 881: Boats To Save People
// https://leetcode.com/problems/boats-to-save-people/
//
// Problem:
//   Given an array `people` where `people[i]` is the weight of the i-th person
//   and an integer `limit` representing the maximum weight capacity of a boat,
//   return the minimum number of boats needed to carry everyone. Each boat can
//   carry at most two people simultaneously, provided their combined weight is
//   at most `limit`. Every person weighs at most `limit`.
//   Constraints: 1 <= people.length <= 5 * 10^4, 1 <= people[i] <= limit <= 3 * 10^4.
//
// Algorithm:
//   Greedy sorting + opposite-direction two pointers:
//   1. Sort `people` in non-decreasing order of weight.
//   2. Maintain a half-open range `[left, right)` of unassigned people, where
//      `people[left]` is the lightest remaining person and `people[right - 1]`
//      is the heaviest remaining person.
//   3. At every step, the heaviest person MUST take a boat; try pairing them
//      with the lightest remaining person. If `people[left] + people[right - 1]
//      <= limit`, both share the boat (`++left`, `--right`); otherwise, nobody
//      can pair with the heaviest person, so they ride alone (`--right`).
//   Exchange argument: Pairing the heaviest person with the lightest feasible
//   partner never hurts future pairings because any other partner could also
//   be replaced by the lightest person without exceeding `limit`.
//   Time Complexity:  O(N log N) for sorting, plus O(N) two-pointer scan.
//   Space Complexity: O(log N) stack space for `std::ranges::sort` (in-place).
//
// Data Structures:
//   - Sorted `std::vector<int>` (`people`): enables greedy lightest-heaviest
//     pairing via two converging indices (`left`, `right`).
//
// Real-World Applications:
//   - Two-item bin packing in logistics, freight container loading, and
//     ride-sharing passenger pairing under vehicle capacity limits.
//   - Cloud job co-scheduling (packing pairs of memory/CPU-bounded tasks onto
//     fixed-capacity worker nodes).
//   - Memory slab allocation pairing small and large objects into fixed-size
//     pages.
//
// Related LeetCode Problems:
//   - #1099 Two Sum Less Than K [done] (sorted two-pointer sum constrained by an upper bound)
//   - #167 Two Sum II [done] - Input Array Is Sorted (converging two pointers on sorted array)
//   - #455 Assign Cookies [new] (greedy two-pointer matching after sorting)
//   - #1877 Minimize Maximum Pair Sum in Array [done] (pairing lightest with heaviest after sorting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum number of boats (capacity <= `limit`, max 2 people per
  // boat) required to transport everyone in `people`.
  // Invariant: Every person outside `[left, right)` has already been assigned
  // optimally; each iteration consumes 1 boat and shrinks `[left, right)` by 1
  // or 2 elements.
  // Time: O(N log N), Space: O(log N) for in-place sort.
  int numRescueBoats(std::vector<int> people, int limit) {
    // Sort ascending so lightest is at `left` and heaviest is at `right - 1`.
    std::ranges::sort(people);

    std::size_t left = 0uz;
    std::size_t right = people.size();  // Exclusive upper bound avoids underflow.
    int boats = 0;

    while (left < right) {
      // Always place the heaviest unassigned person onto a new boat.
      --right;
      // Pair with the lightest unassigned person if another person remains and
      // their combined weight stays within `limit`.
      if (left < right && people[left] + people[right] <= limit) {
        ++left;
      }
      ++boats;
    }

    return boats;
  }
};

LC_PROBLEM(881, "Boats To Save People", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_single_boat", [&] {
    ctx.expect_eq(sol.numRescueBoats({1, 2}, 3), 1);
  });

  ctx.test("example_2_three_boats", [&] {
    ctx.expect_eq(sol.numRescueBoats({3, 2, 2, 1}, 3), 3);
  });

  ctx.test("example_3_none_can_pair", [&] {
    ctx.expect_eq(sol.numRescueBoats({3, 5, 3, 4}, 5), 4);
  });

  ctx.test("single_person_and_empty", [&] {
    ctx.expect_eq(sol.numRescueBoats({5}, 5), 1);
    ctx.expect_eq(sol.numRescueBoats({}, 5), 0);
  });

  ctx.test("all_light_perfect_pairs", [&] {
    ctx.expect_eq(sol.numRescueBoats({1, 1, 1, 1, 1}, 2), 3);
    ctx.expect_eq(sol.numRescueBoats({1, 2, 2, 3}, 4), 2);
  });
}

}  // namespace
