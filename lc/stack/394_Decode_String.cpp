// LeetCode 394: Decode String
// https://leetcode.com/problems/decode-string/
//
// Problem:
//   Given an encoded string `s` following the rule `k[encoded_string]`, where
//   `encoded_string` inside the square brackets is repeated exactly `k` times
//   (with positive integer `k` and possible arbitrary nesting), return the
//   fully decoded string. The input is guaranteed to be well-formed and
//   original text never contains digits.
//
// Algorithm:
//   Explicit frame-stack parser:
//   - Maintain a stack of `Frame{repeat_count, text}` entries seeded with a
//     root sentinel frame `{1, ""}` and an integer accumulator `current_count`.
//   - Scan `s` character by character:
//     * Digit `'0'..'9'`: accumulate multi-digit repeat count via
//       `current_count = (current_count * 10) + (c - '0')`.
//     * `'['`: push a new frame `{current_count, ""}` to capture the nested
//       substring and reset `current_count = 0`.
//     * `']'`: pop the top frame, pre-reserve capacity on the parent frame's
//       buffer, and append the popped text `frame.repeat_count` times to the
//       parent frame.
//     * Any other character: append directly to `stack.back().text`.
//   - After scanning completes, the root frame holds the fully decoded string.
//   Time Complexity: O(M), where M is the length of the decoded output string.
//   Space Complexity: O(M + D) for the accumulated output and nesting depth D.
//
// Data Structures:
//   - `std::vector<Frame>` stack storing `(repeat_count, std::string)` pairs
//     for each active bracket nesting level.
//
// Real-World Applications:
//   - Run-length and macro-based template decompression in network protocols
//     and configuration languages.
//   - Parsing nested repetition operators in regex engines, shell brace
//     expansion, and domain-specific languages.
//   - Evaluating nested chemical formulas or music-notation repeat blocks.
//
// Related LeetCode Problems:
//   - #471 Encode String with Shortest Length [new] (inverse compression problem using interval DP)
//   - #726 Number of Atoms [new] (stack-based parsing of nested parenthesized groups with multipliers)
//   - #1087 Brace Expansion [new] (expanding nested bracketed string templates)
//   - #224 Basic Calculator [done] (evaluating nested parenthesized expressions via a frame stack)
//   - #385 Mini Parser [new] (stack-driven parsing of recursively nested bracketed structures)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Decodes `k[encoded_string]` expressions with arbitrary nesting.
  // Time: O(decoded_length), Space: O(decoded_length + nesting_depth).
  std::string decodeString(std::string_view s) {
    // Seed with a root frame so top-level characters and closed bracket blocks
    // always have a valid parent buffer to append into.
    std::vector<Frame> stack;
    stack.push_back(Frame{.repeat_count = 1, .text = {}});

    int current_count = 0;

    for (const char c : s) {
      if (c >= '0' && c <= '9') {
        // Parse multi-digit repeat counts before '['.
        current_count = (current_count * 10) + (c - '0');
      } else if (c == '[') {
        // Open a new nested scope with the parsed repeat count.
        stack.push_back(Frame{.repeat_count = current_count, .text = {}});
        current_count = 0;
      } else if (c == ']') {
        // Close the current scope and materialize its repeated text into the
        // enclosing parent frame.
        const Frame completed = std::move(stack.back());
        stack.pop_back();

        std::string& parent = stack.back().text;
        const auto repeats = static_cast<std::size_t>(completed.repeat_count);
        parent.reserve(parent.size() + (completed.text.size() * repeats));
        for (std::size_t i = 0uz; i < repeats; ++i) {
          parent.append(completed.text);
        }
      } else {
        stack.back().text.push_back(c);
      }
    }

    std::string result = std::move(stack.back().text);
    return result;
  }

 private:
  struct Frame {
    int repeat_count = 1;
    std::string text;
  };
};

LC_PROBLEM(394, "Decode String", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.decodeString("3[a]2[bc]"), "aaabcbc");
    ctx.expect_eq(sol.decodeString("3[a2[c]]"), "accaccacc");
    ctx.expect_eq(sol.decodeString("2[abc]3[cd]ef"), "abcabccdcdcdef");
  });

  ctx.test("multi_digit_and_deep_nesting", [&] {
    ctx.expect_eq(sol.decodeString("10[a]"), "aaaaaaaaaa");
    ctx.expect_eq(sol.decodeString("2[2[y]pq4[2[jk]e1[f]]]ef"),
                  "yypqjkjkefjkjkefjkjkefjkjkefyypqjkjkefjkjkefjkjkefjkjkefef");
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.decodeString(""), "");
    ctx.expect_eq(sol.decodeString("plain"), "plain");
    ctx.expect_eq(sol.decodeString("1[x]"), "x");
    ctx.expect_eq(sol.decodeString("3[a]b2[c]d"), "aaabccd");
  });
}

}  // namespace
