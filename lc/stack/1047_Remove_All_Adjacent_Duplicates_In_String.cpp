// LeetCode 1047: Remove All Adjacent Duplicates In String
// https://leetcode.com/problems/remove-all-adjacent-duplicates-in-string/
//
// Problem:
//   Given a string `s` of lowercase English letters, repeatedly choose two
//   adjacent equal characters and delete them until no adjacent duplicate pair
//   remains. Return the resulting unique string.
//
// Examples:
//   Example 1:
//     Input:  s = "abbaca"
//     Output: "ca"
//     Explanation:
//       - Push 'a' then 'b' onto result, forming "ab".
//       - Next 'b' matches result.back() ('b'), so pop 'b' to leave result = "a".
//       - Next 'a' now matches result.back() ('a'), triggering a cascading pop that
//         empties result to "".
//       - Push 'c' then 'a' onto the empty buffer since neither matches the top
//         character, returning "ca".
//
//   Example 2:
//     Input:  s = "azxxzy"
//     Output: "ay"
//     Explanation:
//       - Push 'a' and 'z' onto result to form "az".
//       - Push the first 'x' ("azx"), then pop it when the second 'x' matches
//         result.back(), returning result to "az".
//       - Next 'z' matches result.back() ('z'), so pop 'z' to leave result = "a".
//       - Push 'y' onto "a", returning "ay".
//
//   Example 3:
//     Input:  s = "aaaaa"
//     Output: "a"
//     Explanation:
//       - Push the first 'a' ("a"), then pop it when the second 'a' matches
//         result.back() ("").
//       - Push the third 'a' ("a"), then pop it when the fourth 'a' matches
//         result.back() ("").
//       - Push the fifth 'a' onto the empty result buffer and return "a".
//
// Algorithm:
//   Single-pass character stack. Use the output `std::string result` directly
//   as a stack. For each character `ch` in `s`:
//   - If `result` is non-empty and `result.back() == ch`, the two characters
//     form an adjacent duplicate pair (either originally adjacent or brought
//     together by prior cancellations), so pop `result.back()`.
//   - Otherwise, append `ch` onto `result`.
//   Because the reduction system is confluent (the order of pair removals does
//   not affect the final irreducible string), one greedy left-to-right stack
//   pass always produces the exact unique result.
//   Time Complexity: O(N), where N = s.size(); every character is pushed and
//   popped at most once.
//   Space Complexity: O(N) for the output string buffer (O(1) extra space
//   beyond the returned string).
//
// Data Structures:
//   - `std::string result`: acts simultaneously as the LIFO cancellation stack
//     and the final returned string, avoiding a separate join step.
//
// Real-World Applications:
//   - Algebraic word reduction in free groups and Coxeter groups (cancelling
//     adjacent inverse generators such as `x * x^-1`).
//   - Peephole compiler optimizations eliminating redundant inverse instruction
//     pairs (e.g., consecutive `NEG`/`NEG`, `NOT`/`NOT`, or `PUSH`/`POP`).
//   - Text and DNA/signal stream deduplication and bracket/token cancellation.
//
// Related LeetCode Problems:
//   - #1209 Remove All Adjacent Duplicates in String II [done] (generalized to k duplicates)
//   - #844 Backspace String Compare [done] (character stack simulation)
//   - #1544 Make The String Great [new] (adjacent opposite-case pair removal)
//   - #20 Valid Parentheses [done] (adjacent matching-pair cancellation)
//   - #2390 Removing Stars From a String [new] (single-pass string stack)

#include "framework/lc_framework.h"

#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Repeatedly cancels all adjacent equal character pairs in `s` in one pass
  // using the result string itself as a stack.
  // Invariant: `result` contains no adjacent equal characters at the end of
  // every iteration.
  // Time: O(N), Space: O(N).
  std::string removeDuplicates(std::string_view s) {
    std::string result;
    result.reserve(s.size());

    for (const char ch : s) {
      if (!result.empty() && result.back() == ch) {
        // Cancel `ch` against the immediately preceding surviving character.
        result.pop_back();
      } else {
        result.push_back(ch);
      }
    }

    return result;
  }
};

LC_PROBLEM(1047, "Remove All Adjacent Duplicates In String", "stack", ctx) {
  Solution sol;

  ctx.test("example 1 cascading removal",
           [&] { ctx.expect_eq(sol.removeDuplicates("abbaca"), "ca"); });

  ctx.test("example 2",
           [&] { ctx.expect_eq(sol.removeDuplicates("azxxzy"), "ay"); });

  ctx.test("complete cancellation and odd runs", [&] {
    ctx.expect_eq(sol.removeDuplicates("abba"), "");
    ctx.expect_eq(sol.removeDuplicates("aaaa"), "");
    ctx.expect_eq(sol.removeDuplicates("aaaaa"), "a");
  });

  ctx.test("single char and already reduced", [&] {
    ctx.expect_eq(sol.removeDuplicates("a"), "a");
    ctx.expect_eq(sol.removeDuplicates("abcdef"), "abcdef");
    ctx.expect_eq(sol.removeDuplicates(""), "");
  });
}

}  // namespace
