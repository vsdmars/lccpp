// LeetCode 682: Baseball Game
// https://leetcode.com/problems/baseball-game/
//
// Problem:
//   You are keeping scores for a baseball game with special rules, starting
//   from an empty score record. Given a sequence of string operations where
//   each element is one of:
//     - An integer string `x` (possibly negative): record a new score of `x`.
//     - `"+"`: record a new score equal to the sum of the previous two valid
//       scores.
//     - `"D"`: record a new score equal to double the previous valid score.
//     - `"C"`: invalidate and remove the previous valid score from the record.
//   Return the sum of all valid scores remaining on the record after applying
//   all operations.
//
// Examples:
//   Example 1:
//     Input:  ops = ["5", "2", "C", "D", "+"]
//     Output: 30
//     Explanation:
//       - Parse "5" and "2" with std::stoi to push them onto the stack, giving
//         scores = [5, 2].
//       - Process "C" by popping the top element, reducing scores to [5].
//       - Process "D" by doubling scores.back() (5 * 2 = 10) and pushing it, giving
//         scores = [5, 10].
//       - Process "+" by adding the top two scores (10 + 5 = 15) and pushing 15,
//         giving scores = [5, 10, 15].
//       - Sum all remaining elements in scores with std::accumulate (5 + 10 + 15)
//         to return 30.
//
//   Example 2:
//     Input:  ops = ["5", "-2", "4", "C", "D", "9", "+", "+"]
//     Output: 27
//     Explanation:
//       - Parse the initial integer tokens "5", "-2", and "4" sequentially,
//         building scores = [5, -2, 4].
//       - Apply "C" to pop 4 (scores = [5, -2]), then apply "D" to double -2 and
//         push -4 (scores = [5, -2, -4]).
//       - Push 9 for token "9", then apply the first "+" to push 9 + (-4) = 5,
//         yielding scores = [5, -2, -4, 9, 5].
//       - Apply the second "+" to push 5 + 9 = 14 (scores = [5, -2, -4, 9, 5, 14]),
//         and sum the stack (5 - 2 - 4 + 9 + 5 + 14) to return 27.
//
//   Example 3:
//     Input:  ops = ["1", "C"]
//     Output: 0
//     Explanation:
//       - Parse "1" via std::stoi and push 1 onto the stack, giving scores = [1].
//       - Process "C" by seeing !scores.empty() and popping 1, leaving scores
//         empty.
//       - Sum the empty scores vector with std::accumulate starting from 0 to
//         return 0.
//
// Algorithm:
//   Stack-Based Simulation (O(n) time, O(n) space):
//     Maintain a dynamic array (`std::vector<int>`) acting as a LIFO stack of
//     active valid scores. Iterate through each operation string once:
//       - `"C"` pops the top score.
//       - `"D"` pushes `2 * top`.
//       - `"+"` pushes the sum of the top two scores.
//       - Otherwise parse the signed integer with `std::stoi` and push it
//         (avoiding naive `isdigit` checks that fail on negative numbers).
//     Finally, sum all remaining elements in the stack.
//
//   Time Complexity: O(n), where `n` is the number of operations (each
//   operation performs O(1) amortized stack work).
//   Space Complexity: O(n) in the worst case when all operations push scores.
//
// Data Structures:
//   - `std::vector<int>` used as a stack so both LIFO pop (`"C"`), top
//     inspection (`"D"`), and second-from-top inspection (`"+"`) run in O(1)
//     time with contiguous cache-friendly storage.
//
// Real-World Applications:
//   - Undo/redo command histories in text editors, CAD tools, and spreadsheet
//     engines where recent entries can be cancelled or referenced.
//   - Stack-based virtual machines and Reverse Polish Notation (RPN)
//     expression evaluators (e.g., Forth, PostScript, JVM/WebAssembly frames).
//   - Financial ledger and streaming event-log reconciliation handling void /
//     reversal entries alongside derived adjustments.
//
// Related LeetCode Problems:
//   - #150 Evaluate Reverse Polish Notation [new] (classic stack-based token evaluator)
//   - #155 Min Stack [done] (stack simulation with O(1) aggregate tracking)
//   - #844 Backspace String Compare [done] (stack simulation with cancellation tokens)
//   - #2390 Removing Stars From a String [new] (direct stack pop on cancel symbol)

#include "framework/lc_framework.h"

#include <numeric>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Simulates the baseball score record over `operations` and returns the
  // total sum of all valid scores remaining on the record.
  //
  // Invariant:
  //   After processing `operations[0..i]`, `scores` holds all valid uncancelled
  //   round scores in chronological order.
  //
  // Time Complexity: O(n), where `n = operations.size()`.
  // Space Complexity: O(n).
  int calPoints(const std::vector<std::string>& operations) {
    std::vector<int> scores;
    scores.reserve(operations.size());

    for (const std::string& op : operations) {
      if (op == "C") {
        // Invalidate and remove the most recent valid score.
        if (!scores.empty()) {
          scores.pop_back();
        }
      } else if (op == "D") {
        // Record double the most recent valid score.
        if (!scores.empty()) {
          scores.push_back(scores.back() * 2);
        }
      } else if (op == "+") {
        // Record the sum of the two most recent valid scores.
        if (scores.size() >= 2) {
          const int prev_two_sum =
              scores[scores.size() - 1] + scores[scores.size() - 2];
          scores.push_back(prev_two_sum);
        }
      } else {
        // Parse positive, zero, or negative integer score strings cleanly.
        scores.push_back(std::stoi(op));
      }
    }

    return std::accumulate(scores.begin(), scores.end(), 0);
  }
};

LC_PROBLEM(682, "Baseball Game", "array", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    const std::vector<std::string> ops = {"5", "2", "C", "D", "+"};
    ctx.expect_eq(sol.calPoints(ops), 30);
  });

  ctx.test("example_2_negative_scores", [&] {
    // Record progression: [5] -> [5,-2] -> [5,-2,4] -> [5,-2] -> [5,-2,-4]
    // -> [5,-2,-4,9] -> [5,-2,-4,9,5] -> [5,-2,-4,9,5,14]; total = 27.
    const std::vector<std::string> ops = {"5", "-2", "4", "C",
                                          "D", "9",  "+", "+"};
    ctx.expect_eq(sol.calPoints(ops), 27);
  });

  ctx.test("example_3_all_cancelled", [&] {
    const std::vector<std::string> ops = {"1", "C"};
    ctx.expect_eq(sol.calPoints(ops), 0);
  });

  ctx.test("single_negative_and_empty", [&] {
    ctx.expect_eq(sol.calPoints({"-10"}), -10);
    ctx.expect_eq(sol.calPoints({}), 0);
  });

  ctx.test("chained_doubles_and_additions", [&] {
    // [3, 6, 12, 18] -> sum = 39.
    const std::vector<std::string> ops = {"3", "D", "D", "+"};
    ctx.expect_eq(sol.calPoints(ops), 39);
  });
}

}  // namespace
