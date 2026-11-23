// LeetCode 1282: Group The People Given The Group Size They Belong To
// https://leetcode.com/problems/group-the-people-given-the-group-size-they-belong-to/
//
// Problem:
//   There are `n` people labeled with unique IDs `0` through `n - 1`. Given an
//   integer array `groupSizes` of length `n`, where `groupSizes[i]` is the
//   required size of the group containing person `i`, partition all `n` people
//   into disjoint groups such that every person `i` belongs to a group of size
//   `groupSizes[i]`. Any valid grouping may be returned; at least one valid
//   solution is guaranteed to exist.
//   Constraints: 1 <= n <= 500; 1 <= groupSizes[i] <= n.
//
// Examples:
//   Example 1:
//     Input:  groupSizes = [3, 3, 3, 3, 3, 1, 3]
//     Output: [[0, 1, 2], [5], [3, 4, 6]]
//     Explanation:
//       - Scan indices 0, 1, 2 (size 3): buckets[3] grows to [0, 1, 2], reaches
//         length 3, flushes [0, 1, 2] into result, and erases key 3 from buckets.
//       - Scan indices 3 and 4 (size 3): buckets[3] accumulates [3, 4].
//       - Scan index 5 (size 1): buckets[1] becomes [5], immediately reaches length
//         1, flushes [5] into result, and erases key 1.
//       - Scan index 6 (size 3): buckets[3] becomes [3, 4, 6], reaches length 3,
//         and flushes [3, 4, 6] into result, returning [[0, 1, 2], [5], [3, 4, 6]].
//
//   Example 2:
//     Input:  groupSizes = [2, 1, 3, 3, 3, 2]
//     Output: [[1], [2, 3, 4], [0, 5]]
//     Explanation:
//       - Scan index 0 (size 2): buckets[2] stores [0].
//       - Scan index 1 (size 1): buckets[1] receives [1], hits length 1, and
//         flushes [1] into result.
//       - Scan indices 2, 3, 4 (size 3): buckets[3] accumulates [2, 3, 4], hits
//         length 3, flushes [2, 3, 4] into result, and erases key 3.
//       - Scan index 5 (size 2): buckets[2] becomes [0, 5], hits length 2, and
//         flushes [0, 5] into result, returning [[1], [2, 3, 4], [0, 5]].
//
//   Example 3:
//     Input:  groupSizes = [1]
//     Output: [[0]]
//     Explanation:
//       - At i = 0 with size = 1, append person 0 to buckets[1] to get [0].
//       - Because buckets[1].size() equals 1, move [0] into result, erase key 1
//         from buckets, and return [[0]].
//
// Algorithm:
//   Greedy hash bucketing in a single linear pass:
//   Maintain an open bucket (vector of person IDs) for each required group
//   size. As each person `i` with required size `s = groupSizes[i]` is
//   scanned, append `i` to `buckets[s]`. Whenever `buckets[s]` reaches
//   length `s`, immediately emit it into the result list via `std::move` and
//   clear the bucket so subsequent people needing size `s` begin forming the
//   next group. Because a valid solution is guaranteed to exist, the total
//   count of people requiring size `s` is always an exact multiple of `s`, so
//   every bucket is flushed cleanly by the end of the pass.
//   Time Complexity: O(n), since each person is appended and moved once.
//   Space Complexity: O(n) for the temporary buckets and output groups.
//
// Data Structures:
//   - std::unordered_map<int, std::vector<int>>: Maps each required group size
//     `s` to the currently accumulating incomplete group of person IDs.
//   - std::vector<std::vector<int>>: Stores completed groups of exact size.
//
// Real-World Applications:
//   - Matchmaking lobbies in online games that assemble fixed-size teams or
//     squads from queued players requesting specific party sizes.
//   - Batching micro-requests or database queries by fixed batch-size
//     requirements before dispatching to worker pools.
//   - Organizing classroom study groups, hackathon teams, or table seating
//     by requested party size.
//
// Related LeetCode Problems:
//   - #49 Group Anagrams [done] (hash-map bucketing by a canonical key)
//   - #1338 Reduce Array Size to The Half [new] (frequency bucketing and greedy selection)
//   - #659 Split Array into Consecutive Subsequences [done] (greedy bucket formation)
//   - #1296 Divide Array in Sets of K Consecutive Numbers [new] (partitioning elements into fixed-size groups)

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Partitions people `0..n-1` into disjoint groups where every person `i`
  // belongs to a group of length `group_sizes[i]`.
  // Time: O(n), Space: O(n).
  std::vector<std::vector<int>> groupThePeople(
      const std::vector<int>& group_sizes) {
    // `buckets[size]` holds the currently filling group for people who require
    // a group of length `size`.
    std::unordered_map<int, std::vector<int>> buckets;
    std::vector<std::vector<int>> result;

    for (std::size_t i = 0uz; i < group_sizes.size(); ++i) {
      const int size = group_sizes[i];
      auto& bucket = buckets[size];
      bucket.push_back(static_cast<int>(i));

      // Once a bucket hits its target capacity, flush it to `result` and
      // remove the completed bucket so the next person needing `size` starts
      // a fresh group.
      if (std::cmp_equal(bucket.size(), size)) {
        result.push_back(std::move(bucket));
        buckets.erase(size);
      }
    }

    return result;
  }
};

// Validates that `groups` is an exact partition of `0..n-1` and every member
// `p` sits in a group whose length equals `group_sizes[p]`.
void expect_valid_grouping(lc::TestContext& ctx,
                           const std::vector<int>& group_sizes,
                           const std::vector<std::vector<int>>& groups) {
  const std::size_t n = group_sizes.size();
  std::vector<int> seen(n, 0);
  std::size_t total_assigned = 0uz;

  for (const auto& group : groups) {
    ctx.expect_false(group.empty(), "group must be non-empty");
    for (const int person : group) {
      ctx.expect_true(person >= 0 && std::cmp_less(person, n),
                      "person ID out of range");
      const auto idx = static_cast<std::size_t>(person);
      ctx.expect_eq(seen[idx], 0);
      seen[idx] = 1;
      ctx.expect_true(std::cmp_equal(group.size(), group_sizes[idx]),
                      "group size mismatch for person");
      ++total_assigned;
    }
  }
  ctx.expect_eq(total_assigned, n);
}

LC_PROBLEM(1282, "Group The People Given The Group Size They Belong To", "hash",
           ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> sizes = {3, 3, 3, 3, 3, 1, 3};
    const auto groups = sol.groupThePeople(sizes);
    ctx.expect_eq(groups.size(), 3uz);
    expect_valid_grouping(ctx, sizes, groups);
  });

  ctx.test("example_2", [&] {
    const std::vector<int> sizes = {2, 1, 3, 3, 3, 2};
    const auto groups = sol.groupThePeople(sizes);
    ctx.expect_eq(groups.size(), 3uz);
    expect_valid_grouping(ctx, sizes, groups);
  });

  ctx.test("single_person", [&] {
    const std::vector<int> sizes = {1};
    const auto groups = sol.groupThePeople(sizes);
    ctx.expect_eq(groups, std::vector<std::vector<int>>{{0}});
  });

  ctx.test("all_same_single_group", [&] {
    const std::vector<int> sizes = {4, 4, 4, 4};
    const auto groups = sol.groupThePeople(sizes);
    ctx.expect_eq(groups.size(), 1uz);
    expect_valid_grouping(ctx, sizes, groups);
  });

  ctx.test("all_singleton_groups", [&] {
    const std::vector<int> sizes = {1, 1, 1, 1};
    const auto groups = sol.groupThePeople(sizes);
    ctx.expect_eq(groups.size(), 4uz);
    expect_valid_grouping(ctx, sizes, groups);
  });
}

}  // namespace
