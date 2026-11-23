// LeetCode 636: Exclusive Time Of Functions
// https://leetcode.com/problems/exclusive-time-of-functions/
//
// Problem:
//   Given `n` functions (IDs `0` through `n - 1`) running on a single-threaded,
//   non-preemptive CPU and a chronological list of execution logs formatted as
//   `"<function_id>:<start|end>:<timestamp>"`, compute the exclusive execution
//   time of every function (excluding time spent inside nested callees).
//   A `start` timestamp marks the beginning of that unit, while an `end`
//   timestamp marks the end of that unit (so `[start, end]` spans
//   `end - start + 1` units). Functions may call themselves recursively.
//
// Algorithm:
//   Simulate the CPU call stack as each log entry arrives:
//     - On a `start` event `(id, timestamp)`, push `{id, timestamp}` onto the
//       call stack.
//     - On an `end` event `(id, timestamp)`, pop the matching top frame
//       `{frame_id, start_time}` and compute its inclusive span
//       `duration = (timestamp - start_time) + 1`.
//       Add `duration` to `result[frame_id]`. If a caller frame remains on top
//       of the stack, subtract `duration` from `result[caller_id]` so that when
//       the caller eventually adds its own full inclusive span, the callee's
//       time is cleanly excluded (even across recursive calls).
//   Time Complexity:  O(m), where m is the number of log entries (or O(L) in
//                     total input characters across all log strings).
//   Space Complexity: O(m) worst-case call-stack depth plus O(n) output.
//
// Data Structures:
//   - Call stack (`std::vector<CallFrame>`): tracks active `(id, start_time)`
//     frames in LIFO order.
//   - Accumulator vector (`std::vector<int>`): stores per-function exclusive
//     time indexed by function ID.
//
// Real-World Applications:
//   - Sampling and instrumentation CPU profilers (perf, Instruments, FlameGraph
//     self-time vs. total-time attribution).
//   - Distributed tracing span analysis (computing self-duration of RPC spans
//     in OpenTelemetry / Jaeger).
//   - Database query execution plan profilers separating operator self-time
//     from child-operator time.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (matched start/end nesting via stack)
//   - #394 Decode String [done] (nested frame stack evaluation)
//   - #591 Tag Validator [done] (balanced start/end tag stack)
//   - #1441 Build an Array With Stack Operations [new] (stack simulation)
//   - #1614 Maximum Nesting Depth of the Parentheses [done] (call-depth tracking)

#include "framework/lc_framework.h"

#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the exclusive CPU time for each function `0..n-1` from execution
  // logs ordered by timestamp.
  // Time: O(L) total log characters, Space: O(n + stack depth).
  std::vector<int> exclusiveTime(int n, const std::vector<std::string>& logs) {
    std::vector<int> result(static_cast<std::size_t>(n), 0);
    std::vector<CallFrame> stack;
    stack.reserve(logs.size() / 2uz);

    for (const std::string& log : logs) {
      const LogEntry entry = parse_log(log);

      if (entry.is_start) {
        stack.push_back(CallFrame{.id = entry.id, .start_time = entry.time});
      } else if (!stack.empty()) {
        // Compute inclusive duration of the finishing frame.
        const CallFrame frame = stack.back();
        stack.pop_back();
        const int duration = (entry.time - frame.start_time) + 1;
        result[static_cast<std::size_t>(frame.id)] += duration;

        // Pre-deduct this callee's inclusive span from the immediate caller's
        // accumulator; when the caller ends and adds its own inclusive span,
        // the net contribution is purely the caller's exclusive time.
        if (!stack.empty()) {
          result[static_cast<std::size_t>(stack.back().id)] -= duration;
        }
      }
    }

    return result;
  }

 private:
  struct CallFrame {
    int id = 0;
    int start_time = 0;
  };

  struct LogEntry {
    int id = 0;
    bool is_start = false;
    int time = 0;
  };

  static int parse_int(std::string_view token) {
    int value = 0;
    const auto res =
        std::from_chars(token.data(), token.data() + token.size(), value);
    if (res.ec != std::errc{}) {
      return 0;
    }
    return value;
  }

  // Parses `"<id>:<start|end>:<timestamp>"` without allocating substrings.
  static LogEntry parse_log(std::string_view log) {
    const std::size_t first_colon = log.find(':');
    const std::size_t second_colon = log.find(':', first_colon + 1uz);
    const std::string_view id_part = log.substr(0uz, first_colon);
    const std::string_view op_part =
        log.substr(first_colon + 1uz, second_colon - (first_colon + 1uz));
    const std::string_view time_part = log.substr(second_colon + 1uz);

    return LogEntry{
        .id = parse_int(id_part),
        .is_start = (op_part == "start"),
        .time = parse_int(time_part),
    };
  }
};

LC_PROBLEM(636, "Exclusive Time Of Functions", "stack", ctx) {
  ctx.test("example_nested_two_functions", [&] {
    Solution sol;
    const std::vector<std::string> logs = {
        "0:start:0",
        "1:start:2",
        "1:end:5",
        "0:end:6",
    };
    ctx.expect_eq(sol.exclusiveTime(2, logs), std::vector<int>{3, 4});
  });

  ctx.test("source_build_five_functions", [&] {
    Solution sol;
    const std::vector<std::string> logs = {
        "0:start:3", "1:start:4", "2:start:6",  "2:end:7",
        "1:end:8",   "3:start:9", "3:end:10",   "0:end:11",
        "4:start:12", "4:end:19", "0:start:20", "0:end:22",
    };
    ctx.expect_eq(sol.exclusiveTime(5, logs), std::vector<int>{5, 3, 2, 2, 8});
  });

  ctx.test("recursive_self_calls", [&] {
    Solution sol;
    const std::vector<std::string> logs = {
        "0:start:0", "0:start:2", "0:end:5",
        "0:start:6", "0:end:6",   "0:end:7",
    };
    ctx.expect_eq(sol.exclusiveTime(1, logs), std::vector<int>{8});
  });

  ctx.test("mutual_recursion", [&] {
    Solution sol;
    const std::vector<std::string> logs = {
        "0:start:0", "1:start:2", "0:start:3",
        "0:end:4",   "1:end:5",   "0:end:6",
    };
    ctx.expect_eq(sol.exclusiveTime(2, logs), std::vector<int>{5, 2});
  });

  ctx.test("single_unit_call", [&] {
    Solution sol;
    const std::vector<std::string> logs = {"0:start:0", "0:end:0"};
    ctx.expect_eq(sol.exclusiveTime(1, logs), std::vector<int>{1});
  });
}

}  // namespace
