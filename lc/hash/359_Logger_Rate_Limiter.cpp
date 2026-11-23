// LeetCode 359: Logger Rate Limiter
// https://leetcode.com/problems/logger-rate-limiter/
//
// Problem:
//   Design a logger system that receives a stream of messages along with their
//   integer timestamps (in seconds, arriving in non-decreasing order). Each
//   unique message should be printed if and only if it has not been printed in
//   the last 10 seconds (i.e., if printed at time t, identical messages are
//   suppressed for timestamps in [t, t + 9] and allowed again at t + 10).
//   Implement `shouldPrintMessage(timestamp, message)` returning true if the
//   message should be printed and false otherwise.
//
// Algorithm:
//   Maintain a hash map storing, for each previously printed message, the
//   earliest next timestamp at which it is allowed to be printed again
//   (`printed_timestamp + 10`). When `shouldPrintMessage(timestamp, message)`
//   is called:
//   1. Look up `message` in the map.
//   2. If found and `timestamp < next_allowed_time`, reject the message and
//      return false WITHOUT updating the stored timestamp (suppressed calls do
//      not extend the cooldown window).
//   3. Otherwise, record `next_allowed_time[message] = timestamp + 10` and
//      return true.
//   Time Complexity: O(1) average per call (O(L) for hashing a message of
//   length L).
//   Space Complexity: O(M) where M is the number of distinct messages printed.
//
// Data Structures:
//   - std::unordered_map<std::string, int>: maps each message string to its
//     next allowed print timestamp.
//
// Real-World Applications:
//   - Log deduplication and alert-storm suppression in observability stacks
//     (e.g., syslog rate-limiting, Prometheus Alertmanager silence windows).
//   - API gateway per-key fixed/sliding-window throttling and abuse prevention.
//   - Notification debouncing in distributed monitoring and incident paging.
//
// Related LeetCode Problems:
//   - #362 Design Hit Counter [done] (time-window event tracking over a stream of timestamps)
//   - #380 Insert Delete GetRandom O [done] (1) (O(1) hash-backed online data structure design)
//   - #146 LRU Cache [done] (hash-map-backed online eviction / state tracking)
//   - #3508 Implement Router [new] (stream packet deduplication and timestamp rate tracking)

#include "framework/lc_framework.h"

#include <string>
#include <unordered_map>

namespace {

// Rate-limiting logger that suppresses duplicate messages within a 10-second
// cooldown window after each printed occurrence.
class Logger {
 public:
  // Returns true if `message` has not been printed within the previous 10
  // seconds and records its new next-allowed timestamp; returns false otherwise
  // without extending the existing cooldown.
  //
  // Time Complexity: O(1) average (linear in message length for hashing).
  // Space Complexity: O(M) across M unique printed messages.
  bool shouldPrintMessage(int timestamp, const std::string& message) {
    const auto it = next_allowed_time_.find(message);
    if (it != next_allowed_time_.end() && timestamp < it->second) {
      return false;
    }

    // Message is allowed: block duplicates until timestamp + 10.
    next_allowed_time_[message] = timestamp + kCooldownSeconds;
    return true;
  }

 private:
  static constexpr int kCooldownSeconds = 10;

  // Maps each printed message to the earliest timestamp it may print again.
  std::unordered_map<std::string, int> next_allowed_time_;
};

LC_PROBLEM(359, "Logger Rate Limiter", "hash", ctx) {
  ctx.test("leetcode_example_sequence", [&] {
    Logger logger;
    ctx.expect_true(logger.shouldPrintMessage(1, "foo"));
    ctx.expect_true(logger.shouldPrintMessage(2, "bar"));
    ctx.expect_false(logger.shouldPrintMessage(3, "foo"));
    ctx.expect_false(logger.shouldPrintMessage(8, "bar"));
    ctx.expect_false(logger.shouldPrintMessage(10, "foo"));
    ctx.expect_true(logger.shouldPrintMessage(11, "foo"));
  });

  ctx.test("zero_timestamp_and_exact_boundary", [&] {
    Logger logger;
    ctx.expect_true(logger.shouldPrintMessage(0, "init"));
    ctx.expect_false(logger.shouldPrintMessage(0, "init"));
    ctx.expect_false(logger.shouldPrintMessage(9, "init"));
    ctx.expect_true(logger.shouldPrintMessage(10, "init"));
    ctx.expect_false(logger.shouldPrintMessage(19, "init"));
    ctx.expect_true(logger.shouldPrintMessage(20, "init"));
  });

  ctx.test("suppressed_messages_do_not_reset_cooldown", [&] {
    Logger logger;
    ctx.expect_true(logger.shouldPrintMessage(5, "warn"));
    ctx.expect_false(logger.shouldPrintMessage(14, "warn"));
    // Cooldown started at t = 5, so t = 15 must succeed even though t = 14 was
    // rejected.
    ctx.expect_true(logger.shouldPrintMessage(15, "warn"));
  });

  ctx.test("multiple_messages_same_timestamp", [&] {
    Logger logger;
    ctx.expect_true(logger.shouldPrintMessage(100, "a"));
    ctx.expect_true(logger.shouldPrintMessage(100, "b"));
    ctx.expect_true(logger.shouldPrintMessage(100, "c"));
    ctx.expect_false(logger.shouldPrintMessage(100, "b"));
    ctx.expect_true(logger.shouldPrintMessage(110, "b"));
  });
}

}  // namespace
