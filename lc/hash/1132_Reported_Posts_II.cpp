// LeetCode 1132: Reported Posts II
// https://leetcode.com/problems/reported-posts-ii/
//
// Problem:
//   Originally a SQL problem over two tables:
//   - `Actions(user_id, post_id, action_date, action, extra)` (may contain
//     duplicate rows; `action` is one of view/like/reaction/comment/report/
//     share; `extra` holds the report reason such as `"spam"` or `"racism"`).
//   - `Removals(post_id, remove_date)` (`post_id` is the primary key).
//   Compute the average daily percentage of posts that were removed after being
//   reported as spam, rounded to 2 decimal places:
//   - Only consider days (`action_date`) that have at least one `"report"` row
//     with `extra == "spam"`.
//   - Deduplicate `post_id` within each day so multiple spam reports on the
//     same post on the same day are counted once.
//   - For each qualifying day, its daily removal percentage is
//     `100.0 * (# distinct spam-reported posts present in Removals) /
//              (# distinct spam-reported posts on that day)`.
//   - Return the arithmetic mean of those daily percentages rounded to 2
//     decimal places (or `0.0` if no spam reports exist).
//
// Examples:
//   Example 1:
//     Input:  actions = [(1, 1, "2019-07-01", "view", ""), (1, 1, "2019-07-01",
//             "like", ""), (1, 1, "2019-07-01", "share", ""), (2, 2, "2019-07-04",
//             "view", ""), (2, 2, "2019-07-04", "report", "spam"), (3, 4,
//             "2019-07-04", "view", ""), (3, 4, "2019-07-04", "report", "spam"),
//             (4, 3, "2019-07-02", "view", ""), (4, 3, "2019-07-02", "report",
//             "spam"), (5, 2, "2019-07-03", "view", ""), (5, 2, "2019-07-03",
//             "report", "racism"), (5, 5, "2019-07-03", "view", ""), (5, 5,
//             "2019-07-03", "report", "racism")], removals = [(2, "2019-07-20"),
//             (3, "2019-07-18")]
//     Output: 75.0
//     Explanation:
//       - Index removals into the hash set removed_posts = {2, 3}.
//       - Scan actions and keep only rows with action == "report" and extra ==
//         "spam", building daily_spam_posts["2019-07-04"] = {2, 4} and
//         daily_spam_posts["2019-07-02"] = {3} while skipping "2019-07-01"
//         non-reports and "2019-07-03" racism reports.
//       - For "2019-07-04", 1 of 2 distinct spam posts (post 2) is in
//         removed_posts, yielding daily_pct = 50.0; for "2019-07-02", 1 of 1 post
//         (post 3) is in removed_posts, yielding daily_pct = 100.0.
//       - Average total_daily_pct = 150.0 across 2 qualifying days to get 75.0, and
//         round to 2 decimal places to return 75.0.
//
//   Example 2:
//     Input:  actions = [(1, 2, "2019-07-04", "report", "spam"), (2, 2,
//             "2019-07-04", "report", "spam"), (2, 2, "2019-07-04", "report",
//             "spam"), (3, 4, "2019-07-04", "report", "spam")], removals = [(2,
//             "2019-07-05")]
//     Output: 50.0
//     Explanation:
//       - Populate removed_posts = {2} from removals.
//       - Inserting each spam report's post_id into the std::unordered_set at
//         daily_spam_posts["2019-07-04"] deduplicates the three reports for post 2,
//         producing {2, 4}.
//       - Counting membership in removed_posts finds removed_count = 1 out of 2
//         distinct posts, giving daily_pct = 100.0 * 1 / 2 = 50.0.
//       - With 1 qualifying day in daily_spam_posts, avg_pct is 50.0 and
//         std::round(50.0 * 100.0) / 100.0 returns 50.0.
//
//   Example 3:
//     Input:  actions = [(1, 1, "2019-07-01", "report", "racism"), (2, 2,
//             "2019-07-01", "view", "spam")], removals = [(1, "2019-07-02")]
//     Output: 0.0
//     Explanation:
//       - Populate removed_posts = {1} from removals.
//       - Scan actions: row 1 has extra = "racism" instead of "spam", and row 2 has
//         action = "view" instead of "report", so neither is inserted into
//         daily_spam_posts.
//       - Because daily_spam_posts is empty, the early-return guard triggers and
//         returns 0.0.
//
// Algorithm:
//   Hash-Join + Daily Distinct Aggregation (C++23 equivalent of the SQL query):
//   1. Index all removed `post_id`s into a hash set `removed_posts` in O(R).
//   2. Scan `actions` in O(A); for every row with `action == "report"` and
//      `extra == "spam"`, insert `post_id` into `daily_spam_posts[action_date]`
//      (a hash set per date that deduplicates repeated reports on the same post
//      and day).
//   3. For each `(date, post_ids)` bucket, count how many `post_id`s exist in
//      `removed_posts`, compute that day's removal percentage, and accumulate
//      into `total_daily_pct`.
//   4. Divide `total_daily_pct` by the number of qualifying days and round to 2
//      decimal places via `std::round(avg * 100.0) / 100.0`.
//   Time Complexity: O(A + R) expected, where A = actions.size() and
//   R = removals.size().
//   Space Complexity: O(A + R) for the removed-post hash set and daily distinct
//   spam-post buckets.
//
// Data Structures:
//   - Hash set (`std::unordered_set<int>`): provides O(1) expected membership
//     tests for removed `post_id`s (hash-join probe table) and per-day
//     deduplication of spam-reported posts.
//   - Hash map (`std::unordered_map<std::string, std::unordered_set<int>>`):
//     implements `GROUP BY action_date` with `COUNT(DISTINCT post_id)`.
//
// Real-World Applications:
//   - Trust & Safety / content-moderation analytics: measuring daily takedown
//     precision (true-positive spam report rate) across social platforms.
//   - Alert-triage telemetry in security operations (SOC): tracking daily
//     percentage of flagged incidents that resulted in confirmed remediation.
//   - In-memory OLAP / ETL hash-join and grouped distinct-count aggregation
//     pipelines.
//
// Related LeetCode Problems:
//   - #1113 Reported Posts [new] (predecessor SQL query counting distinct reported posts by reason)
//   - #614 Second Degree Follower [done] (SQL-modeled grouped distinct counting in C++)
//   - #1211 Queries Quality and Percentage [new] (grouped percentage averaging and rounding)
//   - #1174 Immediate Food Delivery II [new] (two-stage grouped percentage calculation)

#include "framework/lc_framework.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

struct ActionRow {
  int user_id = 0;
  int post_id = 0;
  std::string action_date;
  std::string action;
  std::string extra;
};

struct RemovalRow {
  int post_id = 0;
  std::string remove_date;
};

class Solution {
 public:
  // Computes the average daily percentage of spam-reported posts that were
  // removed, rounded to 2 decimal places.
  // Time: O(A + R) expected, Space: O(A + R).
  double averageDailyRemovedSpamPercent(
      const std::vector<ActionRow>& actions,
      const std::vector<RemovalRow>& removals) {
    // Build hash-join probe set of all removed post IDs.
    std::unordered_set<int> removed_posts;
    removed_posts.reserve(removals.size());
    for (const auto& row : removals) {
      removed_posts.insert(row.post_id);
    }

    // Group distinct spam-reported post IDs by `action_date`.
    std::unordered_map<std::string, std::unordered_set<int>> daily_spam_posts;
    for (const auto& row : actions) {
      if (row.action == "report" && row.extra == "spam") {
        daily_spam_posts[row.action_date].insert(row.post_id);
      }
    }

    if (daily_spam_posts.empty()) {
      return 0.0;
    }

    // Compute each qualifying day's removal percentage and average across days.
    double total_daily_pct = 0.0;
    for (const auto& entry : daily_spam_posts) {
      const auto& reported_posts = entry.second;
      int removed_count = 0;
      for (const int post_id : reported_posts) {
        if (removed_posts.contains(post_id)) {
          ++removed_count;
        }
      }
      const double daily_pct =
          (100.0 * static_cast<double>(removed_count)) /
          static_cast<double>(reported_posts.size());
      total_daily_pct += daily_pct;
    }

    const double avg_pct =
        total_daily_pct / static_cast<double>(daily_spam_posts.size());
    return std::round(avg_pct * 100.0) / 100.0;
  }
};

LC_PROBLEM(1132, "Reported Posts II", "hash", ctx) {
  constexpr double kEps = 1e-5;
  Solution sol;

  ctx.test("leetcode_official_example", [&] {
    // 2019-07-04: spam posts {2, 4}, removed {2} -> 50.0%
    // 2019-07-02: spam posts {3},    removed {3} -> 100.0%
    // 2019-07-03: only "racism" reports -> excluded
    // Average = (50 + 100) / 2 = 75.00%
    const std::vector<ActionRow> actions = {
        {.user_id = 1,
         .post_id = 1,
         .action_date = "2019-07-01",
         .action = "view",
         .extra = ""},
        {.user_id = 1,
         .post_id = 1,
         .action_date = "2019-07-01",
         .action = "like",
         .extra = ""},
        {.user_id = 1,
         .post_id = 1,
         .action_date = "2019-07-01",
         .action = "share",
         .extra = ""},
        {.user_id = 2,
         .post_id = 2,
         .action_date = "2019-07-04",
         .action = "view",
         .extra = ""},
        {.user_id = 2,
         .post_id = 2,
         .action_date = "2019-07-04",
         .action = "report",
         .extra = "spam"},
        {.user_id = 3,
         .post_id = 4,
         .action_date = "2019-07-04",
         .action = "view",
         .extra = ""},
        {.user_id = 3,
         .post_id = 4,
         .action_date = "2019-07-04",
         .action = "report",
         .extra = "spam"},
        {.user_id = 4,
         .post_id = 3,
         .action_date = "2019-07-02",
         .action = "view",
         .extra = ""},
        {.user_id = 4,
         .post_id = 3,
         .action_date = "2019-07-02",
         .action = "report",
         .extra = "spam"},
        {.user_id = 5,
         .post_id = 2,
         .action_date = "2019-07-03",
         .action = "view",
         .extra = ""},
        {.user_id = 5,
         .post_id = 2,
         .action_date = "2019-07-03",
         .action = "report",
         .extra = "racism"},
        {.user_id = 5,
         .post_id = 5,
         .action_date = "2019-07-03",
         .action = "view",
         .extra = ""},
        {.user_id = 5,
         .post_id = 5,
         .action_date = "2019-07-03",
         .action = "report",
         .extra = "racism"},
    };
    const std::vector<RemovalRow> removals = {
        {.post_id = 2, .remove_date = "2019-07-20"},
        {.post_id = 3, .remove_date = "2019-07-18"},
    };
    const double actual =
        sol.averageDailyRemovedSpamPercent(actions, removals);
    ctx.expect_true(std::fabs(actual - 75.0) <= kEps);
  });

  ctx.test("duplicate_spam_reports_same_day_deduplicated", [&] {
    // Post 2 reported 3 times on 2019-07-04, post 4 reported once -> distinct
    // spam posts = {2, 4}, removed = {2} -> 50.00% (not 75.00%).
    const std::vector<ActionRow> actions = {
        {.user_id = 1,
         .post_id = 2,
         .action_date = "2019-07-04",
         .action = "report",
         .extra = "spam"},
        {.user_id = 2,
         .post_id = 2,
         .action_date = "2019-07-04",
         .action = "report",
         .extra = "spam"},
        {.user_id = 2,
         .post_id = 2,
         .action_date = "2019-07-04",
         .action = "report",
         .extra = "spam"},
        {.user_id = 3,
         .post_id = 4,
         .action_date = "2019-07-04",
         .action = "report",
         .extra = "spam"},
    };
    const std::vector<RemovalRow> removals = {
        {.post_id = 2, .remove_date = "2019-07-05"},
    };
    const double actual =
        sol.averageDailyRemovedSpamPercent(actions, removals);
    ctx.expect_true(std::fabs(actual - 50.0) <= kEps);
  });

  ctx.test("rounding_to_two_decimal_places", [&] {
    // 1 of 3 distinct spam posts removed -> 33.3333...% -> rounds to 33.33%.
    const std::vector<ActionRow> actions = {
        {.user_id = 1,
         .post_id = 10,
         .action_date = "2019-07-01",
         .action = "report",
         .extra = "spam"},
        {.user_id = 2,
         .post_id = 11,
         .action_date = "2019-07-01",
         .action = "report",
         .extra = "spam"},
        {.user_id = 3,
         .post_id = 12,
         .action_date = "2019-07-01",
         .action = "report",
         .extra = "spam"},
    };
    const std::vector<RemovalRow> removals = {
        {.post_id = 10, .remove_date = "2019-07-02"},
    };
    const double actual =
        sol.averageDailyRemovedSpamPercent(actions, removals);
    ctx.expect_true(std::fabs(actual - 33.33) <= kEps);
  });

  ctx.test("no_spam_reports_returns_zero", [&] {
    const std::vector<ActionRow> actions = {
        {.user_id = 1,
         .post_id = 1,
         .action_date = "2019-07-01",
         .action = "report",
         .extra = "racism"},
        {.user_id = 2,
         .post_id = 2,
         .action_date = "2019-07-01",
         .action = "view",
         .extra = "spam"},
    };
    const std::vector<RemovalRow> removals = {
        {.post_id = 1, .remove_date = "2019-07-02"},
    };
    const double actual =
        sol.averageDailyRemovedSpamPercent(actions, removals);
    ctx.expect_true(std::fabs(actual - 0.0) <= kEps);
  });
}

}  // namespace
