// LeetCode 591: Tag Validator
// https://leetcode.com/problems/tag-validator/
//
// Problem:
//   Determine whether a given code snippet string is a valid XML-like document
//   under the following rules:
//     1. The entire snippet must be wrapped in a single valid closed tag:
//        `<TAG_NAME>TAG_CONTENT</TAG_NAME>`.
//     2. A valid `TAG_NAME` contains only uppercase ASCII letters (`A`-`Z`)
//        and has length in `[1, 9]`.
//     3. `TAG_CONTENT` may contain nested valid closed tags, CDATA sections
//        of the form `<![CDATA[...]]>` (ending at the first subsequent `]]>`),
//        and arbitrary plain characters except unmatched `<` or invalid tags.
//     4. All start and end tags must be properly balanced and nested, and no
//        characters (not even CDATA or another tag) may appear outside the
//        single outermost closed tag.
//
// Algorithm:
//   Perform a single left-to-right lexical scan using an explicit stack of
//   currently open tag names (`std::string_view` slices into `code`):
//     - Reject empty input immediately.
//     - At every position `i > 0`, if the tag stack is empty, the outermost
//       root tag has already closed while trailing content remains -> invalid.
//     - Dispatch on the prefix at `code[i]`:
//       1. `<![CDATA[`: allowed only when inside an open tag (`!stack.empty()`).
//          Jump past the first `]]>` delimiter; fail if missing.
//       2. `</`: parse up to the next `'>'`, validate the tag name, verify it
//          matches `stack.back()`, and pop it.
//       3. `<`: parse up to the next `'>'`, validate the tag name, and push it.
//       4. Any other character: allowed only when inside an open tag
//          (`!stack.empty()`), then advance by one character.
//     - The snippet is valid iff the scan finishes cleanly with an empty stack.
//   Time Complexity:  O(n), where n is the length of `code` (each character is
//                     inspected a constant number of times).
//   Space Complexity: O(n) worst-case stack depth for deeply nested tags.
//
// Data Structures:
//   - Open-tag stack (`std::vector<std::string_view>`): enforces LIFO matching
//     between `<TAG_NAME>` and `</TAG_NAME>` without copying tag strings.
//
// Real-World Applications:
//   - Streaming XML / XHTML / SVG well-formedness validators (SAX parsers).
//   - Sanitization and static analysis of template engines and markup snippets
//     that embed raw literal blocks (CDATA / verbatim sections).
//   - IDE syntax checkers verifying balanced hierarchical delimiters.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (foundational stack-based delimiter matching)
//   - #394 Decode String [done] (nested delimiter parsing with a stack)
//   - #636 Exclusive Time Of Functions [done] (nested start/end event stack)
//   - #1249 Minimum Remove to Make Valid Parentheses [done] (balanced markup)
//   - #726 Number of Atoms [new] (recursive/stack-based lexical parser)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Validates whether `code` is a single properly closed, well-formed tag tree.
  // Time: O(n), Space: O(n).
  bool isValid(std::string_view code) {
    if (code.empty()) {
      return false;
    }

    std::vector<std::string_view> open_tags;
    std::size_t idx = 0uz;
    const std::size_t n = code.size();

    while (idx < n) {
      // Once the outermost root tag closes, no trailing tokens are permitted.
      if (idx > 0uz && open_tags.empty()) {
        return false;
      }

      const std::string_view remaining = code.substr(idx);
      if (remaining.starts_with(kCdataOpen)) {
        if (!consume_cdata(code, idx, open_tags)) {
          return false;
        }
      } else if (remaining.starts_with("</")) {
        if (!consume_end_tag(code, idx, open_tags)) {
          return false;
        }
      } else if (code[idx] == '<') {
        if (!consume_start_tag(code, idx, open_tags)) {
          return false;
        }
      } else {
        // Plain text must reside inside an open tag (rejects leading text).
        if (open_tags.empty()) {
          return false;
        }
        ++idx;
      }
    }

    return open_tags.empty();
  }

 private:
  static constexpr std::string_view kCdataOpen = "<![CDATA[";
  static constexpr std::string_view kCdataClose = "]]>";
  static constexpr std::size_t kMinTagLen = 1uz;
  static constexpr std::size_t kMaxTagLen = 9uz;

  // A valid TAG_NAME has length in [1, 9] and only uppercase ASCII letters.
  static bool is_valid_tag_name(std::string_view tag) {
    if (tag.size() < kMinTagLen || tag.size() > kMaxTagLen) {
      return false;
    }
    return std::ranges::all_of(
        tag, [](char ch) { return ch >= 'A' && ch <= 'Z'; });
  }

  // Consumes `<![CDATA[...]]>` starting at `idx`, stopping at the first `]]>`.
  static bool consume_cdata(std::string_view code, std::size_t& idx,
                            const std::vector<std::string_view>& open_tags) {
    if (open_tags.empty()) {
      return false;
    }
    const std::size_t close_pos =
        code.find(kCdataClose, idx + kCdataOpen.size());
    if (close_pos == std::string_view::npos) {
      return false;
    }
    idx = close_pos + kCdataClose.size();
    return true;
  }

  // Consumes `</TAG_NAME>` starting at `idx` and matches it with stack top.
  static bool consume_end_tag(std::string_view code, std::size_t& idx,
                              std::vector<std::string_view>& open_tags) {
    const std::size_t name_start = idx + 2uz;
    const std::size_t close_pos = code.find('>', name_start);
    if (close_pos == std::string_view::npos) {
      return false;
    }
    const std::string_view tag =
        code.substr(name_start, close_pos - name_start);
    if (!is_valid_tag_name(tag) || open_tags.empty() ||
        open_tags.back() != tag) {
      return false;
    }
    open_tags.pop_back();
    idx = close_pos + 1uz;
    return true;
  }

  // Consumes `<TAG_NAME>` starting at `idx` and pushes it onto the stack.
  static bool consume_start_tag(std::string_view code, std::size_t& idx,
                                std::vector<std::string_view>& open_tags) {
    const std::size_t name_start = idx + 1uz;
    const std::size_t close_pos = code.find('>', name_start);
    if (close_pos == std::string_view::npos) {
      return false;
    }
    const std::string_view tag =
        code.substr(name_start, close_pos - name_start);
    if (!is_valid_tag_name(tag)) {
      return false;
    }
    open_tags.push_back(tag);
    idx = close_pos + 1uz;
    return true;
  }
};

LC_PROBLEM(591, "Tag Validator", "stack", ctx) {
  ctx.test("valid_examples_with_cdata", [&] {
    Solution sol;
    ctx.expect_true(
        sol.isValid("<DIV>This is the first line <![CDATA[<div>]]></DIV>"));
    ctx.expect_true(
        sol.isValid("<DIV>>>  ![cdata[]] <![CDATA[<div>]>]]>]]>>]</DIV>"));
    ctx.expect_true(sol.isValid("<A><![CDATA[</A>]]></A>"));
    ctx.expect_true(sol.isValid("<ABCDEFGHI><A></A></ABCDEFGHI>"));
  });

  ctx.test("invalid_unbalanced_and_unclosed", [&] {
    Solution sol;
    ctx.expect_false(sol.isValid("<A>  <B> </A>   </B>"));
    ctx.expect_false(sol.isValid("<DIV>  div tag is not closed  <DIV>"));
    ctx.expect_false(sol.isValid("<DIV>  unmatched <  </DIV>"));
    ctx.expect_false(
        sol.isValid("<DIV>  unmatched start tag <B>  and unmatched end tag "
                    "</C>  </DIV>"));
  });

  ctx.test("invalid_tag_names", [&] {
    Solution sol;
    ctx.expect_false(
        sol.isValid("<DIV> closed tags with invalid tag name  <b>123</b> "
                    "</DIV>"));
    ctx.expect_false(sol.isValid(
        "<DIV> unmatched tags with invalid tag name  </1234567890> and "
        "<CDATA[[]]>  </DIV>"));
    ctx.expect_false(sol.isValid("<ABCDEFGHIJ></ABCDEFGHIJ>"));
    ctx.expect_false(sol.isValid("<></>"));
  });

  ctx.test("invalid_root_wrapping_and_cdata", [&] {
    Solution sol;
    ctx.expect_false(sol.isValid(""));
    ctx.expect_false(sol.isValid("<A></A><B></B>"));
    ctx.expect_false(sol.isValid("<A></A> "));
    ctx.expect_false(sol.isValid(" <A></A>"));
    ctx.expect_false(sol.isValid("<![CDATA[wahaha]]]><![CDATA[]> wahaha]]>"));
    ctx.expect_false(sol.isValid("<A><![CDATA[unclosed</A>"));
  });
}

}  // namespace
