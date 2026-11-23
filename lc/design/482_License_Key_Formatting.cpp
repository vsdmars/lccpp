// LeetCode 482: License Key Formatting
// https://leetcode.com/problems/license-key-formatting/
//
// Problem:
//   Given a string `s` consisting of alphanumeric characters and dashes '-',
//   and a positive integer `k`, reformat `s` so that:
//     1. All existing dashes are removed.
//     2. Every lowercase ASCII letter is converted to uppercase.
//     3. Characters are grouped into dash-separated segments of exact length
//        `k`, except the first (leftmost) segment which may be shorter than `k`
//        (but must contain at least one character whenever any alphanumeric
//        character exists).
//
// Examples:
//   Example 1:
//     Input:  s = "5F3Z-2e-9-w", k = 4
//     Output: "5F3Z-2E9W"
//     Explanation:
//       - Scan s from right to left skipping '-', uppercasing and pushing 'W', '9',
//         'E', and '2' until current_group_len reaches 4.
//       - Before pushing 'Z', current_group_len == 4 triggers appending '-' and
//         resetting current_group_len to 0, yielding "W9E2-".
//       - Continue scanning past '-' to append 'Z', '3', 'F', and '5', forming
//         "W9E2-Z3F5".
//       - Reverse the buffer in place with std::ranges::reverse to return
//         "5F3Z-2E9W".
//
//   Example 2:
//     Input:  s = "2-5g-3-J", k = 2
//     Output: "2-5G-3J"
//     Explanation:
//       - Scan right to left with group_size = 2, appending 'J' and '3' to form
//         "J3" with current_group_len = 2.
//       - On 'g', current_group_len == 2 inserts '-' before uppercased 'G', then
//         '5' is appended to give "J3-G5" with current_group_len = 2.
//       - On the leftmost '2', current_group_len == 2 inserts another '-' before
//         '2', producing "J3-G5-2" with a shorter final group.
//       - Reverse the buffer in place to return "2-5G-3J".
//
//   Example 3:
//     Input:  s = "--a-a-a-a--", k = 2
//     Output: "AA-AA"
//     Explanation:
//       - Reverse scan skips the trailing "--" and appends two uppercased 'A'
//         characters so formatted = "AA" and current_group_len = 2.
//       - When reaching the third 'a', current_group_len == 2 emits '-' before 'A',
//         and the fourth 'a' completes "AA-AA".
//       - The leading "--" characters are skipped without emitting a dash because
//         no further alphanumeric characters follow, and reversing "AA-AA" yields
//         "AA-AA".
//
// Algorithm:
//   Scan `s` in a single reverse pass from right to left. Ignore dashes; for
//   each alphanumeric character, if the current right-aligned group has
//   already reached length `k`, emit a '-' separator first and reset the group
//   counter, then append the uppercased character. Emitting dashes lazily
//   before the next valid character guarantees no stray leading/trailing dash
//   even when the input contains only dashes or its alphanumeric length is an
//   exact multiple of `k`. Finally, reverse the constructed string in place.
//   Time Complexity:  O(N), where N = s.size().
//   Space Complexity: O(N) for the formatted output string (O(1) auxiliary).
//
// Data Structures:
//   - std::string: Pre-reserved output buffer built in reverse and reversed
//     once via std::ranges::reverse.
//
// Real-World Applications:
//   - Software product key, serial number, and activation code canonicalization
//     during user input normalization.
//   - Credit card, IBAN, phone number, and UUID/MAC-address display formatting
//     with fixed-width delimiter grouping.
//   - Checksum-block formatting in cryptographic recovery codes and 2FA backup
//     tokens.
//
// Related LeetCode Problems:
//   - #1694 Reformat Phone Number [new] (grouped dash formatting with tail rules)
//   - #1556 Thousand Separator [new] (right-to-left fixed-width delimiter pass)
//   - #709 To Lower Case [new] (ASCII case normalization)
//   - #541 Reverse String II [new] (chunked string reversal and grouping)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Reformats a raw license key into uppercase dash-separated groups of size
  // `k` aligned from the right.
  // Time: O(N). Space: O(N).
  std::string licenseKeyFormatting(std::string_view s, int k) {
    if (k <= 0) {
      return {};
    }

    const auto group_size = static_cast<std::size_t>(k);
    std::string formatted;
    // Upper bound on output size avoids repeated reallocations.
    formatted.reserve(s.size() + (s.size() / group_size));

    std::size_t current_group_len = 0uz;
    for (std::size_t i = s.size(); i > 0uz; --i) {
      const char ch = s[i - 1uz];
      if (ch == '-') {
        continue;
      }

      // Insert a dash only when a subsequent character starts a new group,
      // which naturally avoids any leading dash after reversal.
      if (current_group_len == group_size) {
        formatted.push_back('-');
        current_group_len = 0uz;
      }

      // Cast through unsigned char before std::toupper to avoid UB on signed
      // char inputs.
      const auto uchar = static_cast<unsigned char>(ch);
      formatted.push_back(static_cast<char>(std::toupper(uchar)));
      ++current_group_len;
    }

    std::ranges::reverse(formatted);
    return formatted;
  }
};

LC_PROBLEM(482, "License Key Formatting", "design", ctx) {
  ctx.test("leetcode_example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.licenseKeyFormatting("5F3Z-2e-9-w", 4),
                  std::string("5F3Z-2E9W"));
  });

  ctx.test("leetcode_example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.licenseKeyFormatting("2-5g-3-J", 2),
                  std::string("2-5G-3J"));
  });

  ctx.test("python_build_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.licenseKeyFormatting("r", 1), std::string("R"));
    ctx.expect_eq(sol.licenseKeyFormatting("2", 2), std::string("2"));
  });

  ctx.test("only_dashes_and_leading_trailing_dashes", [&] {
    Solution sol;
    ctx.expect_eq(sol.licenseKeyFormatting("---", 3), std::string());
    ctx.expect_eq(sol.licenseKeyFormatting("--a-a-a-a--", 2),
                  std::string("AA-AA"));
  });

  ctx.test("group_size_larger_than_input", [&] {
    Solution sol;
    ctx.expect_eq(sol.licenseKeyFormatting("a-b-c", 10), std::string("ABC"));
  });
}

}  // namespace
