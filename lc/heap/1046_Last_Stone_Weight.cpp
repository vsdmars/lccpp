// LeetCode 1046: Last Stone Weight
// https://leetcode.com/problems/last-stone-weight/
//
// Problem:
//   You are given an array of positive integers `stones` representing stone
//   weights. On each turn, pick the two heaviest stones `y >= x` and smash
//   them together:
//   - If `x == y`, both stones are destroyed.
//   - If `x != y`, the lighter stone `x` is destroyed and the heavier stone
//     remains with new weight `y - x`.
//   Repeat until at most one stone remains and return its weight (or 0 if no
//   stones remain).
//
// Examples:
//   Example 1:
//     Input:  stones = [2, 7, 4, 1, 8, 1]
//     Output: 1
//     Explanation:
//       - Heapify builds max_heap with elements [8, 7, 4, 2, 1, 1].
//       - Round 1 pops 8 and 7 and pushes 8 - 7 = 1 (leaving [4, 2, 1, 1, 1]);
//         Round 2 pops 4 and 2 and pushes 4 - 2 = 2 (leaving [2, 1, 1, 1]).
//       - Round 3 pops 2 and 1 and pushes 2 - 1 = 1 (leaving [1, 1, 1]); Round 4
//         pops 1 and 1 and pushes nothing because heaviest == second.
//       - With one stone remaining ([1]), max_heap.size() > 1 becomes false and
//         lastStoneWeight returns max_heap.top() = 1.
//
//   Example 2:
//     Input:  stones = [3, 7, 2, 2]
//     Output: 0
//     Explanation:
//       - Heapify initializes max_heap with [7, 3, 2, 2].
//       - Round 1 pops 7 and 3 and pushes 7 - 3 = 4 (leaving [4, 2, 2]); Round 2
//         pops 4 and 2 and pushes 4 - 2 = 2 (leaving [2, 2]).
//       - Round 3 pops 2 and 2 without pushing a remainder, leaving max_heap empty
//         so lastStoneWeight returns 0.
//
//   Example 3:
//     Input:  stones = [4, 4, 4]
//     Output: 4
//     Explanation:
//       - Heapify initializes max_heap with [4, 4, 4].
//       - Round 1 pops heaviest = 4 and second = 4 and pushes nothing because they
//         are equal, leaving [4] in max_heap.
//       - Because max_heap.size() == 1, the while loop terminates and returns
//         max_heap.top() = 4.
//
// Algorithm:
//   Greedy Simulation with a Max-Heap:
//   1. Build a max-heap from all initial stone weights in O(n) time.
//   2. While at least two stones remain in the heap, extract the two largest
//      weights `heaviest` and `second`.
//   3. If `heaviest > second`, push the remainder `heaviest - second` back
//      into the max-heap. Each iteration reduces the stone count by at least 1
//      (by 2 on equal weights), guaranteeing termination in at most n - 1
//      rounds.
//   4. Return `max_heap.top()` if one stone remains, or 0 if empty.
//   Time:  O(n * log(n))
//   Space: O(n)
//
// Data Structures:
//   - Max-heap (`std::priority_queue<int>`) supporting O(n) bottom-up
//     construction and O(log n) repeated extraction/re-insertion of the
//     heaviest stones.
//
// Real-World Applications:
//   - Priority-driven discrete-event simulation where the largest pending
//     tasks or payloads are repeatedly paired and reduced.
//   - Greedy difference-minimization heuristics (Karmarkar-Karp / largest
//     differencing method for number partitioning and load balancing).
//   - Order-book matching / netting engines repeatedly cancelling the largest
//     opposing positions.
//
// Related LeetCode Problems:
//   - #1049 Last Stone Weight II [new] (subset-sum DP version minimizing final weight)
//   - #1167 Minimum Cost to Connect Sticks [new] (min-heap greedy pairwise merging)
//   - #215 Kth Largest Element in an Array [done] (heap-based largest-element queries)
//   - #703 Kth Largest Element in a Stream [done] (heap maintenance over dynamic items)

#include "framework/lc_framework.h"

#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // Simulates smashing the two heaviest stones until at most one remains.
  // Time: O(n * log(n)), Space: O(n).
  int lastStoneWeight(const std::vector<int>& stones) {
    // Bottom-up heapify builds the max-heap in O(n) time.
    std::priority_queue<int> max_heap(stones.begin(), stones.end());

    while (max_heap.size() > 1) {
      const int heaviest = max_heap.top();
      max_heap.pop();
      const int second = max_heap.top();
      max_heap.pop();

      // Equal stones annihilate each other; otherwise re-insert the remnant.
      if (heaviest != second) {
        max_heap.push(heaviest - second);
      }
    }

    return max_heap.empty() ? 0 : max_heap.top();
  }
};

LC_PROBLEM(1046, "Last Stone Weight", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.lastStoneWeight({2, 7, 4, 1, 8, 1}), 1);
  });

  ctx.test("example_2_single_stone", [&] {
    Solution sol;
    ctx.expect_eq(sol.lastStoneWeight({1}), 1);
  });

  ctx.test("all_stones_annihilated", [&] {
    Solution sol;
    ctx.expect_eq(sol.lastStoneWeight({5, 5}), 0);
    ctx.expect_eq(sol.lastStoneWeight({3, 7, 2, 2}), 0);
  });

  ctx.test("empty_and_odd_equal_weights", [&] {
    Solution sol;
    ctx.expect_eq(sol.lastStoneWeight({}), 0);
    ctx.expect_eq(sol.lastStoneWeight({4, 4, 4}), 4);
  });
}

}  // namespace
