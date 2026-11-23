// LeetCode 1057: Campus Bikes
// https://leetcode.com/problems/campus-bikes/
//
// Problem:
//   On a 2D grid representing a campus, there are N workers and M bikes
//   (1 <= N <= M <= 1000) at distinct integer coordinates in [0, 999]^2.
//   Repeatedly select the available (worker, bike) pair with the smallest
//   Manhattan distance |x1 - x2| + |y1 - y2|. Break ties by smallest worker
//   index, then by smallest bike index. Assign that bike to that worker until
//   every worker has a bike, and return `ans` where `ans[i]` is the bike index
//   assigned to worker `i`.
//
// Examples:
//   Example 1:
//     Input:  workers = [[0, 0], [2, 1]], bikes = [[1, 2], [3, 3]]
//     Output: [1, 0]
//     Explanation:
//       - Worker 0's candidates sort descending as [(6, w=0, b=1), (3, w=0, b=0)]
//         and worker 1's as [(3, w=1, b=1), (2, w=1, b=0)], seeding min_heap with
//         (2, 1, 0) and (3, 0, 0).
//       - First pop is (2, 1, 0); bike 0 is unused, so assignment[1] = 0,
//         bike_used[0] = 1, and assigned_count becomes 1.
//       - Next pop is (3, 0, 0); since bike_used[0] is 1, discard it and push
//         worker 0's remaining candidate (6, 0, 1).
//       - Final pop is (6, 0, 1); bike 1 is unused, so assignment[0] = 1,
//         assigned_count reaches 2, and assignBikes returns [1, 0].
//
//   Example 2:
//     Input:  workers = [[0, 0], [1, 1], [2, 0]], bikes = [[1, 0], [2, 2], [2, 1]]
//     Output: [0, 2, 1]
//     Explanation:
//       - Each worker's closest candidate has distance 1 to bike 0, seeding
//         min_heap with (1, 0, 0), (1, 1, 0), and (1, 2, 0), while workers 1 and 2
//         also have distance-1 candidates to bike 2.
//       - Pop (1, 0, 0) first due to smallest worker index 0, setting assignment[0]
//         = 0 and marking bike_used[0] = 1.
//       - Pop (1, 1, 0), see bike 0 is used, push worker 1's next candidate (1, 1,
//         2), then pop (1, 1, 2) (breaking the distance-1 tie over worker 2) to set
//         assignment[1] = 2 and bike_used[2] = 1.
//       - Pop (1, 2, 0) (pushing (1, 2, 2)), then pop (1, 2, 2) (pushing (2, 2,
//         1)), and finally pop (2, 2, 1) to assign assignment[2] = 1, returning [0,
//         2, 1].
//
//   Example 3:
//     Input:  workers = [[5, 5]], bikes = [[10, 10], [6, 5], [4, 5]]
//     Output: [1]
//     Explanation:
//       - Worker 0's Manhattan distances are 10 to bike 0, 1 to bike 1, and 1 to
//         bike 2, so descending sort orders candidates as [(10, 0, 0), (1, 0, 2),
//         (1, 0, 1)].
//       - Popping back() seeds min_heap with (1, 0, 1) because bike index 1 < 2
//         breaks the distance-1 tie.
//       - The single heap pop assigns assignment[0] = 1, marks bike_used[1] = 1,
//         and returns [1].
//
// Algorithm:
//   Per-Worker Sorted Lists + Multi-Way Min-Heap Merge:
//   1. For each worker `w`, compute its distance to all M bikes and sort the
//      worker's candidate list in descending order of `(distance, w, bike)` so
//      the closest untried bike sits at the back for O(1) `pop_back()`.
//   2. Seed a min-heap with each worker's single best candidate (heap size N).
//   3. Repeatedly pop the globally smallest `(distance, w, bike)` from the
//      min-heap:
//      - If `bike` is already assigned to another worker, discard it and push
//        worker `w`'s next closest bike into the heap.
//      - Otherwise, assign `bike` to `w` and mark `bike` as used.
//   Invariant: At every step, each unassigned worker has exactly one entry in
//   the heap, which is its closest untried bike. Therefore the heap minimum is
//   guaranteed to be the globally optimal available (worker, bike) pair.
//   Time Complexity:  O(N * M * log M) to sort each worker's M bikes, plus
//                     O(N * M * log N) worst-case heap operations (overall
//                     O(N * M * log M) since N <= M).
//   Space Complexity: O(N * M) to store per-worker candidate lists and O(N + M)
//                     for the heap, assignment array, and used-bike bitmap.
//
// Data Structures:
//   - Per-worker sorted `std::vector<PairCandidate>` stacks: provide O(1)
//     access to each worker's next closest bike without re-scanning all bikes.
//   - Min-heap (`std::priority_queue` with `std::greater<>`): maintains the
//     frontier of one best candidate per unassigned worker, yielding the
//     global minimum in O(log N) time.
//   - Boolean byte array (`std::vector<std::uint8_t>`): O(1) lookup to check
//     whether a bike has already been claimed.
//
// Real-World Applications:
//   - Ride-hailing and micromobility dispatch (matching nearby drivers or
//     scooters to riders with deterministic tie-breaking).
//   - Warehouse robotics task allocation (assigning automated guided vehicles
//     to nearest pickup stations).
//   - Edge-cloud request routing (greedily pairing client jobs to nearest
//     available compute nodes under one-to-one capacity limits).
//
// Related LeetCode Problems:
//   - #1066 Campus Bikes II [new] (minimum total Manhattan distance via bitmask DP)
//   - #373 Find K Pairs with Smallest Sums [done] (multi-way min-heap frontier merge)
//   - #1439 Find The Kth Smallest Sum Of A Matrix With Sorted Rows [done] (k-way sorted merge with min-heap)
//   - #23 Merge k Sorted Lists [done] (canonical k-way merge using a min-heap)
//   - #1834 Single Threaded Cpu [done] (greedy priority-queue dispatch with tie-breaking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Represents one (worker, bike) pairing ordered lexicographically by:
//   1. Manhattan distance (ascending)
//   2. Worker index (ascending)
//   3. Bike index (ascending)
struct PairCandidate {
  int distance = 0;
  int worker = 0;
  int bike = 0;

  auto operator<=>(const PairCandidate&) const = default;
};

class Solution {
 public:
  // Assigns each worker a unique bike greedily by smallest Manhattan distance,
  // breaking ties by worker index and then bike index.
  //
  // Time Complexity:  O(N * M * log M)
  // Space Complexity: O(N * M)
  [[nodiscard]] std::vector<int> assignBikes(
      const std::vector<std::vector<int>>& workers,
      const std::vector<std::vector<int>>& bikes) {
    const std::size_t num_workers = workers.size();
    const std::size_t num_bikes = bikes.size();
    if (num_workers == 0uz || num_bikes == 0uz) {
      return {};
    }

    // Phase 1: For each worker, build a list of all bike candidates sorted in
    // descending order so the closest bike is at the back for O(1) popping.
    std::vector<std::vector<PairCandidate>> worker_candidates(num_workers);
    std::vector<PairCandidate> initial_frontier;
    initial_frontier.reserve(num_workers);

    for (std::size_t w = 0uz; w < num_workers; ++w) {
      auto& candidates = worker_candidates[w];
      candidates.reserve(num_bikes);

      for (std::size_t b = 0uz; b < num_bikes; ++b) {
        const int dist =
            manhattan_distance(workers[w][0], workers[w][1], bikes[b][0], bikes[b][1]);
        candidates.push_back(PairCandidate{
            .distance = dist,
            .worker = static_cast<int>(w),
            .bike = static_cast<int>(b),
        });
      }

      // Sort descending so `back()` is the smallest (distance, worker, bike).
      std::ranges::sort(candidates, std::greater<>{});
      initial_frontier.push_back(candidates.back());
      candidates.pop_back();
    }

    // Phase 2: Build the min-heap in O(N) using Floyd's heapify constructor.
    std::priority_queue<PairCandidate, std::vector<PairCandidate>, std::greater<>>
        min_heap(std::greater<>{}, std::move(initial_frontier));

    std::vector<int> assignment(num_workers, -1);
    std::vector<std::uint8_t> bike_used(num_bikes, 0U);
    std::size_t assigned_count = 0uz;

    // Phase 3: Greedily match the globally closest available (worker, bike).
    while (assigned_count < num_workers) {
      const PairCandidate best = min_heap.top();
      min_heap.pop();

      const auto bike_idx = static_cast<std::size_t>(best.bike);
      const auto worker_idx = static_cast<std::size_t>(best.worker);

      if (bike_used[bike_idx] != 0U) {
        // Bike already taken; advance this worker to their next closest bike.
        auto& remaining = worker_candidates[worker_idx];
        min_heap.push(remaining.back());
        remaining.pop_back();
        continue;
      }

      assignment[worker_idx] = best.bike;
      bike_used[bike_idx] = 1U;
      ++assigned_count;
    }

    return assignment;
  }

 private:
  // Computes L1 (Manhattan) distance between two 2D grid points.
  [[nodiscard]] static int manhattan_distance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
  }
};

LC_PROBLEM(1057, "Campus Bikes", "heap", ctx) {
  ctx.test("example_1_no_ties", [&] {
    Solution sol;
    const std::vector<std::vector<int>> workers = {{0, 0}, {2, 1}};
    const std::vector<std::vector<int>> bikes = {{1, 2}, {3, 3}};
    ctx.expect_eq(sol.assignBikes(workers, bikes), std::vector<int>{1, 0});
  });

  ctx.test("example_2_worker_tie_break", [&] {
    Solution sol;
    const std::vector<std::vector<int>> workers = {{0, 0}, {1, 1}, {2, 0}};
    const std::vector<std::vector<int>> bikes = {{1, 0}, {2, 2}, {2, 1}};
    ctx.expect_eq(sol.assignBikes(workers, bikes), std::vector<int>{0, 2, 1});
  });

  ctx.test("bike_index_tie_break_and_extra_bikes", [&] {
    Solution sol;
    // Worker 0 is distance 1 from bikes 0, 1, 2, 3 -> must pick bike 0.
    // Worker 1 is distance 1 from bike 1 and distance 3 from others -> picks bike 1.
    const std::vector<std::vector<int>> workers = {{1, 1}, {2, 2}};
    const std::vector<std::vector<int>> bikes = {{1, 0}, {1, 2}, {0, 1}, {2, 1}};
    ctx.expect_eq(sol.assignBikes(workers, bikes), std::vector<int>{0, 1});
  });

  ctx.test("single_worker_multiple_bikes", [&] {
    Solution sol;
    const std::vector<std::vector<int>> workers = {{5, 5}};
    const std::vector<std::vector<int>> bikes = {{10, 10}, {6, 5}, {4, 5}};
    // Both bike 1 and bike 2 are at distance 1; smaller bike index (1) wins.
    ctx.expect_eq(sol.assignBikes(workers, bikes), std::vector<int>{1});
  });

  ctx.test("empty_input", [&] {
    Solution sol;
    const std::vector<std::vector<int>> empty;
    ctx.expect_eq(sol.assignBikes(empty, empty), std::vector<int>{});
  });
}

}  // namespace
