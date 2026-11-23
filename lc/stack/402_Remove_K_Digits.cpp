// LeetCode 402: Remove K Digits
// https://leetcode.com/problems/remove-k-digits/
//
// Problem:
//   Given a non-negative integer represented as a decimal string `num` and an
//   integer `k`, remove `k` digits from `num` so that the resulting integer is
//   the smallest possible. Return the answer as a string with no leading zeros
//   (returning `"0"` if all digits are removed or only zeros remain).
//
// Examples:
//   Example 1:
//     Input:  num = "1432219", k = 3
//     Output: "1219"
//     Explanation:
//       - Start with stack = "" and remaining = 3, pushing '1' then '4' to get
//         stack = "14".
//       - At '3', pop '4' since '4' > '3' and remaining = 3 > 0 (remaining = 2),
//         then push '3' (stack = "13").
//       - At the first '2', pop '3' (remaining = 1) and push '2' (stack = "12"); at
//         the second '2', '2' > '2' is false so push it (stack = "122").
//       - At '1', pop '2' (remaining = 0) and push '1' (stack = "121"); with
//         remaining == 0, push '9' unchanged to form "1219".
//       - strip_leading_zeros("1219") finds '1' at index 0 and returns "1219".
//
//   Example 2:
//     Input:  num = "10200", k = 1
//     Output: "200"
//     Explanation:
//       - Start with stack = "" and remaining = 1, pushing '1' so stack = "1".
//       - At '0', pop '1' since '1' > '0' and remaining = 1 > 0 (remaining = 0),
//         then push '0' (stack = "0").
//       - With remaining == 0, push the remaining digits '2', '0', '0' directly to
//         obtain stack = "0200".
//       - strip_leading_zeros("0200") finds the first non-zero character '2' at
//         index 1 and returns "200".
//
//   Example 3:
//     Input:  num = "100000", k = 3
//     Output: "0"
//     Explanation:
//       - Push '1' onto stack, then at the first '0' pop '1' (remaining = 2) and
//         push '0' (stack = "0").
//       - The next four '0' digits do not trigger pops since '0' > '0' is false,
//         leaving stack = "00000" and remaining = 2.
//       - Because remaining > 0 after the scan, resize stack from length 5 to 5 - 2
//         = 3 ("000").
//       - strip_leading_zeros("000") finds no non-zero digit (npos) and returns
//         "0".
//
// Algorithm:
//   Greedy monotonic non-decreasing stack:
//   - Leftmost digits carry the highest place value. Whenever a digit `d` is
//     strictly smaller than the preceding kept digit `stack.back()` and we
//     still have removals left (`remaining > 0`), popping `stack.back()` always
//     produces a strictly smaller number than keeping it.
//   - Scan `num` left to right, popping larger preceding digits while
//     `remaining > 0`, then pushing the current digit.
//   - Invariant: at the end of the scan, `stack` has length
//     `num.size() - k + remaining` and is non-decreasing if `remaining > 0`
//     (e.g., `"1234"`). Removing the last `remaining` largest digits from the
//     back completes the `k` deletions cleanly without ever underflowing.
//   - Finally, strip any leading `'0'`s; if nothing remains, return `"0"`.
//   Time Complexity: O(N), since each digit is pushed and popped at most once.
//   Space Complexity: O(N) for the result stack.
//
// Data Structures:
//   - `std::string` acting directly as a monotonic non-decreasing digit stack.
//
// Real-World Applications:
//   - Greedy lexicographic minimization in resource budgeting and digit-budget
//     compression problems.
//   - Selecting optimal minimal-cost subsequences under deletion constraints
//     in scheduling and streaming filters.
//   - Compiler peephole optimizations that eliminate locally suboptimal tokens
//     to minimize an ordered objective.
//
// Related LeetCode Problems:
//   - #316 Remove Duplicate Letters [done] (lexicographically smallest subsequence using a monotonic increasing stack)
//   - #321 Create Maximum Number [done] (selecting lexicographically maximal subsequences via monotonic stacks)
//   - #738 Monotone Increasing Digits [done] (finding the largest non-decreasing digit sequence <= N)
//   - #1673 Find the Most Competitive Subsequence [new] (fixed-length lexicographically smallest subsequence via monotonic stack)
//   - #496 Next Greater Element I [done] (core monotonic stack pattern)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Removes `k` digits from `num` to form the smallest possible non-negative
  // integer without leading zeros.
  // Time: O(N), Space: O(N).
  std::string removeKdigits(std::string_view num, int k) {
    if (k <= 0) {
      return strip_leading_zeros(num);
    }
    if (static_cast<std::size_t>(k) >= num.size()) {
      return "0";
    }

    std::string stack;
    stack.reserve(num.size());
    int remaining = k;

    // Maintain a monotonic non-decreasing stack of digits; popping a larger
    // higher-place-value digit in favor of a smaller digit is strictly optimal.
    for (const char digit : num) {
      while (!stack.empty() && remaining > 0 && stack.back() > digit) {
        stack.pop_back();
        --remaining;
      }
      stack.push_back(digit);
    }

    // If removals remain (e.g., non-decreasing input like "1234" or "100000"),
    // trim the largest digits from the right. By invariant, stack.size() >=
    // remaining always holds here.
    if (remaining > 0) {
      stack.resize(stack.size() - static_cast<std::size_t>(remaining));
    }

    return strip_leading_zeros(stack);
  }

 private:
  // Strips leading `'0'` characters, returning `"0"` if the view is empty or
  // consists solely of zeros.
  static std::string strip_leading_zeros(std::string_view digits) {
    const std::size_t first_non_zero = digits.find_first_not_of('0');
    if (first_non_zero == std::string_view::npos) {
      return "0";
    }
    return std::string(digits.substr(first_non_zero));
  }
};

LC_PROBLEM(402, "Remove K Digits", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.removeKdigits("1432219", 3), "1219");
    ctx.expect_eq(sol.removeKdigits("10200", 1), "200");
    ctx.expect_eq(sol.removeKdigits("10", 2), "0");
  });

  ctx.test("source_build_cases", [&] {
    ctx.expect_eq(sol.removeKdigits("1920", 1), "120");
    ctx.expect_eq(sol.removeKdigits("4121", 2), "11");
    // Regression case that caused IndexError in the original Python source.
    ctx.expect_eq(sol.removeKdigits("100000", 3), "0");
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.removeKdigits("9", 1), "0");
    ctx.expect_eq(sol.removeKdigits("112", 1), "11");
    ctx.expect_eq(sol.removeKdigits("12345", 2), "123");
    ctx.expect_eq(sol.removeKdigits("54321", 2), "321");
    ctx.expect_eq(sol.removeKdigits("100200", 2), "0");
    ctx.expect_eq(sol.removeKdigits("100", 1), "0");
    ctx.expect_eq(sol.removeKdigits("0", 0), "0");
  });
}

}  // namespace
