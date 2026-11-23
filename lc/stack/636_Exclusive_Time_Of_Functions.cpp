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
// Examples:
//   Example 1:
//     Input:  n = 2, logs = ["0:start:0", "1:start:2", "1:end:5", "0:end:6"]
//     Output: [3, 4]
//     Explanation:
//       - Process "0:start:0" and "1:start:2" by pushing frames {id: 0, start_time:
//         0} and {id: 1, start_time: 2} onto stack, with result = [0, 0].
//       - On "1:end:5", pop {1, 2}, compute duration = (5 - 2) + 1 = 4, add 4 to
//         result[1], and subtract 4 from caller result[0], giving [-4, 4].
//       - On "0:end:6", pop {0, 0} with an empty stack, compute duration = (6 - 0)
//         + 1 = 7, and add 7 to result[0], yielding [-4 + 7, 4] = [3, 4].
//
//   Example 2:
//     Input:  n = 1, logs = ["0:start:0", "0:start:2", "0:end:5", "0:start:6",
//             "0:end:6", "0:end:7"]
//     Output: [8]
//     Explanation:
//       - Push frames {0, 0} and {0, 2} onto stack for the first two start logs
//         while result = [0].
//       - On "0:end:5", pop {0, 2} with duration = (5 - 2) + 1 = 4, add 4 to
//         result[0] and subtract 4 from the remaining caller frame 0, keeping
//         result = [0].
//       - Push {0, 6} and pop it at "0:end:6" with duration = (6 - 6) + 1 = 1,
//         adding 1 and subtracting 1 from the outer frame 0 so result stays [0].
//       - On "0:end:7", pop the outermost {0, 0} with duration = (7 - 0) + 1 = 8
//         and an empty stack, adding 8 to result[0] for [8].
//
//   Example 3:
//     Input:  n = 5, logs = ["0:start:3", "1:start:4", "2:start:6", "2:end:7",
//             "1:end:8", "3:start:9", "3:end:10", "0:end:11", "4:start:12",
//             "4:end:19", "0:start:20", "0:end:22"]
//     Output: [5, 3, 2, 2, 8]
//     Explanation:
//       - After pushing 0, 1, and 2, "2:end:7" records duration 2 for function 2
//         (result[2] = 2) and deducts 2 from caller 1; "1:end:8" adds 5 to
//         result[1] (net 3) and deducts 5 from caller 0.
//       - "3:start:9" and "3:end:10" add 2 to result[3] and deduct 2 from caller 0
//         (result[0] = -7), then "0:end:11" adds (11 - 3) + 1 = 9 to bring
//         result[0] to 2.
//       - "4:start:12" and "4:end:19" run with no caller on the stack, adding (19 -
//         12) + 1 = 8 to result[4].
//       - "0:start:20" and "0:end:22" add (22 - 20) + 1 = 3 to result[0], producing
//         final exclusive times [5, 3, 2, 2, 8].
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
