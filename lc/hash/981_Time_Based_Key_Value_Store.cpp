// LeetCode 981: Time Based Key Value Store
// https://leetcode.com/problems/time-based-key-value-store/
//
// Problem:
//   Design a time-based key-value store (`TimeMap`) that stores multiple values
//   for the same key at different timestamps and retrieves a key's value at any
//   queried timestamp:
//   - `set(key, value, timestamp)`: stores `value` for `key` at `timestamp`.
//   - `get(key, timestamp)`: returns the value set at the largest
//     `timestamp_prev <= timestamp` for `key`, or `""` if no such entry exists.
//   Constraints: 1 <= key.length, value.length <= 100, 1 <= timestamp <= 10^7,
//   timestamps passed to `set` are strictly increasing, at most 2 * 10^5 calls.
//
// Algorithm:
//   Hash Map of Timestamp-Sorted Version Vectors + Binary Search:
//   - Map each `key` to a contiguous vector of `(timestamp, value)` entries kept
//     in ascending timestamp order.
//   - `set`: when `timestamp` is strictly greater than the latest stored
//     timestamp (the standard LeetCode guarantee), append in amortized O(1). If
//     an out-of-order timestamp is ever supplied, insert at the sorted position
//     via `std::ranges::upper_bound` to preserve the sorted invariant.
//   - `get`: look up the key's version vector in O(1) expected time, then use
//     `std::ranges::upper_bound` projected on `Entry::timestamp` to find the
//     first entry with `entry.timestamp > timestamp`. If that iterator is
//     `begin()`, no version exists at or before `timestamp` so return `""`;
//     otherwise return the immediately preceding entry's value.
//   Time Complexity: `set` is O(1) amortized (for strictly increasing
//   timestamps); `get` is O(log k), where k is the number of stored versions
//   for the queried key.
//   Space Complexity: O(N) total across N `set` operations.
//
// Data Structures:
//   - Hash map (`std::unordered_map<std::string, std::vector<Entry>>`): O(1)
//     expected dispatch from key to its per-key history.
//   - Sorted dynamic array (`std::vector<Entry>`): cache-friendly contiguous
//     version log supporting O(1) amortized append and O(log k) binary search.
//
// Real-World Applications:
//   - Multi-Version Concurrency Control (MVCC) in databases (PostgreSQL,
//     Spanner, CockroachDB): point-in-time snapshot reads for a row key.
//   - Time-series databases and feature stores (Prometheus, Feast): "as-of"
//     joins retrieving the latest known feature value at or before event time.
//   - Configuration management and audit logs: querying what configuration or
//     secret value was active at a historical incident timestamp.
//
// Related LeetCode Problems:
//   - #1146 Snapshot Array [done] (per-index versioned history with binary search over snapshot IDs)
//   - #146 LRU Cache [done] (hash-map-backed key-value store design)
//   - #729 My Calendar I [done] (ordered timestamp interval queries)
//   - #2034 Stock Price Fluctuation [done] (timestamped price updates with historical/extreme queries)

#include "framework/lc_framework.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class TimeMap {
 public:
  // Stores `(timestamp, value)` for `key`, preserving ascending timestamp
  // order. Amortized O(1) when timestamps are strictly increasing.
  void set(const std::string& key, std::string value, int timestamp) {
    auto& entries = store_[key];
    if (entries.empty() || entries.back().timestamp < timestamp) {
      entries.push_back(
          {.timestamp = timestamp, .value = std::move(value)});
      return;
    }

    // Fallback for out-of-order timestamps: maintain sorted invariant so
    // binary search in `get()` remains valid.
    const auto it = std::ranges::upper_bound(entries, timestamp, {},
                                             &Entry::timestamp);
    entries.insert(it, {.timestamp = timestamp, .value = std::move(value)});
  }

  // Returns the value with the largest `timestamp_prev <= timestamp` for `key`,
  // or `""` if no such entry exists. Time: O(log k).
  [[nodiscard]] std::string get(const std::string& key, int timestamp) const {
    const auto map_it = store_.find(key);
    if (map_it == store_.end()) {
      return "";
    }

    const auto& entries = map_it->second;
    // Find the first entry whose timestamp is strictly greater than `timestamp`.
    const auto it = std::ranges::upper_bound(entries, timestamp, {},
                                             &Entry::timestamp);
    if (it == entries.begin()) {
      return "";
    }
    return std::prev(it)->value;
  }

 private:
  struct Entry {
    int timestamp = 0;
    std::string value;
  };

  std::unordered_map<std::string, std::vector<Entry>> store_;
};

LC_PROBLEM(981, "Time Based Key Value Store", "hash", ctx) {
  ctx.test("example_1_basic_timeline", [&] {
    TimeMap time_map;
    time_map.set("foo", "bar", 1);
    ctx.expect_eq(time_map.get("foo", 1), "bar");
    ctx.expect_eq(time_map.get("foo", 3), "bar");

    time_map.set("foo", "bar2", 4);
    ctx.expect_eq(time_map.get("foo", 4), "bar2");
    ctx.expect_eq(time_map.get("foo", 5), "bar2");
    // Historical read still resolves to the earlier version.
    ctx.expect_eq(time_map.get("foo", 2), "bar");
  });

  ctx.test("before_first_timestamp_and_missing_key", [&] {
    TimeMap time_map;
    time_map.set("love", "high", 10);
    time_map.set("love", "low", 20);

    ctx.expect_eq(time_map.get("love", 5), "");
    ctx.expect_eq(time_map.get("love", 10), "high");
    ctx.expect_eq(time_map.get("love", 15), "high");
    ctx.expect_eq(time_map.get("love", 20), "low");
    ctx.expect_eq(time_map.get("love", 25), "low");
    ctx.expect_eq(time_map.get("missing", 100), "");
  });

  ctx.test("multiple_independent_keys", [&] {
    TimeMap time_map;
    time_map.set("a", "v1", 1);
    time_map.set("b", "x1", 2);
    time_map.set("a", "v2", 5);
    time_map.set("b", "x2", 6);

    ctx.expect_eq(time_map.get("a", 4), "v1");
    ctx.expect_eq(time_map.get("b", 1), "");
    ctx.expect_eq(time_map.get("b", 5), "x1");
    ctx.expect_eq(time_map.get("a", 10), "v2");
    ctx.expect_eq(time_map.get("b", 10), "x2");
  });

  ctx.test("out_of_order_insertion_preserves_order", [&] {
    TimeMap time_map;
    time_map.set("k", "late", 10);
    time_map.set("k", "early", 3);
    time_map.set("k", "mid", 7);

    ctx.expect_eq(time_map.get("k", 2), "");
    ctx.expect_eq(time_map.get("k", 3), "early");
    ctx.expect_eq(time_map.get("k", 8), "mid");
    ctx.expect_eq(time_map.get("k", 12), "late");
  });
}

}  // namespace
