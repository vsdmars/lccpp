// LeetCode 635: Design Log Storage System
// https://leetcode.com/problems/design-log-storage-system/
//
// Problem:
//   Design a log storage system storing entries with a unique integer `id` and
//   a fixed-format zero-padded timestamp `"Year:Month:Day:Hour:Minute:Second"`
//   (e.g., `"2017:01:01:23:59:59"`). Support two operations:
//   - `put(id, timestamp)`: insert a log entry.
//   - `retrieve(start, end, granularity)`: return the IDs of all logs whose
//     timestamps fall inclusively within `[start, end]` when truncated to the
//     specified `granularity` (`"Year"`, `"Month"`, `"Day"`, `"Hour"`,
//     `"Minute"`, or `"Second"`).
//   Constraints: at most 500 calls total; output order is not constrained.
//
// Examples:
//   Example 1:
//     Input:  operations = ["LogSystem", "put", "put", "put", "retrieve",
//             "retrieve"], args = [[], [1, "2017:01:01:23:59:59"], [2,
//             "2017:01:01:22:59:59"], [3, "2016:01:01:00:00:00"],
//             ["2016:01:01:01:01:01", "2017:01:01:23:00:00", "Year"],
//             ["2016:01:01:01:01:01", "2017:01:01:23:00:00", "Hour"]]
//     Output: [null, null, null, null, [1, 2, 3], [1, 2]]
//     Explanation:
//       - The three put calls append (1, "2017:01:01:23:59:59"), (2,
//         "2017:01:01:22:59:59"), and (3, "2016:01:01:00:00:00") into logs_.
//       - For granularity "Year" (prefix length 4), start_prefix = "2016" and
//         end_prefix = "2017" include all three stored prefixes ("2017", "2017",
//         "2016"), returning sorted IDs [1, 2, 3].
//       - For granularity "Hour" (prefix length 13), start_prefix = "2016:01:01:01"
//         and end_prefix = "2017:01:01:23" match logs 1 ("2017:01:01:23") and 2
//         ("2017:01:01:22") while excluding log 3 ("2016:01:01:00"), returning [1,
//         2].
//
//   Example 2:
//     Input:  operations = ["LogSystem", "put", "put", "put", "retrieve",
//             "retrieve"], args = [[], [10, "2017:01:01:23:59:59"], [20,
//             "2017:01:02:00:00:00"], [30, "2017:02:01:00:00:00"],
//             ["2017:01:01:23:59:00", "2017:01:01:23:59:58", "Minute"],
//             ["2017:01:01:23:59:00", "2017:01:01:23:59:58", "Second"]]
//     Output: [null, null, null, null, [10], []]
//     Explanation:
//       - Store entries 10 ("2017:01:01:23:59:59"), 20 ("2017:01:02:00:00:00"), and
//         30 ("2017:02:01:00:00:00") in logs_.
//       - For "Minute" (prefix length 16), both bounds truncate to
//         "2017:01:01:23:59", matching only log 10 and returning [10].
//       - For "Second" (prefix length 19), log 10's "2017:01:01:23:59:59" exceeds
//         end_prefix = "2017:01:01:23:59:58" while logs 20 and 30 also fall
//         outside, returning [].
//
//   Example 3:
//     Input:  operations = ["LogSystem", "retrieve"], args = [[],
//             ["2000:01:01:00:00:00", "2017:12:31:23:59:59", "Year"]]
//     Output: [null, []]
//     Explanation:
//       - Constructing LogSystem initializes logs_ as an empty vector.
//       - Calling retrieve with granularity "Year" (prefix length 4) sets
//         start_prefix = "2000" and end_prefix = "2017", iterates over zero entries
//         in logs_, and returns [].
//
// Algorithm:
//   Prefix-truncation lexicographical comparison:
//   Because every timestamp field is fixed-width and zero-padded in big-endian
//   time order (`YYYY:MM:DD:HH:MM:SS`), lexicographical string comparison on
//   the prefix corresponding to `granularity` is mathematically identical to
//   chronological comparison at that granularity:
//     - `"Year"`   -> prefix length 4  (`"YYYY"`)
//     - `"Month"`  -> prefix length 7  (`"YYYY:MM"`)
//     - `"Day"`    -> prefix length 10 (`"YYYY:MM:DD"`)
//     - `"Hour"`   -> prefix length 13 (`"YYYY:MM:DD:HH"`)
//     - `"Minute"` -> prefix length 16 (`"YYYY:MM:DD:HH:MM"`)
//     - `"Second"` -> prefix length 19 (`"YYYY:MM:DD:HH:MM:SS"`)
//   `put` appends `(id, timestamp)` in O(1) amortized time. `retrieve` slices
//   `start`, `end`, and each stored timestamp to `prefix_len` using zero-copy
//   `std::string_view::substr`, collects all matching IDs, and sorts them for
//   deterministic test verification.
//   Time Complexity: O(1) amortized for `put`; O(N + K log K) for `retrieve`
//   where N is stored log count and K is matching result count (O(N) without
//   sorting).
//   Space Complexity: O(N) to store all inserted log entries.
//
// Data Structures:
//   - `std::vector<LogEntry>`: contiguous storage for `(id, timestamp)` pairs.
//   - `std::array<std::pair<std::string_view, std::size_t>, 6>`: compile-time
//     lookup table mapping each granularity name to its exact prefix length.
//
// Real-World Applications:
//   - Time-series log aggregators (e.g., Elasticsearch, Cloud Logging, Loki)
//     indexing ISO-8601 timestamps lexicographically for prefix/range queries.
//   - Distributed audit trails and metrics rollups querying event windows at
//     configurable bucket granularities (yearly, daily, hourly, per-second).
//   - Partitioned data lakes (Hive/Parquet path prefixes like `YYYY/MM/DD/HH`)
//     pruning partitions via string range comparisons.
//
// Related LeetCode Problems:
//   - #981 Time Based Key-Value Store [done] (storing timestamped values with binary-search retrieval)
//   - #362 Design Hit Counter [done] (tracking and querying event counts within sliding time windows)
//   - #1348 Tweet Counts Per Frequency [new] (bucketing timestamped events by minute/hour/day granularity)
//   - #729 My Calendar I [done] (interval range queries on chronological booking records)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class LogSystem {
 public:
  // Stores a log entry with its unique `id` and zero-padded `timestamp`.
  // Time: O(1) amortized, Space: O(1) per entry.
  void put(int id, std::string timestamp) {
    logs_.push_back(LogEntry{.id = id, .timestamp = std::move(timestamp)});
  }

  // Returns all log IDs whose timestamps fall within [start, end] at the
  // requested `granularity` (sorted ascending for deterministic output).
  // Time: O(N + K log K), Space: O(K) for output.
  [[nodiscard]] std::vector<int> retrieve(std::string_view start,
                                          std::string_view end,
                                          std::string_view granularity) const {
    const std::size_t len = prefix_length(granularity);
    const std::string_view start_prefix = start.substr(0uz, len);
    const std::string_view end_prefix = end.substr(0uz, len);

    std::vector<int> matched_ids;
    for (const auto& entry : logs_) {
      // Zero-copy prefix slice works because fields are fixed-width & padded.
      const std::string_view ts_prefix =
          std::string_view(entry.timestamp).substr(0uz, len);
      if (start_prefix <= ts_prefix && ts_prefix <= end_prefix) {
        matched_ids.push_back(entry.id);
      }
    }

    std::ranges::sort(matched_ids);
    return matched_ids;
  }

 private:
  struct LogEntry {
    int id = 0;
    std::string timestamp;
  };

  // Fixed-width prefix lengths for "YYYY:MM:DD:HH:MM:SS".
  static constexpr std::array<std::pair<std::string_view, std::size_t>, 6>
      kGranularityPrefixLen = {{
          {"Year", 4uz},
          {"Month", 7uz},
          {"Day", 10uz},
          {"Hour", 13uz},
          {"Minute", 16uz},
          {"Second", 19uz},
      }};

  // Maps a granularity name to the corresponding timestamp prefix length.
  static std::size_t prefix_length(std::string_view granularity) {
    for (const auto& [name, length] : kGranularityPrefixLen) {
      if (name == granularity) {
        return length;
      }
    }
    // Fallback to full second-level precision if an unrecognized value arrives.
    return 19uz;
  }

  std::vector<LogEntry> logs_;
};

LC_PROBLEM(635, "Design Log Storage System", "string", ctx) {
  ctx.test("example_1_year_and_hour", [&] {
    LogSystem log_sys;
    log_sys.put(1, "2017:01:01:23:59:59");
    log_sys.put(2, "2017:01:01:22:59:59");
    log_sys.put(3, "2016:01:01:00:00:00");

    ctx.expect_eq(
        log_sys.retrieve("2016:01:01:01:01:01", "2017:01:01:23:00:00", "Year"),
        std::vector<int>{1, 2, 3});
    ctx.expect_eq(
        log_sys.retrieve("2016:01:01:01:01:01", "2017:01:01:23:00:00", "Hour"),
        std::vector<int>{1, 2});
  });

  ctx.test("all_granularities_and_boundaries", [&] {
    LogSystem log_sys;
    log_sys.put(10, "2017:01:01:23:59:59");
    log_sys.put(20, "2017:01:02:00:00:00");
    log_sys.put(30, "2017:02:01:00:00:00");

    ctx.expect_eq(
        log_sys.retrieve("2017:01:01:00:00:00", "2017:01:01:00:00:00", "Day"),
        std::vector<int>{10});
    ctx.expect_eq(
        log_sys.retrieve("2017:01:01:00:00:00", "2017:01:31:23:59:59", "Month"),
        std::vector<int>{10, 20});
    ctx.expect_eq(log_sys.retrieve("2017:01:01:23:59:00", "2017:01:01:23:59:58",
                                   "Minute"),
                  std::vector<int>{10});
    ctx.expect_eq(log_sys.retrieve("2017:01:01:23:59:00", "2017:01:01:23:59:58",
                                   "Second"),
                  std::vector<int>{});
  });

  ctx.test("empty_system_returns_empty", [&] {
    LogSystem log_sys;
    ctx.expect_eq(
        log_sys.retrieve("2000:01:01:00:00:00", "2017:12:31:23:59:59", "Year"),
        std::vector<int>{});
  });
}

}  // namespace
