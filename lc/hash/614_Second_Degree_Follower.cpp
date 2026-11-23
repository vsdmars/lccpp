// LeetCode 614: Second Degree Follower
// https://leetcode.com/problems/second-degree-follower/
//
// Problem:
//   Given a social-network `Follow` table where each row `(followee, follower)`
//   records that user `follower` follows user `followee`, identify every
//   "second-degree follower" — a user who BOTH:
//     1. follows at least one user (appears in the `follower` column), AND
//     2. is followed by at least one user (appears in the `followee` column).
//   Return each qualifying user together with their number of distinct
//   followers (`num`), ordered alphabetically ascending by user name.
//   (Original LeetCode problem is SQL; modeled here directly as typed C++23
//   table rows executing the exact relational join/aggregation semantics.)
//
// Algorithm:
//   Equivalent SQL query:
//     SELECT f1.followee AS follower, COUNT(DISTINCT f1.follower) AS num
//     FROM Follow f1
//     INNER JOIN Follow f2 ON f1.followee = f2.follower
//     GROUP BY f1.followee
//     ORDER BY f1.followee ASC;
//   In-memory hash-join + aggregation:
//     1. Scan the `Follow` rows once: insert `row.follower` into the set
//        `active_followers` (users who follow someone), and insert
//        `row.follower` into `followee_to_followers[row.followee]` (a set so
//        duplicate rows are deduplicated just like `COUNT(DISTINCT follower)`).
//     2. Iterate over `followee_to_followers`: any user who also exists in
//        `active_followers` is a second-degree follower; emit
//        `{user, distinct_follower_count}`.
//     3. Sort the output rows ascending by `follower`.
//   Time Complexity:  O(E + U log U), where E is the number of follow edges
//                     and U is the number of qualifying second-degree users.
//   Space Complexity: O(E) to store the deduplicated adjacency sets and
//                     follower membership set.
//
// Data Structures:
//   - std::unordered_set<std::string> active_followers: O(1) expected check
//     for whether a user follows at least one other user (out-degree >= 1).
//   - std::unordered_map<std::string, std::unordered_set<std::string>>:
//     maps each followee to their set of distinct followers (in-degree >= 1).
//   - std::vector<FollowerCount>: holds the sorted query result set.
//
// Real-World Applications:
//   - Social graph analytics identifying active "bridge" / hub accounts that
//     both consume and produce content (in-degree >= 1 and out-degree >= 1).
//   - Relational database query execution engines implementing hash-based
//     semi-joins paired with DISTINCT group-by aggregation.
//   - Citation and dependency network analysis finding intermediate nodes that
//     both cite prior work and are cited by subsequent papers.
//
// Related LeetCode Problems:
//   - #997 Find the Town Judge [new] (directed graph in-degree and out-degree filtering)
//   - #1557 Minimum Number of Vertices to Reach All Nodes [new] (in-degree analysis on directed graphs)
//   - #602 Friend Requests II [new] : Who Has the Most Friends (SQL social-graph degree aggregation)
//   - #133 Clone Graph [done] (directed/undirected graph adjacency representation)

#include "framework/lc_framework.h"

#include <algorithm>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Represents a single row of the SQL `Follow(followee, follower)` table.
struct FollowRow {
  std::string followee;
  std::string follower;
};

// Represents an output row `(follower, num)` of the Second Degree Follower
// query, where `num` is the user's distinct follower count.
struct FollowerCount {
  std::string follower;
  int num = 0;

  bool operator==(const FollowerCount&) const = default;
};

// Streams a FollowerCount for readable assertion diagnostics.
std::ostream& operator<<(std::ostream& os, const FollowerCount& row) {
  return os << '{' << row.follower << ": " << row.num << '}';
}

class Solution {
 public:
  // Executes the Second Degree Follower query over the `Follow` table rows and
  // returns qualifying `(follower, num)` records sorted ascending by name.
  // Time: O(E + U log U), Space: O(E).
  std::vector<FollowerCount> secondDegreeFollower(
      const std::vector<FollowRow>& follow_table) {
    // Users who follow at least one other user (out-degree >= 1).
    std::unordered_set<std::string> active_followers;
    // Maps each followed user to their set of distinct followers (in-degree).
    std::unordered_map<std::string, std::unordered_set<std::string>>
        followee_to_followers;

    // Phase 1: Build out-degree membership and deduplicated in-degree sets.
    for (const auto& row : follow_table) {
      active_followers.insert(row.follower);
      followee_to_followers[row.followee].insert(row.follower);
    }

    // Phase 2: Hash semi-join — retain users present in both roles.
    std::vector<FollowerCount> result;
    result.reserve(followee_to_followers.size());
    for (const auto& [user, followers] : followee_to_followers) {
      if (active_followers.contains(user) && !followers.empty()) {
        result.push_back(FollowerCount{
            .follower = user,
            .num = static_cast<int>(followers.size()),
        });
      }
    }

    // Phase 3: Order output alphabetically ascending by `follower`.
    std::ranges::sort(result, {}, &FollowerCount::follower);
    return result;
  }
};

LC_PROBLEM(614, "Second Degree Follower", "hash", ctx) {
  ctx.test("example_social_graph", [&] {
    Solution sol;
    const std::vector<FollowRow> table = {
        {.followee = "Alice", .follower = "Bob"},
        {.followee = "Bob", .follower = "Cena"},
        {.followee = "Bob", .follower = "Donald"},
        {.followee = "Donald", .follower = "Edward"},
    };
    const std::vector<FollowerCount> expected = {
        {.follower = "Bob", .num = 2},
        {.follower = "Donald", .num = 1},
    };
    ctx.expect_eq(sol.secondDegreeFollower(table), expected);
  });

  ctx.test("duplicate_rows_counted_distinct", [&] {
    Solution sol;
    // Mirrors classic SQL COUNT(DISTINCT follower) deduplication.
    const std::vector<FollowRow> table = {
        {.followee = "A", .follower = "B"},
        {.followee = "B", .follower = "C"},
        {.followee = "B", .follower = "C"},
        {.followee = "B", .follower = "D"},
    };
    const std::vector<FollowerCount> expected = {
        {.follower = "B", .num = 2},
    };
    ctx.expect_eq(sol.secondDegreeFollower(table), expected);
  });

  ctx.test("mutual_cycle_and_alphabetical_order", [&] {
    Solution sol;
    const std::vector<FollowRow> table = {
        {.followee = "Zoe", .follower = "Amy"},
        {.followee = "Amy", .follower = "Zoe"},
        {.followee = "Mid", .follower = "Zoe"},
        {.followee = "Amy", .follower = "Mid"},
    };
    const std::vector<FollowerCount> expected = {
        {.follower = "Amy", .num = 2},
        {.follower = "Mid", .num = 1},
        {.follower = "Zoe", .num = 1},
    };
    ctx.expect_eq(sol.secondDegreeFollower(table), expected);
  });

  ctx.test("no_second_degree_users_and_empty_table", [&] {
    Solution sol;
    const std::vector<FollowRow> star = {
        {.followee = "Celeb", .follower = "Fan1"},
        {.followee = "Celeb", .follower = "Fan2"},
    };
    ctx.expect_true(sol.secondDegreeFollower(star).empty());
    ctx.expect_true(sol.secondDegreeFollower({}).empty());
  });
}

}  // namespace
