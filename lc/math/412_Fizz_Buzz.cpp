// LeetCode 412: Fizz Buzz
// https://leetcode.com/problems/fizz-buzz/
//
// Problem:
//   Given an integer `n` (1 <= n <= 10^4), return a 1-indexed string array
//   `answer` of length `n` where for each i in [1, n]:
//     - answer[i - 1] == "FizzBuzz" if i is divisible by both 3 and 5,
//     - answer[i - 1] == "Fizz" if i is divisible by 3,
//     - answer[i - 1] == "Buzz" if i is divisible by 5,
//     - answer[i - 1] == decimal string of i otherwise.
//
// Algorithm:
//   Pre-allocate the output vector to size `n` and iterate i from 1 through n.
//   Check divisibility by 3 and 5 (equivalently divisibility by lcm(3, 5) = 15)
//   first so multiples of 15 emit "FizzBuzz" before falling through to single
//   divisor checks or `std::to_string(i)`.
//   Time Complexity: O(n) (each number formats at most O(log10 n) <= 5 chars).
//   Space Complexity: O(1) auxiliary space beyond the O(n) output array.
//
// Data Structures:
//   `std::vector<std::string>` pre-reserved to `n` elements to avoid
//   reallocations while appending.
//
// Real-World Applications:
//   Rule-based token classification pipelines, periodic task/event scheduling
//   with overlapping intervals (handling LCM collisions first), and basic
//   report formatting.
//
// Related LeetCode Problems:
//   - #1195 Fizz Buzz Multithreaded [new] (concurrent synchronization across Fizz/Buzz/FizzBuzz threads)
//   - #2778 Sum of Squares of Special Elements [new] (1-indexed divisibility filtering)
//   - #2652 Sum Multiples [new] (divisibility classification across multiples of 3, 5, and 7)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Generates the 1-indexed Fizz Buzz sequence of length `n`.
  // Time: O(n), Space: O(1) auxiliary (O(n) for the returned vector).
  std::vector<std::string> fizzBuzz(int n) {
    if (n < 1) {
      return {};
    }

    std::vector<std::string> result;
    result.reserve(static_cast<std::size_t>(n));

    for (int i = 1; i <= n; ++i) {
      const bool div_by_3 = (i % 3) == 0;
      const bool div_by_5 = (i % 5) == 0;

      // Test the combined condition first so multiples of 15 map to "FizzBuzz".
      if (div_by_3 && div_by_5) {
        result.emplace_back("FizzBuzz");
      } else if (div_by_3) {
        result.emplace_back("Fizz");
      } else if (div_by_5) {
        result.emplace_back("Buzz");
      } else {
        result.push_back(std::to_string(i));
      }
    }

    return result;
  }
};

LC_PROBLEM(412, "Fizz Buzz", "math", ctx) {
  ctx.test("example_1_n_3", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"1", "2", "Fizz"};
    ctx.expect_eq(sol.fizzBuzz(3), expected);
  });

  ctx.test("example_2_n_5", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"1", "2", "Fizz", "4", "Buzz"};
    ctx.expect_eq(sol.fizzBuzz(5), expected);
  });

  ctx.test("example_3_n_15", [&] {
    Solution sol;
    const std::vector<std::string> expected = {
        "1",    "2",    "Fizz", "4",    "Buzz", "Fizz", "7",       "8",
        "Fizz", "Buzz", "11",   "Fizz", "13",   "14",   "FizzBuzz"};
    ctx.expect_eq(sol.fizzBuzz(15), expected);
  });

  ctx.test("edge_n_1_and_non_positive", [&] {
    Solution sol;
    const std::vector<std::string> single = {"1"};
    const std::vector<std::string> empty;
    ctx.expect_eq(sol.fizzBuzz(1), single);
    ctx.expect_eq(sol.fizzBuzz(0), empty);
  });
}

}  // namespace
